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

char data_file_name[2000];

class edge//图中边的类
{
public:
	int y;

	edge(){y=-1;}
	edge(int yy):y(yy){};
};


int N_PEO=0;//人数

class query//每个query2的信息
{
public:
	int num;
	int date;
	int ori;

	query(){}
	query(int nn,int dd,int oo){num=nn;date=dd;ori=oo;}
};

int operator<(const query & a,const query & b)
{
	return a.date>b.date;
}

class person//每个人的信息
{
public:
	int num;
	int birthday;
	vector<int> interest;
	person(){birthday=0;}
};
int operator<(const person & a,const person & b)
{
	return a.birthday > b.birthday;
}

vector<edge> s[10001];//关系图

vector<query> ask;//询问
vector<int> ans[5005];

person per[10010];//人的信息
string tagname[30005]={};//tag的名称
int s2per[10010]={};//sort之后与原先的对应
int num_tag=0;//tag的数目

vector<int> ever;//被扩充过的tag的名称
int vever[30005]={};

inline void init_query2(char ** argv,int f)//query2的初始化
{
	long long len_pkp=0;
	long long len_per=0;
	long long len_pht=0;
	long long len_tag=0;
	int fd_pkp=0;
	int fd_per=0;
	int fd_pht=0;
	int fd_tag=0;
	char * pkp;
	char * peo;
	char * pht;
	char * tag;

	//读入person_knows_person.csv
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

	//读入person.csv
	{
	FILE * p=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) p=fopen(strcat(data_file_name,"person.csv"),"r");
	else p=fopen(strcat(data_file_name,"/person.csv"),"r");
	if(p==NULL)
	{
		printf("Can't open source dataper\n");
		exit(0);
	}
	fseek(p,0,SEEK_END);
	len_per=ftell(p);

	strcpy(data_file_name, argv[2]);
	if(f) fd_per=open(strcat(data_file_name,"person.csv"),O_RDONLY,0);
	else fd_per=open(strcat(data_file_name,"/person.csv"),O_RDONLY,0);
	peo=(char *)mmap(NULL,len_per,PROT_READ,MAP_PRIVATE,fd_per,0);
	char * rest=peo;
	char * end =peo+len_per;

	while( !( ((*peo)<='9')&&((*peo)>='0') ) ) ++peo;

	int id=0;
	int birthday=0;
	int past=0;
	int nameflag=1,birflag=0;

	while(peo<end)
	{
		if(nameflag==1)
		{
			while((*peo)<='9'&&(*peo)>='0') {id=id*10+(*peo)-'0';++peo;}
			nameflag=0;
		}
		if(birflag==1)
		{
			while((*peo)<='9'&&(*peo)>='0')
			{
				birthday=birthday*10+(*peo)-'0';
				++peo;
			}
			++peo;
			while((*peo)<='9'&&(*peo)>='0')
			{
				birthday=birthday*10+(*peo)-'0';
				++peo;
			}
			++peo;
			while((*peo)<='9'&&(*peo)>='0')
			{
				birthday=birthday*10+(*peo)-'0';
				++peo;
			}

			per[id].birthday=birthday;
			per[id].num=id;
			birflag=0;
		}

		if(*peo=='|') {++past;if(past==4) birflag=1;}
		if(*peo=='\n')  {past=0;nameflag=1;birflag=0;id=0;birthday=0;++N_PEO;}

		++peo;
	}
	munmap(rest,len_per);
	}

	//读入person_hasInterest_tag.csv
	{
	FILE * p=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) p=fopen(strcat(data_file_name,"person_hasInterest_tag.csv"),"r");
	else p=fopen(strcat(data_file_name,"/person_hasInterest_tag.csv"),"r");
	if(p==NULL)
	{
		printf("Can't open source datapht\n");
		exit(0);
	}
	fseek(p,0,SEEK_END);
	len_pht=ftell(p);

	strcpy(data_file_name, argv[2]);
	if(f) fd_pht=open(strcat(data_file_name,"person_hasInterest_tag.csv"),O_RDONLY,0);
	else  fd_pht=open(strcat(data_file_name,"/person_hasInterest_tag.csv"),O_RDONLY,0);
	pht=(char *)mmap(NULL,len_pht,PROT_READ,MAP_PRIVATE,fd_pht,0);
	char * rest=pht;

	char * end=pht+len_pht;

	for(;(*pht)<'0'||(*pht)>'9';++pht);

	int sum=0;
	char temp;
	int id=0;
	while( pht<end )
	{
		temp=(*pht);

		if(temp>='0'&&temp<='9') {sum=sum*10+temp-'0';}
		else if(temp=='|')  {id=sum;sum=0;}
		else if(temp=='\n') {per[id].interest.push_back(sum);sum=0;}
		++pht;
	}
	munmap(rest,len_pht);
	}

	sort(per,per+N_PEO);
	for(int k=0;k<N_PEO;++k)
		s2per[per[k].num]=k;
	for(int k=0;k<N_PEO;++k)
		sort(per[k].interest.begin(),per[k].interest.end());


	//读入tag.csv 得到名称
	{
	FILE * t=NULL;
	strcpy(data_file_name, argv[2]);
	if(f) t=fopen(strcat(data_file_name,"tag.csv"),"r");
	else t=fopen(strcat(data_file_name,"/tag.csv"),"r");
	if(t==NULL)
	{
		printf("Can't open source datatag\n");
		exit(0);
	}
	fseek(t,0,SEEK_END);
	len_tag=ftell(t);

	strcpy(data_file_name, argv[2]);
	if(f) fd_tag=open(strcat(data_file_name,"tag.csv"),O_RDONLY,0);
	else fd_tag=open(strcat(data_file_name,"/tag.csv"),O_RDONLY,0);
	tag=(char *)mmap(NULL,len_tag,PROT_READ,MAP_PRIVATE,fd_tag,0);
	char * rest=tag;

	char * end=tag+len_tag;

	for(;(*tag)!='\n';++tag);++tag;

	int sum=0;
	char temp[205]={};
	while( tag<end )
	{
		while( (*tag)<='9' && (*tag)>='0' ) {sum=sum*10+*tag-'0'; ++tag;}
		++tag;
		for(int k=0;*tag!='|';++k,++tag)
		{
			temp[k]=*tag;
		}
		tag+=25;
		while( (*tag)!='\n') ++tag;++tag;

		tagname[sum]=temp;
		if(sum>num_tag) num_tag=sum;
		memset(temp,0,sizeof(temp));
		sum=0;
	}
	munmap(rest,len_tag);
	}
}

