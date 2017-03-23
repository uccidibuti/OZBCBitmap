/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
#include "ozbc.h"
#include "macro.h"

OZBCBitmap::OZBCBitmap(){
	number_words_8bits = 0;
}




void OZBCBitmap::set(uint32_t i){
	uint8_t dirty_bit=(uint8_t)(i&7);
	uint8_t dirty_word=(uint8_t)(1<<dirty_bit); 
	int32_t words_zero=(int32_t)((i>>3)-number_words_8bits);

	if(words_zero>=0){
		number_words_8bits += words_zero+1;
		if(words_zero<128){
			buffer.push_back( (uint16_t)(((words_zero)<<8)|dirty_word) );
		}
		else{
			const uint32_t max_words_zero=4194176;
			const uint32_t max_word=(1<<15)-1;
			while((uint32_t)words_zero>max_words_zero){
				buffer.push_back( (uint16_t)(max_word|(1<<15)) );
				words_zero -= max_words_zero;
			}
			buffer.push_back( (uint16_t)( (words_zero>>7)|(1<<15)) );
			buffer.push_back( (uint16_t)(((words_zero&127)<<8)|dirty_word) );
		}
	}
	else{
		uint32_t pos_last=buffer.size()-1;
		buffer[pos_last] = buffer[pos_last]|dirty_word; 
	}
}




OZBCBitmap OZBCBitmap::logicaland(OZBCBitmap &b){
	uint32_t i=0, j=0, n1=buffer.size(), n2=b.buffer.size();
	uint32_t nz_words1=0, nz_words2=0, words=0;
	OZBCBitmap r;

	uint16_t *v1=NULL, *v2=NULL;
	const uint32_t max_word=(1<<15)-1;
	const uint32_t max_words_zero=41994176;
	uint8_t type_word=0, dirty_word=0;
	uint32_t wz1=0, wz2=0, words_zero=0;
		
	v1 = &(buffer[0]);
	v2 = &(b.buffer[0]);
		

	while(i<n1&&j<n2){
		type_word = (uint8_t)( (v1[i]>>15)|((v2[j]>>14)&2) );
		switch(type_word){
			case 0:
				wz1 = (v1[i]>>8)+1;
				wz2 = (v2[j]>>8)+1;
				break;
			case 1:
				wz1 = ((uint32_t)(v1[i]&max_word))<<7;
				wz2 = (v2[j]>>8)+1;
				break;
			case 2:
				wz1 = (v1[i]>>8)+1;
				wz2 = ((uint32_t)(v2[j]&max_word))<<7;
				break;
			case 3:
				wz1 = ((uint32_t)(v1[i]&max_word))<<7;
				wz2 = ((uint32_t)(v2[j]&max_word))<<7;
				break;
			default:
				break;	
		}

		nz_words1 += wz1;
		nz_words2 += wz2;
		i++;
		j++;

		if(nz_words1<nz_words2){
			nz_words2 -= wz2;
			j--;
		}

		else if(nz_words1>nz_words2){
			nz_words1 -= wz1;
			i--;
		}

		else if(type_word==0){
			words_zero = nz_words1-words-1;
			dirty_word = (uint8_t)(v1[i-1]&v2[j-1]);
			if(dirty_word!=0){
					
				words = nz_words1;
				if(words_zero<128){
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				}
					
				else{
					while(words_zero>max_words_zero){
						r.buffer.push_back( (uint16_t)(max_word|(1<<15)) );
						words_zero -= max_words_zero;
					}
					r.buffer.push_back( (uint16_t)( (words_zero>>7)|(1<<15)) );
					r.buffer.push_back( (uint16_t)(((words_zero&127)<<8)|dirty_word) );
				}

			}//end if dirtyword!=0	
		}//end elseif
	}//end while

	r.number_words_8bits = words;

	return r;
}




