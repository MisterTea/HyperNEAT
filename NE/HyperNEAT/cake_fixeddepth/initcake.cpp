#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
//#include <winbase.h>
#include <assert.h>
#include "structs.h"
#include "consts.h"
#include "switches.h"
#include "cakepp.h"
#include "initcake.h"
#include "cake_misc.h"
#include <string.h>


#ifdef BOOK
HASHENTRY *loadbook(int *bookentries, int *bookmovenum)
// loads the cake book file and returns a pointer to the book,
// sets bookentries to the number of entries in the opening book.
{
	FILE *Lfp;
	int i,j;
	char Lstr[256];
	static HASHENTRY *book;


	Lfp = fopen("cake_engines/book.bin","rb");
	if(Lfp == NULL)
		{
		/* book file not found */
		book = NULL;
		printf("ERROR: no opening book detected\n");
                exit(1);
		logtofile(Lstr);
		return 0;	
		}

	fscanf(Lfp,"%i",bookentries);
#ifdef WINMEM
	book = VirtualAlloc(0, bookentries*sizeof(struct bookhashentry), MEM_COMMIT, PAGE_READWRITE);
#else
	book = (HASHENTRY*)malloc((*bookentries)*sizeof(struct bookhashentry));
#endif

	if (book == NULL)
	{
		sprintf(Lstr,"malloc failure in initcake (book malloc failed)");
		logtofile(Lstr);
		return NULL;
	}

	fread(book,sizeof(struct bookhashentry),(*bookentries),Lfp);

	sprintf(Lstr,"allocated %i KB for book hashtable",sizeof(struct bookhashentry)*(*bookentries)/1024);
	logtofile(Lstr);
	sprintf(Lstr,"book hashtable with %i entries allocated\n",(*bookentries));
	logtofile(Lstr);

	j=0;
	for(i=0;i<(*bookentries);i++)
		{
		if(book[i].lock != 0)
			j++;
		}

	sprintf(Lstr,"%i moves in opening book\n",j);
	logtofile(Lstr);

	*bookmovenum = j;

	fclose(Lfp);
	return book;
	}

#endif

int initbitoperations(unsigned char bitsinword[65536], unsigned char LSBarray[256])
{
	int i;

	for(i=0;i<65536;i++)
		bitsinword[i] = recbitcount((int32)i);
	
	//for(i=0;i<256;i++)
	//	bitsinbyte[i] = recbitcount((int32)i);

	// initialize LSBarray;
	for(i=0;i<256;i++)
		{
		if(i&128) LSBarray[i] = 7;
		if(i&64) LSBarray[i] = 6;
		if(i&32) LSBarray[i] = 5;
		if(i&16) LSBarray[i] = 4;
		if(i&8) LSBarray[i] = 3;
		if(i&4) LSBarray[i] = 2;
		if(i&2) LSBarray[i] = 1;
		if(i&1) LSBarray[i] = 0;
		}
	return 1;
}


HASHENTRY *inithashtable(int hashsize)
	{
	// allocate memory for the hashtable. 
	// align the hashtable on a 64-byte-boundary
	// terminate program if hashtable cannot be allocated.
	char Lstr[256];
	unsigned long long int i;
	HASHENTRY *ptr;

    printf("Creating hashtable of size %ld\n",((hashsize+HASHITER)*sizeof(HASHENTRY)+256));
#ifdef WINMEM
    printf("WINMEM is defined\n");
	hashtable = VirtualAlloc(0, (hashsize+HASHITER)*sizeof(HASHENTRY), MEM_COMMIT, PAGE_READWRITE);
#else
	ptr = (HASHENTRY*)malloc((hashsize+HASHITER)*sizeof(HASHENTRY)+256);
#endif
    printf("Hashtable created at %X\n",ptr);

	if(ptr == NULL)
		{
		printf("malloc failure in initcake (hashtable memory allocation failed)");
		sprintf(Lstr,"malloc failure in initcake (hashtable memory allocation failed)");
		logtofile(Lstr);
		exit(0);
		}

	// TODO: the code below generates warnings - can i do that suomehow without warnings?
	// we have a hashtable, now align it on a 64-bit-boundary:
	i = (unsigned long long int)ptr;
    i += (64-(i%64));
	ptr = (HASHENTRY*)i;

	return ptr;
}


