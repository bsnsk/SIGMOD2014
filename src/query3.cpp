#include "Query3.h"

#define MAXLINE 1000
#define CHECK(x) (Tree[x].present==true)	//see if x is relevant to current query
#define MIN(a,b) ((a)<(b) ? (a):(b))
static char tmp_dir[MAXLINE];	//for temporary input direction
char buf[MAXLINE];
FILE* OutFile;
char WorkDir[1111];
char QueryFile[1111];
char OutputFile[1111] = "query3.out";
FILE* Myopen(const char* filename)  //wrap functions to open relative direction to input directions
{
	strcpy(tmp_dir,WorkDir);
	if (tmp_dir[strlen(tmp_dir)-1]!='/')
		strcat(tmp_dir, "/");
	strcat(tmp_dir,filename);
	return fopen(tmp_dir,"r");
}

FILE* Myopen(const char* filename, int out)	//wrap functions to open relative direction to input directions for output
{
	//strcpy(tmp_dir,WorkDir);
	strcpy(tmp_dir,filename);
	return fopen(tmp_dir,"w");
}

void init()		//read all the needed input files
{
	read_place_name();
	read_place_belong();
	read_tag();
	read_Person_location();
	read_study_org();
	read_work_org();
	read_org_location();
	read_graph();
}

int forwarding; //query_k
int calc_simtag(const Person_t& a,const Person_t& b)	//calculate the similar tags between two persons in linear time
{
	int i=0,j=0,res=0,best=heap.top().sim;
	if (heap.size()<forwarding)	
	while (i<a.tag.size() && j<b.tag.size())
	{
		if (a.tag[i]==b.tag[j])
			{
				++res;
				++i; ++j;
				continue;
			}
		while (i<a.tag.size() && a.tag[i]<b.tag[j]) ++i;
		if (i==a.tag.size()) continue;
		while (j<b.tag.size() && a.tag[i]>b.tag[j]) ++j;
	}
	else 
	while (i<a.tag.size() && j<b.tag.size())
	{
		if (res+MIN(a.tag.size()-i,b.tag.size()-j) < best) break;	//optimization to avoid impossible answers
		if (a.tag[i]==b.tag[j])
			{
				++res;
				++i; ++j;
				continue;
			}
		while (i<a.tag.size() && a.tag[i]<b.tag[j]) ++i;
		if (i==a.tag.size()) continue;
		while (j<b.tag.size() && a.tag[i]>b.tag[j]) ++j;
	}
	return res;
}



bool operator < (const Ans_t& a, const Ans_t& b)
{
	if (a.sim!=b.sim) return a.sim>b.sim;
	if (a.pers1!=b.pers1) return a.pers1<b.pers1;
	if (a.pers2!=b.pers2) return a.pers2<b.pers2;
	return 0;
}

void query3(int query_k, int query_h,int place_id)
{
	//using dfs time orders to find relevant persons
	forwarding=query_k;
	int l=Tree[place_id].l_time,r=Tree[place_id].r_time;
	for (int i=0; i<Place_count; ++i)
		if (l<=Tree[i].l_time && Tree[i].r_time<=r) 
			Tree[i].present=true;
		else Tree[i].present=false;
	for (int i=0; i<Person.size(); ++i)
	{
		Person[i].present=false;
		if (CHECK(Person[i].loca))
		{
			Person[i].present=true;
			continue;
		}
		for (int j=0; j<Person[i].work_org.size(); ++j)
			if (CHECK(Org[Person[i].work_org[j]]))
			{
				Person[i].present=true;
				break;
			}
		if (Person[i].present) continue;

		for (int j=0; j<Person[i].study_org.size(); ++j)
			if (CHECK(Org[Person[i].study_org[j]]))
			{
				Person[i].present=true;
				break;
			}
		if (Person[i].present) continue;
	}
	
	//Main body to bfs and find the answer
	while (!heap.empty()) heap.pop();
	heap.push(Ans_t(-1,-1,-1));
	memset(visited,-1,sizeof(int) * Person.size());	//visit[k]==person_id means that k is already visited
	
	Person_t now,nxt,pers1;
	int now_id,now_dist,nxt_id,tmp;

	for (int i=0; i<Person.size(); ++i)
	if (Person[i].present)
	{
		pers1=Person[i];
		while (!bfs_q.empty()) bfs_q.pop();
		bfs_q.push(data(i,0));
		visited[i]=i;
		while (!bfs_q.empty())
		{	
			now_id=bfs_q.front().id;
			now_dist=bfs_q.front().dist;
			now=Person[now_id];
			bfs_q.pop();
			for (int k=0; k<now.neighbor.size(); ++k)
			if (visited[now.neighbor[k]]!=i)
			{
				nxt_id=now.neighbor[k];
				nxt=Person[nxt_id];
				visited[nxt_id]=i;
				if (now_dist+1<query_h) bfs_q.push(data(nxt_id,now_dist+1));
				if (nxt.present && i<nxt_id)	//calc top k similar
				{
					tmp=calc_simtag(pers1,nxt);
					if (tmp>heap.top().sim)
					{
						if (heap.size()>=query_k) heap.pop();
						heap.push(Ans_t(i,nxt_id,tmp));
					}
				}
			}
		}
	}
	vector<Ans_t> ans_vec;
	while (!heap.empty())
	{
		ans_vec.push_back(heap.top());
		heap.pop();
	}
	reverse(ans_vec.begin(),ans_vec.end());
	for (int i=0; i<ans_vec.size(); ++i)
		fprintf(OutFile,"%d|%d ",ans_vec[i].pers1,ans_vec[i].pers2);
	fprintf(OutFile,"\n");
}

