// =================================== //
// Matthew Moffitt                     //
// Assignment 2                        //
// Parallel And Concurrent Programming //
// Due Feb. 21, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define generate_data(i, j) (i) + (j) * (j)

void main(int argc, char **argv)
{
    int i, j, pid, np, mtag, count;
    double t0, t1;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int data[np];

    // Outcome will be three different prints of the same row sum from all of the processes

    if (pid == 0)
    {
        for (int i = 0; i < np; i++){
            data[i] = pid;
        }
        mtag = 1;
        for (int i = 0; i < np; i++){
            if (i != pid){
                MPI_Send(data, 10, MPI_INT, i, mtag, MPI_COMM_WORLD);
            }
        }
        for (int i = 0; i < np; i++){
            if (i != pid){
                MPI_Recv(data, 10, MPI_INT, i, mtag, MPI_COMM_WORLD, &status);
            }
        }
        printf("%d\n", pid);
    }
    else /*** all other processes ***/
    {
        mtag = 1;
        for (int i = 0; i < np; i++){
            if (i != pid){
                MPI_Send(data, 10, MPI_INT, i, mtag, MPI_COMM_WORLD);
            }
        }
        for (int i = 0; i < np; i++){
            if (i != pid){
                MPI_Recv(data, 10, MPI_INT, i, mtag, MPI_COMM_WORLD, &status);
            }
        }
        printf("%d\n", pid);
    }/****** End of else ******/
    MPI_Finalize();
}