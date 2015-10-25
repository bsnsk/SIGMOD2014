/*************************************************************************
    > File Name: query4_algorithm.cpp
    > Author: 
    > Created Time: 日 11/23 23:11:36 2014
 ************************************************************************/

#include "query4_algorithm.h"

void query(int k, int t) {
	int query_start_time = clock();
#if DEBUG
	fprintf(stderr, "\nquerying\n");
#endif
	srand(query_start_time);

	int CNT_PFS=0;

	/* Construct Induced Graph */
	memset(person_in_induced_graph, 0, personcnt * sizeof(person_in_induced_graph[0]));
	vector <int> * vec = tag_to_forum[t];
	for (vector <int> :: iterator f=vec->begin(); f!=vec->end(); f++){
		vector <int> *pers = &(forums[*f].persons);
		for (vector <int> :: iterator p=pers->begin(); p!=pers->end(); p++)
			person_in_induced_graph[*p] = 1;
	}

	i_n=0;
	vertex_id.clear();
	for (int i=0; i<personcnt; i++)
		if (person_in_induced_graph[i]){
			i_n++;
			vertex_id.push_back(i);
		}
	id_2_subid.resize(personcnt);
	i_edges_from.resize(i_n);
	i_edges_to.resize(i_n);
	for (int i=0; i<i_n; i++){
		id_2_subid[vertex_id[i]] = i;
		i_edges_from[i] = i_edges_to[i] = NULL;
	}

	for (int i=0; i<i_n; i++){
		for (struct edge *e = edges_from[vertex_id[i]]; e; e=e->nxt)
			if (person_in_induced_graph[e->vertex] && i < id_2_subid[e->vertex])
				i_add_edge(i, id_2_subid[e->vertex]);
	}

#if DEBUG
	fprintf(stderr, "person:\t%d of %d\n", i_n, personcnt);
#endif

	/* Resize Vectors */
	_sigma.resize(i_n);
	sigma.resize(i_n);
	for (int i=0; i<=L ;i++)
		reachable_num[i] = new vector <int> (i_n, 0);
	_centrality.resize(i_n, 0);
	centrality.resize(i_n);
	for (int i=0; i<i_n; i++)
		sigma[i] = -1, centrality[i] = -1;
	cur_lvls = new int[i_n];

	/* Calculate Approximation */
	calc_apprx(_sigma, reachable_num, fa);

#if DEBUG
	assert(A.empty());
	fprintf(stderr, "Start main alg, time: %.2lf sec(s).\n", ((double)clock()-query_start_time)/CLOCKS_PER_SEC);
#endif

#ifdef MORE_PRUNNING
	/* Choose Seeds */
	for (int i=0; i<lambda * k; i++){
		int u = rand() % i_n;
		if (sigma[u]==-1){
			PFS(u, cur_lvls, &CNT_PFS);
			if (sigma[u])
				update(&A, u, centrality[u], k);
		}
	}
#endif

#if DEBUG
	fprintf(stderr, "Seeds done. A.size = %lu\n", A.size());
	assert(A.size()==k);
#endif

	/* Initialize Candidate Set */
	candidate_set * candidates = new candidate_set;
	for (int i=0; i<i_n; i++)
		if (centrality[i] < -0.5)
			candidates->push(make_pair(0, i));

	/* Main Process */
	while (!candidates->empty() && (A.size()<k || get_candidate_centrality(candidates->top()) > A.top().first)){
		if (!candidates->top().first){
			int cur_v = candidates->top().second;
			PFS(cur_v, cur_lvls, &CNT_PFS);
			candidates->pop();
			if (sigma[cur_v])
				candidates->push(make_pair(1, cur_v));
		}
		if (candidates->top().first){
			double cur_c = get_candidate_centrality(candidates->top());
			if ((A.size()==k) && (A.top().first > cur_c || (ABS(A.top().first - cur_c) <= eps && vertex_id[A.top().second] < vertex_id[candidates->top().second])))
				break;
			update(&A, candidates->top().second, centrality[candidates->top().second], k);
			candidates->pop();
		}
	}
	
#if DEBUG
	fprintf(stderr, "A.size() == %lu. Time: %.2lf sec(s).\n", A.size(), ((double)clock()-query_start_time)/CLOCKS_PER_SEC);
	assert(A.size()==k);
#endif

	/* Print Answers */
	vector<int> print_seq(k);
	for (int i=0; !A.empty(); ++i) {
		print_seq[i] = A.top().second;
#if DEBUG
		fprintf(stderr, "(%g,%d)", A.top().first, A.top().second);
#endif
		A.pop();
	}
#if DEBUG
	fprintf(stderr, "\n");
#endif
	
	for (int i=k-1; i>0; i--)
		printf("%d ", print_seq[i]);
	printf("%d\n", print_seq[0]);
	fflush(stdout);
	
#if DEBUG
#define I(x) (id_2_subid[x])
#define PRINTC(x) {fprintf(stderr, "c(%d)=%g(%g)\n", x, centrality[I(x)], _centrality[I(x)]);}
	PRINTC(430);
	PRINTC(323);
#endif

	delete candidates;
	delete []cur_lvls;
	for (int i=0; i<=L; i++)
		delete reachable_num[i];
	for (int i=0; i<i_n; i++){
		erase_edges(i_edges_from[i]);
		erase_edges(i_edges_to[i]);
	}
#if DEBUG
	fprintf(stderr, "Query Time: %.2lf sec(s).\n", ((double)clock()-query_start_time)/CLOCKS_PER_SEC);
#endif
}


