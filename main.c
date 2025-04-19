//https://www.md5hashgenerator.com/
//https://www.md5.cz/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "md5.h"
#include <mpi.h>
#include <stddef.h>
#include<math.h>
#include <string.h>

void print_bytes(void *p, size_t length){
	uint8_t *pp = (uint8_t *)p;
	for(unsigned int i = 0; i < length; ++i){
		if(i && !(i % 16)){
			printf("\n\n");
		}
		printf("%02X ", pp[i]);
	}
	printf("\n");
}

void print_hash(uint8_t *p){
	for(unsigned int i = 0; i < 16; ++i){
		printf("%02x", p[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[]){

	int rank, size;
	MPI_Status status;
	
	//setting up timer
	double start, end, max_end = 0, min_start = 10000;
	
	//total time 
	double total_time = 0.0;
	double time= 0.0;
	
	//MPI INITIALIZATION
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	//time starts
	start = MPI_Wtime();
	//MAIN TASK BEGINS HERE
	//printf("Hello World from process %d of %d\n", rank, size);
	uint8_t *result;
	if(rank == 0){
		if(size < 2){
			printf("\n\nminimum 3 threads needed\n\n");
			//return 1;
		}
	}
	
	if(argc > 1){
	    for(int i = 1; i < argc; ++i){
	    		//md5 Algorithm Starts Here
				result = md5String(argv[i],rank,size);
				MPI_Barrier(MPI_COMM_WORLD);
	//root process will print the result
				if(rank == 0){
					//printf("%s\n\n",argv[1]);
					printf("\n input length = %ld\n\n",strlen(argv[i]));
					print_hash(result);
				}
		}
	}
	else{
		//md5 Algorithm Starts Here
		result = md5File(stdin, rank, size);
		MPI_Barrier(MPI_COMM_WORLD);
		//root process will print the result
		if(rank == 0){
			print_hash(result);
		}
	}
	free(result);
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	//time ends
	end = MPI_Wtime();
	printf("Ending time of process %d is %f\n",rank, end-start);
/*
	total_time = end -  start;
	MPI_Reduce(&total_time, &time, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0){
		printf("Total time taken is: %f\n",time);
	}
*/
	MPI_Finalize();
	return 0;
}
	
