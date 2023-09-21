#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"
#include <getopt.h>
#include <math.h>
#include <unistd.h>

typedef struct{
        int valid;
        int tag;
        int last;
}ca;
typedef ca *block;
typedef block *set;

int vagmt;
int s;
int E;
int b;	
int hit=0;
int miss=0;
int evict=0;
int time=0; 

char *trace;
int input;

set cache;

void sEbt(int argc, char *argv[]){
        while((input = getopt(argc, argv, "hvs:E:b:t:")) != -1){
                switch(input){
                        case 'v':
                                vagmt = 1;
                                break;
			case 't':
                                trace = optarg;
                                break;
                        case 's':
                                s = atoi(optarg);
                                break;
			case 'b':
                                b = atoi(optarg);
                                break;
                        case 'E':
                                E = atoi(optarg);
                                break;
			case 'h':
                                break;
                }
        }
}

void writeCache(unsigned long long addr){
	int eTime=0; 
        int index=0;
	int tag = addr >> (s+b);
	int setIdx = (addr>>b) - (tag<<s);
	int hitNum = 0;
	int evictNum = 1;
	int blockIdx = 0;
	time++;
	eTime = time;
	
	for(; blockIdx < E; blockIdx++){	
		if(!(cache[setIdx][blockIdx].last > eTime)){
			eTime = cache[setIdx][blockIdx].last;
			index = blockIdx;
		}

		if(cache[setIdx][blockIdx].valid==0){
			index = blockIdx;
			evictNum = 0;
			break;
		}
		else{
			if(cache[setIdx][blockIdx].tag == tag){
				hitNum = 1;
				break; 
			}
		}
	}
	
	if(hitNum == 0){
		miss++;
		if(vagmt){
			printf(" miss ");
		}

		if(evictNum==1){
			if(vagmt)
				printf(" eviction\n");
			evict++;
		}

		cache[setIdx][index].valid = 1;
		cache[setIdx][index].tag = tag;
		cache[setIdx][index].last = time;
	}

	 else{
                if(vagmt)
                        printf(" hit\n");
                hit++;
                cache[setIdx][blockIdx].last = time;
        }
}

int main(int argc, char **argv)
{
	FILE *file;
	char input;
	unsigned long long addr;
	int size;
	int i, j = 0;

	sEbt(argc,argv);
	cache = (block*)malloc(sizeof(block)*(1<<s));
	
	for(i = 0; i < (1<<s) ; i++){
		cache[i] = (ca*)malloc(sizeof(ca)*E);
		for(j = 0; j < E; j++){
			cache[i][j].valid = 0;
			cache[i][j].tag = 0;
			cache[i][j].last = 0;
		}
	}
	
	file = fopen(trace, "r");
	if(file != NULL){
		while(fscanf(file, " %c %llx,%d", &input,&addr,&size) == 3){
			switch(input){
				case 'L':
					writeCache(addr);
					break;
				case 'S':
					writeCache(addr);
					break;
				case 'M':
					writeCache(addr);
					writeCache(addr);
					break;
				case 'I':
                                        break;
			}
		}
	}
    printSummary(hit, miss, evict);
    fclose(file);
    return 0;
}
