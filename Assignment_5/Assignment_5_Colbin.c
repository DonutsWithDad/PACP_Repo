// =================================== //
// Matthew Moffitt                     //
// Assignment 1                        //
// Parallel And Concurrent Programming //
// Due Feb. 7, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#define sgn(x) ( ((x)<0.0) ? (-1.0) : (1.0) )
#define c1 1.23456
#define c2 6.54321

int IsFacing(int pid, int np);

void main(int argc, char **argv)
{
    int i, j, pid, np, mtag;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    // note that upside down tringales will have sqrt(np) more triangles than the right side up ones
    // we can find this number by taking (np/2 + sqrt(np)/2) for upside down and (np/2 - sqrt(np)/2) for rightside up

    int facing = IsFacing(pid, np);
    printf("%d: %d", pid, facing);
    

    MPI_Finalize();
}

//input: pid, process id; np, number of processes
//output: facing, which way the triangle is facing, 1 = rightside up, 0 = upside down
int IsFacing(int pid, int np){
    // if the process occurs before root(np) then return upside down
    if (pid < sqrt(np)) return 0;

    // initalize thresholds for determining if a triangle is upside down or right side up
    int upsddown = sqrt(np) - 1;
    int ritsidup = sqrt(np) - 1;

    for(int i = sqrt(np) - 1; i > 0; i--){
        // set new thresholds
        ritsidup = ritsidup + i;
        upsddown = upsddown + (2 * i);

        //compare
        if (pid <= ritsidup) return 1;
        if (pid <= upsddown) return 0;

        // update rightside up to match upside down for next iteration
        ritsidup = ritsidup + i;
    }
    return 0;
}

/* Input: n, x[n]. Note that x[i] ≠ x[j] for i ≠ j.
 Output: f[n].
*/
/*
void calc_force(int n, double *x, double *f)
{
    int i,j;
    double diff, tmp;
    for(i=0; i<n; i++) f[i] = 0.0;

    for(i=1; i<n; i++){
        for(j=0; j<i; j++) {
            diff = x[i] - x[j] ;
            tmp = c1/(diff*diff*diff) - c2*sgn(diff)/(diff*diff) ;
            f[i] += tmp;
            f[j] -= tmp;
        }
    }
}
*/