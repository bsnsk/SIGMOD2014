#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <fcntl.h>

using namespace std;

#define MIN(a,b) (a<b? a:b)
#define MAX(a,b) (a>b? a:b)


char data_file_name[2000];//文件路径

class edge//图中的边
{
public:
	int y;

	edge(){y=-1;}
	edge(int yy):y(yy){};
};
int peo_peo[10005][10005]={};
vector<edge> s[10001];
short com_peo[280000000]={};

inline void init_query1(char ** argv,int f)//query1的初始化,包括读入,预处理
{
	long long len_pkp=0;//文件长度
	long long len_ccp=0;
	long long len_crc=0;
	int fd_pkp=0;//打开的文件的fd
	int fd_ccp=0;
	int fd_crc=0;
	char * pkp;
	char * ccp;
	char * crc;

	//读入comment_hasCreator_person.csv
	{
	FILE * c=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) c=fopen(strcat(data_file_name,"comment_hasCreator_person.csv"),"r");
	else c=fopen(strcat(data_file_name,"/comment_hasCreator_person.csv"),"r");
	if(c==NULL)
	{
		printf("Can't open source dataccp\n");
		exit(0);
	}
	fseek(c,0,SEEK_END);
	len_ccp=ftell(c);

	strcpy(data_file_name, argv[2]);
	if(f) fd_ccp=open(strcat(data_file_name,"comment_hasCreator_person.csv"),O_RDONLY,0);
	else   fd_ccp=open(strcat(data_file_name,"/comment_hasCreator_person.csv"),O_RDONLY,0);
	ccp=(char *)mmap(NULL,len_ccp,PROT_READ,MAP_PRIVATE,fd_ccp,0);
	char * rest=ccp;

	char * end=ccp+len_ccp;

	for(;(*ccp)<'0'||(*ccp)>'9';++ccp);

	int com=0;
	int sum=0;
	char temp;
	while( ccp<end )
	{
		temp=(*ccp);

		if(temp>='0'&&temp<='9') {sum=sum*10+temp-'0';}
		else if(temp=='|')  {com=sum;sum=0;}
		else if(temp=='\n') {/*peo_com[sum]=com;*/com_peo[com]=sum;sum=0;}
		++ccp;
	}
	munmap(rest,len_ccp);
	}

	//读入comment_replyOf_comment.csv,完成回复数量的统计
	{
	FILE * cc=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) cc=fopen(strcat(data_file_name,"comment_replyOf_comment.csv"),"r");
	else cc=fopen(strcat(data_file_name,"/comment_replyOf_comment.csv"),"r");
	if(cc==NULL)
	{
		printf("Can't open source datacrc\n");
		exit(0);
	}
	fseek(cc,0,SEEK_END);
	len_crc=ftell(cc);

	strcpy(data_file_name, argv[2]);
	if(f)fd_crc=open(strcat(data_file_name,"comment_replyOf_comment.csv"),O_RDONLY,0);
	else fd_crc=open(strcat(data_file_name,"/comment_replyOf_comment.csv"),O_RDONLY,0);
	crc=(char *)mmap(NULL,len_crc,PROT_READ,MAP_PRIVATE,fd_crc,0);
	char * rest=crc;

	int x;
	char * end=crc+len_crc;
	for(;(*crc)<'0'||(*crc)>'9';++crc);

	int sum=0;
	char temp;
	while( crc<end )
	{
		temp=(*crc);
		if(temp>='0'&&temp<='9') {sum=sum*10+temp-'0';}
		else if(temp=='|')  {x=sum;sum=0;}
		else if(temp=='\n')	{++peo_peo[com_peo[x]][com_peo[sum]];sum=0;}
		++crc;
	}
	munmap(rest,len_crc);
	}

	//读入person_knows_person.csv 建图
	{
	FILE * p=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) p=fopen(strcat(data_file_name,"person_knows_person.csv"),"r");
	else p=fopen(strcat(data_file_name,"/person_knows_person.csv"),"r");
	if(p==NULL)
	{
		printf("Can't open source datapkp\n");
		exit(0);
	}
	fseek(p,0,SEEK_END);
	len_pkp=ftell(p);

	strcpy(data_file_name, argv[2]);
	if(f)fd_pkp=open(strcat(data_file_name,"person_knows_person.csv"),O_RDONLY,0);
	else fd_pkp=open(strcat(data_file_name,"/person_knows_person.csv"),O_RDONLY,0);
	pkp=(char *)mmap(NULL,len_pkp,PROT_READ,MAP_PRIVATE,fd_pkp,0);
	char * rest=pkp;

	int x;
	char * end=pkp+len_pkp;

	for(;(*pkp)<'0'||(*pkp)>'9';++pkp);

	int sum=0;
	char temp;
	while( pkp<end )
	{
		temp=*pkp;
		if(temp>='0'&&temp<='9') {sum=sum*10+temp-'0';}
		else if(temp=='|') {x=sum;sum=0;}
		else if(temp=='\n')
		{
		s[x].push_back( edge(sum) );
		s[sum].push_back( edge(x) );
		sum=0;
		}
		++pkp;
	}
	munmap(rest,len_pkp);
	}
}

