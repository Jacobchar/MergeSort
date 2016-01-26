#define FILE_END -1
#define INT_SIZE  sizeof(int)
#define SHARED_MEM_MODE 0600

int* readFile(char* fileName);
void printInfo(char* fileName, int* unsortedList, int* sortedList, int listSize);

