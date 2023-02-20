// =================================== //
// Colbin Hobbs                        //
// Assignment 2                        //
// Parallel And Concurrent Programming //
// Due Feb. 21, before midnight        //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void main(int argc, char **argv)
{
    int i, j, pid, np, mtag, count;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int data[np];

    mtag = 1;
    for (int i = 0; i < np; i++){
        if (i != pid){
            MPI_Send(data, np, MPI_INT, i, mtag, MPI_COMM_WORLD);
        }
    }
    for (int i = 0; i < np; i++){
        if (i != pid){
            MPI_Recv(data, np, MPI_INT, i, mtag, MPI_COMM_WORLD, &status);
        }
    }

    printf("%d\n", pid);
    MPI_Finalize();
}