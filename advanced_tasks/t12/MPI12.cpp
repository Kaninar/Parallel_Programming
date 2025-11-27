#include <mpi.h>
#include <iostream>
#include <vector>
#include <iomanip>

int main(int argc, char* argv[])
{
    MPI_Init(&argc, &argv);

    int rank, size;
    int buf[2];
	MPI_Request reqs[4];
	MPI_Status stats[4];

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
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

    // Ожидание выполнения остальных процессов в указанном коммуникаторе
    MPI_Barrier(MPI_COMM_WORLD);

    // Астрономическое время в секундах на момент начала процедуры
    double start_time = MPI_Wtime();
	// Неблокирующий прием данных от соседних процессов
	// MPI_Irecv - Неблокирующее получение в стандартном режиме
	// MPI_Irecv(buf, count, datatype, source, tag, comm, request)
	// Параметры функции:
	// buf - Начальный адрес буфера приёма (на выбор)
	// count - Количество элементов в буфере приёма (целое число)
	// datatype - Тип данных каждого элемента буфера приёма
	// source - Ранг процесса-отправителя
	// tag - Метка сообщения
	// comm - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
	// request - Запрос на связь (дескриптор)
    MPI_Irecv(&buf[0], 1, MPI_INT, prev, 5, MPI_COMM_WORLD, &reqs[0]);
    MPI_Irecv(&buf[1], 1, MPI_INT, next, 6, MPI_COMM_WORLD, &reqs[1]);
    // MPI_Send - неблокирующая отправка в стандартном режиме
	// MPI_Send(buf, count, datatype, dest, tag, comm, request)
	// Параметры функции:
	// buf - Начальный адрес буфера отправки (на выбор)
	// count - Количество элементов в буфере отправки (целое число)
	// datatype - Тип данных каждого элемента буфера отправки
	// dest - Ранг процесса-получателя
	// tag - Метка сообщения
	// comm - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
	// request - Запрос на связь (дескриптор)
    MPI_Isend(&rank, 1, MPI_INT, prev, 6, MPI_COMM_WORLD, &reqs[2]);
    MPI_Isend(&rank, 1, MPI_INT, next, 5, MPI_COMM_WORLD, &reqs[3]);
	// Ожидание завершения всех указанных операций связи
    // MPI_Waitall(count, requests, statuses)
	// Параметры функции:
	// count - Длина списка (целое число)
	// requests - Массив запросов (массив дескрипторов)
	// statuses - Массив статусов
    MPI_Waitall(4, reqs, stats);

    int local_sum = buf[0] + buf[1] + rank;
    int global_sum = 0;
	// Сбор значений local_sum от всех процессов в global_sum на всех процессах
    MPI_Allreduce(&local_sum, &global_sum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    // Ожидание выполнения остальных процессов в указанном коммуникаторе
    MPI_Barrier(MPI_COMM_WORLD);

    // Астрономическое время в секундах на момент окончания процедуры
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
	// Сбор значений elapsed_time от всех процессов в total_time на процессе с рангом 0
    MPI_Reduce(&elapsed_time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << "Average communication time: "
            << (total_time / size) << " sec" << std::endl;
    }

    MPI_Finalize();
    return 0;
}