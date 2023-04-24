// =================================== //
// Matthew Moffitt,                    //
// Colbin Hobbs, Alejandro Camberos    //
// Assignment 3                        //
// Parallel And Concurrent Programming //
// Due Mar. 9, before midnight         //
// =================================== //

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <windef.h>
#define lower_bound(pid, n, np) (pid) * (n) / (np)
#define upper_bound(pid, n, np) (pid + 1) * (n) / (np)

// ===================================================================== //
// Input: n and edge[n][n], where n is the number of vertices of a graph //
// edge[i][j] is the length of the edge from vertex i to vertex j        //
// Output: distance[n], the distance from the SOURCE vertex to vertex i. //
//                                                                       //
// Note: The purpose of this function is to verify output. This is not   //
// the part of the assignment that we wrote. This is the linear function //
// provided in the assignment.                                           //
// ===================================================================== //
void shortest_path(int SOURCE, int n, int **edge, int *distance)
{
    int i, j, count, tmp, least, leastPos, *found;

    found = (int *)calloc(n, sizeof(int));
    for (i = 0; i < n; i++)
    {
        found[i] = 0;
        distance[i] = edge[SOURCE][i];
    }
    found[SOURCE] = 1;
    count = 1;
    while (count < n)
    {
        least = 987654321;
        for (i = 0; i < n; i++)
        { // <-- parallelize this loop
            tmp = distance[i];
            if ((!found[i]) && (tmp < least))
            {
                least = tmp;
                //on pid 0: send least to pid 1
                leastPos = i;
            }
        }
        found[leastPos] = 1;
        count++;
        for (i = 0; i < n; i++)
        { // <-- parallelize this loop
            if (!(found[i]))
                distance[i] = min(distance[i], least + edge[leastPos][i]); // on pid 1, recieve from pid 0 where least is present.
        }
    } /*** End of while ***/
    free(found);
}

