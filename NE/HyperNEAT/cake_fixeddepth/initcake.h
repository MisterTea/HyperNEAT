HASHENTRY *loadbook(int *bookentries, int *bookmovenum);
int initbitoperations(unsigned char bitsinword[65536], unsigned char LSBarray[256]);
HASHENTRY *inithashtable(int hashsize);
int initializematerial(short materialeval[13][13][13][13]);
int initializebackrank(char blackbackrankeval[256], char whitebackrankeval[256], char blackbackrankpower[256], char whitebackrankpower[256]);
int initxors(int *ptr);
int recbitcount(int32 n);

