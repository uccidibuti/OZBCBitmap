/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ozbcbitmap.h"


/*----------------------------------------------------------------------------*/


#define OZBC_MAX_128_BYTES_ZERO ((1<<15) - 1) /* 2^15 - 1 */
#define OZBC_MAX_BYTES_ZERO (OZBC_MAX_128_BYTES_ZERO << 7) /* 2^7 * (2^15 - 1) = 2^22 - 2^7 */


/*----------------------------------------------------------------------------*/


#define ALIGN_OR_0_0(scanned_bytes1, scanned_bytes2,                           \
    bytes_in_word1, bytes_in_word2, i, j,                                      \
	v1, v2, bytes_zero, count_bytes, dirty_byte, r)                            \
	scanned_bytes1 += bytes_in_word1;                                          \
	scanned_bytes2 += bytes_in_word2;                                          \
	i++;                                                                       \
	j++;                                                                       \
	if(scanned_bytes1 < scanned_bytes2) {                                      \
		scanned_bytes2 -= bytes_in_word2;                                      \
		j--;                                                                   \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		dirty_byte = (uint8_t)v1[i - 1];                                       \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));        \
	}                                                                          \
	else if(scanned_bytes1 > scanned_bytes2) {                                 \
		scanned_bytes1 -= bytes_in_word1;                                      \
		i--;                                                                   \
		bytes_zero = scanned_bytes2 - count_bytes - 1;                         \
		count_bytes = scanned_bytes2;                                          \
		dirty_byte = (uint8_t)v2[j - 1];                                       \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));        \
	}                                                                          \
	else {                                                                     \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		dirty_byte = (uint8_t)(v1[i - 1] | v2[j - 1]);                         \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));        \
	}


/*----------------------------------------------------------------------------*/


#define ALIGN_OR_1_0(scanned_bytes1, scanned_bytes2,                           \
	bytes_in_word1, bytes_in_word2, i, j,                                      \
	v1, v2, bytes_zero, count_bytes, dirty_byte, r)                            \
	scanned_bytes1 += bytes_in_word1;                                          \
	scanned_bytes2 += bytes_in_word2;                                          \
	i++;                                                                       \
	j++;                                                                       \
	if(scanned_bytes1 < scanned_bytes2) {                                      \
		scanned_bytes2 -= bytes_in_word2;                                      \
		j--;                                                                   \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		dirty_byte = (uint8_t)v1[i - 1];                                       \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));        \
	}                                                                          \
	else if(scanned_bytes1 > scanned_bytes2) {                                 \
		scanned_bytes1 -= bytes_in_word1;                                      \
		i--;                                                                   \
		bytes_zero = scanned_bytes2 - count_bytes - 1;                         \
		count_bytes = scanned_bytes2;                                          \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | 0));                 \
	}                                                                          \
	else {                                                                     \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		dirty_byte = (uint8_t)(v1[i - 1]);                                     \
		r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));        \
	}


/*----------------------------------------------------------------------------*/


#define ALIGN_OR_0_1(scanned_bytes1, scanned_bytes2,                           \
		     bytes_in_word1, bytes_in_word2, i, j,                             \
		     v1, v2, bytes_zero_to_add, count_bytes, dirty_byte, r)            \
  ALIGN_OR_1_0(scanned_bytes2, scanned_bytes1,                                 \
	       bytes_in_word2, bytes_in_word1, j, i,                               \
	       v2, v1, bytes_zero_to_add, count_bytes, dirty_byte, r);


/*----------------------------------------------------------------------------*/


