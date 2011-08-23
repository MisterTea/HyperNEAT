#define DEBUG_POOL_MAP (1)

#include "JGTL_StackMap.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace JGTL;

int main()
{
    try
    {
        StackMap<string,int,32> myMap;

        myMap.insert("My",1);
        myMap.insert("Name",2);
        myMap.insert("Is",3);
        myMap.insert("Jason",4);
        myMap.insert("Gauci",5);

		cout << myMap.getIndex(0).first << ": " << myMap.getIndex(0).second << endl;
		cout << myMap.getIndex(1).first << ": " << myMap.getIndex(1).second << endl;
		cout << myMap.getIndex(2).first << ": " << myMap.getIndex(2).second << endl;
		cout << myMap.getIndex(3).first << ": " << myMap.getIndex(3).second << endl;
		cout << myMap.getIndex(4).first << ": " << myMap.getIndex(4).second << endl;

        cout << endl;

        cout << myMap.hasKey("My") << endl << endl;

        cout << myMap.size() << endl;
    }
    catch (const std::exception &ex)
    {
        cout << ex.what() << endl;
    }

	system("PAUSE");
}
