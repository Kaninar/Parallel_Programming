#include <mpi.h>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <iterator>

int main(int argc, char *argv[])
{
    int BUFFER_SIZE = sizeof(double) * 2 + MPI_BSEND_OVERHEAD, // Размер буфера
        rank,
        size;
    double values[2],
        prod,
        scalar_prod;
    void *bsend_buf = NULL;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<double> v1(size - 1);
    std::vector<double> v2(size - 1);
    // Происходит конвертация типов, и тип обозначен как int, 
    // но для выполнения задания достаточно
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{1, 10};

    auto gen = [&]()
    {
        return dist(mersenne_engine);
    };

    std::generate(v1.begin(), v1.end(), gen);
    std::generate(v2.begin(), v2.end(), gen);
	// MPI_Buffer_attach выделяет буфер для использования 
    // в операциях буферизованной отправки (MPI_Bsend)
    // В каждом процессе может быть только один такой буфер
	// MPI_Buffer_attach(BUFFER, SIZE)
	// Параметры функции:
	// BUFFER - Начальный адрес буфера (на выбор)
	// SIZE - Размер буфера в байтах (целое число)
    MPI_Buffer_attach(malloc(BUFFER_SIZE), BUFFER_SIZE);

    if (rank)
    {
        // Если не master процесс (rank > 0), то чтение буфера значений
        MPI_Recv(values, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &status);

        prod = values[0] * values[1];
        // Отправка результата обратно master процессу (rank == 0)
        MPI_Send(&prod, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        for (int i = 1; i < size; ++i)
        {
            values[0] = v1[i - 1];
            values[1] = v2[i - 1];
            // Отправка буфера значений каждому i-му child процессу (rank > 0 {1..6})
			// MPI_Bsend — буферизованная отправка: выполняет операцию 
            // отправки в буферизованном режиме.
			// MPI_Bsend(BUF, COUNT, DATATYPE, DEST, MSGTAG, COMM)
			// Параметры функции:
			// BUF - Начальный адрес буфера отправки (на выбор)
			// COUNT - Количество записей в буфере отправки (целое число, большее нуля)
			// DATATYPE - Тип данных (MPI_INT, MPI_FLOAT, MPI_DOUBLE и т.д.)
			// DEST - Ранг процесса-получателя
			// MSGTAG - Тег сообщения (целое число для идентификации типа сообщения)
			// COMM - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
            MPI_Bsend(values, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            // Получение результата умножения от i-го child процесса
            MPI_Recv(&prod, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &status);

            scalar_prod += prod;
        }
        // Округление, так как все в векторах только int
        printf("Result: %.0lf\n", scalar_prod);
    }
	// MPI_Buffer_detach - Удаляет существующий буфер 
    // (для использования в MPI_Bsend и т. д.)
	// После вызова этой функции буфер больше не используется для
	// операций буферизованной отправки
	// MPI_Buffer_detach(BUFFER, SIZE)
	// Параметры функции:
	// BUFFER - Начальный адрес буфера (на выбор)
	// SIZE - Размер буфера в байтах (целое число)
    MPI_Buffer_detach(&bsend_buf, &BUFFER_SIZE);
    free(bsend_buf);
    MPI_Finalize();

    return 0;
}
