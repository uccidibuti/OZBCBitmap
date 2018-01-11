/*
	This code is released under the
	Gnu Lesser General Public Licensev3.0:
	https://www.gnu.org/licenses/lgpl-3.0.en.html

	(c) Lorenzo Vannucci
*/
#ifndef OZBCBITMAP
#define OZBCBITMAP


#include <inttypes.h>
#include <vector>



/*
 	OZBCBitmap is a compressed bitmap class.
	The bitmap is rappresented from
	a vector of 16bits words. 
	There are two type of words:
	1): 1bit type_word=0 | 7bit bytes_zero | 8bit byte_word
	2): 1bit type_word=1 | 15bit 128_bytes_zero 
*/
class OZBCBitmap{
private:
	std::vector<uint16_t> buffer;
	uint32_t num_bytes;

public:	
	OZBCBitmap();


	/**
	*	Set the ith bit to true (starting at zero).
	*	You must set the bitmap in increasing order:
	*	set(15), set(16), set(1000) is ok.
	*	set(0), set(100), set(50) is not ok.
	*	Once that bit ith is setted, you cannot change the value 
	*	of bits 0 to ith.
	*/
	void set(uint32_t i);


	/**
	*	Computes the logical and with another OZBCBitmap
	*	and return a new OZBCBitmap answer.
	*/
	OZBCBitmap logicaland(OZBCBitmap &b);


	/**
	*	Computes the logical or with another OZBCBitmap
	*	and return a new OZBCBitmap answer. 	
	*/
	OZBCBitmap logicalor(OZBCBitmap &b);


	/**
	*	Return the size of bitmap in bytes
	*	+ 4 bytes needed to serialize the bitmap.
	*	If portable=false return the size of bitmap in memory.	
	*/
	uint32_t size(bool portable);


	/**
	*	Write the bitmap on buffer 'b' of length 'len'
	*	('len' must be >= then 'return value')
	*	and return the number of bytes written:
	*		if(write_header == 0) 
	*			return sizeBitmapOnDisk(true).
	*		
	*		if(write_header != 0)
	*			return sizeBitmapOnDisk(false).
	*
	*	If an error occured return 0.
	*/
	uint32_t writeToBuffer(char *b, uint32_t len, bool write_header);


	/**
	*	Read a bitmap from a buffer 'b' with length 'len'
	*	saved with writeBitmapOnBuffer method
	*	and return the number of bytes readed.
	*	If you have stored the bitmap with write_header != 0,
	*	you need to set 'size'
	*	of the bitmap. Else if you have stored the bitmap
	*	with num_words==0 you don't need to know the bitmap size,
	*	in this case you must set 'size = 0'.
	*
	*	If an error occured return 0. 
	*/
  	uint32_t readFromBuffer(char *b, uint32_t len, uint32_t size);


	/**
	*	Write the bitmap on file 'f'
	*	and return the number of bytes written:
	*		if(write_header == 0) 
	*			return sizeBitmapOnDisk(true).
	*		
	*		if(write_header != 0)
	*			return sizeBitmapOnDisk().
	*
	*	If an error occured return 0.
	*/
	uint32_t writeToFile(FILE *f, bool write_header);


	/**
	*	Read a bitmap from a file 'f'
	*	saved with writeBitmapOnFile method
	*	and return the number of bytes readed.
	*	If you have stored the bitmap with write_header != 0,
	*	you need to set 'size'
	*	of the bitmap. Else if you have stored the bitmap
	*	with write_header == 0 you don't need to know bitmap size,
	*	in this case you must set 'size = 0'.
	*
	*	If an error occured return 0. 
	*/
  	uint32_t readFromFile(FILE *f, uint32_t size);


	/**
	*	Print on file each word of buffer in this format:
	*		type_word|bytes_zero|dirty_byte
	*
	*	type_word, bytes_zero and dirty_word are printed in binary.
	*	If type_word=1 there isn't dirty_byte.
	*/
	void print(FILE *f);

	
	/**
	*	Swap the content of this bitmap with another bitmap.
	*/
	void swap(OZBCBitmap &b);


	/**
	*	Return a copy of this bitmap.
	*/
	OZBCBitmap copy();


	/**
	*	Reset this bitmap and free buffer memory.
	*/
	void reset();


	/**
	*	Return a vector<uint32_t> contaning the position of the set
	*	bits in increasing order. 
	*/
	std::vector<uint32_t> toVector();


	~OZBCBitmap();
};



#endif