/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
/*
	This test create an index on 10M uint16
	distinct values and confront the time to
	make an equality-query with the linear scan. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include <vector>
#include "ozbcbitmap.h"

#define N 10000000 //10M
#define X 100

uint32_t elapsed_time(struct timeval *t1, struct timeval *t2){
	return 1000000*(t2->tv_sec-t1->tv_sec)+t2->tv_usec-t1->tv_usec;
}

int main(){
	printf("\nSTART EXAMPLE INDEX\n");

	OZBCBitmap *OZBCIndex=NULL;
	uint16_t *v=NULL;
	uint32_t i;

	v = (uint16_t*)malloc(sizeof(uint16_t)*N);
	if(v==NULL){
		printf("Error allocating v\n");
		fflush(stdout);
		return 1;
	}

	srand(time(NULL));
	/*Generate Random Number in [0,k-1] interval*/
	for(i=0;i<N;i++){
		v[i] = (uint16_t)(rand()%65536);
	}
	uint16_t to_search=v[X], time_diff=0;
	struct timeval t1, t2;

	/*Create OZBCIndex*/
	OZBCIndex = new OZBCBitmap[65536];
	if(OZBCIndex==NULL){
		printf("Error allocating 65536 bitmap\n");
		fflush(stdout);
		return 1;
	}
	gettimeofday(&t1,NULL);
	for(i=0;i<N;i++){
		OZBCIndex[v[i]].set(i);
	}
	gettimeofday(&t2,NULL);
	time_diff = elapsed_time(&t1,&t2);
	printf("CREATED INDEX IN %u microsecond\n",time_diff);
	printf("-----------------------------------\n");

	/*Make an equality-query with index and with linear scan
	and confront the query-time*/ 
	printf("MAKE QUERY ON 10M VALUES\n");
	printf("-----------------------------------\n");
	
	std::vector<uint32_t> result_index, result_scan;

	gettimeofday(&t1,NULL);
	result_index = OZBCIndex[to_search].toVector();
	gettimeofday(&t2,NULL);
	time_diff = elapsed_time(&t1,&t2);
	printf("OZBCIndex make query in: %u microseconds\n",time_diff);
	printf("-----------------------------------\n");

	gettimeofday(&t1,NULL);
	for(i=0;i<N;i++){
		if(v[i]==to_search){
			result_scan.push_back(i);
		}
	}
	gettimeofday(&t2,NULL);	
	time_diff = elapsed_time(&t1,&t2);
	printf("Linear Scan make query in: %u microseconds\n",time_diff);
	printf("-----------------------------------\n");

	/*Check the result*/
	if(result_index.size()!=result_scan.size()){
		printf("There is an error in OZBVBitmap\n");
		return 1;
	}
	for(i=0;i<result_scan.size();i++){
		if(result_index[i]!=result_scan[i]){
			printf("There is an error in OZBVBitmap\n");
			return 1;
		}
	}
	printf("Result of OZBCIndex is correct\n");
	fflush(stdout);

	return 0;
}