#include <mpi.h>
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
    int rank, size; // rank — ранг (id) текущего процесса, size — общее число процессов
    int value;
    MPI_Status status; // структура для получения статуса приёма сообщений

    // Инициализация MPI — обязателен вызов перед любыми другими MPI-функциями
    // Передаём argc/argv, чтобы MPI обрабатывал опции командной строки
    MPI_Init(&argc, &argv);

    // Получаем ранг текущего процесса в коммуникаторе MPI_COMM_WORLD
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Получаем общее число процессов в коммуникаторе MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (true)
    {
        // Читаем значение из процесса 0
        if (rank == 0)
        {
            cout << "\nInput integer (negative for out): ";
            cin >> value;

            // Отправляем введённое значение каждому другому процессу (rank 1..size-1)
            for (int i = 1; i < size; i++)
            {
                // MPI_Send(BUF, COUNT, DATATYPE, DEST, MSGTAG, COMM, IERR)
                // BUF - массив данных для отправки
                // COUNT - количество элементов в массиве
                // DATATYPE - тип данных (MPI_INT, MPI_FLOAT, MPI_DOUBLE и т.д.)
                // DEST - ранг процесса-получателя
                // MSGTAG - тег сообщения (целое число для идентификации типа сообщения)
                // COMM - коммуникатор (обычно MPI_COMM_WORLD)
                // IERR - код ошибки (обычно не используется в C/C++)
                MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        else
        {
            // Все остальные процессы получают сообщение от процесса 0
            // MPI_Recv — блокирующий вызов: он будет ждать, пока сообщение не будет получено
            // MPI_Recv(BUF, COUNT, DATATYPE, DEST, MSGTAG, COMM, STATUS, IERR)
            // BUF - массив для приёма данных
            // COUNT - количество элементов для приёма
            // DATATYPE - тип данных (MPI_INT, MPI_FLOAT, MPI_DOUBLE и т.д.)
            // DEST - ранг процесса-отправителя (или MPI_ANY_SOURCE для приёма от любого)
            // MSGTAG - тег сообщения (или MPI_ANY_TAG для приёма с любым тегом)
            // COMM - коммуникатор (обычно MPI_COMM_WORLD)
            // STATUS - структура для получения статуса приёма (источник, тег, длина и т.д.)
            // IERR - код ошибки (обычно не используется в C/C++)
            MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        // Если введённое значение отрицательное выходим из цикла
        if (value < 0)
        {
            break;
        }

        // Печать полученного значения по порядку процессов
        // Циклом проходим по процессам
        for (int i = 0; i < size; i++)
        {
            // Синхронизируем Barrier: все процессы ждут, пока не достигнут этой точки
            // MPI_Barrier(COMM, IERR)
            // COMM - коммуникатор (обычно MPI_COMM_WORLD)
            // IERR - код ошибки (обычно не используется в C/C++)
            MPI_Barrier(MPI_COMM_WORLD);

            // Процесс печатает свою строчку
            if (rank == i)
            {
                cout << "Proc " << rank << " received value " << value << endl;
            }
        }
        // Еще раз пишем барьер чтобы процессы гарантированно завершили вывод и можно было снова взаимодействовать
        MPI_Barrier(MPI_COMM_WORLD);
    }

    // Завершаем работу MPI — освобождаются ресурсы, можно вызывать только один раз в конце
    MPI_Finalize();

    return 0;
}