int initializematerial(short materialeval[13][13][13][13])
	{
	// initialize material value array for value[bm][bk][wm][wk]
	int i,j,k,l;
	int v1,v2;

	for(i=0;i<13;i++)
		{
		for(j=0;j<13;j++)
			{
			for(k=0;k<13;k++)
				{
				for(l=0;l<13;l++)
					{
					/*bm bk wm wk */
					v1 = 100*i + 130*j;
					v2 = 100*k + 130*l;
					if(v1+v2 == 0) 
						continue;
					v1 = v1-v2+(EXBIAS*(v1-v2))/(v1+v2);

					/* take away the 10 points which a side is up over 100 with a one
						man advantage in the 12-11 position */
					if(v1<=-100) 
						v1+=10;
					if(v1>=100) 
						v1-=10;
					
					materialeval[i][j][k][l] = v1;
					}
				}
			}
		}
	return 1;
	}

	
int initializebackrank(char blackbackrankeval[256], char whitebackrankeval[256], char blackbackrankpower[256], char whitebackrankpower[256])
//int initializebackrank(void)
	{
	// initializes the arrays   blackbackrankeval
	//							whitebackrankeval
	//							blackbackrankpower
	//							whitebackrankpower

	static int br[32]={0,0,2, 2, 4,6,10,10,1,4,16,16,6,10,24,16,
					   0,0,2, 2, 4,10,16,16,1,4,16,16,6,10,24,16};

	// contains the back rank evaluation 
	// strange: pdntool insists that the back rank 0 x 0 x (bridge) is worse than 0 x x 0 which
	//	leaves the double corner open
	// should try a change there! 
	POSITION p;
	int32 u;
	int32 index;
	const int devsinglecornerval = 5;
	const int intactdoublecornerval = 5;
	
	const int oreoval = 8; 
	
	const int idealdoublecornerval = 4; 
	

	/* 		WHITE
		28  29  30	31
	  24  25  26  27
		20  21  22	23
	  16  17  18  19
		12  13  14	15
	   8  9	  10  11
		 4   5	6	 7
	   0   1   2   3
			BLACK */

	for(u=0; u<256; u++)
		{
		// for all possible back ranks do: 
		p.bm = u;
		p.wm = u<<24;
		
		index = p.bm&0x0000000F;
		if(p.bm & BIT7) 
			index += 16;
		blackbackrankeval[u] = br[index];

		index = 0;
		if(p.wm & BIT31) 
			index++;
		if(p.wm & BIT30) 
			index+=2;
		if(p.wm & BIT29) 
			index+=4;
		if(p.wm & BIT28) 
			index+=8;
		if(p.wm & BIT24) 
			index+=16;

		whitebackrankeval[u] = br[index];
	 
	/* 		WHITE
		28  29  30	31
	  24  25  26  27
		20  21  22	23
	  16  17  18  19
		12  13  14	15
	   8  9	  10  11
		 4   5	6	 7
	   0   1   2   3
			BLACK */

		/* oreo */
		// TODO: use squares instead of bits
		if( match1(p.bm, SQ2|SQ3|SQ7)) 
			{
			blackbackrankeval[u] += oreoval; 
			}
		if( match1(p.wm, SQ31|SQ30|SQ26) ) 
			{
			whitebackrankeval[u] += oreoval; 
			}

		// the stuff below doesn't work at all!
		/*if( match1(p.bm, (SQ2|SQ5|SQ6)) && !(p.bm&SQ1))
			blackbackrankeval[u] += 2;
		if( match1(p.wm, (SQ31|SQ28|SQ27)) && !(p.wm&SQ32))
			whitebackrankeval[u] += 2;*/

	
		// developed single corner
		// TODO: make this more fine-grained: if men on 4 and 8, subtract something. if
		//		 only man on 4, but not on 8, subtract less
		//       if only man on 8 but not on 4 subract the least.
		
		if( ((~p.bm)&SQ4) && ((~p.bm)&SQ8) ) 
			blackbackrankeval[u] += devsinglecornerval; //devsinglecorner 5!!
		if( ((~p.wm)&SQ29) && ((~p.wm)&SQ25) ) 
			whitebackrankeval[u] += devsinglecornerval;//2!!

		// developed single corner bonus only if there is a man on sq 3/30
		/*if(p.bm & SQ3)
			{
			if((~p.bm) & SQ4)
				{
				blackbackrankeval[u] += 2;
				if((~p.bm) & SQ8)
					blackbackrankeval[u] += 2;
				}
			}

		if(p.wm & SQ30)
			{
			if((~p.wm) & SQ29)
				{
				whitebackrankeval[u] += 2;
				if((~p.wm) & SQ25)
					whitebackrankeval[u] += 2;
				}
			}*/

		
		//  double corner evals: intact and developed 
		
		if(match1(p.bm, (SQ2|SQ5|SQ6)) && !(p.bm&SQ1))
			blackbackrankeval[u] += intactdoublecornerval;
		if(match1(p.wm, (SQ27|SQ28|SQ31)) && !(p.wm&SQ32))
			whitebackrankeval[u] += intactdoublecornerval;


		// ideal double corner
	/* 		WHITE
		28  29  30	31
	  24  25  26  27
		20  21  22	23
	  16  17  18  19
		12  13  14	15
	   8  9	  10  11
		 4   5	6	 7
	   0   1   2   3
			BLACK */
		if( (p.bm&BIT3) && (p.bm&BIT2) && (p.bm&BIT6) && (!(p.bm&BIT7)))
			blackbackrankeval[u] += idealdoublecornerval;//2!!
		// maybe also allow 2,6,7 &!3 to be ideal?
		if( (p.wm&BIT28) && (p.wm&BIT29) && (p.wm&BIT25) && (!(p.wm&BIT24)))
			whitebackrankeval[u] += idealdoublecornerval;//2!!

		// new 1.41'
		// maybe also allow 2,6,7 &!3 to be ideal?
		if( (p.bm&BIT7) && (p.bm&BIT2) && (p.bm&BIT6) && (!(p.bm&BIT3)))
			blackbackrankeval[u] += intactdoublecornerval;//2!!
		if( (p.wm&BIT24) && (p.wm&BIT29) && (p.wm&BIT25) && (!(p.wm&BIT28)))
			whitebackrankeval[u] += intactdoublecornerval;//2!!	

	

		// backrankpower gives the power of the back rank to withstand onslaught
		// by enemy men - only used in the case of man down.

		// TODO: power is non-zero if black has a bridge for instance
		// TODO: write this with squares to make sure it's readable.
		if(match1(p.bm, 0xE))
			blackbackrankpower[u] = 30;
		if(match1(p.bm, 0x2E) || match1(p.bm, 0x4E))
			blackbackrankpower[u] = 35;
		if(match1(p.bm, 0x6E))
			blackbackrankpower[u] = 40;

		if(match1(p.wm, 0x70000000))
			whitebackrankpower[u] = 30;
		if(match1(p.wm, 0x72000000) || match1(p.wm, 0x74000000))
			whitebackrankpower[u] = 35;
		if(match1(p.wm, 0x76000000))
			whitebackrankpower[u] = 40;
		}
	
	return 1;
	}



