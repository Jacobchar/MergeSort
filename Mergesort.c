#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#include "Mergesort.h"

int main(int args, char* fileName[]){
	int* unsortedList = readFile(fileName[1]);
	//ensure there is a file to read
	if(unsortedList == NULL){
		return -1;
	}
	int* sortedList = mergesort(unsortedList+1, 0, unsortedList[0]);
	printInfo(fileName[1], unsortedList+1, sortedList+1, unsortedList[0]);
	free(unsortedList);
	free(sortedList);
}

/** @brief A function to ensure that each child process doesn't call fork immediately
*   @param pid: the pid of the first child process called
*   @return the pid_t of the new process
**/
pid_t separateProcess(pid_t pid){
  if(pid > 0){
    return fork();
  }
}

/** @brief Splits the list into halves that each have their own process
*           which recursively runs until the 'list' is only a single element
*   @param unsortedList: a pointer to the beginning of the list
*   @param start: the start index of the list
*   @param end: the end of the index of the list
*   @return a pointer that points to the start of the sortedList
**/
int* mergesort(int* unsortedList, int start, int end){
  
  if ((end - start) < 2){return unsortedList-1;}

  int middle = (end + start) / 2;

  int leftMem = sharedMem(middle);
  int* leftPointer = breakdown(leftMem, unsortedList, start, middle);

  int rightMem = sharedMem(middle+1);
  int* rightPointer = breakdown(rightMem, unsortedList, middle, end);

  int pid_left = fork();
  int pid_right = separateProcess(pid_left);
  
  if(pid_left == 0){
    childSort(leftPointer);
  } else if (pid_right == 0){
    childSort(rightPointer);
  }else{
    waitpid(pid_left, NULL, 0);
    waitpid(pid_right, NULL, 0);
    
    int* sortedList = merge(leftPointer, rightPointer, start, end);  
    
    removeMem(leftPointer, leftMem);
    removeMem(rightPointer, rightMem);

    return sortedList;
    
  }
}

/** @brief Removes the shared memory
*   @param memPointer: points to the shared memory buffer
*   @param mem: the ID of the pointer
*   @return void
**/
void removeMem(int* memPointer, int mem){
  shmdt(memPointer);
  shmctl(mem, IPC_RMID, 0);
}

/** @brief Creates a shared memory ID
*   @param size: an in of the size needing to be allocated
*   @return an int of the memory allocated
**/
int sharedMem(int size){
  int mem = shmget(IPC_PRIVATE, size, SHARED_MEM_MODE);
  if(mem == -1){
    perror("Shared Memory Failed");
    return mem;
  }
  return mem;
}

/** @brief Helps mergesort split to single elements
*   @param sharedMem: a pointer to the shared memory of the child and parent
*   @return void
**/
void childSort(int* sharedMem){
  int end = sharedMem[0];
  updateMem(sharedMem, mergesort(sharedMem+1, 0, end));
  _exit(0);
}

/** @brief Updates the shared memory
*   @param oldMem: pointer to the old memory
*   @param newMem: pointer to the new memory
*   @return void
**/
void updateMem(int* oldMem, int* newMem){
  for(int i = 0; i <= newMem[0]; i++){
    oldMem[i] = newMem[i];
  }
}

/** @brief Merges the single element lists into a sorted list
*   @param left: pointer to the left shared memory buffer
*   @param right: pointer to the right shared memory buffer
*   @param start: the first index of the sorted array
*   @param end: the last index of the sorted array
*   @return pointer to the merged left and right memory buffers
**/
int* merge(int* left, int* right, int start, int end) {
  int* sortedList = malloc((end + 1)*INT_SIZE);
  int leftIndex = 1;
  int rightIndex = 1;
  sortedList[0] = end;
  int rightSize = right[0];
  int leftSize = left[0];

  for(int i = 1; i <= end; i++){
    if((leftIndex <= leftSize) && ((left[leftIndex] < right[rightIndex]) || (rightIndex > rightSize))){
      sortedList[i] = left[leftIndex];
      leftIndex ++;
    }else{
      sortedList[i] = right[rightIndex];
      rightIndex ++;
    }
  }
  return sortedList;
}

/** @brief Creates the mshared mempointer and places it into memory
*   @param mem: the ID of the memory buffer
*   @param unsortedList: the list to be sorted
*   @param start: the start index to be placed in mem
*   @param end: the end index to be placed in mem
*   @return a pointer to the shared memory
**/
int* breakdown(int mem, int* unsortedList, int start, int end){
  int* sharedMem = shmat(mem, 0, 0);
  if((void* ) sharedMem == (void* ) -1){
    perror("Share Memory Attach Failed");
    return sharedMem;
  }

  int buffer = 0;

  for(int i = start; i < end; i++){
    sharedMem[sharedMem[0] + 1] = unsortedList[i];
    sharedMem[0]++;
  }
  return sharedMem;
}

/** @brief Reads the file
*   @param fileName: the name of the file to be read
*   @return pointer to the integer array that holds our list
**/
int* readFile(char* fileName){
	FILE* openFile = fopen(fileName, "r");
	if(openFile == NULL){
		perror("File Not Found");
		return NULL;
	}
	int* contents = calloc(1, sizeof(int));
	int foundValue = 0;

	while(fscanf(openFile, "%d", &foundValue) != FILE_END){
		contents = realloc(contents, (contents[0] + 2) * INT_SIZE);
	 	contents[contents[0] + 1] = foundValue;
	 	contents[0]++;
	}

	fclose(openFile);
	return contents;
}

/** @brief Prints the information required for the assignment to the screen 
*   @param fileName: the name of the file we are reading from
*   @param unsortedList: a pointer the the list to be sorted
*   @param sortedList: a pointer the the new sortedList
*   @param listSize: the size of the list we are sorting
*   @return void
**/
void printInfo(char* fileName, int* unsortedList, int* sortedList, int listSize){
	printf("Sorting file: %s. \n", fileName);
	printf("%d elements read.\nInput Numbers: \n", listSize);

	for(int i = 0; i < listSize; i++){
		printf("%d ", unsortedList[i]);
	}

	printf("\nSorted Numbers:\n");

	for(int i = 0; i<listSize; i++){
		printf("%d ",sortedList[i]);
	}
	
	printf("\n");
}