OZBCBitmap OZBCBitmap::logicalor(OZBCBitmap &b){
	uint32_t i=0, j=0, n1=buffer.size(), n2=b.buffer.size();
	uint32_t nz_words1=0, nz_words2=0, words=0;
	OZBCBitmap r;

	uint16_t *v1=NULL, *v2=NULL;
	const uint32_t max_word=(1<<15)-1;
	uint8_t type_word=0, dirty_word=0;
	uint32_t wz1=0, wz2=0, words_zero=0;
		
	v1 = &(buffer[0]);
	v2 = &(b.buffer[0]);
		
	//make or
	while(i<n1&&j<n2){
		type_word = (uint8_t)( (v1[i]>>15)|((v2[j]>>14)&2) );
		switch(type_word){
			case 0:
				wz1 = (v1[i]>>8)+1;
				wz2 = (v2[j]>>8)+1;
				ALIGN_OR_0_0(nz_words1,nz_words2,wz1,wz2,i,j,
					v1,v2,words_zero,words,dirty_word,r);

				break;

			case 1:
				wz1 = ((uint32_t)(v1[i]&max_word))<<7;
				wz2 = (v2[j]>>8)+1;
				ALIGN_OR_1_0(nz_words2,nz_words1,wz2,wz1,j,i,
					v2,v1,words_zero,words,dirty_word,r);

				break;

			case 2:
				wz1 = (v1[i]>>8)+1;
				wz2 = ((uint32_t)(v2[j]&max_word))<<7;
				ALIGN_OR_1_0(nz_words1,nz_words2,wz1,wz2,i,j,
					v1,v2,words_zero,words,dirty_word,r);

				break;

			case 3:
				wz1 = ((uint32_t)(v1[i]&max_word))<<7;
				wz2 = ((uint32_t)(v2[j]&max_word))<<7;
				ALIGN_OR_1_1(nz_words1,nz_words2,wz1,wz2,i,j,
					v1,v2,words_zero,words,dirty_word,r);

				break;
			default:
				break;	
		}

		
	}//end while

	r.number_words_8bits = number_words_8bits;

	if(i>=n1&&j<n2){
		type_word = (uint8_t)(v2[j]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v2[j];
				nz_words2 += (v2[j]>>8)+1;
				words_zero = nz_words2-words-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_words2 += ((uint32_t)(v2[j]&max_word))<<7;
				words_zero = nz_words2-words-1;
				r.buffer.push_back((uint16_t)( (1<<15)|((words_zero)>>7)) );
				r.buffer.push_back((uint16_t)( ((words_zero)&127)<<8) );
				break;
			default:
				break;	
		}
		j++;
		while(j<n2){
			r.buffer.push_back(v2[j]);
			j++;
		}
		r.number_words_8bits = b.number_words_8bits;
	}
	else if(j>=n2&&i<n1){
		type_word = (uint8_t)(v1[i]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v1[i];
				nz_words1 += (v1[i]>>8)+1;
				words_zero = nz_words1-words-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_words1 += (v1[i]&max_word)<<7;
				words_zero = nz_words1-words-1;
				r.buffer.push_back((uint16_t)( (1<<15)|((words_zero)>>7)) );
				r.buffer.push_back((uint16_t)( ((words_zero)&127)<<8) );
				break;
			default:
				break;	
		}
		i++;
		while(i<n1){
			r.buffer.push_back(v1[i]);
			i++;
		}

	}
	return r;
}





uint32_t OZBCBitmap::getNumWords(){
	return buffer.size();
}




uint32_t OZBCBitmap::sizeBitmapOnDisk(bool portable){
	if(portable==true)
		return (buffer.size()*sizeof(uint16_t))+(2*sizeof(uint32_t));
		
	return (buffer.size()*sizeof(uint16_t))+sizeof(uint32_t);

}




uint32_t OZBCBitmap::writeBitmapOnBuffer(char *b,
	uint32_t len,uint32_t num_words){

  uint32_t size_buffer=(uint32_t)buffer.size();
  uint32_t dim_el=(uint32_t)(sizeof(uint16_t));
  uint32_t dim_tot=(size_buffer*dim_el);
  if((num_words==0&&dim_tot+8<=len)||(num_words!=0&&dim_tot+4<=len)){
    if(num_words==0){
      memcpy(b,&size_buffer,sizeof(uint32_t));
      b += (sizeof(uint32_t));
    }
    memcpy(b,&number_words_8bits,sizeof(uint32_t));
    b += (sizeof(uint32_t));
    memcpy(b,&(buffer[0]),size_buffer*sizeof(uint16_t));

    if(num_words==0)
      return dim_tot+(2*sizeof(uint32_t));

    return dim_tot+(sizeof(uint32_t));
  }
  return 0;
}