#define ALIGN_OR_1_1(scanned_bytes1, scanned_bytes2,                           \
  bytes_in_word1, bytes_in_word2, i, j,                                        \
	v1, v2, bytes_zero, count_bytes, dirty_byte, r)                            \
	scanned_bytes1 += bytes_in_word1;                                          \
	scanned_bytes2 += bytes_in_word2;                                          \
	i++;                                                                       \
	j++;                                                                       \
	if(scanned_bytes1 < scanned_bytes2) {                                      \
		scanned_bytes2 -= bytes_in_word2;                                      \
		j--;                                                                   \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		r.buffer.push_back((uint16_t)( (1 << 15) | ((bytes_zero) >> 7)) );     \
		r.buffer.push_back((uint16_t)( ((bytes_zero) & 127) << 8) );           \
	}                                                                          \
	else if(scanned_bytes1 > scanned_bytes2) {                                 \
		scanned_bytes1 -= bytes_in_word1;                                      \
		i--;                                                                   \
		bytes_zero = scanned_bytes2 - count_bytes - 1;                         \
		count_bytes = scanned_bytes2;                                          \
		r.buffer.push_back((uint16_t)( (1 << 15) | ((bytes_zero) >> 7)) );     \
		r.buffer.push_back((uint16_t)( ((bytes_zero) & 127) << 8) );           \
	}                                                                          \
	else {                                                                     \
		bytes_zero = scanned_bytes1 - count_bytes - 1;                         \
		count_bytes = scanned_bytes1;                                          \
		r.buffer.push_back((uint16_t)( (1 << 15) | ((bytes_zero) >> 7)) );     \
		r.buffer.push_back((uint16_t)( ((bytes_zero) & 127) << 8) );           \
	}


/*----------------------------------------------------------------------------*/


OZBCBitmap::OZBCBitmap(){
	num_bytes = 0;
}


/*----------------------------------------------------------------------------*/


void OZBCBitmap::set(uint32_t i) {
	uint8_t dirty_bit = (uint8_t)(i & 7);
	uint8_t dirty_byte = (uint8_t)(1 << dirty_bit); 
	int32_t bytes_zero = (int32_t)((i >> 3) - num_bytes);

	if(bytes_zero >= 0) {
		num_bytes += bytes_zero + 1;
		if(bytes_zero < 128) {
			buffer.push_back( (uint16_t)(((bytes_zero) << 8) | dirty_byte) );
		}
		else {
			while((uint32_t)bytes_zero > OZBC_MAX_BYTES_ZERO) {
				buffer.push_back( (uint16_t)(OZBC_MAX_128_BYTES_ZERO | (1 << 15)) );
				bytes_zero -= OZBC_MAX_BYTES_ZERO;
			}
			buffer.push_back( (uint16_t)( (bytes_zero >> 7) | (1 << 15)) );
			buffer.push_back( (uint16_t)(((bytes_zero & 127) << 8) | dirty_byte) );
		}
	}
	else {
		uint32_t pos_last=buffer.size() - 1;
		buffer[pos_last] = buffer[pos_last] | dirty_byte; 
	}
}


/*----------------------------------------------------------------------------*/