inline void query1(const int  & x,const int & y,const int & m)//query1的处理
{
	if(x==y) {printf("0\n");return ;}

	int shortest_dis=-1;

	int size1=0,size2=0;
	int step=0;
	bool v1[10001]={};//visited
	bool v2[10001]={};
	queue<int> a1;
	queue<int> a2;

	int now;

	a1.push(x);a2.push(y);
	++size1;++size2;
	v1[x]=1;v2[y]=1;


	while(step<=7)//双向广搜
	{
		vector<edge>::iterator p,q;

		if(size1<=size2)//选择小的一边
		{
			++step;
			now=size1;
			size1=0;

			for(int k=1,temp;k<=now;++k)
			{
				temp=a1.front();a1.pop();

				if(!v2[temp])//仍未找到
				{
					p=s[temp].begin();q=s[temp].end();

					for(;p!=q;++p)
					{
					if(!v1[(*p).y])
					{	if( peo_peo[temp][(*p).y] > m )
						{
							if( peo_peo[(*p).y][temp] > m )
							{
								a1.push( (*p).y );++size1;
								v1[(*p).y]=1;
							}
						}
					}

					}
				}
				else//已找到
				{
					shortest_dis=step;
					step=100;
					break;
				}
			}
		}

		else//对另一侧的扩展
		{
			++step;
			now=size2;
			size2=0;

			for(int k=1,temp;k<=now;++k)
			{
				temp=a2.front();a2.pop();

				if(!v1[temp])
				{
					p=s[temp].begin();q=s[temp].end();

					for(;p!=q;++p)
					{
					if(!v2[(*p).y])
					{
						if( peo_peo[temp][(*p).y] > m )
						{
							if( peo_peo[(*p).y][temp] > m )
							{
								a2.push( (*p).y );++size2;
								v2[(*p).y]=1;
							}
						}
					}

					}
				}
				else
				{
					shortest_dis=step;
					step=100;
					break;
				}
			}
		}
	}

	printf("%d\n",shortest_dis==-1?-1:shortest_dis-1);
}

int main (int argc,char ** argv){
	int flag=0;

	FILE * unused2;

	unused2=freopen(argv[1],"r",stdin);

	if(argv[2][strlen(argv[2])-1] == '/') flag=1;
	else flag=0;

	init_query1(argv,flag);
	int x,y,m;

	unused2=freopen("query1.out","w",stdout);//读入询问
	while(scanf("query1(%d, %d, %d)\n",&x,&y,&m) == 3 )
	{
		query1(x,y,m);//逐个处理
	}

	fclose(stdout);
    fclose(stdin);
    return 0;
}
