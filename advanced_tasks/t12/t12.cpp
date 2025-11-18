#include <mpi.h>
#include <iostream>
#include <vector>
#include <iomanip>

int main(int argc, char *argv[])
{
    // инициализация MPI
    MPI_Init(&argc, &argv);

    int rank, size; // rank — ранг (id) текущего процесса, size — общее число процессов
    int buf[2]; // буфер для приема данных от соседних процессов
    MPI_Request reqs[4]; // массив запросов для неблокирующих операций
    MPI_Status stats[4]; // массив статусов для ожидания завершения операций

    // определение ранга текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // определение количества процессов в коммуникаторе
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int prev = rank - 1;
    int next = rank + 1;

    if (rank == 0)
    {
        prev = size - 1;
    }
    if (rank == size - 1)
    {
        next = 0;
    }

    // ожидание выполнения остальных процессов в указанном коммуникаторе
    MPI_Barrier(MPI_COMM_WORLD);

    // астрономическое время в секундах на момент начала процедуры
    double start_time = MPI_Wtime();

    // неблокирующий прием данных от соседних процессов
    MPI_Irecv(&buf[0], 1, MPI_INT, prev, 5, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(&buf[1], 1, MPI_INT, next, 6, MPI_COMM_WORLD, &reqs[1]);

	// неблокирующая отправка данных соседним процессам
    MPI_Isend(&rank, 1, MPI_INT, prev, 6, MPI_COMM_WORLD, &reqs[2]);
    MPI_Isend(&rank, 1, MPI_INT, next, 5, MPI_COMM_WORLD, &reqs[3]);

	// ожидание завершения всех неблокирующих операций
    MPI_Waitall(4, reqs, stats);

    int local_sum = buf[0] + buf[1] + rank;
    int global_sum = 0;

	// сбор значений local_sum от всех процессов в global_sum на всех процессах
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // ожидание выполнения остальных процессов в указанном коммуникаторе
    MPI_Barrier(MPI_COMM_WORLD);

    // астрономическое время в секундах на момент окончания процедуры
    double end_time = MPI_Wtime();
    double elapsed_time = end_time - start_time;

    std::cout << "Process " << rank
              << " prev=" << buf[0]
              << ", next=" << buf[1]
              << ", local_sum=" << local_sum
              << ", global_sum=" << global_sum
              << ", elapsed_time=" << std::fixed << std::setprecision(6) << elapsed_time
              << " sec" << std::endl;

    double total_time = 0.0;
    
    // сбор значений elapsed_time от всех процессов в total_time на процессе с рангом 0
    MPI_Reduce(&elapsed_time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << "Average communication time: "
                  << (total_time / size) << " sec" << std::endl;
    }

    // завершение работы MPI
    MPI_Finalize();

    return 0;
}