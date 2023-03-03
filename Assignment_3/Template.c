// =================================== //
// Matthew Moffitt                     //
// Assignment 1                        //
// Parallel And Concurrent Programming //
// Due Feb. 7, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void shortest_path(int SOURCE, int n, int **edge, int *distance);
int min(int a, int b);


void main(int argc, char **argv)
{
    int i, j, pid, np, mtag;
    int SOURCE, n = 16; 
    int edge[16][16] = {
        {0, 0, 1, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 12, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int distance[n];
    double t0, t1;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    MPI_Finalize();
}

/*********************************************************************
 Input: n and edge[n][n], where n is the number of vertices of a graph
edge[i][j] is the length of the edge from vertex i to vertex j
Output: distance[n], the distance from the SOURCE vertex to vertex i.
*********************************************************************/
void shortest_path(int SOURCE, int n, int **edge, int *distance) {
    int i, j, count, tmp, least, leastPos, *found;
    found = (int *) calloc( n, sizeof(int) );
    for(i=0; i<n; i++) {
        found[i]= 0;
        distance[i] = edge[SOURCE][i];
    }
    found[SOURCE] = 1 ;
    count = 1 ;
    while( count < n ) {
        least = 987654321 ;
        for(i=0; i<n; i++) { // <-- parallelize this loop
            tmp = distance[i] ;
            if( (!found[i]) && (tmp < least) ) {
                least = tmp ;
                leastPos = i ;
            }
        }
        found[leastPos] = 1;
        count++ ;
        for(i=0; i<n; i++) { // <-- parallelize this loop
            if( !(found[i]) )
                distance[i] = min(distance[i], least+edge[leastPos][i]);
        }
    } /*** End of while ***/
    free(found) ;
    printf("%d\n", distance[n]);
} 

int min(int a, int b){
    if (a > b)
        return a;
    if (a < b)
        return b;
}