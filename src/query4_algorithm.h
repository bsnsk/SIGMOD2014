/*************************************************************************
    > File Name: query4_algorithm.h
    > Author: 
    > Created Time: 四 11/20 15:55:31 2014
 ************************************************************************/

#ifndef QUERY4_ALG

#define QUERY4_ALG

#include "common.h"

using namespace std;

#define SET_LENGTH ((i_n+31)/32)

#define ABS(x) ((x)>0?(x):-(x))
#define eps 1e-6
#define double_sqr(x) (((double)(x)) * (x))

#define max63 1061109567

#ifdef MORE_PRUNNING
/* Parameter for initialization */
#define lambda 3
#define SUBSET 5
#endif

/* Levels for approximation */
#define L 2

// ================ persons =================
extern int personcnt;
extern char *person_in_induced_graph;

// ================ tags =================
extern int tagcnt;
extern unordered_map <string, int> tag_name2id;
extern unordered_map <int, vector <int> *> tag_to_forum;

// ================ forums =================
extern int forumcnt;
#if HASH_HANDWRITE
extern class _unordered_map forums;
#else
extern unordered_map <int, struct forum> forums;
#endif

// ================ graph =================
extern vector < vector <int> * > edges_to_pre;
extern vector <struct edge *> edges_from;
extern vector <struct edge *> edges_to;

extern void erase_edges(struct edge *e);

// ====================================

static vector <struct edge*> i_edges_from;
static vector <struct edge*> i_edges_to;

void i_add_edge(int u, int v);

// prefix "i_" means it is related to the induced graph
// |V_induced|
static int i_n; 

// store the original id of the vertices in the induced graph
static vector <int> vertex_id; 

static vector <int> id_2_subid;

// current levels for PFS
static int * cur_lvls;

// actual reachable (only number matters)
static vector <int> * reachable_num[L+1];

// sigma d(v, v'), upper_bound
static vector <int> _sigma; 

// sigma d(v, v')
static vector <int> sigma;

// centrality c(v) estimated
static vector <double> _centrality;

// centrality c(v)
static vector <double> centrality;

static vector <int> fa;

// candidate pair <flag, num>
typedef pair <bool, int> candidate_pair;
static inline double get_candidate_centrality(const candidate_pair &c){
	return c.first ? centrality[c.second] : _centrality[c.second];
}

class candidateGreater {
	public:
		bool operator () (const candidate_pair &a, const candidate_pair &b){
			double A = get_candidate_centrality(a);
			double B = get_candidate_centrality(b);
			return ABS(A-B)<=eps ? vertex_id[a.second] > vertex_id[b.second] : A < B;
		}
};

// candidate set 
typedef priority_queue < candidate_pair, vector <candidate_pair>, candidateGreater > candidate_set;

// closeness centrality candidate pair: (c(v), v)
typedef pair<double, int> cc_pair; 

class AnswerGreater {
	
public:
	bool operator ()(cc_pair &a, cc_pair &b) {
		if (ABS(a.first-b.first)>eps)
			return a.first > b.first;
		return a.second < b.second;
	}
};

// current answer set
typedef priority_queue < cc_pair, vector <cc_pair>, AnswerGreater > answer_set;
static answer_set A;

// reachable set (bit operation)
struct reachable_set {
	unsigned * bits;
	int size;
	reachable_set(){
		bits = new unsigned[SET_LENGTH];
		memset(bits, 0, sizeof(unsigned) * SET_LENGTH);
		size = -1;
	}
	void set(int pos){
		if (!(bits[pos/32] & (1<<(pos&31)))){
			bits[pos/32] |= 1<<(pos&31);
			if (size >=0) ++size;
		}
	}
	void unset(int pos){
		if ((bits[pos/32] & (1<<(pos&31)))){
			bits[pos/32] &= ~ (1<<(pos&31));
			if (size >=0) --size;
		}
	}
	void Union(const struct reachable_set * b) {
		for (int i=0; i<SET_LENGTH; i++)
			bits[i] |= b->bits[i];
		size = -1;
	}
	void remove(const struct reachable_set * b) {
		for (int i=0; i<SET_LENGTH; i++)
			bits[i] &= ~(b->bits[i]);
		size = -1;
	}
	int cnt_size(){
		if (size != -1)
			return size;
		size = 0;
		for (int i=0; i<i_n; i++)
			size += !!(bits[i/32] & (1<<(i&31)));
		return size;
	}
	~reachable_set(){
		delete bits;
	}
};
vector <struct reachable_set *> * reachable_sets[L+1];

// functions
static void PFS(int v, int * Lvl, int * CNT_PFS);
static void update(answer_set *A, int p, double c, int k);
static int findfa(int x, vector <int> &fa);
static void calc_apprx(vector <int> &_sigma, vector <int> ** reachable_num, vector <int> &fa);
static void prep_dfs(int u, vector <bool> &flag, vector <int> &fa);
void query(int k, int t);

#endif
