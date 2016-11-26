/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
#ifndef BITMAP
#define BITMAP


#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <iostream>
#include <vector>



/*
    Bitmap is a uncompressed bitmap class.
    The bitmap is rappresented from a vector<uint64_t>.
*/
class Bitmap{
private:
    std::vector<uint64_t> buffer;

 public:
    Bitmap();


    void set(const uint32_t i);


    void swap(Bitmap &x);


    void logicaland(Bitmap &b,Bitmap &out);


    void logicalor(Bitmap &b,Bitmap &out);


    /*
        Computes the logical and with another Bitmap and
        write the result in this bitmap. 
    */
    void logicaland(Bitmap &b);


    /*
        Computes the logical or with another Bitmap and
        write the result in this bitmap. 
    */
    void logicalor(Bitmap &b);


    uint32_t sizeBitmapOnDisk();


    void resizeBitmap(uint32_t size);


    uint32_t writeBitmap(FILE *fp);


    uint32_t readBitmap(FILE *fp);


    /*
        Return a vector<uint32_t> contaning the position of the set
        bits in increasing order. 
    */
    std::vector<uint32_t> toArray();


    void freeBitmap();


    ~Bitmap();
};



#endif
