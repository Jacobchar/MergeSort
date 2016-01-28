#define FILE_END -1
#define INT_SIZE  sizeof(int)
#define SHARED_MEM_MODE 0600

int* readFile(char* fileName);
void printInfo(char* fileName, int* unsortedList, int* sortedList, int listSize);
int* mergesort(int* unsortedList, int start, int end);
void childSort(int* sharedMem);
int sharedMem(int size);
void removeMem(int* memPointer, int mem);
void updateMem(int* oldMem, int* newMem);
int* merge(int* left, int* right, int start, int end);
pid_t separateprocess(pid_t pid);
int* breakdown(int me, int* unsortedList, int start, int end);
