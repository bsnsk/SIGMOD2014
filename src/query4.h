/*************************************************************************
    > File Name: query4.h
    > Author: Ivan
    > Created Time: Thu 11/20 08:42:54 2014
 ************************************************************************/

#ifndef QUERY4

#define QUERY4

#include "common.h"

using namespace std;

// ================ file/dir =================
int dir_name_len;
int file_name_max_len;
char *input_file_name; // query file
char *dir_name;
char *data_file_name;

// ================ persons =================
int personcnt;
char *person_in_induced_graph;

// ================ tags =================
int tagcnt;
unordered_map <string, int> tag_name2id;
unordered_map <int, vector <int> *> tag_to_forum;

// ================ queries =================
vector < pair<int, int> > queries; // k, t

// ================ forums =================

int forumcnt;
#if HASH_HANDWRITE
class _unordered_map forums;
#else
unordered_map <int, struct forum> forums;
#endif

// ================ graph =================
vector < vector <int> * > edges_to_pre;
vector <struct edge *> edges_from;
vector <struct edge *> edges_to;

// ================ basic funcs =================
void cnt_data_lines(const char *file_name, int *cnt);

void read_queries();
void read_forums();
void read_tags();
void read_edges();
void convert2undirected();
void erase_edges(struct edge *e);
void add_edge(int u, int v);

void free_memory();
#endif