void query3_entrance()	//Begin to process query3
{
	Person.resize(10000);
	Org.resize(30000);
	int k,h;
	char str_p[MAXLINE];
	visited=new int[Person.size()];
	FILE* inFile=fopen(QueryFile, "r");//Myopen(QueryFile);
	OutFile=Myopen(OutputFile,0);
	while (fgets(buf,MAXLINE,inFile)>0)
	{
		if (buf[5]!='3') continue;
		sscanf(buf,"query3(%d, %d, %[^)]s)",&k,&h,str_p);
		query3(k,h,place[string(str_p)]);
	}
	fclose(OutFile);
	fclose(inFile);
}

void free_mem()
{
	delete Tree;
	delete visited;
}

/*******************Entrance*******************/
int main(int argc, char** argv)
{
	strcpy(WorkDir, argv[1]);
	strcpy(QueryFile, argv[2]);
	init();
//	printf("initialize done...\n");
	query3_entrance();
}


/********************read data and build up the structure*******************/

void read_place_name()
{
	FILE* inFile=Myopen("place.csv");
	char name[MAXLINE];
	int id;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%[^|]",&id,name);
		place[string(name)]=id;
		if (id>=Place_count) Place_count=id;
	}
	++Place_count;
	fclose(inFile);
}

void dfs(Place_index_node* root)
{
	static int dfs_time=0;
	++dfs_time;
	root->l_time=dfs_time;
	for (int i=0; i<root->son.size(); ++i)
		dfs(root->son[i]);
	++dfs_time;
	root->r_time=dfs_time;
}

void read_place_belong()	//read and build_up the index tree of places
{
	Tree=new Place_index_node[Place_count];
	for (int i=0; i<Place_count; ++i) Tree[i]=Place_index_node();
	FILE* inFile=Myopen("place_isPartOf_place.csv");
	int s_place,l_place;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&s_place,&l_place);
		Tree[s_place].fa=&Tree[l_place];
		Tree[l_place].son.push_back(&Tree[s_place]);
	}
	for (int i=0; i<Place_count; ++i)
		if (Tree[i].fa==NULL) 
			Root.son.push_back(&Tree[i]);
	dfs(&Root);
	fclose(inFile);
}

void read_tag() //meanwhile sort the tag of each Person
{
	FILE* inFile=Myopen("person_hasInterest_tag.csv");
	int pers,itag;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&pers,&itag);
		int short_l=pers-Person.size();
		for (int i=0; i<=short_l; ++i)
			Person.push_back(Person_t());
		Person[pers].tag.push_back(itag);
	}
	for (int i=1; i<Person.size(); ++i)
		sort(Person[i].tag.begin(),Person[i].tag.end());
	fclose(inFile);
}

void read_Person_location()
{
	FILE* inFile=Myopen("person_isLocatedIn_place.csv");
	int pers,loca;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&pers,&loca);
		int short_l=pers-Person.size();
		for (int i=0; i<=short_l; ++i)
			Person.push_back(Person_t());
		Person[pers].loca=loca;
	}
	fclose(inFile);
}

void read_study_org()
{
	FILE* inFile=Myopen("person_studyAt_organisation.csv");
	int pers,org;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&pers,&org);
		int short_l=pers-Person.size();
		for (int i=0; i<=short_l; ++i)
			Person.push_back(Person_t());
		Person[pers].study_org.push_back(org);
	}
	fclose(inFile);
}

void read_work_org()
{
	FILE* inFile=Myopen("person_workAt_organisation.csv");
	int pers,org;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&pers,&org);
		int short_l=pers-Person.size();
		for (int i=0; i<=short_l; ++i)
			Person.push_back(Person_t());
		Person[pers].work_org.push_back(org);
	}
	fclose(inFile);
}

void read_org_location()
{
	FILE* inFile=Myopen("organisation_isLocatedIn_place.csv");
	int org,place;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&org,&place);
		int short_l=org-Org.size();
		for (int i=0; i<=short_l; ++i)
			Org.push_back(0);
		Org[org]=place;
	}
	fclose(inFile);
}

void read_graph()
{
	FILE* inFile=Myopen("person_knows_person.csv");
	int per1,per2;
	fscanf(inFile,"%s\n",buf);	//ignore header
	while (fscanf(inFile,"%s\n",buf)>0)
	{
		sscanf(buf,"%d|%d",&per1,&per2);
		Person[per1].neighbor.push_back(per2);
		Person[per2].neighbor.push_back(per1);
	}
	//Person_t nxt=Person[812];
	//printf("%d %d\n",Org[nxt.study_org],Org[nxt.work_org]);
	fclose(inFile);
}
