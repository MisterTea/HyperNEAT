#include "JGTL_QuadTree.h"
#include "JGTL_OctTree.h"
#include "JGTL_Random.h"

#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;
using namespace JGTL;

int SIZE_X=256;
int SIZE_Y=256;

int main()
{
#ifdef _MSC_VER
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
#endif

	{
		int octArray[64][64][64];
		memset(octArray,0,sizeof(int)*64*64*64);

		JGTL::Random RNG(1000);

		for(int trials=0;trials<100;trials++)
		{
			cout << "On Octtree trial: " << trials << endl;
			for(int x=0;x<64;x++)
			{
				for(int y=0;y<64;y++)
				{
					for(int z=0;z<64;z++)
					{
						if(RNG.rollD2()==1)
						{
							octArray[x][y][z]=100;
						}
						else
						{
							octArray[x][y][z]=0;
						}
					}
				}
			}

			OctTree<int> qTree1(64,0);
			for(int x=0;x<64;x++)
			{
				for(int y=0;y<64;y++)
				{
					for(int z=0;z<64;z++)
					{
						qTree1.setValue(x,y,z,octArray[x][y][z]);
						if(qTree1.getValue(x,y,z)!=octArray[x][y][z])
						{
							cout << "ERROR\n";
						}
					}
				}
			}

			OctTree<int> qTree1a(64,0);
			for(int x=63;x>=0;x--)
			{
				for(int y=63;y>=0;y--)
				{
					for(int z=63;z>=0;z--)
					{
						qTree1a.setValue(x,y,z,qTree1.getValue(x,y,z));
						if(qTree1a.getValue(x,y,z)!=octArray[x][y][z])
						{
							cout << "ERROR\n";
						}
					}
				}
			}

			OctTree<int> qTree1b = qTree1;
			for(int x=0;x<64;x++)
			{
				for(int y=0;y<64;y++)
				{
					for(int z=0;z<64;z++)
					{
						if(qTree1b.getValue(x,y,z)!=octArray[x][y][z])
						{
							cout << "ERROR\n";
						}
					}
				}
			}
		}
	}

	{
		int quadArray[256][256];
		memset(quadArray,0,sizeof(int)*256*256);

		JGTL::Random RNG(1000);

		for(int trials=0;trials<100;trials++)
		{
			cout << "On Quadtree trial: " << trials << endl;
			for(int x=0;x<256;x++)
			{
				for(int y=0;y<256;y++)
				{
					if(RNG.rollD2()==1)
					{
						quadArray[x][y]=100;
					}
					else
					{
						quadArray[x][y]=0;
					}
				}
			}

			QuadTree<int> qTree1(256,0);
			for(int x=0;x<256;x++)
			{
				for(int y=0;y<256;y++)
				{
					qTree1.setValue(x,y,quadArray[x][y]);
					if(qTree1.getValue(x,y)!=quadArray[x][y])
					{
						cout << "ERROR\n";
					}
				}
			}

			QuadTree<int> qTree1a(256,0);
			for(int x=255;x>=0;x--)
			{
				for(int y=255;y>=0;y--)
				{
					qTree1a.setValue(x,y,qTree1.getValue(x,y));
					if(qTree1a.getValue(x,y)!=quadArray[x][y])
					{
						cout << "ERROR\n";
					}
				}
			}

			QuadTree<int> qTree1b = qTree1;
			for(int x=0;x<256;x++)
			{
				for(int y=0;y<256;y++)
				{
					if(qTree1b.getValue(x,y)!=quadArray[x][y])
					{
						cout << "ERROR\n";
					}
				}
			}
		}
	}

	return 0;
}
