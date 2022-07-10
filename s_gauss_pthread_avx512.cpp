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
#include <sys/time.h>
#include<time.h>
using namespace std;
//const int id=1,n=130,elinenum=8;
//const int id=2,n=254,elinenum=53;
//const int id=3,n=562,elinenum=53;
//const int id=4,n=1011,elinenum=263;
//const int id=5,n=2362,elinenum=453;
//const int id=6,n=3799,elinenum=1953;
const int id=7,n=8399,elinenum=4535;
//const int id=8,n=23045,elinenum=14325;
//const int id=9,n=37960,elinenum=14921;
//const int id=10,n=43577,elinenum=54274;
//const int id=11,n=85401,elinenum=756;
//给定矩阵的列数、被消元行数
string str1="被消元行7.txt";
string str2="消元子7.txt";
int bytenum=(n+31)/32;
//32位组成一组，一共有的组数
typedef struct
{
    int t_id; // 线程 id
}threadparam_t;
int NUM_THREADS=7;
pthread_barrier_t barrier;
pthread_barrier_t barrier2;
pthread_barrier_t barrier3;
int tid=0;
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
    for(int i=n-1;i>=0;i--)
    {
        //遍历所有的消元子,如果找到非空的
        if(!eliminator[i].isnull())
        {
            for(int j=t_id;j<elinenum;j+=NUM_THREADS)
            {
                //通过pthread并行的方法遍历被消元行
                if(eline[j].getcol()==i)
                {
                    int k=0;
                    //先找到匹配该非空消元子的被消元行
                    for(;k<bytenum-16;k+=16)
                    {
                        __m512 t1,t2,t3;
                        t1=_mm512_loadu_ps((float*)(eline[j].bitVector+k));
                        t2=_mm512_loadu_ps((float*)(eliminator[i].bitVector+k));
                        t3=_mm512_xor_ps(t1,t2);
                        _mm512_storeu_ps((float*)(eline[j].bitVector+k),t3);

                    }
                    if(k<bytenum)
                    {
                        for(;k<bytenum;k++)
                             eline[j].bitVector[k]^=eliminator[i].bitVector[k];

                    }
                    bool flag=1;
                    for(int q=bytenum-1;q>=0&&flag;q--)
                    for(int p=31;p>=0&&flag;p--)
                        if((eline[j].bitVector[q]&(1<<p))!=0)
                        {
                            //32个一组地找，在32内部一个一个的找
                            eline[j].mycol=q*32+p;
                            //找到1后记录首元位置即可
                            flag=0;
                        }
                    if(flag) eline[j].mycol=-1;
                }
            }
        }
        else
        {
            //如果是空的消元子
            pthread_barrier_wait(&barrier);
            if(t_id==0)
            {
                for(int j=0;j<elinenum;j++)
                {
                    if(eline[j].getcol()==i)
                    {
                        //该行没有消元子，可以找打符合条件的被消元行升格成消元子
                        eliminator[i]=eline[j];
                        tid=j+1;
                        break;
                    }
                    tid=j+2;
                }
            }
        }
        pthread_barrier_wait(&barrier2);
        int tid2=t_id;
        while(tid2<tid) tid2+=NUM_THREADS;
        for(int j=tid2;j<elinenum;j+=NUM_THREADS)
        {
             if(eline[j].getcol()==i)
                {
                    int k=0;
                    //先找到匹配该非空消元子的被消元行
                    for(;k<bytenum-16;k+=16)
                    {
                        __m512 t1,t2,t3;
                        t1=_mm512_loadu_ps((float*)(eline[j].bitVector+k));
                        t2=_mm512_loadu_ps((float*)(eliminator[i].bitVector+k));
                        t3=_mm512_xor_ps(t1,t2);
                        _mm512_storeu_ps((float*)(eline[j].bitVector+k),t3);

                    }
                    if(k<bytenum)
                    {
                        for(;k<bytenum;k++)
                             eline[j].bitVector[k]^=eliminator[i].bitVector[k];

                    }
                    bool flag=1;
                    for(int q=bytenum-1;q>=0&&flag;q--)
                    for(int p=31;p>=0&&flag;p--)
                        if((eline[j].bitVector[q]&(1<<p))!=0)
                        {
                            //32个一组地找，在32内部一个一个的找
                            eline[j].mycol=q*32+p;
                            //找到1后记录首元位置即可
                            flag=0;
                        }
                    if(flag) eline[j].mycol=-1;
                }
        }
    }
pthread_exit(NULL);
}
void s_pthread_pro_avx512()
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


int main()
{

    
     readdata();
    struct  timeval start;
    struct  timeval end;
    unsigned  long diff;
    gettimeofday(&start, NULL);
    s_pthread_pro_avx512();
    gettimeofday(&end, NULL);
    diff = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
    cout<<"测试用例"<<id<<" 矩阵列数： "<<n<<" 被消元行个数："<<elinenum<<" time: "<<diff<<"us"<<endl;
   // printmatrix();
    return 0;
}




