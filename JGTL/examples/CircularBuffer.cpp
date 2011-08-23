#define DEBUG_CIRCULAR_BUFFER (1)

#include "JGTL_DynamicCircularBuffer.h"
#include "JGTL_StackCircularBuffer.h"
#include "JGTL_StringConverter.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace JGTL;

int main()
{
#ifdef _MSC_VER
    _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 
#endif

    try
    {
		for(int a=0;a<20;a++)
		{
			DynamicCircularBuffer<string> myBuffer(a);

			for(int b=0;b<20;b++)
			{
				myBuffer.enqueue(toString(b));

				for(int c=0;c<myBuffer.size();c++)
				{
					cout << myBuffer.getIndexRef(c) << ' ';
				}
				cout << endl;
			}

			cout << endl;
		}

        {
			StackCircularBuffer<string,10> myBuffer;

            myBuffer.enqueue("1");
            myBuffer.enqueue("2");
            myBuffer.enqueue("3");
            myBuffer.enqueue("4");
            myBuffer.enqueue("5");
            myBuffer.enqueue("6");
            myBuffer.enqueue("7");
            myBuffer.enqueue("8");
            myBuffer.enqueue("9");
            myBuffer.enqueue("10");

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

            for (int a=0;a<5;a++)
            {
                myBuffer.dequeue();
            }

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

			myBuffer.enqueue("11");
            myBuffer.enqueue("12");
            myBuffer.enqueue("13");
            myBuffer.enqueue("14");
            myBuffer.enqueue("15");

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

            for (int a=0;a<5;a++)
            {
                myBuffer.dequeue();
            }

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

			cout << "********************\n\n";
		}

        {
			DynamicCircularBuffer<string> myBuffer(10);

            myBuffer.enqueue("1");
            myBuffer.enqueue("2");
            myBuffer.enqueue("3");
            myBuffer.enqueue("4");
            myBuffer.enqueue("5");
            myBuffer.enqueue("6");
            myBuffer.enqueue("7");
            myBuffer.enqueue("8");
            myBuffer.enqueue("9");
            myBuffer.enqueue("10");

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

            for (int a=0;a<5;a++)
            {
                myBuffer.dequeue();
            }

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

			myBuffer.enqueue("11");
            myBuffer.enqueue("12");
            myBuffer.enqueue("13");
            myBuffer.enqueue("14");
            myBuffer.enqueue("15");

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

            for (int a=0;a<5;a++)
            {
                myBuffer.dequeue();
            }

			for(int a=0;a<myBuffer.size();a++)
			{
				cout << myBuffer.getIndexRef(a) << ' ';
			}
			cout << endl;

			cout << "********************\n\n";
		}
	}
    catch (const std::exception &ex)
    {
        cout << ex.what() << endl;
    }

	system("PAUSE");
}

