#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>

int main(int argc, char *argv[])
{
    int BUFFER_SIZE = sizeof(double) * 2 + MPI_BSEND_OVERHEAD, // размер буффера
        rank,                                                  // текущий id процесса
        size;                                                  // количество MPI процессов (указываются в аргументах '-n <число процессов>')
    double values[2],
        prod,
        scalar_prod;
    void *bsend_buf = NULL;
    MPI_Status status;

    // инициализация MPI
    MPI_Init(&argc, &argv);

    // определение ранга текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // определение количества процессов в коммуникаторе
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<double> v1(size - 1);
    std::vector<double> v2(size - 1);

    // происходит конвертация типов, и тип обозначен как int, но для выполнения задания достаточно
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{1, 10};

    auto gen = [&]()
    {
        return dist(mersenne_engine);
    };

    std::generate(v1.begin(), v1.end(), gen);
    std::generate(v2.begin(), v2.end(), gen);

    /*  создание буффера путем выделения памяти
        и назначение его соответствующего размера
    */
    MPI_Buffer_attach(malloc(BUFFER_SIZE), BUFFER_SIZE);

    if (rank)
    {
        // если не master процесс (rank > 0), то чтение буффера значений
        MPI_Recv(values, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

        prod = values[0] * values[1];

        // отправка результата обратно master процессу (rank == 0)
        MPI_Send(&prod, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (int i = 1; i < size; ++i)
        {
            values[0] = v1[i - 1];
            values[1] = v2[i - 1];

            // отпрабка буффера значений каждому i-му child процессу (rank > 0 {1..6})
            MPI_Bsend(values, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

            // получение результата умножения от i-го child процесса
            MPI_Recv(&prod, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);

            scalar_prod += prod;
        }

        // округление, так как все в векторах только int
        printf("Result: %.0lf\n", scalar_prod);
    }

    // освобождение выделенного буффера
    MPI_Buffer_detach(&bsend_buf, &BUFFER_SIZE);
    free(bsend_buf);

    // завершение работы MPI
    MPI_Finalize();

    return 0;
}
