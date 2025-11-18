#include <mpi.h>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

int main(int argc, char *argv[])
{
    int n = 4, size;
    int rank;

    MPI_Status status;
    double matrix[n][n] = {0};
    // cout << "text" << endl;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double recieve[n] = {0};
    double resultmatrix[n][n] = {0};

    int count = 0;
    for (int i = 0; i < n; ++i)
    {

        for (int j = 0; j < n; ++j)
        {
            matrix[i][j] = count++;
        }
    }

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            double temp1 = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = temp1;
        }
    }

    MPI_Scatter(matrix, n, MPI_DOUBLE, recieve, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // MPI_Sendrecv(recieve, n, MPI_DOUBLE, 0, 0, recieve, n, MPI_DOUBLE, rank, 0, MPI_COMM_WORLD, &status);
    // MPI_Gather(recieve, n, MPI_DOUBLE, resultmatrix, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    //  }

    for (int i = 0; i < n; ++i)
    {
        printf("%g ", recieve[i]);
    }
    cout << "\n";

    MPI_Finalize();

    return 0;
}
