#ifndef MACROOZBC
#define MACROOZBC

#include "ozbc.h"




#define ALIGN_OR_0_0(nz_words1,nz_words2,wz1,wz2,i,j, \
	v1,v2,words_zero,words,dirty_word,r) \
	nz_words1 += wz1; \
	nz_words2 += wz2; \
	i++; \
	j++; \
	if(nz_words1<nz_words2){ \
		nz_words2 -= wz2; \
		j--; \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		dirty_word = (uint8_t)v1[i-1]; \
		r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word)); \
	} \
	else if(nz_words1>nz_words2){ \
		nz_words1 -= wz1; \
		i--; \
		words_zero = nz_words2-words-1; \
		words = nz_words2; \
		dirty_word = (uint8_t)v2[j-1]; \
		r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word)); \
	} \
	else{ \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		dirty_word = (uint8_t)(v1[i-1]|v2[j-1]); \
		r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word)); \
	}




#define ALIGN_OR_1_0(nz_words1,nz_words2,wz1,wz2,i,j, \
	v1,v2,words_zero,words,dirty_word,r) \
	nz_words1 += wz1; \
	nz_words2 += wz2; \
	i++; \
	j++; \
	if(nz_words1<nz_words2){ \
		nz_words2 -= wz2; \
		j--; \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		dirty_word = (uint8_t)v1[i-1]; \
		r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word)); \
	} \
	else if(nz_words1>nz_words2){ \
		nz_words1 -= wz1; \
		i--; \
		words_zero = nz_words2-words-1; \
		words = nz_words2; \
		r.buffer.push_back((uint16_t)((words_zero<<8)|0)); \
	} \
	else{ \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		dirty_word = (uint8_t)(v1[i-1]|v2[j-1]); \
		r.buffer.push_back((uint16_t)((words_zero<<8)|dirty_word)); \
	}



#define ALIGN_OR_1_1(nz_words1,nz_words2,wz1,wz2,i,j, \
	v1,v2,words_zero,words,dirty_word,r) \
	nz_words1 += wz1; \
	nz_words2 += wz2; \
	i++; \
	j++; \
	if(nz_words1<nz_words2){ \
		nz_words2 -= wz2; \
		j--; \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		r.buffer.push_back((uint16_t)( (1<<15)|((words_zero)>>7)) ); \
		r.buffer.push_back((uint16_t)( ((words_zero)&127)<<8) ); \
	} \
	else if(nz_words1>nz_words2){ \
		nz_words1 -= wz1; \
		i--; \
		words_zero = nz_words2-words-1; \
		words = nz_words2; \
		r.buffer.push_back((uint16_t)( (1<<15)|((words_zero)>>7)) ); \
		r.buffer.push_back((uint16_t)( ((words_zero)&127)<<8) ); \
	} \
	else{ \
		words_zero = nz_words1-words-1; \
		words = nz_words1; \
		r.buffer.push_back((uint16_t)( (1<<15)|((words_zero)>>7)) ); \
		r.buffer.push_back((uint16_t)( ((words_zero)&127)<<8) ); \
	}



#endif