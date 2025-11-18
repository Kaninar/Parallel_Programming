#include <iostream>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size, buf[2];
    MPI_Status status1, status2;

    MPI_Init(&argc, &argv);
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

    /*  метод выполняет совмещенные прием и передачу сообщений с блокировкой.
        позволяет бороться с блокировкой, когда процесс ждёт ответа от заблокированного процесса
        который делает то же самое

        1 - отпрака значения предыдущему процессу
        2 - получение значечения от слудующего

        3 - отправка значения следующему
        4 - получение значение от следующего
    */
    MPI_Sendrecv(&rank, 1, MPI_INT, prev, 6, &buf[1], 1, MPI_INT, next, 6, MPI_COMM_WORLD, &status2);
    MPI_Sendrecv(&rank, 1, MPI_INT, next, 5, &buf[0], 1, MPI_INT, prev, 5, MPI_COMM_WORLD, &status1);

    std::cout << "Process " << rank
              << " prev=" << buf[0]
              << ", next=" << buf[1]
              << std::endl;

    MPI_Finalize();

    return 0;
}