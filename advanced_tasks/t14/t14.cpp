#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int rank, size;
    const int MAXPROC = 128;
    const int NTIMES = 10000;
    int ibuf[MAXPROC];

    double time_start, time_finish;

    MPI_Request req[2 * MAXPROC];
    MPI_Status statuses[MAXPROC];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        for (int i = 1; i < size; i++)
        {
            MPI_Recv_init(&ibuf[i], 0, MPI_INT, i, 5, MPI_COMM_WORLD, &req[i - 1]);
            MPI_Send_init(&rank, 0, MPI_INT, i, 6, MPI_COMM_WORLD, &req[size - 1 + i]);
        }

        time_start = MPI_Wtime();

        for (int i = 0; i < NTIMES; i++)
        {
            MPI_Startall(size - 1, &req[0]);
            MPI_Waitall(size - 1, &req[0], &statuses[0]);

            MPI_Startall(size - 1, &req[size - 1]);
            MPI_Waitall(size - 1, &req[size - 1], &statuses[0]);
        }
    }
    else
    {
        MPI_Recv_init(&ibuf[0], 0, MPI_INT, 0, 6, MPI_COMM_WORLD, &req[0]);
        MPI_Send_init(&rank, 0, MPI_INT, 0, 5, MPI_COMM_WORLD, &req[1]);

        time_start = MPI_Wtime();

        for (int i = 0; i < NTIMES; i++)
        {
            MPI_Start(&req[1]);
            MPI_Wait(&req[1], &statuses[0]);

            MPI_Start(&req[0]);
            MPI_Wait(&req[0], &statuses[0]);
        }
    }

    time_finish = MPI_Wtime() - time_start;

    cout << "rank= " << rank
         << " all time= " << (time_finish / NTIMES) << endl;

    time_start = MPI_Wtime();

    for (int i = 0; i < NTIMES; i++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    time_finish = MPI_Wtime() - time_start;

    cout << "rank= " << rank
         << " barrier time= " << (time_finish / NTIMES) << endl;

    MPI_Finalize();

    return 0;
}