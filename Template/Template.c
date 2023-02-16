// =================================== //
// Matthew Moffitt                     //
// Assignment 1                        //
// Parallel And Concurrent Programming //
// Due Feb. 7, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define generate_data(i, j) (i) + (j) * (j)

void main(int argc, char **argv)
{
    int i, j, pid, np, mtag, count;//, data[100][100], data_recv[100], row_sum[100];
    double t0, t1;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (pid == 0)
    {

    }
    else /*** pid == 1 ***/
    {

    } /****** End of else ******/
    MPI_Finalize();
}