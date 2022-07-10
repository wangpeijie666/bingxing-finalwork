#include<iostream>
#include<fstream>
#include<sstream>
#include <emmintrin.h>
#include <immintrin.h>
#include <stdlib.h>
#include<windows.h>
using namespace std;
const int col=254,elinenum=53; //����������Ԫ����
int bytenum=(col-1)/32+1;   //ÿ��ʵ���е�byte��������
string str1="D:\\bingxing\\homework-X86\\special\\data\\����Ԫ��2.txt";
string str2="D:\\bingxing\\homework-X86\\special\\data\\��Ԫ��2.txt";
class bitmatrix{
public:
    int mycol;    //����
    int *mybyte;
    bitmatrix(){    //��ʼ��
        mycol=-1;
        mybyte=(int *)_aligned_malloc(1024, 1024 * sizeof(int));
        for(int i=0;i<bytenum;i++)
            mybyte[i]=0;
    }
    bool isnull(){  //�жϵ�ǰ���Ƿ�Ϊ����
        if(mycol==-1)return 1;
        return 0;
    }
    void insert(int x){ //���ݶ���
        if(mycol==-1)mycol=x;
        int a=x/32,b=x%32;
        mybyte[a]|=(1<<b);
    }
    void doxor(bitmatrix b){  //�����������������ڽ�����ڱ�ʵ���У�ֻ�б���Ԫ����ִ����һ����,����������Ҫ��������
        int i=0;
        for(;i+4<=bytenum;i+=4){
            __m128i byte1=_mm_load_si128((__m128i*)mybyte+i);
            __m128i byte2=_mm_load_si128((__m128i*)b.mybyte+i);
            byte1=_mm_xor_si128(byte1,byte2);
            _mm_store_si128((__m128i*)mybyte+i,byte1);
            }
        for(;i<bytenum;i++)
             mybyte[i]^=b.mybyte[i];
        for(int i=bytenum-1;i>=0;i--)
            for(int j=31;j>=0;j--)
                if((mybyte[i]&(1<<j))!=0){
                    mycol=i*32+j;
                    return;
                }
        mycol=-1;
    }
};
bitmatrix *eliminer=new bitmatrix[col],*eline=new bitmatrix[elinenum];
void readdata(){
    ifstream ifs;
    ifs.open(str2);  //��Ԫ��
    string temp;
    while(getline(ifs,temp)){
        istringstream ss(temp);
        int x;
        int trow=0;
        while(ss>>x){
            if(!trow)trow=x;    //��һ������Ԫ�ش����к�
            eliminer[trow].insert(x);
        }
    }
    ifs.close();
    ifstream ifs2;
    ifs2.open(str1);     //����Ԫ��,���뷽ʽ����Ԫ�Ӳ�ͬ
    int trow=0;
    while(getline(ifs2,temp)){
        istringstream ss(temp);
        int x;
        while(ss>>x){
            eline[trow].insert(x);
        }
        trow++;
    }
    ifs2.close();
}

void dowork(){  //��Ԫ
    for(int i=0;i<elinenum;i++){
        while(!eline[i].isnull()){  //ֻҪ����Ԫ�зǿգ�ѭ������
            int tcol = eline[i].mycol;  //����Ԫ�е�����
            if(!eliminer[tcol].isnull())    //������ڶ�Ӧ��Ԫ��
                eline[i].doxor(eliminer[tcol]);
            else{
                eliminer[tcol]=eline[i];    //���ڱ���Ԫ������Ϊ��Ԫ�Ӻ󲻲��������������ֱ����=��ǳ����
                break;
            }
        }
    }
}
void printres(){ //��ӡ���
    for(int i=0;i<elinenum;i++){
        if(eline[i].isnull()){puts("");continue;}   //���е��������
        for(int j=bytenum-1;j>=0;j--){
            for(int k=31;k>=0;k--)
                if((eline[i].mybyte[j]&(1<<k))!=0){     //һ��������˰�Сʱ�����ǵ���λΪ1ʱ>>�����ڳ�����
                    printf("%d ",j*32+k);
                }
                }
        puts("");
    }
}
int main(){
    long long head, tail, freq;
    QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
    QueryPerformanceCounter((LARGE_INTEGER *)&head);
    dowork();
   QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    cout<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
    //printres();
    system("pause");
    return 0;
}
