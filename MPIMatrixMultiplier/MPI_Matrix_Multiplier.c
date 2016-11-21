/*
* Program: Multi-Process Matrix Multiplier
* Author: Ron Zappardino
* Date: Nov 21 2016
* Purpose: Uses MPI to perform Matrix Multiplication with parallel computing.
*		   Works for any matrixA of size nxn and any matrixB of size 1xn.
*      Problem is scalable to different numbers of processes, but the number of
*		   MPI processes must be a factor of n
*/

#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define n 4

void Matrix_Generator(int rank, int size);
void Matrix_Multiplier(int rank, int size);

int main(int argc, char *argv[]) {
	int rank, size;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (n % size != 0) {
		if (rank == 0) 
			printf("Number of processes must be a factor of the size of the Matrices. Try again.\n");
		MPI_Finalize();
		return 0;
	}

	Matrix_Generator(rank, size);

	Matrix_Multiplier(rank, size);

	MPI_Finalize();

	return 0;
}

	/*
	* This function generates a MatrixA and a Matrix B and then sends the contents of them
	* throughout the processes
	*/
void Matrix_Generator(int rank, int size) {
	int i, j, sendbuf;

	if (rank == 0) {
		int matrixA[n][n] = {
			{11, 12, 13, 14} ,
			{21, 22, 23, 24} ,
			{31, 32, 33, 34} ,
			{41, 42, 43, 44}
		};
		int matrixB[n] = {1, 2, 3, 4};
		//feel free to change these values to whatever you want
		//make sure to also change the #define n 4 to whatever is necessary


		for (i=0; i<n; i++) { //sending components of matrix B
			sendbuf = matrixB[i];
			int dest = i % size;
			MPI_Send(&sendbuf,1,MPI_INT,dest,0,MPI_COMM_WORLD);
		}

		for (i=0; i<n; i++) { //sending components of matrix A
			for (j=0; j<n; j++) {
				sendbuf = matrixA[i][j];
				int dest = j % size;
				MPI_Send(&sendbuf,1,MPI_INT,dest,0,MPI_COMM_WORLD);
			}
		}
	}
}

	/*
	* This function recieves the matrix contents from the Matrix_Generator function
	* and performs the actual multiplication, then sends the solution to thread 0
	*/
void Matrix_Multiplier(int rank, int size) {
	int i, j, recvbuf;
	MPI_Status Stat;

	int npp = n / size; //number of n for each processor
	int multipliers[npp];

	for (i=0; i<npp; i++) { //recieve its part of matrixB
		MPI_Recv(&recvbuf,1,MPI_INT,0,0,MPI_COMM_WORLD,&Stat);
		multipliers[i] = recvbuf;
	}

	//create necessary data for RS (Reduce Scatter)
	int sendbufRS[n];
	int recvbufRS[n/size];
	int recvcntRS[size];
	for (i=0; i<size; i++)
		recvcntRS[i] = n/size;

	for (i=0; i<n; i++) {
		sendbufRS[i] = 0;
		for (j=0; j<npp; j++) {
			MPI_Recv(&recvbuf,1,MPI_INT,0,0,MPI_COMM_WORLD,&Stat);
			sendbufRS[i] += recvbuf * multipliers[j];
		}
	}
	MPI_Reduce_scatter(sendbufRS, recvbufRS, recvcntRS, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

	//create necessary data for G (Gather)
	int sendrecvcntG = 4/size;
	int recvbufG[4];
	int source = 0;
	int* sendbufG = recvbufRS;
	MPI_Gather(sendbufG, sendrecvcntG, MPI_INT, recvbufG, sendrecvcntG, MPI_INT, source, MPI_COMM_WORLD);

	if (rank==0) {
		printf("Matrix Multiplication solutions:");

        for (i=0; i<4; i++) 
            printf(" %d",recvbufG[i]);
        
        printf("\n\n");
    }
}
