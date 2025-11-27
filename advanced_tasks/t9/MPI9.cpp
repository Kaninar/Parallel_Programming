#include <mpi.h>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

int main(int argc, char* argv[])
{
    const int n = 4;
    int size;
    int rank;

    MPI_Status status;
    double matrix[n][n] = { 0 };
    double matrixT[n][n] = { 0 };
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double recieve[n] = { 0 };
    double resultmatrix[n][n] = { 0 };

    for (int i = 0, count = 0; i < n; ++i)
    {
        for (int j = 0; j < n; matrix[i][j] = count++, ++j)
        {
        }
    }
	// Транспонирование матрицы
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

    double vector[n] = { 0 };
    for (int i = 0; i < n; ++i)
    {
        vector[i] = i;
    }
    // Распределение строк траспонированной матрицы по процессам
    // MPI_Scatter - Распределяет данные из одного члена по всем членам группы
	// Выполняет обратную операцию по отношению к MPI_Gather
	// MPI_Scatter(*sendbuf, sendcount, sendtype, *recvbuf, recvcount, recvtype, root, comm)
	// Параметры функции:
    // *sendbuf - Адрес буфера отправки (на выбор)
	// sendcount - Количество элементов, отправленных каждому процессу 
	// sendtype - Тип данных отправляемых элементов
	// *recvbuf - Адрес буфера приёма (на выбор)
	// recvcount - Количество элементов в буфере получения (целое число)
	// recvtype - Тип данных принимаемых элементов
	// root - Ранг процесса-отправителя
	// comm - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
    MPI_Scatter(matrix, n, MPI_DOUBLE, recieve, n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    // Скалярное произведение векторов
    double sum = 0;
#pragma omp parallel for reduction(+ : sum)
    for (int i = 0; i < n; ++i)
    {
        sum += recieve[i] * vector[i];
    }
    // Сборка произведений из параллельных процессов
    double result[n] = { 0 };
	// MPI_Gather - Собирает данные от всех участников группы к одному участнику
	// Выполняет обратную операцию по отношению к MPI_Scatter
    // MPI_Gather(*sendbuf, sendcount, sendtype, *recvbuf, recvcount, recvtype, root, comm)
	// Параметры функции:
    // *sendbuf - Начальный адрес буфера отправки (на выбор)
	// sendcount - Количество элементов в буфере отправки (целое число)
	// sendtype - Тип данных отправляемых элементов
	// *recvbuf - Адрес буфера приёма (на выбор)
	// recvcount - Количество элементов для любого отдельного приёма
	// recvtype - Тип данных принимаемых элементов
	// root - Ранг процесса-ресивера
	// comm - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
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