int findfa(int x, vector <int> &fa){
	return fa[x]==x ? x : fa[x] = findfa(fa[x], fa);
}

void prep_dfs(int u, vector <bool> &flag, vector <int> &fa){
	flag[u] = 1;
	for (struct edge *e=i_edges_from[u]; e; e=e->nxt)
		if (!flag[e->vertex]){
			fa[findfa(e->vertex, fa)] = findfa(u, fa);
			prep_dfs(e->vertex, flag, fa);
		}
}

void calc_apprx(vector <int> &_sigma, vector <int> ** reachable_num, vector <int> &fa){
	for (int i=1; i<=L; i++) {
		reachable_sets[i] = new vector <struct reachable_set *> (i_n);
		for (int j=0; j<i_n; j++)
			(*reachable_sets[i])[j] = new reachable_set();
	}
	fa.resize(i_n);
	vector <bool> flag(i_n);
	for (int i=0; i<i_n; i++)
		flag[fa[i] = i] = 0;
	for (int i=0; i<i_n; i++)
		if (!flag[i])
			prep_dfs(i, flag, fa);
	for (int i=0; i<i_n; i++)
		(*reachable_num[0])[findfa(i, fa)] ++;
	for (int i=0; i<i_n; i++)
		for (struct edge *e = i_edges_from[i]; e; e=e->nxt)
			(*reachable_sets[1])[i]->set(e->vertex);
	for (int i=0; i<i_n; i++)
		(*reachable_sets[1])[i]->unset(i);

	for (int j=2; j<=L; j++){
		for (int i=0; i<i_n; i++)
			for (struct edge *e = i_edges_from[i]; e; e=e->nxt)
				(*reachable_sets[j])[i]->Union((*reachable_sets[j-1])[e->vertex]);
		for (int i=0; i<i_n; i++)
			(*reachable_sets[j])[i]->remove((*reachable_sets[j-1])[i]);
	}

	for (int j=1; j<=L; j++)
		for (int i=0; i<i_n; i++)
			(*reachable_num[j])[i] = (*reachable_sets[j])[i]->cnt_size();

	for (int i=0, cnt; i<i_n; i++){
		_sigma[i] = 0;
		cnt = (*reachable_num[0])[findfa(i, fa)];
		for (int j=1; j<=L; j++){
			_sigma[i] += j * (*reachable_num[j])[i];
			cnt -= (*reachable_num[j])[i];
		}
		_sigma[i] += cnt * (L+1);
		_centrality[i] = double_sqr((*reachable_num[0])[findfa(i, fa)]) / _sigma[i];
	}

	for (int i=1; i<=L; i++) {
		for (int j=0; j<i_n; j++)
			delete (*reachable_sets[i])[j];
		delete reachable_sets[i];
	}
}

void PFS(int v, int * Lvl, int *CNT_PFS){
	CNT_PFS ++;
	int cnt=1, sum=0;
	queue <int> Q;
	memset(Lvl, -1, sizeof (int) * i_n);
	Q.push(v);
	for (Lvl[v]=0; !Q.empty(); Q.pop()){
		int t = Q.front();
		sum += Lvl[t];
		for (struct edge *e = i_edges_from[t]; e; e=e->nxt)
			if (Lvl[e->vertex] == -1){
				Lvl[e->vertex] = Lvl[t] + 1;
				Q.push(e->vertex);
				cnt++;
			}
	}
	sigma[v] = sum;
	(*reachable_num[0])[v] = cnt;
	centrality[v] = double_sqr(cnt-1) / sum;
#if DEBUG
	//fprintf(stderr, "PFS %d: sigma=%d, reachable_num=%d, centrality = %g\n", vertex_id[v], sum, cnt, double_sqr(cnt) / sum);
#endif
}

void update(answer_set *A, int p, double c, int k){
	if (A->size() < k)
		A->push(make_pair(c, vertex_id[p]));
	else if (A->top().first < c - eps || (ABS(A->top().first-c) <=eps && A->top().second > vertex_id[p])){
		A->pop();
		A->push(make_pair(c, vertex_id[p]));
	}
}

void i_add_edge(int u, int v){
	struct edge *p = new struct edge(v, i_edges_from[u]);
	struct edge *q = new struct edge(v, i_edges_to[u]);
	i_edges_from[u] = p;
	i_edges_to[u] = q;
	p = new edge(u, i_edges_from[v]);
	q = new edge(u, i_edges_to[v]);
	i_edges_from[v] = p;
	i_edges_to[v] = q;
}

