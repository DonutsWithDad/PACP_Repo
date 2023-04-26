// =================================== //
// Matthew Moffitt,                    //
// Colbin Hobbs, Alejandro Camberos    //
// Assignment 5                        //
// Parallel And Concurrent Programming //
// Due Apr. 25, before midnight        //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>
#define sgn(x) (((x) < 0.0) ? (-1.0) : (1.0))
#define c1 1.23456
#define c2 6.54321

// Calculates where in the data this process is calculating
int* calculate_bounds(int np, int minp, int maxp, int n)
{
    int* bounds = (int*)calloc(2, sizeof(int));

    // Calculate how many calculations (roughly) this process will handle
    int all_calculations = n * (n + 1) / 2;
    int calc_per_process = all_calculations / np;
    // This is where in the total calculations this process will handle.
    int data_min = calc_per_process * minp;
    int data_max = calc_per_process * maxp;

    // Translate where in the total calculations we are to where in the particle array we are.
    bool found_min = false;
    bool found_max = false;
    for (int i = 0; i < n; i++)
    {
        if (!found_min)
        {
            // If the sum of all processes before this is greater than the data min, then this is where we will start.
            if (i * (i + 1) / 2 > data_min)
            {
                bounds[0] = i-1;
                found_min = true;
            }
        }
        if (!found_max)
        {
            // If the sum of all processes before this is greater than the data max, then this is where we will end.
            if (i * (i + 1) / 2 > data_max)
            {
                bounds[1] = i-1;
                found_max = true;
            }
        }
    }
    if (maxp == np)
    {
        bounds[1] = n;
    }

    // Return bounds of process
    return bounds;
}

void calc_force(int n, double *x, double *f, int pid, int np)
{
    // Calculate bounds of this process
    int min;
    int max;
    int* bounds = (int*)calculate_bounds(np, pid, pid+1, n);
    int lower_bound = bounds[0];
    int upper_bound = bounds[1];

    // Calculate force for this process' bounds
    int i, j;
    double diff, tmp;
    for (i = lower_bound; i < upper_bound; i++)
    {
        for (j = 0; j < i; j++)
        {
            diff = x[i] - x[j];
            tmp = c1 / (pow(diff, 3) - c2 * (sgn(diff) / pow(diff, 2)));
            f[i] += tmp;
            f[j] -= tmp;
        }
    }

    // Calculate the max number of parallel send steps.
    int maxRank = ceil(log2(np));
    int rank = 1;
    int mtag = 1;
    int tgt;
    MPI_Status status;
    double* fbuffer = (double*)calloc(n, sizeof(double));

    // Consolidate data at process 0
    while (rank <= maxRank)
    {
        if (pid % (int)pow(2, rank) > 0) // Case we are sending
        {
            // Calculate Target
            tgt = pid - pow(2, rank-1);

            // Send
            MPI_Send(f, n, MPI_DOUBLE, tgt, mtag, MPI_COMM_WORLD);
            break;
        }
        else // Case we are receiving
        {
            // Calculate Target
            tgt = pid + pow(2, rank-1);
            if (tgt < np)
            {
                // Receive
                MPI_Recv(fbuffer, n, MPI_DOUBLE, tgt, mtag, MPI_COMM_WORLD, &status);

                // Combine f and fbuffer
                for (int i = 0; i < n; i++)
                {
                    f[i] += fbuffer[i];
                }
            }
        }

        rank++;
    }
}

void main(int argc, char **argv)
{
    int n = 128;
    int forceSeed = 20;
    int i, j, pid, np, mtag, data[n];
    MPI_Request req_s, req_r;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    mtag = 1;

    // Seed Force Generator
    srand(forceSeed);

    double *x = (double*)calloc(n, sizeof(double));
    double *f = (double*)calloc(n, sizeof(double));
    // Generate forces (Same in each process)
    for (int i = 0; i < n; i++)
    {
        x[i] = rand() / ((double)RAND_MAX / 2.0) - 1.5;
    }

    calc_force(n, x, f, pid, np);

    if (pid == 0)
    {
        printf("\n[Original Data]\n");
        for (int i = 0; i < n; i++) printf("[%6.3f] %s", x[i], i % 10 == 9 ? "\n" : "");
        printf("\n\n[Forces]\n");
        for (int i = 0; i < n; i++) printf("[%8.3f] %s", f[i], i % 10 == 9 ? "\n" : "");
    }
    

    // Finalize
    MPI_Finalize();
}
