/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
/*
	This test create two indices (OZBCIndex and BitmapIndex)
	on N (N=1000000) values inserted and confront their size.
	Each value inserted is included between 0 and L-1,
	with L=16,,32,64,128.....,L_MAX.
	OZBCIndex and BitmapIndex are rappresented from L
	different bitmap. To create theese index for each 
	value=X of row=I is setted the Ith bit of the Xth bitmap.
*/    
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include "ozbcbitmap.h"

#define K 16 
#define K_MAX 65536
#define N 1000000

int cmpfunc (const void * a,const void * b) {
	return ( *(uint16_t*)a - *(uint16_t*)b );
}


int main() {
	printf("\nSTART TEST:\n");

	OZBCBitmap *OZBCIndex = NULL;
	uint16_t *v = NULL;
	uint32_t i, k;
	v = (uint16_t*)malloc(sizeof(uint16_t) * N);
	if(v == NULL) {
		printf("Error allocating v\n");
		fflush(stdout);
		return 1;
	}

	srand(time(NULL));
	/*
		The test is repeated each iteration
		and each iteration k=k*2.
	*/
	for(k = K; k <= K_MAX; k = k*2) {
		OZBCIndex = new OZBCBitmap[k];
		if(OZBCIndex == NULL) {
			printf("Error allocating %u bitmap\n", k);
			fflush(stdout);
			return 1;
		}

		/*Generate Random Number in [0,k-1] interval*/
		for(i = 0; i < N; i++) {
			v[i] = (uint16_t)(rand()%k);
		}
   		//qsort(v,N,sizeof(uint16_t),cmpfunc);

		/*Insert Random Number in indeces*/
		for(i = 0; i < N; i++) {
			OZBCIndex[v[i]].set(i);
		}

		uint32_t size_OZBCIndex = 0, size_BitmapIndex = 0;
		/*Get the size of the indices*/
		for(i = 0; i < k; i++) {
			size_OZBCIndex += OZBCIndex[i].size(true);
		}

		printf("---------------------------------------\n");

		/*Print the size of indices on random number*/
		printf("Test compression on %u random number of range [0,%u] inserted:\n",
			N, k-1);

		printf("The size of OZBCIndex with k=%u is:%uKBytes\n",
			k, size_OZBCIndex/1000);

		fflush(stdout);

		delete[](OZBCIndex);
		OZBCIndex = NULL;
	}

	printf("---------------------------------------\n\n");

	return 0;
}