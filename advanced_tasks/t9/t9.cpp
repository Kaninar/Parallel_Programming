#include <mpi.h>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

int main(int argc, char *argv[])
{
    int n = 4, m = 4;
    int size;
    int rank;

    MPI_Status status;
    double matrix[n][n] = {0};
    double matrixT[n][n] = {0};
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double recieve[n] = {0};
    double resultmatrix[n][n] = {0};

    // инициализация 2d матрицы
    for (int i = 0, count = 0; i < n; ++i)
    {
        for (int j = 0; j < n; matrix[i][j] = count++, ++j)
        {
        }
    }

    // транспанирование матрицы
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            double temp = matrix[j][i];
            matrix[j][i] = matrix[i][j];
            matrix[i][j] = temp;
        }
    }

    double vector[n] = {0};
    for (int i = 0; i < n; ++i)
    {
        vector[i] = i;
    }

    // распределение строк траспонированной матрицы по процессам
    MPI_Scatter(matrix, n, MPI_DOUBLE, recieve, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // скалярное произведение векторов
    double sum = 0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; ++i)
    {
        sum += recieve[i] * vector[i];
    }

    // сборка произведений из параллельных процессов
    double result[n] = {0};
    MPI_Gather(&sum, n / size, MPI_DOUBLE, result, n / size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (!rank)
    {
        for (int i = 0; i < n; ++i)
        {
            printf("%g ", result[i]);
        }
    }

    MPI_Finalize();

    return 0;
}
