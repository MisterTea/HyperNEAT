//
// dblookup.c
//
// (c) 2002 Martin Fierz
//
// version 2.0 for single n-piece databases
//
// contains all the necessary code to look up database positions, except for boolean
//
// todo:	
//			-> read in more than one block per disk access
//
// 
//
// some notes on this: endgame databases are huge, and efficient access is not easy.
// the databases are compressed with run-length-encoding (RLE), a scheme which translates
// a string of 11110000000111111 to (value,run-length)-pairs like this (1,4)(0,7)(1,6).
// RLE is a simple form of compression, but it is fast in decompression.
// 
// this code uses the following compression scheme: if a run length is shorter than 5,
// the next 4 values in the database (WIN/ LOSS / DRAW, defined as 0,1,2) are encoded
// directly in numbers 0..80. if the run is 5 or longer, it is saved as a compressed
// byte, a value between 81 and 254, which tells both the value of the run, and the length.
// the encoder writes the index of the original file into an "index file" every time the
// byte count reaches a multiple of 1024. this is used during decoding: if you want to look
// up the value at index X, you first search the largest index in the index file which is
// smaller than X, and decode from there. like this, you don't have to decode the whole
// file, which would be quite impractical :-)
//
// on initialization, this module builds tables which contain all information in the index
// files. these tables can be quite large! for the 8-piece db, about 20MB.
//
// the tables contain all index numbers for every 1K block of data on the disk. they also
// contain a "blockoffset", which is used to compute a unique index for every individual
// block. thus block 0 of two different databases will each have their own unique index 
// this is necessary to maintain a list of pointers to loaded blocks. once a position has
// been converted to an index, and the corresponding database has been identified, and the
// right block in this database has been found, this module looks if the pointer in the 
// blockindex array is nonzero. if yes, the block is already in memory and does not have
// to be reloaded.
//
// names:	"cachebaseaddress" is a pointer to the base address of a large region of memory used 
//					for db caching. the cache consist of cachesize 1K-blocks.
//			
//			"BLOCKNUM" is the number of blocks the database occupies on disk. it is needed to 
//					allocate the array "blockpointer[BLOCKNUM]", which holds a pointer to every 
//					block. naturally, if not all blocks can be held in memory, there will be null
//					pointers in this array, indicating that a block is not loaded.
//
//			"blockinfo" is an array of size cachesize with elements next,prev(ious) and uniqueid.
//					the blockinfo array is a doubly linked list, which is used to implement a LRU
//					buffering scheme. that's what next and prev are for. uniqueid tells what block
//					is at this address in the cache. this is necessary to unload it in case a block
//					is thrown out
// 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <windows.h>
//#include <winbase.h>
#include <assert.h>
#include <ctype.h>
#include <algorithm>
using namespace std;

#include "cake_misc.h"

// TODO: why are these things redefined here??

#ifdef WIN32
typedef __int64 int64;
#else
typedef long long int int64;
#endif
#define int32 unsigned int


#include "dblookup.h"





// definition of a structure for compressed databases
typedef struct compresseddatabase
	{
	int ispresent;			// does db exist?
	int numberofblocks;		// how many disk blocks does this db need?
	int blockoffset;		// offset to calculate unique block id for the blocks of this db
	int firstblock;			// where is the first block in it's db?
	int startbyte;			// at this byte
	int databasesize;		// index range for this database
	int value;				// WIN/LOSS/DRAW if single value, UNKNOWN == 0 else
	int *idx;				// pointer to an array of index numbers, array is 
							// allocated dynamically, to size n
	int fp;					// which file is it in?
	} cprsubdb;




static int64 getdatabasesize(int bm, int bk, int wm, int wk, int bmrank, int wmrank);
static int choose(int n, int k);
int LSB(unsigned int x);
int MSB(unsigned int  x);
static int recbitcount(unsigned int n);
static int bitcount(unsigned int n);
static int parseindexfile(char idxfilename[256],int blockoffset,int fpcount);


#ifdef PRELOAD
static int preload(char out[256]);
#endif

#define hiword(x) (((x)&0xFFFF0000)>>16)
#define loword(x) ((x)&0xFFFF)

static cprsubdb cprsubdatabase[MAXPIECE+1][MAXPIECE+1][MAXPIECE+1][MAXPIECE+1][7][7][2]; // db subslice info
// TODO: what is this stupid ** doing?


static unsigned char **blockpointer;	// pointers to the memory address of block #i, using unique block id i.
										// watch out for 32/64-bit compatibility problem
static unsigned char *cachebaseaddress; // allocate cache memory to this pointer


// a doubly linked list for LRU caching
static struct bi
	{
	int uniqueid; // which block is in this
	int next;     // which is the index of the next blockinfo in the linked list
	int prev;     // which is the index of the last blockinfo in the linked list
	} *blockinfo;

static int head,tail; // array index of head and tail of the linked list.


// run-length-decoder definitions and variables.
#define SKIPS 58
#define MAXSKIP 10000

static const int skip[SKIPS]={5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,36,40,44,48,52,56,60,70,80,90,100,150,200,250,300,400,500,650,800,1000,1200,1400,1600,2000,2400,3200,4000,5000,7500,MAXSKIP};
static int runlength[256]; // holds the run length for every byte
static int value[256];		// holds the value for every byte

static int bicoef[33][33]; // binomial coefficients

// in principle, we could ifdef the two following arrays with WIN64 or USE_ASM, but they
// are so small that they shouldn't hurt.
//static char LSBarray[256];
static char MSBarray[256];

