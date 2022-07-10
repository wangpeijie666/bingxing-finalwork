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
#include <immintrin.h> //AVX��AVX2
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
//�������������������Ԫ����
string str1="D:\\bingxing\\homework-X86\\special\\data\\����Ԫ��11.txt";
string str2="D:\\bingxing\\homework-X86\\special\\data\\��Ԫ��11.txt";
int bytenum=(n+31)/32;
//32λ���һ�飬һ���е�����
typedef struct
{
    int t_id; // �߳� id
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
    //����
    int *bitVector;
    bitmap()
    {
        mycol=-1;
        bitVector = new int[bytenum];
        //bitVector�����д洢����ֳ�����bytenum���飬��ȫ����ʼ��Ϊ0
        for(int i=0;i<bytenum;i++)
            bitVector[i]=0;
    }
    bool isnull(){
        if(mycol==-1)return 1;
        //���û��ָ������Ԫ����˵������
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
//����Ԫ�ӵĿռ��������Ĵ�С
bitmap *eline=new bitmap[elinenum];
//������Ԫ�еĿռ�����Ԫ�и�����С
void readdata(){
    ifstream elininput;
    string elinStr;
    //���뱻��Ԫ��
    //elininput.open("D:\\bingxing\\homework-X86\\special\\data\\����Ԫ��10.txt");
    elininput.open(str1.c_str());
    //�����б����Ҫ˫д���ܶ���
    //elininput.open("����Ԫ��1.txt");
    int thisrow=0;
    while(getline(elininput,elinStr)){
        istringstream ss(elinStr);
        int data;
        while(ss>>data){
            eline[thisrow].insertData(data);
            //����Ԫ�е��к���Ҫ��0��ʼһ��һ����ʵ���ϼӵ�
        }
        thisrow++;
    }
    elininput.close();

    ifstream etorinput;
    //������Ԫ��
    //etorinput.open("D:\\bingxing\\homework-X86\\special\\data\\��Ԫ��10.txt");
    etorinput.open(str2.c_str());
    while(getline(etorinput,elinStr)){
        istringstream iss(elinStr);
        int data;
        int thisrow=0;
        while(iss>>data){
            if(!thisrow)
                thisrow=data;
            //��Ԫ�ӵĵ�һ�����ݱ�ʾ��һ��1���ֵ��У�Ҳ�����к�
            eliminator[thisrow].insertData(data);
            //��һ�е����ݣ���Ԫ�ӣ�������������뵽eliminator����һ��
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
        {  //�������Ԫ�зǿ�
            bool flag=eliminator[eline[i].getcol()].isnull();
            //���ҵ�����Ԫ�е�������Ƿ���ڶ�Ӧ����Ԫ��
            if(!flag)
                //�ǿա���������ڵ����
            {
                for(int j=t_id;j<bytenum;j+=NUM_THREADS)
                eline[i].bitVector[j]^=eliminator[eline[i].getcol()].bitVector[j];

            }
            else
            {
                pthread_barrier_wait(&barrier3);
                eliminator[eline[i].getcol()]=eline[i];
                 //����Ԫ������Ϊ��Ԫ��
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
                            //32��һ����ң���32�ڲ�һ��һ������
                            eline[i].mycol=k*32+j;
                            //�ҵ�1���¼��Ԫλ�ü���
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
    //�����˹pthread�����Ż��㷨
    //�����߳�
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
    cout<<"��������"<<id<<" ���������� "<<n<<" ����Ԫ�и�����"<<elinenum<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
    //printmatrix();
    system("pause");
    return 0;
}