uint32_t OZBCBitmap::readBitmapOnBuffer(char *b,
  uint32_t len,uint32_t r_num_words){

  uint32_t dim_el=(uint32_t)(sizeof(uint16_t));
  uint32_t q=0;
  if(r_num_words==0){
    memcpy(&r_num_words,b,sizeof(uint32_t));
    b += (sizeof(uint32_t));
    q=1;
  }
  uint32_t dim_tot=r_num_words*dim_el;
  if((dim_tot+8<=len&&q==1)||(dim_tot+4<=len&&q!=0)){
    buffer.resize(r_num_words);
    memcpy(&number_words_8bits,b,sizeof(uint32_t));
    b += (sizeof(uint32_t));
    memcpy(&(buffer[0]),b,r_num_words*sizeof(uint16_t));

    if(q==1)
      return (dim_el*r_num_words)+(sizeof(uint32_t)*2);

    return (dim_el*r_num_words)+(sizeof(uint32_t));
  }
  return 0;
}




uint32_t OZBCBitmap::writeBitmapOnFile(FILE *f,
  uint32_t num_words){

  uint32_t tot=0;
  uint32_t size_buffer=(uint32_t)buffer.size();
  uint32_t dim_el=(uint32_t)(sizeof(uint16_t));
  if(f!=NULL){
    if(num_words==0){      
      tot += fwrite(&size_buffer,sizeof(uint32_t),1,f);
    }
    tot += fwrite(&number_words_8bits,sizeof(uint32_t),1,f);
    tot += fwrite(&(buffer[0]),sizeof(uint16_t),size_buffer,f);
    if(num_words==0&&tot==(size_buffer+2))
      return size_buffer*dim_el+(2*sizeof(uint32_t));

    else if(tot==(size_buffer+1))
      return size_buffer*dim_el+(sizeof(uint32_t));
  }
  return 0;
}




uint32_t OZBCBitmap::readBitmapOnFile(FILE *f,
  uint32_t r_num_words){

  uint32_t dim_el=(uint32_t)(sizeof(uint16_t));
  uint32_t tot=0;
  if(f!=NULL){
    if(r_num_words==0){      
      tot += fread(&r_num_words,sizeof(uint32_t),1,f);
    }
    buffer.resize(r_num_words);
    tot += fread(&number_words_8bits,sizeof(uint32_t),1,f);
    tot += fread(&(buffer[0]),sizeof(uint16_t),r_num_words,f);
    if(tot==(r_num_words+2))
      return (r_num_words*dim_el)+(2*sizeof(uint32_t));

    else if(tot==(r_num_words+1))
      return (r_num_words*dim_el)+(sizeof(uint32_t));
  }
  return 0;
}




void OZBCBitmap::printBitmap(FILE *f){
	if(f==NULL)
		return;

	uint32_t i, j;
	uint8_t x, t;
	fprintf(f,"NUMBER OF BITS:%u\n",(uint32_t)number_words_8bits*8);
	for(i=0;i<buffer.size();i++){
		t = (buffer[i]>>15)&1;
		for(j=16;j--;){
			x = (buffer[i]>>j)&1;
			fprintf(f,"%u",x);
			if(j==15||(j==8&&t==0))
				fprintf(f,"|");
		}
		fprintf(f,"\n");
	}
}	




void OZBCBitmap::swap(OZBCBitmap &b){
	buffer.swap(b.buffer);
	uint32_t h=number_words_8bits;
	number_words_8bits = b.number_words_8bits;
	b.number_words_8bits = h; 
}




OZBCBitmap OZBCBitmap::copyBitmap(){
	OZBCBitmap r;
	r.buffer = buffer;
	r.number_words_8bits = number_words_8bits;
	return r;
}




void OZBCBitmap::resetBitmap(){
	std::vector<uint16_t> x;
	x.swap(buffer);
	number_words_8bits = 0;
}




std::vector<uint32_t> OZBCBitmap::toVector(){
	uint32_t i, j, n=buffer.size(), pos_set=0;
	uint8_t type_word=0, dirty_word=0;
	uint32_t number_zero_words=0;
	const uint32_t max_word=(1<<15)-1;
	std::vector<uint32_t> result;

	for(i=0;i<n;i++){
		type_word = (buffer[i]>>15)&1;
			
		switch(type_word){
			case 0:
				number_zero_words = (uint16_t)(buffer[i]>>8);
				pos_set += (uint32_t)(number_zero_words<<3);
				dirty_word = (uint8_t)buffer[i];
				for(j=0;j<8;j++){
					if( (dirty_word>>j)&1==1 ){
						result.push_back(pos_set+j);
					}
				}
				pos_set += 8;
				break;
				
			case 1:
				number_zero_words = ((uint32_t)(buffer[i]&max_word))<<7;
				pos_set += (uint32_t)(number_zero_words<<3);
				break;
				
			default:
				break;		
		}
	}
	return result;
}




OZBCBitmap::~OZBCBitmap(){
	std::vector<uint16_t>().swap(buffer);
}