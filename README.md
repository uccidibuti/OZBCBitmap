# OZBC
OZBC provide an efficent compressed bitmap 
to create a bitmap indexes on high-cardinality columns.
The library also provide a basic Bitmap class which serve
as a tradiotional bitmap.

## Introduction
Bitmap indexes have traditionally been considered
to work well for low-cardinality columns,
which have a modest number of distinct values.
The simplest and most common method of bitmap indexing 
on attribute A with K cardinality associates a bitmap
with every attribute value V then the Vth bitmap rappresent
the predicate A=V. 
This approach ensures an efficient solution for performing
search but on high-cardinality attributes the size of the 
bitmap index increase dramatically.

OZBC is a run-length-encoded hybrid 
compressed bitmap designed exclusively to create
a bitmap indexes on K cardinality attributes where K>=16
and provide bitwise logical operations in running time
complexity proportianl to the compressed bitmap size.

## Why OZBC?
The fundamental property of a bitmap index composed from
K bitmap is that for each row there is only one bitmap 
setted and then for N rows there are N bits=1 independently
of the size of K. This implies that for K bitmaps and N rows
there are in total (K-1)*N bits=0 and N bits=1.
In this scenario to minimize the size of the index (composed
from K bitmap) is very important compress bits=0 sequences in
efficient mode.

Unlike WAH, EWAH, COMPAX and others compressed bitmap which
can compress also sequences of bits=1 and ensure that in the
worse case the size of a compressed bitmap is the same of
a uncompressed bitmap, OZBC can compress only sequences of 
bits=0 and in the worse case the size of a compressed bitmap
is twice of a uncompressed bitmap.

For this reason OZBC isn't designed for general purposes
but in "bitmap indexes on K-cardinality scenario" there
are K bitmaps and if there is a bitmap with a high number 
of bits setted and then double-size compared to the
corresponding uncompressed bitmap then there are others
bitmaps with all bits unsetted and then with a minimal size.

## OZBC compared with EWAH and uncompressed Bitmap
The benchmark compares the size of the Uncompressed,
[EWAH] and OZBC bitmap index that indexes
N=100000 random values included in 0,K-1 range. 

The index size rappresent the effective cost to save K bitmaps on disk.

|-      |Uncompressed index size|EWAH32 index size|EWAH16 index size|OZBC index size|
|-------|-----------------------|-----------------|-----------------|---------------|
|K=16   |              200KBytes|        197Kbytes|        174Kbytes|       161KByte|
|K=32   |              400KBytes|        347Kbytes|        255Kbytes|       179KByte|
|K=64   |              799KBytes|        508Kbytes|        317Kbytes|       189KByte|
|K=128  |             1599KBytes|        633Kbytes|        357Kbytes|       195KByte|
|K=256  |             3193KBytes|        713Kbytes|        380Kbytes|       202KByte|
|K=512  |             6371KBytes|        760Kbytes|        396Kbytes|       229KByte|
|K=1024 |            12676KBytes|        793Kbytes|        413Kbytes|       279KByte|
|K=2048 |            25089KBytes|        820Kbytes|        459Kbytes|       335KByte|
|K=4096 |            49170KBytes|        859Kbytes|        573Kbytes|       386KByte|
|K=8192 |            93918KBytes|        928Kbytes|        675Kbytes|       439KByte|
|K=16384|           171575KBytes|       1060Kbytes|       1237Kbytes|       516KByte|
|K=32768|           282254KBytes|       1329Kbytes|       1932Kbytes|       653KByte|
|K=65536|           399388KBytes|       1905Kbytes|       2941Kbytes|       919KByte|

[EWAH]: https://github.com/lemire/EWAHBoolArray

## How to compile
To compile and generate static library "lib/libOZBC":
- make

To run main test:
- make test 

## Example
See [main_test].

[main_test]: main_test.cpp

## Licensing
Gnu Lesser General Public Licensev3.0.

## Structure of OZBCBitmap
See [OZBCBitmap].

[OZBCBitmap]: /headers/ozbc.h

