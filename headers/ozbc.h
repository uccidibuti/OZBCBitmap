/*
	This code is released under the
	Gnu Lesser General Public Licensev3.0:
	https://www.gnu.org/licenses/lgpl-3.0.en.html

	(c) Lorenzo Vannucci
*/
#ifndef OZBC
#define OZBC


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <vector>



/*
	OZBCBitmap is a compressed bitmap class.
	The bitmap is rappresented from
	a vector of 16bits words. 
	There are two type of words:
	1): 1bit type_word=1 | 15bit bytes_zero 
	2): 1bit type_word=0 | 7bit bytes_zero | 8bit dirty_word

	where:
		bytes_zero = number of consecutive 8bit of zeros:
			if(type_word=1) => max(bytes_zero)=(2^15)-1 
			else => max(bytes_zero)=(2^7)-1
		
		dirty_word = sequence of 8bits uncompressed.

	The max size of this compressed bitmap is twice the size of the same
	uncompressed bitmap.
*/
class OZBCBitmap{
private:
	std::vector<uint16_t> buffer;
	uint32_t number_word_8bits;

public:	
	OZBCBitmap();


	/*
		Set the ith bit to true (starting at zero).
		You must set the bitmap in increasing order:
		set(15), set(16), set(1000) is ok.
		set(0), set(100), set(50) is not ok.
		Once that bit ith is setted, you cannot change the value 
		of bits 0 to ith.
	*/
	void set(uint32_t i);


	/*
		Append a sequence of 8 dirty-bits. 
	*/
	void appendLiteralWord(uint8_t word);


	/*
		Computes the logical and with another OZBCBitmap
		and return a new OZBCBitmap answer. 
	*/
	OZBCBitmap logicaland(OZBCBitmap b);


	/*
		Computes the logical or with another OZBCBitmap
		and return a new OZBCBitmap answer. 	
	*/
	OZBCBitmap logicalor(OZBCBitmap b);


	/*
		Computes the logical not
		and return a new OZBCBitmap answer. 
	*/
	OZBCBitmap logicalnot();


	/*
		Computes the logical xor with another OZBCBitmap
		and return a new OZBCBitmap answer. 	
	*/
	OZBCBitmap logicalxor(OZBCBitmap b);


	/*
		Return the size of buffer + 4 bytes. 
	*/
	uint32_t sizeBufferOnDisk();


	/*
		Return the size of buffer + 8 bytes.		
	*/
	uint32_t sizeBitmapOnDisk();


	/*
		Write the bitmap on ostream. The file format is:
			4 byte for the size of the buffer,
			4 byte for the number_words_8bit,
			buffer content.
		The	number of bytes written are sizeBitmapOnDisk().	
	*/
	void writeBitmap(std::ostream &out);


	/*
		Write the buffer on ostream. The file format is:
			4 byte for the number_words_8bit,
			buffer content.
		The	number of bytes written are sizeBufferOnDisk().	
	*/
	void writeBuffer(std::ostream &out);


	/*
		Read a bitmap saved with writeBitmap method from istream. 
	*/
	void readBitmap(std::istream &in);


	/*
		Read a bitmap saved with writeBuffer method from istream.
		You need know the size of the buffer. 
	*/
	void readBuffer(std::istream &in,size_t size_buffer);


	/*
		Print on file each word of buffer in this format:
			type_word|bytes_zero|dirty_word

		type_word, bytes_zero and dirty_word are printed in binary.
		If type_word=1 there isn't dirty_word.
	*/
	void printBitmap(FILE *f);

	
	/*
		Swap the content of this bitmap with another bitmap.
	*/
	void swap(OZBCBitmap &b);


	/*
		Return a copy of this bitmap.
	*/
	OZBCBitmap copyBitmap();


	/*
		Reset this bitmap and free buffer memory.
	*/
	void resetBitmap();


	/*
		Return a vector<uint32_t> contaning the position of the set
		bits in increasing order. 
	*/
	std::vector<uint32_t> toVector();


	~OZBCBitmap();
};



#endif