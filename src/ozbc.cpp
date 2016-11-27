/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
#include "ozbc.h"


OZBCBitmap::OZBCBitmap(){
	number_word_8bits = 0;
}




void OZBCBitmap::set(uint32_t i){
	uint8_t dirty_bit=(uint8_t)(i&7);
	uint8_t dirty_word=(uint8_t)(1<<dirty_bit); 
	int32_t words_zero=(int32_t)((i>>3)-number_word_8bits);

	if(words_zero>=0){
		number_word_8bits += words_zero+1;
		if(words_zero<128){
			buffer.push_back( (uint16_t)(((words_zero)<<8)|dirty_word) );
		}
		else{
			uint32_t max_word_zero=(1<<15)-1;
			while((uint32_t)words_zero>max_word_zero){
				buffer.push_back( (uint16_t)(max_word_zero|(1<<15)) );
				words_zero -= max_word_zero;
			}
			buffer.push_back( (uint16_t)((words_zero)|(1<<15)) );
			buffer.push_back( (uint16_t)dirty_word );
		}
	}
	else{
		uint32_t pos_last=buffer.size()-1;
		buffer[pos_last] = buffer[pos_last]|dirty_word; 
	}
}




void OZBCBitmap::appendLiteralWord(uint8_t word){
	buffer.push_back((uint16_t)word);
	number_word_8bits++;
}




OZBCBitmap OZBCBitmap::logicaland(OZBCBitmap b){
	uint32_t i=0, j=0, n1=buffer.size(), n2=b.buffer.size();
	uint32_t nz_word1=0, nz_word2=0, word=0;
	OZBCBitmap r;

	uint16_t *v1=NULL, *v2=NULL;
	uint16_t max_word_zero=(1<<15)-1;
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
				wz1 = v1[i]&max_word_zero;
				wz2 = (v2[j]>>8)+1;
				break;
			case 2:
				wz1 = (v1[i]>>8)+1;
				wz2 = v2[j]&max_word_zero;
				break;
			case 3:
				wz1 = v1[i]&max_word_zero;
				wz2 = v2[j]&max_word_zero;
				break;
			default:
				break;	
		}

		nz_word1 += wz1;
		nz_word2 += wz2;
		i++;
		j++;

		if(nz_word1<nz_word2){
			nz_word2 -= wz2;
			j--;
		}

		else if(nz_word1>nz_word2){
			nz_word1 -= wz1;
			i--;
		}

		else if(type_word==0){
			words_zero = nz_word1-word-1;
			dirty_word = (uint8_t)(v1[i-1]&v2[j-1]);
			if(dirty_word!=0){
					
				word = nz_word1;
				if(words_zero<128){
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				}
					
				else{
					uint32_t max_word_zero2=max_word_zero;
					while(words_zero>=max_word_zero2){
						r.buffer.push_back( (uint16_t)(max_word_zero2|(1<<15)) );
						words_zero -= max_word_zero2;
					}
					r.buffer.push_back( (uint16_t)((words_zero)|(1<<15)) );
					r.buffer.push_back( (uint16_t)dirty_word );
				}

			}//end if dirtyword!=0	
		}//end elseif
	}//end while

	r.number_word_8bits = word;

	return r;
}




OZBCBitmap OZBCBitmap::logicalor(OZBCBitmap b){
	uint32_t i=0, j=0, n1=buffer.size(), n2=b.buffer.size();
	uint32_t nz_word1=0, nz_word2=0, word=0;
	OZBCBitmap r;

	uint16_t *v1=NULL, *v2=NULL;
	uint16_t max_word_zero=(1<<15)-1;
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
				break;
			case 1:
				wz1 = v1[i]&max_word_zero;
				wz2 = (v2[j]>>8)+1;
				break;
			case 2:
				wz1 = (v1[i]>>8)+1;
				wz2 = v2[j]&max_word_zero;
				break;
			case 3:
				wz1 = v1[i]&max_word_zero;
				wz2 = v2[j]&max_word_zero;
				break;
			default:
				break;	
		}

		nz_word1 += wz1;
		nz_word2 += wz2;
		i++;
		j++;

		if(nz_word1<nz_word2){
			nz_word2 -= wz2;
			j--;
			words_zero = nz_word1-word-1;
			word = nz_word1;
			if(type_word==0||type_word==2){
				dirty_word = (uint8_t)(v1[i-1]);
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
			}
			else{
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
			}
		}

		else if(nz_word1>nz_word2){
			nz_word1 -= wz1;
			i--;
			words_zero = nz_word2-word-1;
			word = nz_word2;
			if(type_word==0||type_word==1){
				dirty_word = (uint8_t)(v2[j-1]);
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
			}
			else{
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
			}

		}
		else{
			words_zero = nz_word1-word-1;
			word = nz_word1;
			switch(type_word){
				case 0:
					dirty_word = (uint8_t)(v1[i-1]|v2[j-1]);
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 1:
					dirty_word = (uint8_t)v2[j-1];
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 2:
					dirty_word = (uint8_t)v1[i-1];
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 3:													
					r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
					break;
				default:
					break;	
			}
		}
		
	}//end while

	r.number_word_8bits = number_word_8bits;

	if(i>=n1&&j<n2){
		type_word = (uint8_t)(v2[j]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v2[j];
				nz_word2 += (v2[j]>>8)+1;
				words_zero = nz_word2-word-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_word2 += (v2[j]&max_word_zero);
				words_zero = nz_word2-word;
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero)));
				break;
			default:
				break;	
		}
		j++;
		while(j<n2){
			r.buffer.push_back(v2[j]);
			j++;
		}
		r.number_word_8bits = b.number_word_8bits;
	}
	else if(j>=n2&&i<n1){
		type_word = (uint8_t)(v1[i]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v1[i];
				nz_word1 += (v1[i]>>8)+1;
				words_zero = nz_word1-word-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_word1 += (v1[i]&max_word_zero);
				words_zero = nz_word1-word;
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero)));
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




