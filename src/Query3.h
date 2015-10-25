/*
* Header file for query3.cpp
*/
#ifndef Query3

#define Query3

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <map>

using namespace std;

/****************Const definition****************/
/*#ifdef QUERY_1K
char* WorkDir="E:\\Courses\\2014_fall\\数算实习\\SIGMOD大作业\\testdata\\outputDir-1k\\outputDir-1k\\";
char* QueryFile="1k-queries.txt";
#endif

#ifdef QUERY_10K
char* WorkDir= "E:\\Courses\\2014_fall\\数算实习\\SIGMOD大作业\\testdata\\outputDir-10k\\";
char* QueryFile="10k-queries.txt";
#endif

const char* OutputFile="Result.txt";
*/
/*****************Data Structure*****************/
map <string, int > place;

struct Person_t
{
	vector<int> tag;
	vector<int> neighbor;
	vector<int> study_org,work_org; //study at orgnazition, work at orgnazition
	int loca;	//location
	bool present;	//if person is at current query place
	Person_t():loca(-1){}
};
vector<Person_t> Person;
vector<int> Org;

struct Place_index_node
{
	int l_time,r_time;	//for each node, record the time arrived and time leaved when dfs;
	Place_index_node* fa;	//father node
	vector <Place_index_node*> son;
	bool present;	//if this place belongs to the current query place
	Place_index_node ():l_time(0),r_time(0),fa(NULL){}
};	//Index of place

int* visited;
struct data
{
	int id,dist;
	data (int _id, int _dist):id(_id),dist(_dist){};
};

queue<data> bfs_q;	//queue for bfs
struct Ans_t	//tuple for ans
{
	int pers1,pers2,sim;
	Ans_t (int _pers1, int _pers2, int _sim):pers1(_pers1),pers2(_pers2),sim(_sim){};
};
priority_queue<Ans_t> heap;	//min-heap to store the k biggest pair

/******************Global Variable**************/
int Place_count=0;	//total number of place
Place_index_node Root=Place_index_node();	//One super root node to be at top
Place_index_node *Tree;		//Start of all the tree node

/*********************Functions*****************/
void read_place_name();
void read_place_belong();

void read_tag();

void read_Person_location();
void read_study_org();
void read_work_org();
void read_org_location();

void read_graph();
#endif
