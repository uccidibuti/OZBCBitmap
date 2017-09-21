
# OZBCBitmap
OZBC provides an efficent compressed bitmap 
to create bitmap indexes on high-cardinality columns.


## Introduction
Bitmap indexes have traditionally been considered
to work well for low-cardinality columns,
which have a modest number of distinct values.
The simplest and most common method of bitmap indexing 
on attribute A with K cardinality associates a bitmap
with every attribute value V then the Vth bitmap rapresent
the predicate A=V. 
This approach ensures an efficient solution for performing
search but on high-cardinality attributes the size of the 
bitmap index increase dramatically.

OZBC is a run-length-encoded hybrid 
compressed bitmap designed exclusively to create
a bitmap indexes on L cardinality attributes where L>=16
and provide bitwise logical operations in running time
complexity proportianl to the compressed bitmap size.

## Why OZBC?
The fundamental property of a bitmap index composed from
L bitmap is that for each row there is only one bitmap 
setted and then for N rows there are N bits=1 independently
of the size of L. This implies that for L bitmaps and N rows
there are in total (L-1)*N bits=0 and N bits=1.
In this scenario to minimize the size of the index (composed
from L bitmap) is very important compress bits=0 sequences in
efficient mode.

Unlike WAH, EWAH, COMPAX and others compressed bitmap which
can compress also sequences of bits=1 and ensure that in the
worse case the size of a compressed bitmap is the same of
a uncompressed bitmap, OZBC can compress only sequences of 
bits=0 and in the worse case the size of a compressed bitmap
is twice of a uncompressed bitmap.

For this reason OZBC isn't designed for general purposes
but in "bitmap indexes on L-cardinality scenario" there
are L bitmaps and if there is a bitmap with a high number 
of bits setted and then double-size compared to the
corresponding uncompressed bitmap then there are others
bitmaps with all bits unsetted and then with a minimal size.

## OZBC compared with EWAH and uncompressed Bitmap
The benchmark compares the size of [Roaring],
[EWAH] and OZBC bitmap index that indexes
N=1000000 (1M) random and sorted values included in 0,L-1 range. 

Each index is composed from L bitmaps.

The index size rapresent the effective cost to save L bitmaps on disk.

![Alt text](./pictures/size_unsorted.jpg?raw=true "")

![Alt text](./pictures/size_sorted.jpg?raw=true "")


[EWAH]: https://github.com/lemire/EWAHBoolArray
[Roaring]: https://github.com/RoaringBitmap/CRoaring

## How to compile
To compile and generate static library "lib/libOZBC":
- make

To run index test:
- make index

To run perf test:
- make perf

To run serialize test:
- make serialize


## Example
See [Index_example].

[Index_example]: ./test/index.cpp

## Encoding
OZBCBitmap encodes bits in 16bits words.
There are two types of words:

    |1bit type_word=0|7bit    bytes_zero|8bit     dirty_word|
    |1bit type_word=1|         15bit 128_bytes_zero         |

where:
- bytes_zero = number of consecutive sequence of 8bit of zeros.
- dirty_word = sequence of 8bit uncompressed.
- 128_bytes_zero = number of consecutive sequence of 1024bit of zeros.

The max size of this compressed bitmap is twice the size of the same
uncompressed bitmap.

The max number of consecutive zero bits that can be rapresented
from a single word is ((2^15)-1)*(2^10)=(2^25)bits.

## Licensing
Gnu Lesser General Public Licensev3.0.

## Structure and API of OZBCBitmap
See [OZBCBitmap].

[OZBCBitmap]: /headers/ozbcbitmap.h