// TODO: why do we have this in here?? we already have it in cake...
static char bitsinword[65536];
static int32 revword[65536];
static int maxblockid = 0;  // maximal unique block id, set by initlookup after checking what files are here

#define MAXFP 50
static FILE *dbfp[MAXFP]; // file pointers to db2...dbn - always open.
static char dbnames[MAXFP][256]; // write in here what each dbfp is pointing to

// database path
char DBpath[256];

// number of db buffers
static int maxblocknum;
// max number of idx buffers
static int maxidx;
static int maxpieces;
static int maxpiece;
static int cachesize;
static int bytesallocated = 0;

static char dbinfo[1024] = "";

int db_getcachesize(void)
	{
	return cachesize;
	}

int dblookup(POSITION *q,int cl)
	{
	// returns DB_WIN, DB_LOSS,  DB_DRAW or DB_UNKNOWN for a position in a database lookup.
	// from a compressed database. first, this function computes the index
	// of the current position. 
	// then, it loads the index file of that database to determine in which
	// 1K-block that index resides. 
	// finally, it reads and decompresses that block to find the value of the
	// position.
	
	int32 index;
	int bm, bk, wm, wk, bmrank=0, wmrank=0;
	
	// next 4 lines for inlined version
	int i;
	int32 x,y;
	int32 bmindex=0,bkindex=0,wmindex=0,wkindex=0;
	int32 bmrange=1, wmrange=1, bkrange=1;
	int blocknumber;	
	int uniqueblockid;
	//int index512 = 0;

	int reverse = 0;
	int returnvalue=DB_UNKNOWN;
	int memsize = 0;
	unsigned char *diskblock;
	int newhead,prev, next;

	int n;
	int n1,n2,n3;
	int *idx;
	unsigned char byte;
	POSITION revpos;
	POSITION p;
	cprsubdb *dbpointer;
	FILE *errorlogfp;
	
	// get a copy of the board, because we may have to invert the board for a 
	// lookup.
	p=*q;

	// adjust color for different definition in dblookup
	p.color = q->color & 1;
	// set bm, bk, wm, wk, and ranks - bitcount is in bool.c

	// TODO: bm,bk,wm,wk are already known from cakepp.c - why are they being recomputed here??
	// add a MATERIALCOUNT structure to the call...
	bm = bitcount(p.bm);
	bk = bitcount(p.bk);
	wm = bitcount(p.wm);
	wk = bitcount(p.wk);
	
	// if one side has nothing, return appropriate value
	// this means you can call lookup positions
	// where one side has no material - unlike chinook db.

	// TODO: does this ever happen in cake? if no, we could throw this out
	/*if(bm+bk==0)
		return p.color==DB_BLACK?DB_LOSS:DB_WIN;
	if(wm+wk==0)
		return p.color==DB_WHITE?DB_LOSS:DB_WIN;
	*/
	//assert(bm+bk>0);
	//assert(wm+wk>0);

	if( (bm+wm+wk+bk>maxpieces) || (bm+bk>maxpiece) || (wm+wk>maxpiece))
		return DB_UNKNOWN;	

	if(bm)
		bmrank = MSB(p.bm)/4;
	if(wm)
		wmrank = (31-LSB(p.wm))/4;	
	
	// if this position is dominated by the "wrong" side, we have to
	// reverse it!
	if (( ((wm+wk-bm-bk)<<16) + ((wk-bk)<<8) + ((wmrank-bmrank)<<4) + p.color) > 0)
		reverse = 1;

	if (reverse)
		{
		// white is dominating this position, change colors
		revpos.bm = revert(p.wm);
		revpos.bk = revert(p.wk);
		revpos.wm = revert(p.bm);
		revpos.wk = revert(p.bk);
		revpos.color = p.color^1;
		p = revpos;
		
		reverse = bm;
		bm = wm;
		wm = reverse;
		
		reverse = bk;
		bk = wk;
		wk = reverse;
		
		reverse = bmrank;
		bmrank = wmrank;
		wmrank = reverse;
		
		reverse = 1;
		}


// before we do a real lookup, check if this db has only one value:
	
	// get pointer to db
	dbpointer = &cprsubdatabase[bm][bk][wm][wk][bmrank][wmrank][p.color];
	
	// check presence: this is important for the following reason: slices
	// are stuffed into single files; db6.cpr for instance. if MAXPIECES/MAXPIECE
	// were different during generation and in this code, then it is possible that
	// there are slices which this code thinks are present, but in fact, they are not!
	if(dbpointer->ispresent == 0 || (dbfp[dbpointer->fp] == NULL))
		return DB_UNKNOWN;
	// check if the db contains only a single value
	if(dbpointer->value != DB_UNKNOWN)
		return dbpointer->value;

	// uninlined version of positiontoindex would be only this
	// positiontoindex(p, &index, color);
	
	// but i'm using a long inlined version - should be a bit faster
	
/*	

          WHITE
   	   28  29  30  31           
	 24  25  26  27           
	   20  21  22  23          
	 16  17  18  19           
	   12  13  14  15          
	  8   9  10  11          
	    4   5   6   7           
	  0   1   2   3           
	      BLACK
*/



	// first, we set the index for the black men:
	i=1;
	y=p.bm;
	while(y)
		{
		x=LSB(y);
		y=y^(1<<x);
		bmindex+=bicoef[x][i];
		i++;
		}

	// next, we set it for the white men, disregarding black men:
	i=1;
	y=p.wm;
	while(y)
		{
		x=MSB(y);
		y=y^(1<<x);
		x=31-x;
		wmindex+=bicoef[x][i];
		i++;
		}

	// then, black kings. this time, we include interfering black and white men.
	i=1;
	y=p.bk;
	while(y)
		{
		x=LSB(y);
		y=y^(1<<x);
		// next line is the count of men on squares 0....x-1, as x-1 of a 0000010000000 number is 0000000111111111
		x-=bitcount((p.bm|p.wm)&((1<<x)-1)); 
		bkindex+=bicoef[x][i];
		i++;
		}

	// last, white kings, with interfering other pieces
	i=1; 
	y=p.wk;
	while(y)
		{
		x=LSB(y);
		y^=(1<<x);
		x-=bitcount((p.bm|p.bk|p.wm) & ( (1<<x)-1 ) );
		wkindex+=bicoef[x][i];
		i++;
		}



	if(bm)
		bmrange = bicoef[4*(bmrank+1)][bm] - bicoef[4*bmrank][bm];
	if(wm)
		wmrange = bicoef[4*(wmrank+1)][wm] - bicoef[4*wmrank][wm];
	if(bk)
		bkrange = bicoef[32-bm-wm][bk];

	if(bmrank)
		bmindex -= bicoef[4*bmrank][bm];
	if(wmrank)
		wmindex -= bicoef[4*wmrank][wm];

	index = bmindex + wmindex*bmrange + bkindex*bmrange*wmrange + wkindex*bmrange*wmrange*bkrange;


	// end uninlined version

	// now we know the index, and the database, so we look in the .idx file to find the
	// right memory block

	idx = dbpointer->idx;

	

	n= dbpointer->numberofblocks;

	
	// TODO: check if the n<8 is of any use - probably not. it would simplify the code
	// if it was not here.
	if(n<8)
		{
		// now the array idx[] contains the index number at the start of every block.
		// we do a stupid linear search to find the block:
		blocknumber=0;
		// changed the order of the two conditions for cake sans souci 1.02; 
		// it was possible before that idx[n] got read. now thanks to the && behavior
		// of not evaluating the second clause, it is no longer possible.
		while((blocknumber<n) && ((unsigned int)idx[blocknumber] <= index) )
			blocknumber++;
		// we overshot our target - blocknumber is now the first larger idx.
		blocknumber--;
		}
	else
		{
		// try for a better search: binary division search
		n1=0;n2=n;

		while(n2>n1+1)
			{
			n3=(n1+n2)/2;
			if((unsigned int)idx[n3]<=index)
				n1=n3;
			else
				n2=n3;
			}
		blocknumber = n1;
		}

	// we now have the blocknumber inside the database slice in which the position is located.
	// get the unique number which identifies this block
	uniqueblockid = dbpointer->blockoffset+
					dbpointer->firstblock +
					blocknumber;


	// check if it is loaded:
	if(blockpointer[uniqueblockid] != NULL)
		//yes!
		{
		diskblock = blockpointer[uniqueblockid];
		
		// update LRU linked list
		// 1): which index does this block have in the array?
		newhead = int( (diskblock - cachebaseaddress)/1024 );
		if(newhead!=head)
			{
			if(newhead == tail)
				{
				// special case: we will have to reset tail too
				prev = blockinfo[newhead].prev;
				blockinfo[prev].next = -1;
				tail = prev;
				
				blockinfo[head].prev = newhead;
				blockinfo[newhead].next = head;
				blockinfo[newhead].prev = -1;
				head = newhead;
				}
			else
				{
				// remove index "newhead" from doubly linked list:
				// relink previous and next block of newhead
				prev = blockinfo[newhead].prev;
				next = blockinfo[newhead].next;
				blockinfo[prev].next = next;
				blockinfo[next].prev = prev;
				
				// set info for newhead
				blockinfo[newhead].prev = -1;
				blockinfo[newhead].next = head;

				// set info for head
				blockinfo[head].prev = newhead;
				
				head = newhead;
				}
			}
		}
	
	else
		// we must load it
		{
		// if the lookup was a "conditional lookup", we don't load the block
		if(cl==1)
			return DB_NOT_LOOKED_UP;

		//------------------------------------------------
		// new LRU code
		// get address to load it to: write it into tail.

		diskblock = cachebaseaddress + 1024*tail;
		//printf("write to tail %i ",tail);
		// and save it in the blockpointer array
		blockpointer[uniqueblockid]=diskblock;
		// reset blockpointer entry of whatever was there before
		// i use -1 for an empty cache block - nothing to unload...
		// blockpointer[blockid] points to the memory address
		if ( blockinfo[tail].uniqueid !=-1 )
			{
			//printf("overwrite %i  ",cacheindex[tail]);
			blockpointer[blockinfo[tail].uniqueid]=NULL;
			}
		// blockpointer array is updated

		// blockinfo array contains information on the linked list - must update it
		// steps: set current head's previous item
		//        get a new tail, and set it's next item to -1
		//        write new block into space where old tail was as new head
		//        set it's 3 items.

		newhead = tail;
		tail = blockinfo[tail].prev;
		blockinfo[tail].next = -1;
		blockinfo[newhead].uniqueid=uniqueblockid;
		blockinfo[newhead].next = head;
		blockinfo[newhead].prev = -1;
		
		blockinfo[head].prev = newhead;
		head = newhead;
		
	

		// move to the disk block we are looking for
		//fseek(dbfp[bm+bk+wm+wk],blocknumber*1024,SEEK_SET);
		// now, we want to load a block. we have to seek the
		// right position - that is in this file, blocknumber+firstblock
		fseek(dbfp[dbpointer->fp],(blocknumber+dbpointer->firstblock)*1024,SEEK_SET);
		// and read it
		//i = fread(diskblock,1024,1,dbfp[dbpointer->fp]);
		fread(diskblock,1024,1,dbfp[dbpointer->fp]);

#ifdef PRINT
	printf("\nblock with ID %i, loaded to address %i",uniqueblockid,diskblock);
#endif
		}
	// the block we were looking for is now pointed to by diskblock
	// and it has been moved to the head of the linked list
	// now we decompress the memory block
	// get n, the offset index for this diskblock
	
	reverse=0;
	if(	dbpointer->numberofblocks>blocknumber+1)
		//we are not at the last block
		{
		if(idx[blocknumber+1]-index < index-idx[blocknumber])
			reverse=1;
		}

	if(reverse)
		{
		n=idx[blocknumber+1];
		i=1023;
		while((unsigned int)n>index /*&& i>=0*/)
			{
			n-=runlength[diskblock[i]];
			i--;
			}
		i++;
	
		}
	else
		{
		n = idx[blocknumber];

		// and move through the bytes until we overshoot index
		// set the start byte in this block: if it's block 0 of a db, this can be !=0
		i=0;
		if(blocknumber == 0)
			i = dbpointer->startbyte;

		// LOOKUP LOOP - executes about 500 times on average
		// this could be speeded up by storing the index of the first 512 loops

		while((unsigned int)n<=index /*&& i<1024*/)
			{
			n+=runlength[diskblock[i]];
			i++;
			}
		

		//printf("\n%i",i);

		// once we are out here, n>index
		// assert(n>index);
		// we overshot again, move back:
		i--;
		n-=runlength[diskblock[i]];

		// error check:
		//TODO: make this only in debug version - hope that the load of the last block
		// has solved the dberr problem. ehm, not hope, but check if we ever see dberr.txt
		// again!
		if(i>=1024)
			{
			// log error:
			errorlogfp = fopen("dberr.txt","a");
			if(errorlogfp != NULL)
				{
				fprintf(errorlogfp,"db index overflow in bm %u bk %u wm %u wk %u bmrank %u wmrank %u index %u\n",bm,bk,wm,wk,bmrank,wmrank,index);
				fclose(errorlogfp);
				}
			}
		//bytenumber = i;
		}
		// finally, we have found the byte which describes the position we
	// wish to look up. it is diskblock[i].
	//if(reverse)
	//	printf("\nbytenumbers reverse: %i nonreverse %i",bytenumber,i);	
	if(diskblock[i]>80)
		{
		// t'was a compressed byte - easy
		returnvalue = value[diskblock[i]];
		byte = diskblock[i];
		}
	else
		{
		// an uncompressed byte
		byte = diskblock[i];
		i = index-n; // should be 0,1,2,3

		switch(i)
			{
			case 0:
				returnvalue = byte % 3;
				break;
			case 1:
				returnvalue = (byte/3) % 3;
				break;
			case 2:
				returnvalue = (byte/9) % 3;
				break;
			case 3:
				returnvalue = (byte/27) % 3;
				break;
			}
		}
	
	returnvalue++;


	return returnvalue;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////
// initialization stuff below



int64 getdatabasesize(int bm, int bk, int wm, int wk, int bmrank, int wmrank)
	{
	// returns the range of database indices for this database.
	// needs binomial coefficients in the array bicoef[][] = choose from n, k
	int64 dbsize = 1;

	// number of bm configurations:
	// there are bm black men subject to the constraint that one of them is on 
	// the rank bmrank

	if(bm)
		dbsize *= bicoef[4*(bmrank+1)][bm] - bicoef[4*bmrank][bm];
  
	if(wm)
		dbsize *= bicoef[4*(wmrank+1)][wm] - bicoef[4*wmrank][wm];

	// number of bk configurations
	if(bk)
		dbsize *= bicoef[32-bm-wm][bk];

	// number of wk configurations
	if(wk)
		dbsize *= bicoef[32-bm-wm-bk][wk];


	return dbsize;
	}




//------------------------------------------------------------------
// boolean stuff below


// table-lookup bitcount
int bitcount(int32 n)
	// returns the number of bits set in the 32-bit integer n 
	{
	return (bitsinword[n&0x0000FFFF]+bitsinword[(n>>16)&0x0000FFFF]);
	}

int MSB(int32 x)
	{
	//-----------------------------------------------------------------------------------------------------
	// returns the position of the most significant bit in a 32-bit word x 
	// or -1 if not found, if x=0.
	// LSB can maybe be implemented more efficiently on other CPU's which have a 
	// this operation.
	//-----------------------------------------------------------------------------------------------------

    /*
	if(_BitScanReverse(&returnvalue,x))
		return returnvalue;
	else
		return -1;
    */
	

    //old non-intrinsic and portable code	
	if(x&0xFF000000)
		return(MSBarray[(x>>24)&0xFF]+24);
	if(x&0x00FF0000)
		return(MSBarray[(x>>16)&0xFF]+16);
	if(x&0x0000FF00)
		return(MSBarray[(x>>8)&0xFF]+8);
	return(MSBarray[x&0xFF]);
	}

int revert(int32 n)
	// reverses a 4-byte integer
	// needed to reverse positions
	{
	return (revword[hiword(n)] + (revword[loword(n)]<<16));
	}


int recbitcount(int32 n)
	// counts & returns the number of bits which are set in a 32-bit integer
	//	slower than a table-based bitcount if many bits are
	//	set. used to make the table for the table-based bitcount on initialization
	{
	int r=0;
	while(n)
		{
		n=n&(n-1);
		r++;
		}
	return r;
	}


int choose(int n, int k)
	{
	// returns the binomial coefficient for choosing k of n
	int result = 1;
	int i;

	i=k;
	while(i)
		{
		result *= (n-i+1);
		i--;
		}

	i=k;
	while(i)
		{
		result /=i;
		i--;
		}

	return result;
	}



//-------------------------------------------
// initialization below
int db_exit(void)
	{
	// clean up dblookup module, free all memory
	int i;

    /*
	i = VirtualFree(cachebaseaddress,cachesize*1024,MEM_DECOMMIT);
	i = VirtualFree(cachebaseaddress,0,MEM_RELEASE);
	if(i == 0)
		{
		i = GetLastError();
	//	fprintf(fp,"error code is %i",i);
		}
	
	i = VirtualFree(blockpointer,maxblocknum*sizeof(int),MEM_DECOMMIT);		
	i = VirtualFree(blockpointer,0,MEM_RELEASE);
	
	i = VirtualFree(blockinfo,cachesize*sizeof(struct bi),MEM_DECOMMIT);
	i = VirtualFree(blockinfo,0,MEM_RELEASE);
    */

    free(cachebaseaddress);
    free(blockpointer);
    free(blockinfo);
	
	for(i=0;i<50;i++)
		{
		if(dbfp[i] != NULL)
			fclose(dbfp[i]);	
		}

	return 1;
	}

int db_init(int suggestedMB, char out[256])
	// was int initdblookup(char out[256])
	{
	// returns the number of dbpieces that can be looked up, i.e. the largest db found
	// parameters: suggestedMB which is what dblookup will allocate if possible
	// however, if only the 6pc db is present, it will allocate only as much 
	// as necessary.
	// out is a buffer where dbinit can tell users what its doing.
	//extern FILE *cake_fp;
	FILE *fp;
	char dbname[256];
	int i,j,n,nb,nw;
	int bm,bk,wm,wk;
	int singlevalue=0;
	int blockoffset = 0;
	int autoloadnum = 0;
	int fpcount = 0;
	int pifreturnvalue;
	int pieces=0;
	int memsize;
	char str[256];


	
	//dblogfp = fopen("dbinit.txt","w");
	// initialize bitsinword, the number of bits in a word
	
	for(i=0;i<65536;i++)
		bitsinword[i]=recbitcount((int32)i);
	

	// initialize revword, the reverse of a word.
	for(i=0;i<65536;i++)
		{
		revword[i]=0;
		for(j=0;j<16;j++)
			{
			if(i&(1<<j))
				revword[i] +=1<<(15-j);
			}
		}

	// initialize LSB / MSB arrays

	for(i=0;i<256;i++)
		{
		if(i&1) MSBarray[i] = 0;
		if(i&2) MSBarray[i] = 1;
		if(i&4) MSBarray[i] = 2;
		if(i&8) MSBarray[i] = 3;
		if(i&16) MSBarray[i] = 4;
		if(i&32) MSBarray[i] = 5;
		if(i&64) MSBarray[i] = 6;
		if(i&128) MSBarray[i] = 7;
		
        /*
		if(i&128) LSBarray[i] = 7;
		if(i&64) LSBarray[i] = 6;
		if(i&32) LSBarray[i] = 5;
		if(i&16) LSBarray[i] = 4;
		if(i&8) LSBarray[i] = 3;
		if(i&4) LSBarray[i] = 2;
		if(i&2) LSBarray[i] = 1;
		if(i&1) LSBarray[i] = 0;
    	*/	
		}

	// initialize binomial coefficients
	// bicoef[n][k] is supposed to be the number of ways you can choose k from n
	for(i=0;i<33;i++)
		{
		for(j=1;j<=i;j++)
			{
			// choose j from i:
			bicoef[i][j] = choose(i,j);
			}
		// define choosing 0 for simplicity 
		bicoef[i][0] = 1;
		}

	// choosing n from 0: bicoef = 0
	for(i=1;i<33;i++)
		bicoef[0][i]=0;

	// initialize runlength and lookup array 
	for(i=0;i<81;i++)
		runlength[i]=4;

	for(i=81;i<256;i++)
		{
		runlength[i]= skip[(i-81)%SKIPS];
		value[i]= ((i-81)/SKIPS);
		}


	// set the ispresent flag to zero for all databases, because it later gets set to 1, and
	// it is not quite clear that this will not accidentally be one!
	memset(cprsubdatabase, 0, (MAXPIECE+1)*(MAXPIECE+1)*(MAXPIECE+1)*(MAXPIECE+1)*98*sizeof(cprsubdb));
	
			


	// detect largest present database and put the number of pieces in variable pieces:
	for(n=2;n<SPLITSIZE;n++)
		{
		sprintf(dbname,"cake_db/db%i.idx",n);
		fp = fopen(dbname,"rb");
		if(fp)
			{
			pieces=n;
			fclose(fp);
			}
		else
			break;
        }

	// continue detection on 8-piece db
	// it seems like this is the only change necessary to be able to run
	// incomplete set of endgame databases!
	for(n=SPLITSIZE;n<=8;n++)
		{
		for(nb=4;nb<=4;nb++)
			{
			nw=n-nb;
			if(nw>nb)
				continue;
			for(bk=0;bk<=nb;bk++)
				{
				bm=nb-bk;
				for(wk=0;wk<=nw;wk++)
					{
					wm=nw-wk;
					if(bm+bk==wm+wk && wk>bk)
						continue;
					sprintf(dbname,"cake_db/db%i_%i%i%i%i.cpr",bm+bk+wm+wk,bm,bk,wm,wk);
					//printf("\nattempting to open %s",dbname);
					fp = fopen(dbname,"rb");
					if(fp)
						{
						pieces = 8;
						fclose(fp);
						}
					}
				}
			}
		}

	//printf("\nmax number of pieces = %i",pieces);
	//getch();

	// assign values according to pieces"
	switch(pieces)
		{
		case 2:
			;
		case 3:
			;
		case 4:
			maxidx = MAXIDX4;
			maxblocknum = BLOCKNUM4;
			maxpieces = 4;
			maxpiece = 3;
			break;
		case 5:
			maxidx = MAXIDX5;
			maxblocknum = BLOCKNUM5;
			maxpieces = 5;
			maxpiece = 3;
			break;
		case 6:
			maxidx = MAXIDX6;
			maxblocknum = BLOCKNUM6;
			maxpieces = 6;
			maxpiece = 3;
			break;
		case 7:
			maxidx = MAXIDX7;
			maxblocknum = BLOCKNUM7;
			maxpieces = 7;
			maxpiece = 4;
			break;
		case 8:
			maxidx = MAXIDX8;
			maxblocknum = BLOCKNUM8;
			maxpieces = 8;
			maxpiece = 4;
			break;
		}
	
	// get the number of blocks in cache
	cachesize = 1024*suggestedMB;
	if(maxpieces <6)
		cachesize = 2000;
	if(maxpieces == 6)
		cachesize = 42000;
	if(maxpieces > 6)
		{
		// reduce number of buffers by 20'000
		// because then the suggested size in MB will be about 
		// the total dblookup ram usage.
		cachesize = max(cachesize, MINCACHESIZE);
		}

	// parse index files
	// blockoffset is the total number of blocks in all dbs belonging to an index file.
	blockoffset = 0;
	for(n=2;n<=maxpieces;n++)
		{
		if(n>=8)
			continue;
		sprintf(dbname,"cake_db/db%i.idx",n);
		sprintf(out,"parsing %s",dbname);
		fp = fopen(dbname,"rb");
		// another new change 
		if(fp)
			{
			strcat(dbinfo,dbname);
			strcat(dbinfo,"\n");
			logtofile(out);
			fclose(fp);
			}
		// parse next index file
		pifreturnvalue = parseindexfile(dbname,blockoffset,fpcount);
		
		// if the index file is not present, or if an error occurs during parsing,
		// we do nothing. else, we update the max number of pieces.
		if(pifreturnvalue >= 0)
			{
			blockoffset += pifreturnvalue;
			sprintf(str,"  %i blocks",blockoffset);
			logtofile(str);
			fpcount++;
			}
		}
	

	for(n=SPLITSIZE;n<=maxpieces;n++)
		{
		for(nb=maxpieces-maxpiece;nb<=maxpiece;nb++)
			{
			nw=n-nb;
			if(nw>nb)
				continue;
			for(bk=0;bk<=nb;bk++)
				{
				bm=nb-bk;
				for(wk=0;wk<=nw;wk++)
					{
					wm=nw-wk;
					if(bm+bk==wm+wk && wk>bk)
						continue;
					// ok, found a valid db, now do the do: 
					sprintf(dbname,"cake_db/db%i_%i%i%i%i.idx",bm+bk+wm+wk,bm,bk,wm,wk);
					sprintf(out,"parsing %s",dbname);
					fp = fopen(dbname,"rb");
					if(fp != NULL)
						{
						strcat(dbinfo,dbname);
						strcat(dbinfo,"\n");
						logtofile(out);
						fclose(fp);
						}
					pifreturnvalue = parseindexfile(dbname,blockoffset,fpcount);
					if(pifreturnvalue >= 0)
						{
						blockoffset += pifreturnvalue;
						sprintf(str, "  %i blocks",blockoffset);
						logtofile(str);
						fpcount++;
						}
					}
				}
			}
		}
	logtofile("index files parsed");
	sprintf(str,"allocated %i KB for indexing",bytesallocated/1024);
	logtofile(str);
	// index files are parsed!
	
	
	// allocate memory for the cache
	memsize = cachesize*1024;
	//cachebaseaddress = VirtualAlloc(0,memsize,MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,PAGE_READWRITE);
	cachebaseaddress = (unsigned char*)malloc(memsize);


	//cachebaseaddress = VirtualAlloc(0,CACHESIZE*1024,MEM_RESERVE,PAGE_READWRITE);
	
	if(cachebaseaddress == NULL && memsize!=0)
		{	
		sprintf(str,"\ncould not allocate DB cache (%i KB)",(cachesize));
		logtofile(str);
        /*
		error = GetLastError();
		sprintf(str,"\nerror code %i",error);
		logtofile(str);
        */
		exit(0);
		}
	sprintf(str,"\nallocated %i KB for DB cache ",(cachesize));
	logtofile(str);

	// allocate memory for blockpointers
	// statement below was sizeof(int) which returned 4 even on the 64-bit version of windows?!
	memsize = maxblocknum*sizeof(blockpointer);
	//blockpointer = VirtualAlloc(0,memsize,MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_READWRITE);
	blockpointer = (unsigned char**)malloc(memsize);
	if(blockpointer == NULL && memsize != 0 )
		{	
		sprintf(str,"\ncould not allocate blockpointer array (%i blocks)",maxblocknum);
		logtofile(str);
        /*
		error = GetLastError();
		sprintf(str,"\nerror code %i",error);
		logtofile(str);
        */
		exit(0);
		}
	sprintf(str,"\nallocated %i KB for block pointer array",maxblocknum/256);
	logtofile(str);
	// set blockpointers to NULL - this statement crashes on 64-bit machine
	for(i=0;i<maxblocknum;i++)
		blockpointer[i]=NULL;
	
	// allocate memory for doubly linked list LRU
	memsize = cachesize*sizeof(struct bi);
	//blockinfo = VirtualAlloc(0,memsize, MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN, PAGE_READWRITE);
	blockinfo = (bi*)malloc(memsize);
	if(blockinfo == NULL && memsize!=0)
		{	
		sprintf(str,"\ncould not allocate LRU list (%i KB)",(cachesize)*sizeof(struct bi)/1024);
		logtofile(str);
        /*
		error = GetLastError();
		sprintf(str,"\nerror code %i",error);
		logtofile(str);
        */
		exit(0);
		}
	sprintf(str,"\nallocated %i KB for LRU linked list",(cachesize)*sizeof(struct bi)/1024);
	logtofile(str);

	//getch();

#ifdef PRELOAD
	autoloadnum = preload(out);
#endif

	// prepare arrays which describe linked list
	// todo: if we want to autoload all blocks with a blocknumber < X (which preload would return)
	// then we have to go from for i=0 to for i=X 

	autoloadnum=0;
	for(i=autoloadnum;i<cachesize;i++)  //0
		{
		blockinfo[i].next = i+1;
		blockinfo[i].prev = i-1;
		blockinfo[i].uniqueid = i;
		}
	blockinfo[cachesize-1].next=-1;
	blockinfo[autoloadnum].prev=-1; //0
	
	head=autoloadnum; //0
	tail = cachesize-1;



	// open file pointers for each db: keep open all the time.
	fpcount=0;
	for(n=2;n<SPLITSIZE;n++)
		{
		sprintf(dbname, "cake_db/db%i.cpr",n);
		dbfp[fpcount] = fopen(dbname,"rb");
		sprintf(dbnames[fpcount],"%s",dbname);
		if(dbfp[fpcount]==NULL)
			{
			sprintf(str,"\ndbfp[%i] is null!",fpcount);
			logtofile(str);
			//getch();
			//exit(0);
			}
		fpcount++;
		}


	for(n=SPLITSIZE;n<=maxpieces;n++)
		{
		for(nb=maxpieces-maxpiece;nb<=maxpiece;nb++)
			{
			nw=n-nb;
			if(nw>nb)
				continue;
			for(bk=0;bk<=nb;bk++)
				{
				bm=nb-bk;
				for(wk=0;wk<=nw;wk++)
					{
					wm=nw-wk;
					if(bm+bk==wm+wk && wk>bk)
						continue;
					sprintf(dbname,"cake_db/db%i_%i%i%i%i.cpr",bm+bk+wm+wk,bm,bk,wm,wk);
					dbfp[fpcount] = fopen(dbname, "rb");
					sprintf(dbnames[fpcount],"%s",dbname);
					if(dbfp[fpcount]==NULL)
						{
						printf("\n%s: dbfp[%i] is null!",dbname,fpcount);
						//getch();
						//exit(0);
						}
					fpcount++;
					}
				}
			}
		}
	
	return maxpieces;
	}

#ifdef PRELOAD
static int preload(char out[256])
	{
	// preloads the entire db or until the cache is full.
	FILE *fp;
	int n,nb,nw;
	unsigned char *diskblock;
	int cachepointer = 0;
	int bm,bk,wm,wk;
	char dbname[256];
	int blockoffset = 0;
	int autoloadnum = 0;
	int uniqueid = 0;


	for(n=2;n<SPLITSIZE;n++)
		{
		sprintf(dbname, "cake_db/db%i.cpr",n);
		fp = fopen(dbname,"rb");
		if(fp==NULL)
			break;
		printf("\npreloading %i-piece db",n);
		while(!feof(fp))
			{
			// get a memory address to write block to:
			diskblock = cachebaseaddress + 1024*cachepointer;
			// and save it in the blockpointer array
			blockpointer[cachepointer] = diskblock;
			// cacheindex array tells which block id is at a certain place in cache - must update it
			blockinfo[cachepointer].uniqueid = cachepointer; //change to uniqueid
			// say what we're doing
			
			if(!(cachepointer%1024))
				{
				sprintf(out,"preload block %i in %s",cachepointer,dbname);
				printf("\nreading block %i",cachepointer);
				}

			// read it

			// this was weird: fread statement was at the end of this loop before july 30 2007 - 
			// meaning that the last block of the cache was never read into memory?
			fread(diskblock,1024,1,fp);

			cachepointer++;
			//uniqueid++;
			// if we have preloaded the entire cache. no use going on.
			if(cachepointer == cachesize)
				return 1;
			

			}
		fclose(fp);
		// if we get to the autoload limit, we remember the block number up to which 
		// blocks will remain permanently in memory.

		if(n==AUTOLOADSIZE)
			autoloadnum = cachepointer;
		}

	// continue preload on largest db
	for(n=SPLITSIZE;n<=maxpieces;n++)
		{
		for(nb=maxpieces-maxpiece;nb<=maxpiece;nb++)
			{
			nw=n-nb;
			if(nw>nb)
				continue;
			for(bk=0;bk<=nb;bk++)
				{
				bm=nb-bk;
				for(wk=0;wk<=nw;wk++)
					{
					wm=nw-wk;
					if(bm+bk==wm+wk && wk>bk)
						continue;
					sprintf(dbname,"cake_db/db%i_%i%i%i%i.cpr",bm+bk+wm+wk,bm,bk,wm,wk);
					printf("\npreloading %i%i%i%i db",bm,bk,wm,wk);
					fp = fopen(dbname,"rb");
					if(fp==NULL)
						continue;
					printf("\npreloading %i%i%i%i db",bm,bk,wm,wk);
					while(!feof(fp))
						{
						// get a memory address to write block to:
						diskblock = cachebaseaddress + 1024*cachepointer;
						// and save it in the blockpointer array
						blockpointer[cachepointer]=diskblock;
						// cacheindex array tells which block id is at a certain place in cache - must update it
						blockinfo[cachepointer].uniqueid=cachepointer; //change to uniqueid
						// say what we're doing
						sprintf(out,"preload block %i in %s",cachepointer,dbname);
						if(!(cachepointer%1024))
							printf("\nreading block %i",cachepointer);
						
						// WARNING: here and above, the // read it statement was at the end of this loop,
						// AFTER if cachepointer == cachesize return 1 - i have no idea why!
						// read it
						fread(diskblock,1024,1,fp);

						
						cachepointer++;
						//uniqueid++;
						// if we have preloaded the entire cache. no use going on.
						if(cachepointer == cachesize)
							return 1;
						
						}
					}
				}
			}
		}
	return autoloadnum;
	}
#endif

static int parseindexfile(char idxfilename[256],int blockoffset,int fpcount)
	// parse an index file and write all necessary information in *dbpointer.
	// it returns the number of blocks in this database - we need this to compute unique block id
	// if the index file is not present, it returns -1
	// if any other error occurs, it returns -2
	// read file line by line
	{
	FILE *fp;
	char c,colorchar;
	int bm,bk,wm,wk,bmrank,wmrank,color;
	int singlevalue,startbyte;
	int idx[MAXIDX8];
	int num=0;
	int firstblock = 0;
	int stat,stat0;
	cprsubdb *dbpointer;
	char str[256];

	sprintf(str,"parsing index file %s",idxfilename);
	logtofile(str);

	fp = fopen(idxfilename,"r");
	if(fp==0)
		{
		printf("\ncannot open index file %s",idxfilename);
		return -1;
		}

	while (1) {

		// At this point it has to be a BASE line or else end of file. 
		stat = fscanf(fp, " BASE%i,%i,%i,%i,%i,%i,%c:",
				&bm, &bk, &wm, &wk, &bmrank, &wmrank, &colorchar);

		// Done if end-of-file reached. 
		if (stat <= 0)
			break;
		else if (stat < 7) {
			printf("Error parsing!\n");
			return -2;
		}

		// Decode the color. 
		if (colorchar == 'b')
			color = DB_BLACK;
		else
			color = DB_WHITE;

		// Get the rest of the line.  It could be a n/n, or it could just
		// be a single character that is '+', '=', or '-'.
		 
		stat = fgetc(fp);
		if (isdigit(stat)) {
			ungetc(stat, fp);
			stat0 = fscanf(fp, "%d/%d", &firstblock, &startbyte);
			if (stat0 < 2) {
				stat = fscanf(fp, "%c", &c);
				if (stat < 1) {
					printf("Bad line\n");
					return -2;
				}
			}
			dbpointer = &cprsubdatabase[bm][bk][wm][wk][bmrank][wmrank][color];
			dbpointer->firstblock = firstblock;		// which block in db. 
			dbpointer->blockoffset = blockoffset;	// which block overall. 
			dbpointer->ispresent = 1;				// watch out: this should be initialized to 0 for all!
			dbpointer->value = 0;
			dbpointer->startbyte = startbyte;
			dbpointer->fp = fpcount;


			// now, we got the first line, maybe there are more... 
			// count the number of blocks in the db.
			 
			num = 1;
			idx[0] = 0;		// first block is index 0. 
			while (fscanf(fp, "%d", idx + num) == 1) {
				num++;

				// Check for too many indices.
				if (num == maxidx) {
					printf("reached maxidx\n");
					return -2;
				}
			}

			// We stopped reading numbers. 
			dbpointer->numberofblocks = num;
			dbpointer->idx = (int*)malloc(num * sizeof(int));
			bytesallocated += num*sizeof(int);
			if (dbpointer->idx == NULL) {
				printf("malloc error for idx array!\n");
				return -2;
			}
			memcpy(dbpointer->idx, idx, num * sizeof(int));
		}
		else {
			switch (stat) {
			case '+':
				singlevalue = DB_WIN;
				break;
			case '=':
				singlevalue = DB_DRAW;
				break;
			case '-':
				singlevalue = DB_LOSS;
				break;
			default:
				printf("Bad singlevalue line \n");
				return(0);
			}
			dbpointer = &cprsubdatabase[bm][bk][wm][wk][bmrank][wmrank][color];
			dbpointer->blockoffset = 0;
			dbpointer->firstblock = 0;
			dbpointer->idx = NULL;
			dbpointer->ispresent = 1;
			dbpointer->numberofblocks = 0;
			dbpointer->value = singlevalue;
			dbpointer->fp = fpcount;
		}
	}
	fclose(fp);

	// num holds the last number of blocks in a non-single-valued block, 
	// firstblock has the offset relative to the start of the file. the
	// sum of the two is the number of blocks associated with this index file.
	return num+firstblock;
	}

void db_infostring(char *str)
	{
	// print db information in str
	
	sprintf(str,"\nDatabase Details:\n%s",dbinfo);

	return;
	}
