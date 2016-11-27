/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
/*
	This test create two indices (OZBCIndex and BitmapIndex)
	on N (N=100000) values inserted and confront their size.
	Each value inserted is included between 0 and K-1,
	with K=16,,32,64,128.....,K_MAX.
	OZBCIndex and BitmapIndex are rappresented from K
	different bitmap. To create theese index for each 
	value=X of row=I is setted the Ith bit of the Xth bitmap.
*/    
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <inttypes.h>
#include "ozbc.h"
#include "bitmap.h"

#define K 16 
#define K_MAX 65536
#define N 100000



int main(){
	printf("\nSTART TEST:\n");

	OZBCBitmap *OZBCIndex=NULL;
	Bitmap *BitmapIndex=NULL;
	uint16_t *v=NULL;
	uint32_t i, k;
	v = (uint16_t*)malloc(sizeof(uint16_t)*N);
	if(v==NULL){
		printf("Error allocating v\n");
		fflush(stdout);
		return 1;
	}

	srand(time(NULL));
	/*
		The test is repeated each iteration
		and each iteration k=k*2.
	*/
	for(k=K;k<=K_MAX;k=k*2){
		OZBCIndex = new OZBCBitmap[k];
		BitmapIndex = new Bitmap[k];
		if(OZBCIndex==NULL||BitmapIndex==NULL){
			printf("Error allocating %u bitmap\n",k);
			fflush(stdout);
			return 1;
		}

		/*Generate Random Number in [0,k-1] interval*/
		for(i=0;i<N;i++){
			v[i] = (uint16_t)(rand()%k);
		}

		/*Insert Random Number in indeces*/
		for(i=0;i<N;i++){
			OZBCIndex[v[i]].set(i);
			BitmapIndex[v[i]].set(i);
		}

		uint32_t size_OZBCIndex=0, size_BitmapIndex=0;
		/*Get the size of the indices*/
		for(i=0;i<k;i++){
			size_OZBCIndex += OZBCIndex[i].sizeBitmapOnDisk();
			size_BitmapIndex += BitmapIndex[i].sizeBitmapOnDisk();
		}

		printf("---------------------------------------\n");

		/*Print the size of indices on random number*/
		printf("Test compression on %u random number of range [0,%u] inserted:\n",
			N,k-1);

		printf("The size of BitmapIndex with k=%u is:%uKBytes\n",
			k,size_BitmapIndex/1000);

		printf("The size of OZBCIndex with k=%u is:%uKBytes\n",
			k,size_OZBCIndex/1000);

		fflush(stdout);

		delete[](OZBCIndex);
		delete[](BitmapIndex);
		OZBCIndex = NULL;
		BitmapIndex = NULL;
	}

	printf("---------------------------------------\n\n");

	return 0;
}