class answer//每个回答的类
{
public:
	int size;
	int ori;
	answer(){};
	answer(int ss,int oo):size(ss),ori(oo){};
};

int operator<(const answer & a,const answer & b)
{
	if(a.size==b.size) return tagname[a.ori]<tagname[b.ori];
	return a.size > b.size;
}

priority_queue<answer> a;
int total_everyquery2=0;

class Disjoint//并查集
{
public:
	int num;
	int n;
	int max;
	vector<int> father;
	int trans[10005];
	vector<int> size;

	Disjoint()
	{
		max=-1234567;
		n=0;
		father.clear();
		father.push_back(0);
		memset(trans,0,sizeof(trans));
		size.clear();
		size.push_back(0);
	}

	int find(int x)
	{
		if(x==father[x]) return x;

		int temp=find(father[x]);
		father[x]=temp;
		return temp;
	}

	void init(int x)
	{
		if(trans[x]!=0) return ;

		++n;
		trans[x]=n;
		size.push_back(1);
		father.push_back(n);

		if(size[n]>max)
		{
			max=size[n];
		}
	}

	void insert(int x,int y)
	{
//		if(trans[x]==0) init(x);
		if(trans[y]==0) init(y);
		x=trans[x];y=trans[y];

		int fx=find(x),fy=find(y);

		if(fx!=fy)
		{
			father[fx]=fy;
			size[fy]+=size[fx];

			if(size[fy] > max)
			{
				max=size[fy];
			}
		}
	}
};

