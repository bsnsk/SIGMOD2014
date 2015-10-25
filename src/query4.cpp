/*************************************************************************
 > File Name: query4.cpp
 > Author: Ivan
 > Created Time: Thu 11/20 08:36:54 2014
 ************************************************************************/

#include "query4.h"

extern void query(int k, int t);

int _unordered_map :: MOD = 0;

// query4 $inputfile $datapath
int main(int argc, char *argv[]){
	
	// initialization
	input_file_name = argv[1];
	dir_name = argv[2];
	dir_name_len = (int)strlen(argv[2]);
	file_name_max_len = dir_name_len + 50;
	data_file_name = new char [file_name_max_len];
	
	cnt_data_lines("person.csv", &personcnt);
	cnt_data_lines("forum.csv", &forumcnt);
	cnt_data_lines("tag.csv", &tagcnt);
	person_in_induced_graph = new char[personcnt];

#if DEBUG
	printf("cnt done.\n");
#endif
	read_edges();
	convert2undirected();
#if DEBUG
	printf("start forums.\n");
#endif
	read_forums();
	read_tags();
	
	read_queries();
#if DEBUG
	printf("read Q done\n");
#endif
	
	// solve
	freopen("query4.out", "w", stdout);
	for (vector < pair<int, int> > :: iterator it=queries.begin(); it!=queries.end(); it++)
		query(it->first, it->second);
	fclose(stdout);
	
	free_memory();
	return 0;
}

void read_queries(){
	char st[256], *ptr;
	int k, t;
	FILE *in = fopen(input_file_name, "rb");
	if (!in){
		fprintf(stderr, "Unable to open Query File!\n");
		exit(0);
	}
	while (!feof(in)){
		if (fgets(st, 256, in)<=0)
			break;
		if (st[0]=='q' && st[5]!='4') continue;
		sscanf(st, "query4(%d,", &k);
		for (ptr=st+7; *ptr!=' '; ptr++);
		if (st[strlen(st)-1]=='\n')
			st[strlen(st)-1]=0;
		st[strlen(st)-1]=0;
		t = tag_name2id[string(ptr+1)];
		queries.push_back(make_pair(k, t));
		//fprintf(stderr, "\tQ: %d %s(%d)\n", k, (string(ptr+1)).c_str(), t);
	}
	fclose(in);
}

