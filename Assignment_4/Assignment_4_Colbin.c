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
#define generate_data(i, j) (i) + (j) * (j)

int min(int a, int b);

void main(int argc, char **argv)
{
    int i, j, pid, np, mtag;
    double t0, t1;
    int n = 8;
    int D0[8][8] ={ 
         {0, 3, 1, 4, 5, 2, 6, 7}
        ,{4, 0, 3, 1, 2, 4, 7, 9}
        ,{5, 1, 0, 6, 5, 9, 2, 1}
        ,{1, 4, 6, 0, 5, 3, 9, 8}
        ,{5, 3, 7, 2, 0, 9, 4, 7}
        ,{8, 7, 3, 5, 1, 0, 2, 9}
        ,{7, 8, 3, 2, 5, 6, 0, 1}
        ,{4, 6, 2, 6, 8, 1, 3, 0}};
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    int rootnp = (int)sqrt(np);
    int sub_size = n/rootnp;

    int** sub_matrix = (int**)malloc(sub_size * sizeof(int*));
    for (int i = 0; i < sub_size; i++) {
        sub_matrix[i] = (int*)malloc(sub_size * sizeof(int));
    }

    int** recv_buffer = NULL;
    if (pid == 0) {
        recv_buffer = (int**)malloc(n * sizeof(int*));
        for (int i = 0; i < n; i++) {
            recv_buffer[i] = (int*)malloc(n * sizeof(int));
        }
    }

    for (int k = 0; k < n; k++){
        // i start and j start on the sub matrix
        i = (pid % rootnp) * sub_size;
        j = (pid / rootnp) * sub_size;
        while(i < i + sub_size){
            while(j < j + sub_size){
                sub_matrix[i][j] = min(D0[i][j], D0[i][k] + D0[k][j]);
                if (pid == 0){
                    recv_buffer[i][j] = sub_matrix[i][j];
                }
                j++;
            }
            i++;
        }
    }

    // if pid == 0, recieve from all other processes
    if (pid == 0){
        for (int k = 1; k < np; k++){
            i = (pid % rootnp) * sub_size;
            j = (pid / rootnp) * sub_size;
            MPI_Irecv(&recv_buffer[i][j], n * n, MPI_INT, k, 0, MPI_COMM_WORLD, &req_r);
        }
        //print matrix
        i = 0;
        j = 0;
        while(i < n){
            while (j < n){
                printf("%d, ", recv_buffer[i][j]);
                j++;
            }
            printf("\n");
            i++;
        }
    }else{
        MPI_Isend(&sub_matrix[0][0], sub_size * sub_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &req_s);
    }

    // free recv_buffer
    if (pid == 0){
        for (int i = 0; i < n; i++) {
            free(recv_buffer[i]);
        }
        free(recv_buffer);
    }

    //free sub_matrix
    for (int i = 0; i < sub_size; i++) {
        free(sub_matrix[i]);
    }
    free(sub_matrix);


    MPI_Finalize();
}

int min(int a, int b){
    if (a > b)
        return b;
    if (a < b)
        return a;
}