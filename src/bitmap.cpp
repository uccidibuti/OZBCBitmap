/*
    This code is released under the
    Gnu Lesser General Public Licensev3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html
    
    (c) Lorenzo Vannucci
*/
#include "bitmap.h"


Bitmap::Bitmap(){}




void Bitmap::set(const uint32_t i){
	const uint64_t n=(i/64), z=1, size=buffer.size();

    if(n<size){
      	buffer[n] = buffer[n]|(z<<(i%64));
      	return;
    }
    else{
      	buffer.resize(n+1);
      	buffer[n] = buffer[n]|(z<<(i%64));
      	return;
    }	
}




void Bitmap::swap(Bitmap &x){
	x.buffer.swap(buffer);
}





void Bitmap::logicaland(Bitmap &b,Bitmap &out){
    if(buffer.size()<b.buffer.size())
    	out.buffer.resize(buffer.size());
    else
        out.buffer.resize(b.buffer.size());
		
    size_t i;
    for(i=out.buffer.size();i--;)
        out.buffer[i]=( buffer[i] & b.buffer[i] );
}




void Bitmap::logicalor(Bitmap &b,Bitmap &out){
   	if(buffer.size()<b.buffer.size())
     	out.buffer.resize(b.buffer.size());
    else
      	out.buffer.resize(buffer.size());
		
    size_t i;
    for(i=out.buffer.size();i--;)
      	out.buffer[i]=( buffer[i] | b.buffer[i] );
}




void Bitmap::logicaland(Bitmap &b){
    std::vector<uint64_t> *q1=&buffer, *q2=&(b.buffer);
    size_t p1=q1->size(), p2=q2->size();
    if(p2<p1){
      	p1 = p2;
      	q1->resize(p1);
    }
		
    int i;
    uint64_t *o, *o1=&((*q1)[0]), *o2=&((*q2)[0]);	
		
    for(i=p1;i--;){
      	o = (o1+i);
      	(*o) = (*o)&o2[i];
    }
}




void Bitmap::logicalor(Bitmap &b){
    std::vector<uint64_t> *q1=&buffer, *q2=&(b.buffer);
    size_t p1=q1->size(), p2=q2->size();
    if(p2>p1){
      	p1 = p2;
      	q1->resize(p1);
    }
		
    int i;
    uint64_t *o, *o1=&((*q1)[0]), *o2=&((*q2)[0]);	
		
    for(i=p1;i--;){
      	o = (o1+i);
      	(*o) = (*o)|o2[i];
    }
}




uint32_t Bitmap::sizeBitmapOnDisk(){
    return (uint32_t)(buffer.size()*sizeof(uint64_t)+sizeof(uint32_t));
}




void Bitmap::resizeBitmap(uint32_t size){
    if(size%64==0)
      	buffer.resize(size/64);
    else
      	buffer.resize(size/64+1);
}




uint32_t Bitmap::writeBitmap(FILE *fp){
    if(fp==NULL)
      	return 0;

    size_t r=0;
    uint32_t dim[1];
    dim[0] = (uint32_t)buffer.size();
    uint64_t *w=&(buffer[0]);
    r = fwrite(dim,sizeof(uint32_t),1,fp);
    r += fwrite(w,sizeof(uint64_t),buffer.size(),fp);
    return (uint32_t)r;	
}




uint32_t Bitmap::readBitmap(FILE *fp){
    if(fp==NULL)
      	return 0;

    size_t r=0;
    uint32_t dim[1];
    r = fread(dim,sizeof(uint32_t),1,fp);
    if(dim[0]!=0){
      	uint64_t *array=NULL;
      	array = (uint64_t*)malloc(sizeof(uint64_t)*dim[0]);
      	if(array==NULL)
        	return 0;

      	r += fread(array,sizeof(uint64_t),dim[0],fp);
      	std::vector<uint64_t>().swap(buffer);

      	std::vector<uint64_t> c1(array,array+dim[0]);
      	buffer.swap(c1);
    }
    else
      	std::vector<uint64_t>().swap(buffer);
    
    return (uint32_t)r;
}




std::vector<uint32_t> Bitmap::toArray(){
    std::vector<uint32_t> ans;
    uint32_t i, j, n=buffer.size();
    for(i=0;i<n;i++){
        for(j=0;j<64;j++){
            if( ((buffer[i]>>j)&1)==1 ){
                ans.push_back( (uint32_t)((n<<6)|j) );
            }
        }
    }
    return ans;
}




void Bitmap::freeBitmap(){
    std::vector<uint64_t>().swap(buffer);
}




Bitmap::~Bitmap(){
	Bitmap::freeBitmap();
}