void read_tags(){
	FILE *in;
	char st[128];
	int id, len;
	long file_len;
	strcpy(data_file_name, dir_name);
	if (data_file_name[strlen(data_file_name)-1]=='/')
		strcat(data_file_name, "tag.csv");
	else 
		strcat(data_file_name, "/tag.csv");
	in = fopen(data_file_name, "r");
	fseek(in, 0, SEEK_END);
	file_len = ftell(in);
	fclose(in);
	int fd = open(data_file_name, O_RDONLY, 0);
	void * start = mmap(NULL, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	char * ptr = (char *) start, * end = (char *)start + file_len;
	while (isspace(*ptr)) ptr++;
	while (!isspace(*ptr)) ptr++;
	while (ptr < end){
		while ((++ptr) < end && !isdigit(*ptr));
		if (ptr == end) break;
		for (id = *ptr-'0'; ++ptr, isdigit(*ptr); )
			id = id * 10 + (*ptr) - '0';
		for (len=0; *(++ptr) != '|'; )
			st[len++] = *ptr;
		st[len] = 0;
		tag_name2id[string(st)] = id;
	//	fprintf(stderr, "\ttag $%s$%d\n", st, id);
		while (++ptr < end && *ptr!='\n');
	}
	munmap(start, file_len);
#if DEBUG
	fprintf(stderr, "tags read.\n");
#endif
}

void read_forums(){
#if DEBUG
	int start_time = clock();
#endif
	FILE *in;
	int f, x;
	forums.reserve(forumcnt);
	
	long file_len;
	strcpy(data_file_name, dir_name);
	if (data_file_name[strlen(data_file_name)-1]=='/')
		strcat(data_file_name, "forum_hasTag_tag.csv");
	else
		strcat(data_file_name, "/forum_hasTag_tag.csv");
	in = fopen(data_file_name, "r");
	fseek(in, 0, SEEK_END);
	file_len = ftell(in);
	fclose(in);
	
	int fd = open(data_file_name, O_RDONLY, 0);
	void * start = mmap(NULL, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	char * ptr = (char *) start, * end = (char *)start + file_len;
	
	while (isspace(*ptr)) ptr++;
	while (!isspace(*ptr)) ptr++;
	while (ptr < end){
		while ((++ptr) < end && !isdigit(*ptr));
		if (ptr == end) break;
		for (f = *ptr - '0'; isdigit(*(++ptr)); )
			f = f * 10 + (*ptr) - '0';
		for (x = 0; isdigit(*(++ptr)); )
			x = x * 10 + (*ptr) - '0';
	//	fprintf(stderr, "forum|tag: %d|%d\n", f, x);
		while (ptr < end && *ptr != '\n') ptr++;
		if (tag_to_forum.find(x) == tag_to_forum.end())
			tag_to_forum[x] = new vector <int>;
#if HASH_HANDWRITE
		forums.insert(f);
#endif
		tag_to_forum[x]->push_back(f);
	}
	munmap(start, file_len);

#if DEBUG
	printf("GAP\n");
#endif

	strcpy(data_file_name, dir_name);
	if (data_file_name[strlen(data_file_name)-1] == '/')
		strcat(data_file_name, "forum_hasMember_person.csv");
	else 
		strcat(data_file_name, "/forum_hasMember_person.csv");
	in = fopen(data_file_name, "r");
	fseek(in, 0, SEEK_END);
	file_len = ftell(in);
	fclose(in);

	fd = open(data_file_name, O_RDONLY, 0);
	start = mmap(NULL, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	ptr = (char *)start;
	end = (char *)start + file_len;

	while (isspace(*ptr)) ptr++;
	while (!isspace(*ptr)) ptr++;
	while (ptr < end){
		while ((++ptr) < end && !isdigit(*ptr));
		if (ptr == end) break;
		for (f = *ptr - '0'; isdigit(*(++ptr)); )
			f = f * 10 + (*ptr) - '0';
		for (x = 0; isdigit(*(++ptr)); )
			x = x * 10 + (*ptr) - '0';
		forums[f].persons.push_back(x);
		while (ptr < end && *ptr != '\n') ptr++;
	}
	munmap(start, file_len);
#if DEBUG
	printf("%lu forums\n", forums.size());
	fprintf(stderr, "forums read. %.2lf sec(s) \n", (clock()-start_time)*1. / CLOCKS_PER_SEC);
#endif
}

void erase_edges(struct edge *e) {
	if (!e)
		return;
	erase_edges(e->nxt);
	delete e;
}

void add_edge(int u, int v){
	struct edge *p = new struct edge(v, edges_from[u]);
	struct edge *q = new struct edge(v, edges_to[u]);
	edges_from[u] = p;
	edges_to[u] = q;
	p = new edge(u, edges_from[v]);
	q = new edge(u, edges_to[v]);
	edges_from[v] = p;
	edges_to[v] = q;
}

void convert2undirected(){
	edges_from.resize(personcnt, NULL);
	edges_to.resize(personcnt, NULL);
	
	for (int i=0; i<personcnt; i++)
		for (vector <int>::iterator it=edges_to_pre[i]->begin(); it!=edges_to_pre[i]->end(); it++)
			if (*it > i && binary_search(edges_to_pre[*it]->begin(), edges_to_pre[*it]->end(), i))
				add_edge(i, *it);
	
	for (int i=0; i<personcnt; i++)
		delete edges_to_pre[i];
	edges_to_pre.clear();
}

void read_edges(){
	edges_to_pre.resize(personcnt);
	for (int i=0; i<personcnt; i++)
		edges_to_pre[i] = new vector <int>;
	
	strcpy(data_file_name, dir_name);
	if (data_file_name[strlen(data_file_name)-1] == '/')
		strcat(data_file_name, "person_knows_person.csv");
	else
		strcat(data_file_name, "/person_knows_person.csv");
	FILE *in = fopen(data_file_name, "r");
	fseek(in, 0, SEEK_END);
	long file_len = ftell(in);
	fclose(in);

	int fd = open(data_file_name, O_RDONLY, 0);
	void * start = mmap(NULL, file_len, PROT_READ, MAP_PRIVATE, fd, 0);
	char * ptr = (char *) start, * end = (char *)start + file_len;

	int p, q;
	while (*ptr != '\n') ptr++;
	while (ptr < end){
		while ((++ptr) < end && !isdigit(*ptr));
		if (ptr == end) break;
		for (p = *ptr - '0'; isdigit(*(++ptr)); )
			p = p * 10 + (*ptr) - '0';
		for (q = 0; isdigit(*(++ptr)); )
			q = q * 10 + (*ptr) - '0';
		edges_to_pre[q]->push_back(p);
	}
	munmap(start, file_len);
	for (int i=0; i<personcnt; i++){
		sort(edges_to_pre[i]->begin(), edges_to_pre[i]->end());
		unique(edges_to_pre[i]->begin(), edges_to_pre[i]->end());
	}
#if DEBUG
	fprintf(stderr, "edges read.\n");
#endif
}

void cnt_data_lines(const char *file_name, int *cnt){
	strcpy(data_file_name, dir_name);
	if (data_file_name[strlen(data_file_name)-1] == '/')
		strcat(data_file_name, file_name);
	else {
		strcat(data_file_name, "/");
		strcat(data_file_name, file_name);
	}
	FILE *in = fopen(data_file_name, "rb");
	if (!in) {
		printf("Unable to open file: %s\n", data_file_name);
		exit(0);
	}
	fseek(in, 0, SEEK_END);
	long len = ftell(in);
	fclose(in);
	int fd = open(data_file_name, O_RDONLY, 0);
	void * start = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd, 0);
	char * ptr = (char *) start, * end = (char *)start + len;
	int _cnt;
	for (_cnt = 0; ptr < end; ptr++)
		if (*ptr == '\n')
			_cnt ++;
	
	*cnt = _cnt-1;
	munmap(start, len);
}

void free_memory(){
	for (int i=0; i<personcnt; i++){
		erase_edges(edges_from[i]);
		erase_edges(edges_to[i]);
	}
	delete person_in_induced_graph;
	for (unordered_map <int, vector <int> * >::iterator it=tag_to_forum.begin(); it!=tag_to_forum.end(); ++it)
		delete it->second;
}

