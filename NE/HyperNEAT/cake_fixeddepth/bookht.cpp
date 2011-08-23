#include "switches.h"


#ifdef BOOKHT

#include <stdio.h>
#include <stdlib.h> /* malloc() */
#include <string.h> /* memset() */
#include <windows.h>

/* structs.h defines the data structures for cake++ */
#include "structs.h"

/* consts.h defines constants used by cake++ */
#include "consts.h"

/* function prototypes */
#include "cakepp.h"

int bookhtposition_to_hashkey(POSITION *p, int32 *key, int32 *lock, int color);


BOOKHT_ENTRY *bookhashtable;
BOOKHT_ENTRY *spa;

int initbookht(void)
	{
	int i=0,j;
	int32 lock, key;
	FILE *fp;
	SPA_POSITION spapos;
	BOOKHT_ENTRY spaentry;
	POSITION p;
	char *buffer;
	int items;
	int index;
	int k;
	char str[256];


	bookhashtable = NULL;
#ifdef WINMEM
	bookhashtable = VirtualAlloc(0, (BOOKHTSIZE+SPAITER)*sizeof(BOOKHT_ENTRY), MEM_COMMIT, PAGE_READWRITE);
#else
	bookhashtable = malloc((BOOKHTSIZE+SPAITER)*sizeof(BOOKHT_ENTRY));
	memset(bookhashtable,0,(BOOKHTSIZE+SPAITER)*sizeof(BOOKHT_ENTRY));
#endif

	if(bookhashtable == NULL)
		{
		printf("\ncould not allocate book hashtable memory!");
		//getch();
		}
	
	
	fp = fopen("book.spa", "rb");
	if(fp == NULL)
		{
		printf("\ncould not open bookhashtable file!");
			//getch();
		}
	printf("\nreading %s",str);
		// now, load spa position for spa position, transform position into a hashkey,
		// and store in spa hashtable
		//printf("\nsizeof(SPA_ENTRY) is %i",sizeof(SPA_ENTRY));
	buffer = malloc(65536*sizeof(SPA_POSITION));
	i = 0;
	while(!feof(fp))
		{
		// read
		items = fread(buffer,sizeof(SPA_POSITION),65536,fp);
		for(j=0;j<items;j++)
			{
			memcpy(&spapos, buffer+j*sizeof(SPA_POSITION), sizeof(SPA_POSITION));
			// transform to position
			p.bm = spapos.black & (~spapos.kings);
			p.bk = spapos.black & spapos.kings;
			p.wm = spapos.white & (~spapos.kings);
			p.wk = spapos.white & spapos.kings;
			
			// get hashkey
			bookhtposition_to_hashkey(&p, &key, &lock, spapos.staticeval);
			//printf("\nkey,lock: %u %u",key,lock);
			//printboard(&p,BLACK);
			//printf("\nsearch eval, static eval %i %i",spapos.value, spapos.staticeval);
			//getch();
			
			// transform to spaentry
			spaentry.hashkey = lock;
			spaentry.value = spapos.value;
			// we stored color in staticeval field, how ugly...
			spaentry.color = spapos.staticeval;
			
			// is this correct????
			//if(spapos.staticeval == BLACK)
			//	spaentry.value = -spapos.value;

			// save in spatable
			index = key & (BOOKHTSIZE-1);
			
			for(k=0;k<SPAITER;k++)
				{
				if(bookhashtable[index+k].hashkey == 0)
					{
					bookhashtable[index+k] = spaentry;
					}
				}
			// we don't overwrite in SPA table!
			i++;
			if(!(i%(128*1024)))
				printf("\nreading book hashtable: %i positions read",i);
			}
		}
	
	return 1;
	}

int bookht_lookup(int32 key, int32 lock, int *eval, int color)
	{
	int index;
	int i;
	index = key&(BOOKHTSIZE-1);

	
	for(i=0;i<SPAITER;i++)
		{
		if((bookhashtable[index+i].hashkey == lock) && (color==bookhashtable[index+i].color))
			{
			*eval = bookhashtable[index+i].value;
			return 1;
			}
		}
	return 0;
	}

int bookhtposition_to_hashkey(POSITION *p, int32 *key, int32 *lock, int color)
	{
//	int32 x;
	int32 lkey = 0,llock = 0;
	extern int32 Gkey, Glock; // from cakepp.c

	absolutehashkey(p);
	//printf("\nkey, lock: %i %i",Gkey, Glock);
	*key = Gkey;
	*lock = Glock;

	return 1;
	}

#endif // BOOKHT

