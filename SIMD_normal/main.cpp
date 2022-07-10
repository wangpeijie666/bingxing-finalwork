#include<iostream>
#include<fstream>
#include<sstream>
#include<windows.h>
#include<stdlib.h>
using namespace std;
//const int id=1,n=130,elinenum=8;
//const int id=2,n=254,elinenum=53;
//const int id=3,n=562,elinenum=53;
const int id=4,n=1011,elinenum=263;
//const int id=5,n=2362,elinenum=453;
//const int id=6,n=3799,elinenum=1953;
//const int id=7,n=8399,elinenum=4535;
//const int id=8,n=23045,elinenum=14325;
//const int id=9,n=37960,elinenum=14921;
//const int id=10,n=43577,elinenum=54274;
//const int id=11,n=85401,elinenum=756;
//�������������������Ԫ����
string str1="D:\\bingxing\\homework-X86\\special\\data\\����Ԫ��4.txt";
string str2="D:\\bingxing\\homework-X86\\special\\data\\��Ԫ��4.txt";
int bytenum=(n+31)/32;
//32λ���һ�飬һ���е�����
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

    void xorbit(bitmap b)
    {
        for(int i=0;i<bytenum;i++)
            //�����е�ÿһλ������������
            bitVector[i]^=b.bitVector[i];
        //������Ԫ
        for(int i=bytenum-1;i>=0;i--)
            for(int j=31;j>=0;j--)
                if((bitVector[i]&(1<<j))!=0)
                {
                    //32��һ����ң���32�ڲ�һ��һ������
                    mycol=i*32+j;
                    //�ҵ�1���¼��Ԫλ�ü���
                    return;
                }
        mycol=-1;
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
void S_gauss_normal(){
    //�����˹�����㷨
    for(int i=0;i<elinenum;i++)
    {
        while(!eline[i].isnull())
        {  //�������Ԫ�зǿ�
            bool flag=eliminator[eline[i].getcol()].isnull();
            //���ҵ�����Ԫ�е�������Ƿ���ڶ�Ӧ����Ԫ��
            if(!flag)
                //�ǿա���������ڵ����
            {
                eline[i].xorbit(eliminator[eline[i].getcol()]);

            }
            else
            {
                eliminator[eline[i].getcol()]=eline[i];
                 //����Ԫ������Ϊ��Ԫ��
                break;
            }
        }
    }
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
    S_gauss_normal();
    QueryPerformanceCounter((LARGE_INTEGER *)&tail);
    cout<<"��������"<<id<<" ���������� "<<n<<" ����Ԫ�и�����"<<elinenum<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
   // printmatrix();
    system("pause");
    return 0;
}