OZBCBitmap OZBCBitmap::logicalnot(){
	uint32_t n=buffer.size();
	uint32_t i, max_word_zero=(1<<15)-1;
	uint16_t *v=NULL, x, words_zero, j;
	uint8_t type_word=0;
	v = &(buffer[0]);
	OZBCBitmap r;

	for(i=0;i<n;i++){
		x = v[i];
		type_word = (uint8_t)(x>>15);
		switch(type_word){
			case 0:
				words_zero = (x>>8);
				for(j=words_zero;j--;){
					r.buffer.push_back(255);
				}
				r.buffer.push_back((~x)&255);
				break;
			case 1:
				words_zero = x&max_word_zero;
				for(j=words_zero;j--;){
					r.buffer.push_back(255);
				}
				break;	
			default: 
				break;	
		}	
	}
	r.number_word_8bits = number_word_8bits;
	return r;
}




OZBCBitmap OZBCBitmap::logicalxor(OZBCBitmap b){
	uint32_t i=0, j=0, n1=buffer.size(), n2=b.buffer.size();
	uint32_t nz_word1=0, nz_word2=0, word=0;
	OZBCBitmap r;

	uint16_t *v1=NULL, *v2=NULL;
	uint16_t max_word_zero=(1<<15)-1;
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
				break;
			case 1:
				wz1 = v1[i]&max_word_zero;
				wz2 = (v2[j]>>8)+1;
				break;
			case 2:
				wz1 = (v1[i]>>8)+1;
				wz2 = v2[j]&max_word_zero;
				break;
			case 3:
				wz1 = v1[i]&max_word_zero;
				wz2 = v2[j]&max_word_zero;
				break;
			default:
				break;	
		}

		nz_word1 += wz1;
		nz_word2 += wz2;
		i++;
		j++;

		if(nz_word1<nz_word2){
			nz_word2 -= wz2;
			j--;
			words_zero = nz_word1-word-1;
			word = nz_word1;
			if(type_word==0||type_word==2){
				dirty_word = (uint8_t)(v1[i-1]);
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
			}
			else{
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
			}
		}

		else if(nz_word1>nz_word2){
			nz_word1 -= wz1;
			i--;
			words_zero = nz_word2-word-1;
			word = nz_word2;
			if(type_word==0||type_word==1){
				dirty_word = (uint8_t)(v2[j-1]);
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
			}
			else{
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
			}

		}
		else{
			words_zero = nz_word1-word-1;
			word = nz_word1;
			switch(type_word){
				case 0:
					dirty_word = (uint8_t)(v1[i-1]^v2[j-1]);
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 1:
					dirty_word = (uint8_t)v2[j-1];
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 2:
					dirty_word = (uint8_t)v1[i-1];
					r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
					break;
				case 3:													
					r.buffer.push_back((uint16_t)((1<<15)|(words_zero+1)));
					break;
				default:
					break;	
			}
		}
		
	}//end while

	r.number_word_8bits = number_word_8bits;

	if(i>=n1&&j<n2){
		type_word = (uint8_t)(v2[j]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v2[j];
				nz_word2 += (v2[j]>>8)+1;
				words_zero = nz_word2-word-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_word2 += (v2[j]&max_word_zero);
				words_zero = nz_word2-word;
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero)));
				break;
			default:
				break;	
		}
		j++;
		while(j<n2){
			r.buffer.push_back(v2[j]);
			j++;
		}
		r.number_word_8bits = b.number_word_8bits;
	}
	else if(j>=n2&&i<n1){
		type_word = (uint8_t)(v1[i]>>15);
		switch(type_word){
			case 0:
				dirty_word = (uint8_t)v1[i];
				nz_word1 += (v1[i]>>8)+1;
				words_zero = nz_word1-word-1;
				r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word));
				break;
			case 1:
				nz_word1 += (v1[i]&max_word_zero);
				words_zero = nz_word1-word;
				r.buffer.push_back((uint16_t)((1<<15)|(words_zero)));
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




