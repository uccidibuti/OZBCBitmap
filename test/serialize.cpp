/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
 /*
	This test create and serialaze a bitmap. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <vector>
#include "ozbcbitmap.h"

int main(){
	OZBCBitmap bitmap;
	bitmap.set(100);
	bitmap.set(1000);
	bitmap.set(10000);

	char *buffer=NULL;
	uint32_t r=0;
	uint32_t size=bitmap.size(true);

	
	buffer = (char*)malloc(size);
	if(buffer==NULL){
		printf("Error allocating buffer\n");
		fflush(stdout);
		return 1;
	}

	r = bitmap.write(buffer,size,true);
	if(r==0){
		printf("Failed write\n");
		fflush(stdout);
		return 1;
	}
	bitmap.reset();

	r = bitmap.read(buffer,size,0);
	if(r==0){
		printf("Failed write\n");
		fflush(stdout);
		return 1;
	}

	std::vector<uint32_t> values=bitmap.toVector();
	if(values.size()!=3){
		printf("Error occured in write or read");
		fflush(stdout);
		return 1;
	}
	if(values[0]!=100||values[1]!=1000||
		values[2]!=10000){

		printf("Error occured in write or read");
		fflush(stdout);
		return 1;		
	}
	printf("Serialize OK!\n");

	return 0;
}