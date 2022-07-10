#include<iostream>
#include<math.h>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <xmmintrin.h> //SSE
#include <emmintrin.h> //SSE2
#include <pmmintrin.h> //SSE3
#include <tmmintrin.h> //SSSE3
#include <smmintrin.h> //SSE4.1
#include <nmmintrin.h> //SSSE4.2
#include <immintrin.h> //AVX、AVX2
#include<malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <ctime>
#include <windows.h>
using namespace std;
//const int id=1,n=130,elinenum=8;
//const int id=2,n=254,elinenum=53;
//const int id=3,n=562,elinenum=53;
//const int id=4,n=1011,elinenum=263;
//const int id=5,n=2362,elinenum=453;
//const int id=6,n=3799,elinenum=1953;
//const int id=7,n=8399,elinenum=4535;
//const int id=8,n=23045,elinenum=14325;
//const int id=9,n=37960,elinenum=14921;
//const int id=10,n=43577,elinenum=54274;
const int id=11,n=85401,elinenum=756;
//给定矩阵的列数、被消元行数
string str1="D:\\bingxing\\homework-X86\\special\\data\\被消元行11.txt";
string str2="D:\\bingxing\\homework-X86\\special\\data\\消元子11.txt";
int bytenum=(n+31)/32;
//32位组成一组，一共有的组数
typedef struct
{
    int t_id; // 线程 id
}threadparam_t;
#define NUM_THREADS 4
sem_t sem_leader;
sem_t sem_Division[NUM_THREADS-1];
sem_t sem_Elimination[NUM_THREADS-1];
pthread_barrier_t barrier;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;
class bitmap
{
public:
    int mycol;
    //首项
    int *bitVector;
    bitmap()
    {
        mycol=-1;
        bitVector = new int[bytenum];
        //bitVector数组中存储着这分出来的bytenum个组，并全部初始化为0
        for(int i=0;i<bytenum;i++)
            bitVector[i]=0;
    }
    bool isnull(){
        if(mycol==-1)return 1;
        //如果没有指定的首元，则说明空行
        return 0;
    }
    void insertData(int index)
    {
        if(mycol==-1)
            mycol=index;
        int a=index/32,b=index%32;
        bitVector[a]|=(1<<b);
    }
    int getcol()
    {
        return mycol;
    }

    void showData()
    {
        for (int i=0;i<n;i++)
        {
            if ((bitVector[i>>5])&(1<<(i&31)))
            {
                cout<<i<<" ";
            }
        }
        cout<<endl;
    }
};
bitmap *eliminator=new bitmap[n];
//给消元子的空间是整个的大小
bitmap *eline=new bitmap[elinenum];
//给被消元行的空间是消元行个数大小
void readdata(){
    ifstream elininput;
    string elinStr;
    //读入被消元行
    //elininput.open("D:\\bingxing\\homework-X86\\special\\data\\被消元行10.txt");
    elininput.open(str1.c_str());
    //这里的斜杠需要双写才能读入
    //elininput.open("被消元行1.txt");
    int thisrow=0;
    while(getline(elininput,elinStr)){
        istringstream ss(elinStr);
        int data;
        while(ss>>data){
            eline[thisrow].insertData(data);
            //被消元行的行号是要从0开始一行一行老实往上加的
        }
        thisrow++;
    }
    elininput.close();

    ifstream etorinput;
    //读入消元子
    //etorinput.open("D:\\bingxing\\homework-X86\\special\\data\\消元子10.txt");
    etorinput.open(str2.c_str());
    while(getline(etorinput,elinStr)){
        istringstream iss(elinStr);
        int data;
        int thisrow=0;
        while(iss>>data){
            if(!thisrow)
                thisrow=data;
            //消元子的第一个数据表示第一个1出现的行，也就是行号
            eliminator[thisrow].insertData(data);
            //这一行的数据（消元子）都逐个读出加入到eliminator的这一行
        }
    }
    etorinput.close();
}
void *pthreadfunc(void* param)
{
    threadparam_t *p=(threadparam_t *)param;
    int t_id=p->t_id;
     for(int i=0;i<elinenum;i++)
    {
        while(!eline[i].isnull())
        {  //如果被消元行非空
            bool flag=eliminator[eline[i].getcol()].isnull();
            //先找到被消元行的首项，看是否存在对应的消元子
            if(!flag)
                //非空。即如果存在的情况
            {
                for(int j=t_id;j<bytenum;j+=NUM_THREADS)
                eline[i].bitVector[j]^=eliminator[eline[i].getcol()].bitVector[j];

            }
            else
            {
                pthread_barrier_wait(&barrier3);
                eliminator[eline[i].getcol()]=eline[i];
                 //被消元行升格为消元子
                break;
            }
            pthread_barrier_wait(&barrier);
            if(t_id==0)
            {
                bool flag=1;
                for(int k=bytenum-1;k>=0&&flag;k--)
                    for(int j=31;j>=0&&flag;j--)
                        if((eline[i].bitVector[k]&(1<<j))!=0)
                        {
                            //32个一组地找，在32内部一个一个的找
                            eline[i].mycol=k*32+j;
                            //找到1后记录首元位置即可
                            flag=0;
                        }
                if(flag) eline[i].mycol=-1;
            }
            pthread_barrier_wait(&barrier2);
        }
    }
    pthread_exit(NULL);
}
void s_pthread()
{
    //特殊高斯pthread并行优化算法
    //创建线程
    pthread_barrier_init(&barrier,NULL,NUM_THREADS);
    pthread_barrier_init(&barrier2,NULL,NUM_THREADS);
    pthread_barrier_init(&barrier3,NULL,NUM_THREADS);
    pthread_t handles[NUM_THREADS];
    threadparam_t param[NUM_THREADS];
    for(int t_id=0;t_id<NUM_THREADS;t_id++)
   {
       param[t_id].t_id=t_id;
       pthread_create(&handles[t_id],NULL,pthreadfunc,(void *)&param[t_id]);
   }
   for(int i=0;i<NUM_THREADS;i++)
        pthread_join(handles[i],NULL);
   pthread_barrier_destroy(&barrier);
   pthread_barrier_destroy(&barrier2);
   pthread_barrier_destroy(&barrier3);
}

void printmatrix()
{
    for(int i=0;i<elinenum;i++){
        if(eline[i].isnull()){puts("");continue;}
        for(int j=bytenum-1;j>=0;j--){
            for(int k=31;k>=0;k--)
                if((eline[i].bitVector[j]&(1<<k))!=0){
                    printf("%d ",j*32+k);
                }
                }
        puts("");
    }
}

int main()
{

    readdata();
    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    s_pthread();
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    cout<<"测试用例"<<id<<" 矩阵列数： "<<n<<" 被消元行个数："<<elinenum<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
    //printmatrix();
    system("pause");
    return 0;
}



