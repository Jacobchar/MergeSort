#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#include "Mergesort.h"

int main(int args, char* file[]){
	int* unsortedList = readFile(file[1]);
	//ensure there is a file to read
	if(unsortedList == NULL){
		return -1;
	}
}




int* readFile(char* file){
	FILE* openFile = fopen(file, "r");
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


void printInfo(char* file, int* sortedList, int* unsortedList, int listSize){
	printf("Sorting file: %s. \n", file);
	printf("%d elements read.\nInput Numbers: \n", listSize);

	for(int i = 0; i < listSize; i++){
		printf("%d ", unsortedList[i]);
	}

	printf("\nSorted Numbers:\n");

	for(int i = 0; i < listSize; i++){
		printf("%d ", sortedList[i]);
	}

	printf("\n");
}
