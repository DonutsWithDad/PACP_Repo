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
} 