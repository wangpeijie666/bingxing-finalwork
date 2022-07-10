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
//给定矩阵的列数、被消元行数
string str1="D:\\bingxing\\homework-X86\\special\\data\\被消元行4.txt";
string str2="D:\\bingxing\\homework-X86\\special\\data\\消元子4.txt";
int bytenum=(n+31)/32;
//32位组成一组，一共有的组数
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

    void xorbit(bitmap b)
    {
        for(int i=0;i<bytenum;i++)
            //对两行的每一位都进行异或操作
            bitVector[i]^=b.bitVector[i];
        //更新首元
        for(int i=bytenum-1;i>=0;i--)
            for(int j=31;j>=0;j--)
                if((bitVector[i]&(1<<j))!=0)
                {
                    //32个一组地找，在32内部一个一个的找
                    mycol=i*32+j;
                    //找到1后记录首元位置即可
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
void S_gauss_normal(){
    //特殊高斯串行算法
    for(int i=0;i<elinenum;i++)
    {
        while(!eline[i].isnull())
        {  //如果被消元行非空
            bool flag=eliminator[eline[i].getcol()].isnull();
            //先找到被消元行的首项，看是否存在对应的消元子
            if(!flag)
                //非空。即如果存在的情况
            {
                eline[i].xorbit(eliminator[eline[i].getcol()]);

            }
            else
            {
                eliminator[eline[i].getcol()]=eline[i];
                 //被消元行升格为消元子
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
    cout<<"测试用例"<<id<<" 矩阵列数： "<<n<<" 被消元行个数："<<elinenum<<" time: "<<(tail-head)*1000.0 / freq<<"ms"<<endl;
   // printmatrix();
    system("pause");
    return 0;
}
