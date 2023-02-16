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

void Verify(int input_row_sum[100])
{
    int correct_sums[] = { 328350,
        328450, 328550, 328650, 328750, 328850, 328950, 329050, 329150, 329250, 329350,
        329450, 329550, 329650, 329750, 329850, 329950, 330050, 330150, 330250, 330350,
        330450, 330550, 330650, 330750, 330850, 330950, 331050, 331150, 331250, 331350,
        331450, 331550, 331650, 331750, 331850, 331950, 332050, 332150, 332250, 332350,
        332450, 332550, 332650, 332750, 332850, 332950, 333050, 333150, 333250, 333350,
        333450, 333550, 333650, 333750, 333850, 333950, 334050, 334150, 334250, 334350,
        334450, 334550, 334650, 334750, 334850, 334950, 335050, 335150, 335250, 335350,
        335450, 335550, 335650, 335750, 335850, 335950, 336050, 336150, 336250, 336350,
        336450, 336550, 336650, 336750, 336850, 336950, 337050, 337150, 337250, 337350,
        337450, 337550, 337650, 337750, 337850, 337950, 338050, 338150, 338250
    };

    int mismatches = 0;
    for (int i = 0; i < 100; i++)
    {
        if (input_row_sum[i] != correct_sums[i])
        {
            mismatches++;
        }
    }

    if (mismatches > 0)
    {
        printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n ERROR: %d mismatches in result!\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", mismatches);
    }
    else
    {
        printf("\nProgram ran successfully with 0 errors.");
    }

    return;
}

void main(int argc, char **argv)
{
    int i, t,/*t is i + 51*/ j, pid, np, mtag, count, data[100][100], data_recv[100], row_sum[100];
    double t0, t1;
    MPI_Status status;
    MPI_Request req_s, req_r, req_finished;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    double startTime = MPI_Wtime();

    if (pid == 0)
    {
        // for each of the first 50 (i) rows (and for each of the last 50 rows (t))
        for (i = 0; i < 50; i++)
        {
            t = i + 50;
            //for each element in each row
            for (j = 0; j < 100; j++)
            {
                //generate data[]
                data[i][j] = generate_data(i, j);
                data[t][j] = generate_data(i, j);
            }
            mtag = 1;
            //if the first, dont wait, nothing to wait on, other ones wait for pid 1
            if (i > 0)
                MPI_Wait(&req_s, &status);
            //non-blocking send to pid 1 the ith row
            MPI_Isend(data[i], 100, MPI_INT, 1, mtag, MPI_COMM_WORLD, &req_s);
            row_sum[t] = 0;
            for (j = 0; j < 100; j++){
                row_sum[t] += data[t][j];
            }
        }
        /*** receive computed row_sums from pid 1 ***/
        mtag = 2;
        MPI_Recv(row_sum, 50, MPI_INT, 1, mtag, MPI_COMM_WORLD, &status);
        for (i = 0; i < 100; i++)
        {
            printf(" %d ", row_sum[i]);
            if (i % 10 == 0)
            {
                printf("\n");
            }
        }
    }
    else /*** pid == 1 ***/
    {
        for (i = 0; i < 50; i++)
        {
            mtag = 1;
            MPI_Recv(data_recv, 100, MPI_INT, 0, mtag, MPI_COMM_WORLD, &status);
            row_sum[i] = 0;
            for (j = 0; j < 100; j++)
            {
                row_sum[i] += data_recv[j];
            }
        }
        /*** Send computed row_sums to pid 0 ***/
        mtag = 2;
        MPI_Send(row_sum, 50, MPI_INT, 0, mtag, MPI_COMM_WORLD);
    } /****** End of else ******/
    double deltaTime = MPI_Wtime() - startTime;
    MPI_Finalize();

    if (pid == 0) { Verify(row_sum); }
}