Disjoint eachtag[20005];

inline void check(int x,int y)//check两人是否有相同爱好
{
	vector<int>::iterator p1,q1,p2,q2;
	p1=per[x].interest.begin();q1=per[x].interest.end();
	p2=per[y].interest.begin();q2=per[y].interest.end();

	while((p1!=q1) && (p2!=q2))
	{
		if(*p1<*p2) ++p1;
		else if(*p1>*p2) ++p2;
		else
		{
			if(!vever[*p1]) {ever.push_back(*p1);vever[*p1]=1;}
			eachtag[*p1].insert(x,y);
			++p1;++p2;
		}
	}
}

inline void query2(int num_query2)//对query2的处理
{
	for(int k=0;k<=num_tag;++k)
		eachtag[k].num=k;

	vector<query>::iterator p,q;
	int now=0;
	for(p=ask.begin(),q=ask.end();p!=q;++p)//对每个询问
	{
		total_everyquery2=(*p).num;

		for(;per[now].birthday>=(*p).date;++now)//对于符合条件的每个人
		{
			vector<int>::iterator h = per[now].interest.begin();
			vector<int>::iterator r = per[now].interest.end();
			for(;h!=r;++h)
			{
				eachtag[*h].init(now);
				if(!vever[*h]) {ever.push_back(*h);vever[*h]=1;}
			}

			vector<edge>::iterator k,end;
			int temp=per[now].num;

			for(k=s[temp].begin(),end=s[temp].end();k!=end;++k)
			{
				if(per[s2per[(*k).y]].birthday>=(*p).date)
				{
					check(s2per[temp],s2per[(*k).y]);//扫描有边相连的人
				}
			}
		}

		vector<int>::iterator h=ever.begin(),r=ever.end();
		for(;h!=r;++h)//统计最大的若干个
		{
			int k = *h;

			int mymax=eachtag[k].max;
		    if(a.size()<total_everyquery2 && tagname[k].size() != 0) a.push(answer(mymax,k));
            else
            {
               	if(mymax>a.top().size && tagname[k].size() != 0)
                   	{
               			a.pop();
               			a.push(answer(mymax,k));
                   	}
               	else if(mymax==a.top().size && tagname[k].size() != 0)
               		{
               			if(tagname[k]<tagname[a.top().ori])
               			{
               			a.pop();
               			a.push(answer(mymax,k));
               			}
               		}
            }
        }

		while( a.size()>0 )//整理
		{
			answer temp=a.top();a.pop();
			ans[(*p).ori].push_back(temp.ori);
		}
	}

	for(int k=0;k<=num_query2;++k)//对应输出
	{
		vector<int>::iterator pp,qq;
		for(pp=ans[k].begin(),qq=ans[k].end()-1;pp<=qq;--qq)
		{
			printf("%s ",tagname[*qq].c_str());
		}
		printf("\n");
	}
}

int main (int argc,char ** argv){
	int flag=0;

	int unused=0;//used to make gcc quiet

	FILE * unused2;

	unused2=freopen(argv[1],"r",stdin);

	if(argv[2][strlen(argv[2])-1] == '/') flag=1;
	else flag=0;


	init_query2(argv,flag);

	unused2=freopen("query2.out","w",stdout);

	int num;
	int year,month,day;
	int num_query2=-1;

    while(scanf("query2(%d, %d-%d-%d)\n",&num,&year,&month,&day) == 4)//读入询问
    {
    	ask.push_back(query(num,year*10000+month*100+day,++num_query2));
    }

    sort(ask.begin(),ask.end());

    query2(num_query2);//处理

    fclose(stdout);
    fclose(stdin);
    return 0;
}
