// =================================== //
// Matthew Moffitt,                    //
// Colbin Hobbs, Alejandro Camberos    //
// Assignment 4                        //
// Parallel And Concurrent Programming //
// Due April 11, before midnight       //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <windef.h>

void printMatrix(int** D, int sizx, int sizy);

void main(int argc, char **argv)
{
    int n = 10;
    int pid, np, mtag;
    MPI_Status status;
    MPI_Request req_s, req_r;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    mtag = 1;

    // The length/width of this process' matrix.
    int siz = n / sqrt(np);
    // The coordinate of this process on the global matrix.
    int xcoord = pid % (int)sqrt(np);
    int ycoord = pid / (int)sqrt(np);
    // The upper and lower bounds of the matrix, relative
    // to the large global matrix.
    int xlower_bound = xcoord * siz;
    int xupper_bound = xcoord * siz + siz;
    int ylower_bound = ycoord * siz;
    int yupper_bound = ycoord * siz + siz;

    // --------------------------
    // Initialize array for this process
    // --------------------------

    // First, D0
    // As we only need a certain amount of data
    // for each k, we do not need to copy D -> D0
    // each time. We only need to store the values
    // of D[i][k] and D[k][j] for each given range
    // of i and j. This equates to two rows.
    int** D0 = (int**)calloc(2, sizeof(int*));
    for (int i = 0; i < 2; i++)
    {
        D0[i] = (int*)calloc(siz, sizeof(int));
    }

    // D, this process' part of the array is stored
    // here.
    int** D = (int**)calloc(siz, sizeof(int*));
    for (int i = 0; i < siz; i++)
    {
        D[i] = (int*)calloc(siz, sizeof(int));
        for (int j = 0; j < siz; j++)
        {
            // Initialize to calculated graph edge function with multi-node paths that
            // are more optimal than two-node paths.
            D[i][j] = abs(10 * sin(i + ycoord * (siz)) + 10 * cos(j + xcoord * siz)) + 1;
            // Make all nodes distance 0 from themselves
            if (i == j && xcoord == ycoord)
            {
                D[i][j] = 0;
            }
        }
    }

    printf("\n---=== [ Process %d ] ===---\nProcesses From Left: %d\nProcesses From Right: %d\n", pid, xcoord, ycoord);

    printf("\nInput Edges:\n");
    printMatrix(D, siz, siz);
    // ----------------------
    // Perform calculation
    // ----------------------
    for (int k = 0; k < n; k++)
    {
        // Communicate comparison values for D0

        // First, send/receive the horizontal K-row
        if (ylower_bound <= k && k < yupper_bound) // K-row in this process, we are sending
        {
            // Get K-row
            for (int i = 0; i < siz; i++)
            {
                D0[0][i] = D[k % siz][i];
            }

            // Calculate targets to send to that need this data
            for (int tgt = xcoord; tgt < np; tgt += sqrt(np))
            {
                if (tgt != ycoord * sqrt(np) + xcoord) // If not current process, send
                {
                    MPI_Send(D0[0], siz, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
                }
            }
        }
        else // K-row not in this process, we are receiving
        {
            int tgt = xcoord + (k / siz) * sqrt(np);
            MPI_Recv(D0[0], siz, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
        }
        
        // Then, send/receive the vertical K-column
        if (xlower_bound <= k && k < xupper_bound) // K-column in this process, we are sending
        {
            // Calculate K-column
            for (int i = 0; i < siz; i++)
            {
                D0[1][i] = D[i][k % siz];
            }

            // Calculate targets to send to
            for (int tgt = ycoord * sqrt(np); tgt < (ycoord + 1) * sqrt(np); tgt++)
            {
                if (tgt != ycoord * sqrt(np) + xcoord) // If not current process, send
                {
                    MPI_Send(D0[1], siz, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
                }
            }
        }
        else // K-column not in this process, we are receiving
        {
            int tgt = ycoord * sqrt(np) + (k / siz); 
            MPI_Recv(D0[1], siz, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
        }

        // Calculate minimum between D and K-Adjusted values
        for (int i = 0; i < siz; i++)
        {
            for (int j = 0; j < siz; j++)
            {
                D[i][j] = min(D[i][j], D0[0][j] + D0[1][i]);
            }
        }
    }
    printf("Output Edges:\n");
    printMatrix(D, siz, siz);

    // Finalize
    MPI_Finalize();
}

// Function to print the matrix for easy debugging/presentation
void printMatrix(int** D, int sizx, int sizy)
{
    for (int i = 0; i < sizx; i++)
    {
        for (int j = 0; j < sizy; j++)
        {
            printf("%4d ", D[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
