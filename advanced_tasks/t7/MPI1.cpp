#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int rank, size;
    int value;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    do
    {
        cout << "\nInput integer (negative for out): ";
        cin >> value;

        if (value < 0)
            break;

        for (int i = 1; i < size; ++i)
            MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);

        MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        for (int i = 0; i < size; i++)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            if (rank == i)
            {
                cout << "Proc " << rank << " received value " << value << endl;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}
