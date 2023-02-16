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
    int i, j, pid, np, mtag, count;//, data[100][100], data_recv[100], row_sum[100];
    double t0, t1;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // Outcome will be three different prints of the same row sum from all of the processes

    if (pid == 0)
    {
        //generate some data (maybe 30 rows of data for row sum)
        //send 10 rows to pid 1
        //send another 10 rows to pid 2
        // ** evaluate lat 10 rows here **//
        // ** receive all data here from pid 1 and 2 ** // Causing barrier
        // print ALL row sums from pid 0
    }
    else if (pid == 1)
    {
        // recieve data here
        // evaluate row sums
        // ** send to other pids **//
        // ** recieve from 0 and 2 **// Causing our barrier
        // print all row sums
    }
    else /*** pid == 2 ***/
    {
        // recieve data here
        // evaluate row sums
        // ** send data to other pids **//
        // ** recieve data from pid 1 and 0 **// causing our barrier
        // print all row sums
    }/****** End of else ******/
    MPI_Finalize();
}