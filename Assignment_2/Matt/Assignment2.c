// =================================== //
// Matthew Moffitt,                    //
// Colbin Hobbs, Alejandro Camberos    //
// Assignment 2                        //
// Parallel And Concurrent Programming //
// Due Feb. 21, before midnight        //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#define generate_data(i, j) (i) + (j) * (j)

void main(int argc, char **argv)
{
    int n = 2;
    int i, j, pid, np, mtag;
    MPI_Status status;
    MPI_Request req_s, req_r;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int data[np * n];

    mtag = 1;

    // Problem: We want to generate p * np random numbers. We divy out the generation to each process, and
    // implement a barrier waiting for the processes to finish. 

    srand(pid);
    for (int i = 0; i < n; i++)
    {
        data[i] = rand() % 500;
    }

    // Calculate if the current process is an outermost process
    int leaf_limit = pow(2, ceil(log(np)/log(2))) / 2;
    int recv_size = pow(2, floor(log2(leaf_limit)) - floor(log(pid+1)/log(2))) - 1;

    // Here is the barrier. All processes must send back through the chain leading to p0. When a process
    // completes, it will send to process pid/2. 

    // Case we are a leaf process
    if (pid >= leaf_limit)
    {
        // Calculate parent process
        int dest = (pid+1) / 2 - 1;

        // Send to parent process
        MPI_Send(data, n, MPI_INT, dest, mtag, MPI_COMM_WORLD);

        // Receive from parent process
        MPI_Recv(data, n * np, MPI_INT, dest, mtag, MPI_COMM_WORLD, &status);
    }
    // Case we are a branch process
    else if (pid > 0)
    {
        // Calculate child processes
        int src0 = (pid+1) * 2 - 1;
        int src1 = (pid+1) * 2;

        // If child nodes exist, receive
        if (src0 < np) MPI_Recv(data + n, n * recv_size, MPI_INT, src0, mtag, MPI_COMM_WORLD, &status);
        if (src1 < np) MPI_Recv(data + n + n * recv_size, n * recv_size, MPI_INT, src1, mtag, MPI_COMM_WORLD, &status);
        // Send to parent node
        int dest = (pid+1) / 2 - 1;
        MPI_Send(data, n + n * recv_size * 2, MPI_INT, dest, mtag, MPI_COMM_WORLD);

        // Once we recieve here, we will know that every province has entered the barrier, and it is safe to continue.
        MPI_Recv(data, n * np, MPI_INT, dest, mtag, MPI_COMM_WORLD, &status);
        // Sending to child nodes.
        if (src0 < np) MPI_Send(data, n * np, MPI_INT, src0, mtag, MPI_COMM_WORLD);
        if (src1 < np) MPI_Send(data, n * np, MPI_INT, src1, mtag, MPI_COMM_WORLD);
    }
    // Case we are the root process
    else
    {
        // Calculate leaf nodes
        int src0 = (pid+1) * 2 - 1;
        int src1 = (pid+1) * 2;
        // If leaf nodes exist, recieve
        if (src0 < np) MPI_Recv(data + n, n * recv_size, MPI_INT, src0, mtag, MPI_COMM_WORLD, &status);
        if (src1 < np) MPI_Recv(data + n + n * recv_size, n * recv_size, MPI_INT, src1, mtag, MPI_COMM_WORLD, &status);

        // Now that we, at the root process, have recieved confirmation from every process, we can now send out to 
        // every process that every process has entered the barrier.
        if (src0 < np) MPI_Send(data, n * np, MPI_INT, src0, mtag, MPI_COMM_WORLD);
        if (src1 < np) MPI_Send(data, n * np, MPI_INT, src1, mtag, MPI_COMM_WORLD);
    }

    // Print to show that data from all processes has been copied to all processes
    printf("Process %0.2d: ", pid);
    for (i = 0; i < n * np; i++)
    {
        printf("%d ", data[i]);
    }
    
    //printf("\nProcess %d complete.", pid);
    MPI_Finalize();
}
