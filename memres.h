// Author: maxmus <christoph-keller@web.de>, (C) 2008
//
// I put this file under a Free BSD license, because I have to use it in other projects too

//
#ifndef MEMRES_H
#define MEMRES_H

template <class T, int size> class MRSData {
public:
	T data[size];
	bool used;
};

template <class T, int size> class MemResFree {
public:
	MemResFree(int sC=100) {
		freed=sum=step=0;
		actCount=startCount=sC;
		next=data[0]=new MRSData<T,size>[actCount];
		for(int k=0; k < actCount; k++)
		{
			data[step][k].used = false;
		}
		max=next+startCount;
		doRecycle=recycle=false;
		doRecycling();
	};
	
	~MemResFree() {
		clear();
		delete[] data[0];
	};
	
	void free(T* fr) {
		((MRSData<T,size>*)fr)->used=false;
		freed++;
	};

	T* reserve() {
		if(doRecycle) {
			while(recNext < recMax) {
				if(recNext->used==false) {
					recNext->used=true;
					recNext++;
					freed--;
					return (T*)(recNext-1);
				}
				else
					recNext++;
			}
			if(recStep==step) {
				doRecycle=false;
				return reserve();
			}
			else {
				recStep++;
				recActCount*=2;
				recNext=data[recStep];
				recMax=recNext+recActCount;
				return reserve();
			}
		}
		else {
			if(next < max) {
				assert(next->used == false);
				next->used=true;
				next++;
				return (T*)(next-1);
			}
			else {
				if(recycle && (freed*5 > (sum+actCount))) {
					doRecycle=true;
					recNext=data[0];
					recMax=recNext+startCount;
					recStep=0;
					recActCount=startCount;
					return reserve();
				}
				sum+=actCount;
	            step++;
	            actCount *= 2;
	            next=data[step]=new MRSData<T,size>[actCount];
				for(int k=0; k < actCount; k++)
				{
					data[step][k].used = false;
				}
	            max=next+actCount;
	            next++;
	            (next-1)->used = true;
	            return (T*)(next-1);
			}
		}
	};
	
	void clear() {
		sum=0;
		actCount=startCount;
		for(int x=1; x <= step ; x++)
			delete[] data[x];
		next=data[0];
		max=next+startCount;
		doRecycle=recycle=false;
		freed=0;
		step=0;
	};
	
	void doRecycling() {
		//after the next try to 
		//reserve sth. recycling is enabled
		recycle=true;
	};

	MRSData<T,size> *data[30];
    //The last and maximum item in the actual data[step] 
    MRSData<T,size> *next, *max;
    //which s in data[x] it cactual
    int step;
    //the number of items in data[step]
    int actCount;

    //the amount of items in data[0]
    int startCount;
	
    
    //recycle memory before reserving new?
    bool recycle, doRecycle;
    //Meaning as step, actCount
    int recStep, recActCount;
    //Meanint as next, max
    MRSData<T,size> *recNext, *recMax;
    
    //sum+actual is the amount of reserved space
    int sum;
    //The amount of items freed by MemResFree
    int freed;
};

class MemResFreeCont
{
public:
	MemResFree<int, 1> byte4;
	MemResFree<int, 2> byte8;
	MemResFree<int, 4> byte16;
	MemResFree<int, 8> byte32;
	MemResFree<int, 16> byte64;

	void* reserve(int sz)
	{
		if(sz <= 4) return byte4.reserve();
		if(sz <= 8) return byte8.reserve();
		if(sz <= 16) return byte16.reserve();
		if(sz <= 32) return byte32.reserve();
		if(sz <= 64) return byte64.reserve();
		return (void*)(new char[sz]);
	}
	void free(void* ptr, int sz)
	{
		     if(sz <= 4)  byte4.free((int*)ptr);
		else if(sz <= 8)  byte8.free((int*)ptr);
		else if(sz <= 16) byte16.free((int*)ptr);
		else if(sz <= 32) byte32.free((int*)ptr);
		else if(sz <= 64) byte64.free((int*)ptr);
		else              delete[] ptr;


	}
};

#endif