// =============================================== //
// Sends and receives the index and value of the   //
// position with the least value.                  //
// Input: The index and the value calculated.      //
// Output: The index of the least value throughout //
// all the processes.                              //
// =============================================== //
int SynchronizeLeastPos(int index, int value)
{
    int np, pid, rank, tgt, mtag, data[2], storedIndex, storedValue;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Status status;
    MPI_Request req_s, req_r;

    storedIndex = index;
    storedValue = value;

    // Calculate how many send-receive operations we will have to do.
    rank = ceil(log2(np));

    // Consolidate data at process 0
    while (rank > 0)
    {
        
        if (pid < pow(2, rank-1)) // We are a receiving node
        {
            tgt = pid + pow(2, rank-1);
            if (tgt < np)
            {
                MPI_Recv(data, 2, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);

                if (data[1] < storedValue)
                {
                    storedIndex = data[0];
                    storedValue = data[1];
                }
                else
                {
                    data[0] = storedIndex;
                    data[1] = storedValue;
                }
            }
        }
        else if (pid < pow(2, rank)) // We are a sending node
        {
            tgt = pid - pow(2, rank-1);
            data[0] = storedIndex;
            data[1] = storedValue;
            MPI_Send(data, 2, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
        }

        rank--;
    }

    // Distribute data to processes.
    while (1)
    {
        if (pid < pow(2, rank))
        {
            tgt = pid + pow(2, rank);
            if (tgt >= np)
            {
                break;
            }
            MPI_Send(data, 2, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
        }
        else if (pid < pow(2, rank) * 2)
        {
            tgt = pid - pow(2, rank);
            MPI_Recv(data, 2, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
        }

        rank++;
    }

    return data[0];
}

// =============================================== //
// Sends and receives the index and value of the   //
// position with the least value.                  //
// Input: The distance array calculated by this    //
// process, and the number of elements in          //
// that array.                                     //
// Output: The synchronized distance array         //
// =============================================== //
void SynchronizeDistance(int* distance, int n)
{
    int np, pid, rank, tgt, mtag;//, *dataRecv, *dataSend;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Status status;
    MPI_Request req_s, req_r;

    mtag = 1;

    //dataSend = (int*)calloc(n, sizeof(int));
    //dataRecv = (int*)calloc(n, sizeof(int));

    int maxRank = ceil(log2(np));
    rank = 1;

    printf("Process %d\n", pid);

    // Consolidate data at process 0
    while (rank <= maxRank)
    {
        if (pid % (int)pow(2, rank) > 0) // Case we are sending
        {
            tgt = pid - pow(2, rank-1);

            int sendSiz = pow(2, rank-1);
            int lower_bound = lower_bound(pid, n, np);
            int upper_bound = min(upper_bound(pid + sendSiz - 1, n, np), upper_bound(np-1, n, np));
            int length = upper_bound - lower_bound;

            printf("SND ->  %d  :::  %d - %d (Size: %d, %d)\n", tgt, lower_bound, upper_bound, sendSiz, length);
            MPI_Send(distance + lower_bound, length, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
            break;
        }
        else // Case we are receiving
        {
            tgt = pid + pow(2, rank-1);
            if (tgt < np)
            {
                int recvSiz = pow(2, rank-1);
                int lower_bound = lower_bound(tgt, n, np);
                int upper_bound = min(upper_bound(tgt + recvSiz - 1, n, np), upper_bound(np-1, n, np));
                int length = upper_bound - lower_bound;

                printf("RCV ->  %d  :::  %d - %d (Size: %d, %d)\n", tgt, lower_bound, upper_bound, recvSiz, length);
                MPI_Recv(distance + lower_bound, length, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
            }
        }

        rank++;
    }

    rank = 0;
    // Distribute data to processes.
    while (rank < maxRank)
    {
        if (pid < pow(2, rank))
        {
            tgt = pid + pow(2, rank);
            if (tgt >= np)
            {
                break;
            }
            MPI_Send(distance, n, MPI_INT, tgt, mtag, MPI_COMM_WORLD);
        }
        else if (pid < pow(2, rank) * 2)
        {
            tgt = pid - pow(2, rank);
            MPI_Recv(distance, n, MPI_INT, tgt, mtag, MPI_COMM_WORLD, &status);
        }

        rank++;
    }

    return;
}

// ===================================================================== //
// Input: n and edge[n][n], where n is the number of vertices of a graph //
// edge[i][j] is the length of the edge from vertex i to vertex j        //
// Output: distance[n], the distance from the SOURCE vertex to vertex i. //
// ===================================================================== //
void shortest_path_parallel(int SOURCE, int n, int **edge, int *distance)
{
    int np, pid;
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int i, j, count, tmp, least, leastPos, *found, upper_bound, lower_bound;

    // Calculate the boundaries of distance this process will be covering
    lower_bound = lower_bound(pid, n, np);
    upper_bound = upper_bound(pid, n, np);

    found = (int *)calloc(n, sizeof(int));
    for (i = 0; i < n; i++)
    {
        found[i] = 0;
        distance[i] = edge[SOURCE][i];
    }
    found[SOURCE] = 1;
    count = 1;
    while (count < n)
    {
        // Find portion of least on each process
        least = 987654321;
        for (i = lower_bound; i < upper_bound; i++)
        {
            tmp = distance[i];
            if ((!found[i]) && (tmp < least))
            {
                least = tmp;
                leastPos = i;
            }
        }

        // First communication to synchronize the least position
        leastPos = SynchronizeLeastPos(leastPos, least);
        least = distance[leastPos];

        // Calculate portion of distance on each process
        found[leastPos] = 1;
        count++;
        for (i = lower_bound; i < upper_bound; i++)
        { 
            if (!(found[i]))
                distance[i] = min(distance[i], least + edge[leastPos][i]);
        }

        // Second communication to synchronize the distance
        SynchronizeDistance(distance, n);
        printf("\n{ ");
        for (int i = 0; i < n; i++)
        {
            printf("%d ", distance[i]);
        }
        printf(" }");
    }
    free(found);
}

void main(int argc, char **argv)
{
    int mtag = 1;
    int pid, np;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    
    int n = 16;
    int** edge;
    int* dist;
    int seed = 20;

    srand(seed);

    // Initialize edge array
    edge = (int**)calloc(n, sizeof(int*));
    for (int i = 0; i < n; i++)
    {
        edge[i] = (int*)calloc(n, sizeof(int));
        for (int j = 0; j < n; j++)
        {
            edge[i][j] = rand() % 100;
            if (i == j) edge[i][j] = 0;
        }
    }

    /*if (pid == 0)
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                printf("%3d ", edge[i][j]);
            }
            printf("\n");
        }
    }*/

    dist = (int*)calloc(n, sizeof(int));
    
    shortest_path_parallel(0, n, edge, dist);

    if (pid >= 0)
    {
        for (int i = 0; i < n; i++)
        {
            printf("%3d ", dist[i]);
        }
        srand(seed);
        int* dist_verify = (int*)calloc(n, sizeof(int));
        shortest_path(0, n, edge, dist_verify);

        int mismatches = 0;
        for (int i = 0; i < n; i++)
        {
            if (dist[i] != dist_verify[i])
            {
                mismatches++;
            }
        }

        if (mismatches > 0)
        {
            printf("\n[%d] ERROR: %d mismatches in output result.\n", pid, mismatches);
        }
        else
        {
            printf("\nVerification passed.");
            printf("\n-=-=-=-=-=-\nAnwers:\n");
        }

        for (int i = 0; i < n; i++)
        {
            printf("%3d ", dist_verify[i]);
        }
    }

    // Free edge array
    for (int i = 0; i < n; i++)
    {
        free(edge[i]);
    }
    free(edge);

    MPI_Finalize();
    return;
}