int initxors(int *ptr)
{

	// constants for xoring to make hashcode 
	// the hash 'lock' has it's last two bits cleared to store the color 
	// like this in old code: hashxors[1][i][j]&=0xFFFFFFFC; 

	int32 xors[256]= {690208388U, 1385187825U, 3014056764U, 610143516U,
							3004321224U, 520313469U, 1574226940U, 319654154U,
							546984174U, 306787062U, 2907641074U, 1808019639U,
							4129460637U, 2966067458U, 746618565U, 3001731532U,
							3205813886U, 2213500745U, 1640878366U, 1927683568U,
							47704121U, 303969140U, 3567244452U, 841151692U,
							1221824373U, 3854628651U, 241127424U, 2063930631U,
							410030810U, 2235459861U, 2524122841U, 3572976202U,
							4195793681U, 2046710491U, 4098216920U, 3311425853U,
							1982714050U, 1231714597U, 2973461157U, 3601402759U,
							354261653U, 2129242266U, 1421843175U, 2419599569U,
							1582227760U, 2682430946U, 1698179654U, 2054584514U,
							3660148030U, 884947366U, 1829692268U, 499892739U,
							252970341U, 3320261774U, 4087909886U, 4288729147U,
							2272817997U, 3686942273U, 2138108071U, 2858117139U,
							3401481631U, 4256714600U, 2580402957U, 2791723827U,
							1582099459U, 3023113898U, 3391619032U, 2032621890U,
							3448538912U, 2274708656U, 2582068657U, 2902999529U,
							2682525855U, 4266447137U, 2406699840U, 3396326863U,
							1522427175U, 2713609638U, 842271020U, 1455872925U,
							2290169953U, 4183085287U, 227682589U, 3681819898U,
							2962577397U, 2373245323U, 2784393661U, 3772866703U,
							1533184650U, 3850848827U, 2227618737U, 4000673539U,
							273718216U, 2331437470U, 829214116U, 2358776063U,
							1472680025U, 52985693U, 877551676U, 1118965035U,
							3090117602U, 3632583455U, 383140641U, 2552966190U,
							3169896974U, 2457120653U, 1584021756U, 392902321U,
							3933347458U, 3093223758U, 2490637685U, 1188754011U,
							3407689449U, 3534494219U, 1079747379U, 2917214933U,
							3847937257U, 1774533228U, 3951532767U, 3588662470U,
							3280983594U, 68223256U, 476105248U, 1163078103U,
							3928419669U, 737687480U, 1548679839U, 2240709040U,
							3782006444U, 4055624168U, 2265726616U, 112674780U,
							1136364452U, 154674460U, 2532656440U, 484159024U,
							1081620220U, 2659011036U, 1145071312U, 842768672U,
							3783991256U, 3111296328U, 2580932992U, 1405052884U,
							769429092U, 1644128652U, 2644252272U, 2219788916U,
							743689160U, 2651649236U, 1501443532U, 3486748716U,
							3872988456U, 2021754692U, 3563163068U, 993231388U,
							1683279536U, 4165364228U, 372205416U, 3495389300U,
							2913870984U, 625206588U, 684592116U, 2535950936U,
							3385682132U, 140591124U, 287883660U, 2542546928U,
							3248218560U, 2810626740U, 3643003220U, 3647775076U,
							2633947504U, 211530820U, 2309587576U, 1672554204U,
							1658885648U, 1065967344U, 681647648U, 1061226372U,
							455631304U, 1177946332U, 3203591024U, 1963363452U,
							3502146236U, 730659376U, 1418080916U, 199687412U,
							2307539224U, 4134697748U, 4031003316U, 197894416U,
							1282663940U, 2916079024U, 2635353588U, 2090648056U,
							593803428U, 1519210028U, 1625207168U, 1732355540U,
							356646948U, 76754864U, 1133162364U, 3550582220U,
							720766892U, 3033120872U, 1291947796U, 3366377480U,
							1801982560U, 1897351288U, 2250873200U, 1329051828U,
							3749735532U, 1130696968U, 4159378872U, 1849457652U,
							2401721588U, 849043776U, 2804959760U, 798911212U,
							2168662312U, 1201766728U, 2665382304U, 2588134860U,
							3216841764U, 2304751944U, 3856532120U, 3054824632U,
							1367244060U, 29249640U, 2199459096U, 2348355292U,
							1030591536U, 3047461076U, 2155217480U, 568032600U,
							2373036172U, 2952445872U, 3856404568U, 2174779228U,
							1232339540U, 4155057696U, 3107293288U, 2989645064U,
							2002643892U, 2476215636U, 73970336U, 3161939852U,
							555002324U, 2377038704U, 3479033388U, 2824457228U,
							1317045464U, 3990190752U, 570178612U, 2109730480U};

	// copy xor values 
	memcpy(ptr, xors, 256*sizeof(int));

	return 1;
}


// recursive bitcount 
int recbitcount(int32 n)
	/* counts & returns the number of bits which are set in a 32-bit integer
		slower than a table-based bitcount if many bits are
		set. used to make the table for the table-based bitcount on initialization
	*/
	{
	int r=0;
	while(n)
		{
		n=n&(n-1);
		r++;
		}
	return r;
	}
