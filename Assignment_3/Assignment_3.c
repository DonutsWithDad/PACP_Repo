// =================================== //
// Matthew Moffitt                     //
// Assignment 1                        //
// Parallel And Concurrent Programming //
// Due Feb. 7, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void shortest_path(int SOURCE, int n, int edge[][16], int data[][16]);
int min(int a, int b);


void main(int argc, char **argv)
{
    int i, j, pid, np, mtag;
    int SOURCE, n = 16;
    // if 99, edge does not exist between points
    int edge[16][16] = {
        {0, 99,  1, 99,  4, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
        {99, 0,  2,  1, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
        { 1,  2, 0,  3, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99},
        {99,  1,  3, 0, 99, 99, 99, 99, 10, 99, 99, 99, 99, 99, 99, 99},
        { 4, 99, 99, 99, 0,  4,  6, 99, 99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99,  4, 0, 99, 99,  8, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99,  6, 99, 0,  9, 99, 99, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99,  9, 0,  1, 99,  3,  4, 99, 99, 99, 99},
        {99, 99, 99, 10, 99,  8, 99,  1, 0, 12, 99, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99, 12, 0,  7, 99, 99, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99,  3, 99,  7, 0, 99,  6, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99,  4, 99, 99, 99, 0,  8, 99, 99, 99},
        {99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  6,  8, 0,  2,  1, 99},
        {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  2, 0,  3,  4},
        {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  1,  3, 0, 99},
        {99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,  4, 99, 0}
    };
    int data[2][n]; //distance[n], found[n]
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    SOURCE = 0;

    shortest_path(SOURCE, n, edge, data);

    MPI_Finalize();
}

/*********************************************************************
 Input: n and edge[n][n], where n is the number of vertices of a graph
edge[i][j] is the length of the edge from vertex i to vertex j
Output: distance[n], the distance from the SOURCE vertex to vertex i.
*********************************************************************/
void shortest_path(int SOURCE, int n, int edge[][16], int data[][16]) {
    int i, j, count, tmp, least, leastPos;
    //data[0] = distance
    //data[1] = found
    for(i=0; i<n; i++) {
        data[1][i]= 0;
        data[0][i] = edge[SOURCE][i];
    }
    data[1][SOURCE] = 1 ;
    count = 1 ;
    while( count < n ) {
        least = 987654321 ;
        for(i=0; i<n; i++) { // <-- parallelize this loop, making it so that every process finds 
            tmp = data[0][i] ;
            if( (!data[1][i]) && (tmp < least) ) {
                least = tmp ;
                leastPos = i ;
            }
        }
        data[1][leastPos] = 1;
        count++ ;
        for(i=0; i<n; i++) { // <-- parallelize this loop
            if( !(data[1][i]) )
                data[0][i] = min(data[0][i], least+edge[leastPos][i]);
        }
    }
    for (int i = 0; i < n; i++){
        printf("shortest path to vertex %d: %d\n", i + 1, data[0][i]);
    }
} 

int min(int a, int b){
    if (a > b)
        return b;
    if (a < b)
        return a;
}
