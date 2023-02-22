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
    int n = 1;
    int i, j, pid, np, mtag, data[n];
    MPI_Status status;
    MPI_Request req_s, req_r;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    mtag = 1;

    // To implement our barrier, we use the following algorithm:
    // Until we reach 1 process remaining:
    //   1) Divide the number of processes in 2.
    //   2) If a process is in the second half, that process sends.
    //   3) If a process is in the first half, that process receives.
    //   4) The second half of processes is now sent, and we can ignore for
    //      now. We then repeat the loop with the receiving processes until
    //      we have 1 process left.

    // Once we have one process left, we do the same thing, but in reverse. This
    // is to let all other processes know that every process has entered the
    // barrier, and it is safe to continue.

    printf("-=-=-=-=-=-=:: PROCESS %d ::=-=-=-=-=-=-\n", pid);
    printf(" Barrier Entry:\n  ", pid);
    int max = np;
    int tgt;

    // Barrier Entry, log2(p)
    while (1)
    {
        if (max == 1)
        {
            break;
        }
        // First, we check if we are a sender or reciever
        // Sender
        if (pid >= max / 2 + max % 2)
        {
            tgt = pid - max / 2 - max % 2;
            printf("(%d -> %d)\n  ", pid, tgt);
            MPI_Send(data, 1, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
            break;
        }
        // Receiver
        else if (pid < max / 2)
        {
            tgt = pid + max / 2 + max % 2;
            printf("(%d <- %d)\n  ", pid, tgt);
            MPI_Recv(data, n, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);

            // Calculate new number of processes
            max = max / 2 + max % 2;
        }
        // Case there is an odd number of processes that still hasn't sent, and the middle one need not do anything yet.
        else
        {
            printf("(%d waited)\n  ", pid);

            // Calculate new number of processes
            max = max / 2 + max % 2;
        }
    }

    // Next, for the barrier exit, we use a simpler algorithm:
    //   1) Start with a rank of 0. If our process is less than 2^rank (in case of pid0 this is 1), then
    //      we send. 
    //   2) If our process is less than 2*2^rank, or 2^(rank + 1), then we receive.
    //   3) If our process' destination is out of bounds, then we do nothing.
    printf("\n Barrier Exit:\n  ", pid);
    int rank = 0;
    max = 0;
    // Barrier Exit, log2(p)
    while (1)
    {
        if (pid < pow(2, rank))
        {
            tgt = pid + pow(2, rank);
            if (tgt >= np)
            {
                break;
            }
            printf("(%d -> %d)\n  ", pid, tgt);
            MPI_Send(data, 1, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
        }
        else if (pid < pow(2, rank) * 2)
        {
            tgt = pid - pow(2, rank);
            printf("(%d <- %d)\n  ", pid, tgt);
            MPI_Recv(data, n, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
        }

        rank++;
    }

    // More code...

    // Finalize
    MPI_Finalize();
}
