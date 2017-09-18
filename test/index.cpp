/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
/*
	This test create an index on N uint16
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

uint64_t N = 1*100*1000*1000;
#define X 100

uint32_t elapsed_time(struct timeval *t1, struct timeval *t2){
	return 1000000*(t2->tv_sec-t1->tv_sec)+t2->tv_usec-t1->tv_usec;
}

int main(int argc, char **argv){

        if (argc == 2 && atoi(argv[1]) > 100)
        {
          N = atoi(argv[1]);
        }

        printf("testing %lld elements\n", N);

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

	struct timeval t1, t2;
	uint16_t to_search=v[X];
        uint32_t time_diff=0;

	/*Generate Random Number in [0,k-1] interval*/

	srand(time(NULL));

	gettimeofday(&t1,NULL);

	for(i=0;i<N;i++){
		v[i] = (uint16_t)(rand()%65536);
	}

	gettimeofday(&t2,NULL);
	time_diff = elapsed_time(&t1,&t2);

	printf("GENERATED RANDOMS #s IN %u microsecond (%9.6f seconds)\n",time_diff, time_diff / (float) 1000000);
	printf("-----------------------------------\n");
        fflush(stdout);

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

	printf("CREATED INDEX IN %u microsecond (%9.6f seconds)\n",time_diff, time_diff / (float) 1000000);
	printf("-----------------------------------\n");
        fflush(stdout);

	/*Make an equality-query with index and with linear scan
	and confront the query-time*/ 

	printf("MAKE QUERY ON %6.3f million VALUES\n", N / (float) 1000000);
	printf("-----------------------------------\n");
	
	std::vector<uint32_t> result_index, result_scan;

	gettimeofday(&t1,NULL);

	result_index = OZBCIndex[to_search].toVector();

	gettimeofday(&t2,NULL);
	time_diff = elapsed_time(&t1,&t2);

	printf("OZBCIndex make query in: %u microseconds (%9.6f seconds)\n",time_diff, time_diff / (float) 1000000);
	printf("-----------------------------------\n");
        fflush(stdout);

	gettimeofday(&t1,NULL);

	for(i=0;i<N;i++){
		if(v[i]==to_search){
			result_scan.push_back(i);
		}
	}

	gettimeofday(&t2,NULL);	
	time_diff = elapsed_time(&t1,&t2);

	printf("Linear Scan make query in: %u microseconds (%9.6f seconds)\n",time_diff, time_diff / (float) 1000000);
	printf("-----------------------------------\n");
        fflush(stdout);

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
