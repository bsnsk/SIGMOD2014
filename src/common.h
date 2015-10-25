/*************************************************************************
    > File Name: common.h
    > Author: 
    > Created Time: 一 11/24 10:55:50 2014
 ************************************************************************/

#ifndef COMMON_4
#define COMMON_4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <queue>
#include <cmath>

#define HASH_HANDWRITE 0

#define DEBUG 0

struct forum {
	std::vector <int> persons;
};

struct edge {
	int vertex;
	struct edge *nxt;	
	
	edge(int v=0, struct edge *N=0): vertex(v), nxt(N) {}
};

class _unordered_map {
	static int MOD;
	const static int MULT = 132977;
	struct forum ** ptrs;
	bool *flag;
	int *num;
	unsigned long _size;
	public:
		_unordered_map(){
			//memset(ptrs, 0, sizeof ptrs);
			//memset(flag, 0, sizeof flag);
			ptrs = 0;
			flag = 0;
			num = 0;
			_size = 0;
		}

		void reserve(int n){
			for (MOD = 100000; MOD <= n; MOD *= 100);
			MOD += 7;
			ptrs = (struct forum **)calloc(MOD, sizeof(struct forum *));
			flag = (bool *)calloc(MOD, sizeof(bool));
			num = (int *)calloc(MOD, sizeof(int));
			fprintf(stderr, "MOD=%d\n", MOD);
			assert(n < MOD);
		}

		struct forum * insert(int idx){
			int k;
			for (k = ((unsigned)idx * MULT) % MOD; flag[k] && num[k]!=idx; k=(++k==MOD ? 0 : k));
			if (!flag[k]) _size++;
			flag[k] = 1;
			num[k] = idx;
			return ptrs[k] = new struct forum;
		}

		unsigned long size(){
			return _size;
		}

		struct forum & operator[](int idx){
			int k;
			for (k = ((unsigned)idx * MULT) % MOD; flag[k] && num[k]!=idx; k=(++k==MOD ? 0 : k));
			if (!flag[k]) fprintf(stderr, "#@%d\n", idx);
			assert(flag[k]);
			return *ptrs[k];
		}

		~_unordered_map (){
			//for (int i=0; i<MOD; i++)
			//	if (flag[i])
			//		delete (struct forum *)ptrs[i];
		}
};

#endif