uint32_t OZBCBitmap::sizeBufferOnDisk(){
	return sizeof(uint32_t)+sizeof(uint16_t)*buffer.size();
}




uint32_t OZBCBitmap::sizeBitmapOnDisk(){
	return sizeBufferOnDisk()+sizeof(uint32_t);
}




void OZBCBitmap::writeBitmap(std::ostream &out){
	uint32_t size_buffer=(uint32_t)buffer.size();
	out.write((char*)&(size_buffer),sizeof(uint32_t));
	out.write((char*)&number_word_8bits,sizeof(uint32_t));
	out.write((char*)&(buffer[0]),sizeof(uint16_t)*buffer.size());
}




void OZBCBitmap::writeBuffer(std::ostream &out){
	out.write((char*)&number_word_8bits,sizeof(uint32_t));
	out.write((char*)&(buffer[0]),sizeof(uint16_t)*buffer.size());
}




void OZBCBitmap::readBitmap(std::istream &in){
	uint32_t size_buffer=0;
	uint32_t n_w=0;
	uint16_t *v=NULL;
	in.read((char*)&(size_buffer),sizeof(uint32_t));
	in.read((char*)&(n_w),sizeof(uint32_t));
	buffer.resize(size_buffer);
	v = &(buffer[0]);
	in.read((char*)v,sizeof(uint16_t)*size_buffer);
	number_word_8bits = n_w;
}




void OZBCBitmap::readBuffer(std::istream &in,size_t size_buffer){
	uint16_t *v=NULL;
	uint32_t n_w=0;
	in.read((char*)&(n_w),sizeof(uint32_t));
	buffer.resize(size_buffer);
	v = &(buffer[0]);
	in.read((char*)v,sizeof(uint16_t)*size_buffer);
	number_word_8bits = n_w;
}




void OZBCBitmap::printBitmap(FILE *f){
	if(f==NULL)
		return;

	uint32_t i, j;
	uint8_t x, t;
	fprintf(f,"NUMBER OF BITS:%u\n",(uint32_t)number_word_8bits*8);
	for(i=0;i<buffer.size();i++){
		t = (buffer[i]>>15)&1;
		for(j=16;j--;){
			x = (buffer[i]>>j)&1;
			fprintf(f,"%u",x);
			if(j==15||(j==8&&t==0))
				fprintf(f,"|");
			else if(j==8)
				fprintf(f," ");
		}
		fprintf(f,"\n");
	}
}	




void OZBCBitmap::swap(OZBCBitmap &b){
	buffer.swap(b.buffer);
	uint32_t h=number_word_8bits;
	number_word_8bits = b.number_word_8bits;
	b.number_word_8bits = h; 
}




OZBCBitmap OZBCBitmap::copyBitmap(){
	OZBCBitmap r;
	r.buffer = buffer;
	r.number_word_8bits = number_word_8bits;
	return r;
}




void OZBCBitmap::resetBitmap(){
	std::vector<uint16_t> x;
	x.swap(buffer);
	number_word_8bits = 0;
}




std::vector<uint32_t> OZBCBitmap::toVector(){
	uint32_t i, j, n=buffer.size(), pos_set=0;
	uint8_t type_word=0, dirty_word=0;
	uint16_t number_zero_word=0;
	std::vector<uint32_t> result;

	for(i=0;i<n;i++){
		type_word = (buffer[i]>>15)&1;
			
		switch(type_word){
			case 0:
				number_zero_word = (uint16_t)(buffer[i]>>8);
				pos_set += (uint32_t)(number_zero_word<<3);
				dirty_word = (uint8_t)buffer[i];
				for(j=0;j<8;j++){
					if( (dirty_word>>j)&1==1 ){
						result.push_back(pos_set+j);
					}
				}
				pos_set += 8;
				break;
				
			case 1:
				number_zero_word = buffer[i]&((1<<15)-1);
				pos_set += (uint32_t)(number_zero_word<<3);
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