OZBCBitmap OZBCBitmap::logicaland(OZBCBitmap &b) {
	uint32_t i = 0, j = 0, n1 = buffer.size(), n2 = b.buffer.size();
	uint32_t scanned_bytes1 = 0, scanned_bytes2 = 0, count_bytes = 0;
	OZBCBitmap r;

	uint16_t *v1 = NULL, *v2 = NULL;
	uint8_t type_word = 0, dirty_byte = 0;
	uint32_t bytes_in_word1 = 0, bytes_in_word2 = 0, bytes_zero = 0;
		
	v1 = &(buffer[0]);
	v2 = &(b.buffer[0]);
		

	while(i < n1 && j < n2){
		type_word = (uint8_t)( (v1[i] >> 15) | ((v2[j] >> 14) & 2) );
		switch(type_word) {
			case 0:
				bytes_in_word1 = (v1[i] >> 8) + 1;
				bytes_in_word2 = (v2[j] >> 8) + 1;
				break;
			case 1:
				bytes_in_word1 = ((uint32_t)(v1[i] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				bytes_in_word2 = (v2[j] >> 8) + 1;
				break;
			case 2:
				bytes_in_word1 = (v1[i] >> 8) + 1;
				bytes_in_word2 = ((uint32_t)(v2[j] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				break;
			case 3:
				bytes_in_word1 = ((uint32_t)(v1[i] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				bytes_in_word2 = ((uint32_t)(v2[j] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				break;
			default:
				break;	
		}

		scanned_bytes1 += bytes_in_word1;
		scanned_bytes2 += bytes_in_word2;
		i++;
		j++;

		if(scanned_bytes1 < scanned_bytes2) {
			scanned_bytes2 -= bytes_in_word2;
			j--;
		}

		else if(scanned_bytes1 > scanned_bytes2) {
			scanned_bytes1 -= bytes_in_word1;
			i--;
		}

		else if(type_word == 0) {
			bytes_zero = scanned_bytes1 - count_bytes - 1;
			dirty_byte = (uint8_t)(v1[i - 1] & v2[j - 1]);
			if(dirty_byte != 0){
					
				count_bytes = scanned_bytes1;
				if(bytes_zero < 128) {
					r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));
				}
					
				else {
					while(bytes_zero > OZBC_MAX_BYTES_ZERO) {
						r.buffer.push_back( (uint16_t)(OZBC_MAX_128_BYTES_ZERO | (1 << 15)) );
						bytes_zero -= OZBC_MAX_BYTES_ZERO;
					}
					r.buffer.push_back( (uint16_t)( (bytes_zero >> 7) | (1 << 15)) );
					r.buffer.push_back( (uint16_t)(((bytes_zero & 127) << 8) | dirty_byte) );
				}

			}//end if dirtybyte!=0	
		}//end elseif
	}//end while

	r.num_bytes = count_bytes;

	return r;
}


/*----------------------------------------------------------------------------*/


OZBCBitmap OZBCBitmap::logicalor(OZBCBitmap &b) {
	uint32_t i = 0, j = 0, n1 = buffer.size(), n2 = b.buffer.size();
	uint32_t scanned_bytes1 = 0, scanned_bytes2 = 0, count_bytes = 0;
	OZBCBitmap r;

	uint16_t *v1 = NULL, *v2 = NULL;
	uint8_t type_word = 0, dirty_byte = 0;
	uint32_t bytes_in_word1 = 0, bytes_in_word2 = 0, bytes_zero = 0;
		
	v1 = &(buffer[0]);
	v2 = &(b.buffer[0]);
		
	//make or
	while(i < n1 && j < n2) {
		type_word = (uint8_t)( (v1[i] >> 15) | ((v2[j] >> 14) & 2) );
		switch(type_word) {
			case 0:
				bytes_in_word1 = (v1[i] >> 8) + 1;
				bytes_in_word2 = (v2[j] >> 8) + 1;
				ALIGN_OR_0_0(scanned_bytes1, scanned_bytes2,
					bytes_in_word1, bytes_in_word2, i, j,
					v1, v2, bytes_zero, count_bytes, dirty_byte, r);
				break;

			case 1:
				bytes_in_word1 = ((uint32_t)(v1[i] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				bytes_in_word2 = (v2[j] >> 8) + 1;
				ALIGN_OR_0_1(scanned_bytes1, scanned_bytes2,
					bytes_in_word1, bytes_in_word2, i, j,
					v1, v2, bytes_zero, count_bytes, dirty_byte, r);
				break;

			case 2:
				bytes_in_word1 = (v1[i] >> 8) + 1;
				bytes_in_word2 = ((uint32_t)(v2[j] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				ALIGN_OR_1_0(scanned_bytes1, scanned_bytes2,
					bytes_in_word1, bytes_in_word2, i, j,
					v1, v2, bytes_zero, count_bytes, dirty_byte, r);
				break;

			case 3:
				bytes_in_word1 = ((uint32_t)(v1[i] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				bytes_in_word2 = ((uint32_t)(v2[j] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				ALIGN_OR_1_1(scanned_bytes1, scanned_bytes2,
					bytes_in_word1, bytes_in_word2, i, j,
					v1, v2, bytes_zero, count_bytes, dirty_byte, r);
				break;

			default:
				break;	
		}
	}//end while

	r.num_bytes = num_bytes;

	if(i >= n1 && j < n2) {
		type_word = (uint8_t)(v2[j] >> 15);
		switch(type_word) {
			case 0:
				dirty_byte = (uint8_t)v2[j];
				scanned_bytes2 += (v2[j] >> 8) + 1;
				bytes_zero = scanned_bytes2 - count_bytes - 1;
				r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));
				break;
			case 1:
				scanned_bytes2 += ((uint32_t)(v2[j] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				bytes_zero = scanned_bytes2 - count_bytes - 1;
				r.buffer.push_back((uint16_t)( (1 << 15) | ((bytes_zero) >> 7)) );
				r.buffer.push_back((uint16_t)( ((bytes_zero) & 127) << 8) );
				break;
			default:
				break;	
		}
		j++;
		while(j < n2){
			r.buffer.push_back(v2[j]);
			j++;
		}
		r.num_bytes = b.num_bytes;
	}
	else if(j >=n2 && i < n1) {
		type_word = (uint8_t)(v1[i] >> 15);
		switch(type_word) {
			case 0:
				dirty_byte = (uint8_t)v1[i];
				scanned_bytes1 += (v1[i] >> 8) + 1;
				bytes_zero = scanned_bytes1 - count_bytes - 1;
				r.buffer.push_back((uint16_t)((bytes_zero << 8) | dirty_byte));
				break;
			case 1:
				scanned_bytes1 += (v1[i] & OZBC_MAX_128_BYTES_ZERO) << 7;
				bytes_zero = scanned_bytes1 - count_bytes - 1;
				r.buffer.push_back((uint16_t)( (1 << 15)|((bytes_zero) >> 7)) );
				r.buffer.push_back((uint16_t)( ((bytes_zero) & 127) << 8) );
				break;
			default:
				break;	
		}
		i++;
		while(i < n1){
			r.buffer.push_back(v1[i]);
			i++;
		}
	}
	return r;
}


/*----------------------------------------------------------------------------*/


uint32_t OZBCBitmap::size(bool portable) {
	if(portable == true)
		return (buffer.size() * sizeof(uint16_t)) + (2 * sizeof(uint32_t));
		
	return (buffer.size() * sizeof(uint16_t)) + sizeof(uint32_t);
}


/*----------------------------------------------------------------------------*/


uint32_t OZBCBitmap::writeToBuffer(char *b,
	uint32_t len, bool write_header) {

  	uint32_t size_buffer = (uint32_t)buffer.size();
  	uint32_t dim_el = (uint32_t)(sizeof(uint16_t));
  	uint32_t dim_tot = (size_buffer * dim_el);
  	if((write_header == true && dim_tot + 8 <= len)
  		|| (write_header == false && dim_tot + 4 <= len)) {
    
    	if(write_header == true) {
      		memcpy(b, &size_buffer, sizeof(uint32_t));
      		b += (sizeof(uint32_t));
    	}
    	memcpy(b, &num_bytes, sizeof(uint32_t));
    	b += (sizeof(uint32_t));
    	memcpy(b, &(buffer[0]), size_buffer * sizeof(uint16_t));

    	if(write_header==true)
      		return dim_tot + (2 * sizeof(uint32_t));

    	return dim_tot + (sizeof(uint32_t));
  	}
  	return 0;
}


/*----------------------------------------------------------------------------*/


uint32_t OZBCBitmap::readFromBuffer(char *b,
	uint32_t len, uint32_t size) {

	uint32_t num_words = 0;
	if(size == 0) {
    	memcpy(&num_words, b, sizeof(uint32_t));
    	b += (sizeof(uint32_t));
    	size = num_words * sizeof(uint16_t) + 8;
  	}
  	else {
    	num_words = ((size - sizeof(uint32_t)) / sizeof(uint16_t));
	}
  	if(size <= len) {
    	buffer.resize(num_words);
    	memcpy(&num_bytes, b, sizeof(uint32_t));
    	b += (sizeof(uint32_t));
    	memcpy(&(buffer[0]), b, num_words * sizeof(uint16_t));
    	return size;
  	}
  	num_bytes = 0;
  	return 0;
}


/*----------------------------------------------------------------------------*/


uint32_t OZBCBitmap::writeToFile(FILE *f,
  bool write_header) {

  uint32_t tot = 0;
  uint32_t size_buffer = (uint32_t)buffer.size();
  uint32_t dim_el = (uint32_t)(sizeof(uint16_t));
  if(f != NULL) {
    if(write_header == true) {      
      tot += fwrite(&size_buffer, sizeof(uint32_t), 1, f);
    }
    tot += fwrite(&num_bytes, sizeof(uint32_t), 1, f);
    tot += fwrite(&(buffer[0]), sizeof(uint16_t), size_buffer, f);
    if(write_header == true && tot == (size_buffer + 2))
      return size_buffer * dim_el + (2 * sizeof(uint32_t));

    else if(write_header == false && tot == (size_buffer + 1))
      return size_buffer * dim_el + (sizeof(uint32_t));
  	}
  return 0;
}


/*----------------------------------------------------------------------------*/


uint32_t OZBCBitmap::readFromFile(FILE *f,
  	uint32_t size) {

	uint32_t tot=0, num_words=0;
	if(f != NULL) {
		if(size == 0) {
			size_t num = fread(&num_words, sizeof(uint32_t), 1, f);
			if(num != 1)
				return(0);
			size = (num_words * sizeof(uint16_t)) + 8;
    	} else {
			num_words = (size - sizeof(uint32_t)) / sizeof(uint16_t);
		}
	buffer.resize(num_words);
	tot += fread(&num_bytes, sizeof(uint32_t), 1, f);
	tot += fread(&(buffer[0]), sizeof(uint16_t), num_words, f);
	if(tot == (num_words + 1))
		return size;
	}
	num_bytes = 0;
	return 0;
}


/*----------------------------------------------------------------------------*/


void OZBCBitmap::print(FILE *f) {
	if(f == NULL)
		return;

	uint32_t i, j;
	uint8_t x, t;
	fprintf(f, "NUMBER OF BITS:%u\n",(uint32_t)num_bytes << 3);
	for(i = 0;i < buffer.size(); i++) {
		t = (buffer[i] >> 15) & 1;
		for(j = 16; j--;) {
			x = (buffer[i] >> j) & 1;
			fprintf(f,"%u", x);
			if(j == 15 || (j == 8 && t == 0))
				fprintf(f, "|");
		}
		fprintf(f, "\n");
	}
}


/*----------------------------------------------------------------------------*/


void OZBCBitmap::swap(OZBCBitmap &b) {
	buffer.swap(b.buffer);
	uint32_t h = num_bytes;
	num_bytes = b.num_bytes;
	b.num_bytes = h; 
}


/*----------------------------------------------------------------------------*/


OZBCBitmap OZBCBitmap::copy() {
	OZBCBitmap r;
	r.buffer = buffer;
	r.num_bytes = num_bytes;
	return r;
}


/*----------------------------------------------------------------------------*/


void OZBCBitmap::reset() {
	std::vector<uint16_t> x;
	x.swap(buffer);
	num_bytes = 0;
}


/*----------------------------------------------------------------------------*/


std::vector<uint32_t> OZBCBitmap::toVector() {
	uint32_t i, j, n = buffer.size(), pos_set = 0;
	uint8_t type_word = 0, dirty_byte = 0;
	uint32_t bytes_zero = 0;
	std::vector<uint32_t> result;

	for(i = 0; i < n; i++) {
		type_word = (buffer[i] >> 15) & 1;
			
		switch(type_word) {
			case 0:
				bytes_zero = (uint16_t)(buffer[i] >> 8);
				pos_set += (uint32_t)(bytes_zero << 3);
				dirty_byte = (uint8_t)buffer[i];
				for(j = 0; j < 8; j++) {
					if( (dirty_byte >> j) & 1 ==1 ) {
						result.push_back(pos_set + j);
					}
				}
				pos_set += 8;
				break;
				
			case 1:
				bytes_zero = ((uint32_t)(buffer[i] & OZBC_MAX_128_BYTES_ZERO)) << 7;
				pos_set += (uint32_t)(bytes_zero << 3);
				break;
				
			default:
				break;		
		}
	}
	return result;
}


/*----------------------------------------------------------------------------*/


OZBCBitmap::~OZBCBitmap() {
	std::vector<uint16_t>().swap(buffer);
}