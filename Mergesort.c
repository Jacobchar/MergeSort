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

pid_t separateProcess(pid_t pid){
  if(pid > 0){
    return fork();
  }
}

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

void removeMem(int* memPointer, int mem){
  shmdt(memPointer);
  shmctl(mem, IPC_RMID, 0);
}

int sharedMem(int size){
  int mem = shmget(IPC_PRIVATE, size, SHARED_MEM_MODE);
  if(mem == -1){
    perror("Shared Memory Failed");
    return mem;
  }
  return mem;
}

void childSort(int* sharedMem){
  int end = sharedMem[0];
  updateMem(sharedMem, mergesort(sharedMem+1, 0, end));
  _exit(0);
}

void updateMem(int* oldMem, int* newMem){
  for(int i = 0; i <= newMem[0]; i++){
    oldMem[i] = newMem[i];
  }
}

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

int* breakdown(int mem, int* unsortedList, int start, int end){
  int* sharedMem = shmat(mem, 0, 0);
  int buffer = 0;

  for(int i = start; i < end; i++){
    sharedMem[sharedMem[0] + 1] = unsortedList[i];
    sharedMem[0]++;
  }
  return sharedMem;
}

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
