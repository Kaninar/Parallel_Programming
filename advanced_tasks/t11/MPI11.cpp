#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, char* argv[]) {
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size % 2 != 0) {
        if (rank == 0)
            // Проверка на четность числа процессов
			cout << "Error: number of processes must be even." << endl;
        MPI_Finalize();
        return 0;
    }

    int partner = (rank + size / 2) % size; // Процесс на расстоянии size/2
	double send_val = rank * 1.0; // Значение для отправки
	double recv_val = 0.0; // Значение для получения

    const int iterations = 100000 / size; // Число итераций для усреднения
    const int repeats = 10; // Повторов для статистики

    double start_time, end_time;
    double elapsed_time = 0.0, avg_time = 0.0, global_avg = 0.0;

	vector<double> repeat_times(repeats, 0.0); // Времена для каждого повтора
	// Основной цикл измерений
    for (int r = 0; r < repeats; r++) {
		// Синхронизация перед началом измерений
        MPI_Barrier(MPI_COMM_WORLD);
        // Замеряем время выполнения операции отправки и получения данных
        start_time = MPI_Wtime();
		// Цикл обмена сообщениями
        for (int i = 0; i < iterations; i++) {
			// Одновременная отправка и получение данных
			// MPI_Sendrecv: отправка и получение данных в одном вызове
			// MPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag,
			//              recvbuf, recvcount, recvtype, source, recvtag,
			//              comm, status)
			// sendbuf: Начальный адрес буфера отправки (на выбор)
			// sendcount: Количество отправляемых элемнетов (целое число)
			// sendtype: Тип данных для отправки
			// dest: Ранг процесса-получателя
			// sendtag: Метка сообщения для отправки
			// recvbuf: Начальный адрес буфера приема (на выбор)
			// recvcount: Максимальное количество получаемых элементов (целое число)
			// recvtype: Тип данных для получения
			// source: Ранг процесса-отправителя
			// recvtag: Метка сообщения для получения
			// comm: Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
			// status: Объект состояния, ссылающийся на операцию получения
            MPI_Sendrecv(&send_val, 1, MPI_DOUBLE, partner, 0,
                &recv_val, 1, MPI_DOUBLE, partner, 0,
                MPI_COMM_WORLD, &status);
        }
        end_time = MPI_Wtime();
        repeat_times[r] = end_time - start_time;
        elapsed_time += repeat_times[r];
    }

    avg_time = elapsed_time / repeats;

    double* avg_buf = nullptr;
    if (rank == 0) avg_buf = new double[size];
	// Сбор средних времен от всех процессов
    MPI_Gather(&avg_time, 1, MPI_DOUBLE,
        avg_buf, 1, MPI_DOUBLE,
        0, MPI_COMM_WORLD);

    double* all_repeat_times = nullptr;
    if (rank == 0) all_repeat_times = new double[size * repeats];
	// Сбор времен повторов от всех процессов
    MPI_Gather(repeat_times.data(), repeats, MPI_DOUBLE,
        all_repeat_times, repeats, MPI_DOUBLE,
        0, MPI_COMM_WORLD);
	// Вычисление глобального среднего времени
    // MPI_Reduce - Выполняет глобальную операцию уменьшения для всех членов группы
	// MPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm)
	// sendbuf: Адрес буфера отправки (на выбор)
	// recvbuf: Адрес буфера получения (на выбор)
	// count: Количество элементов в буфере отправки
	// datatype: Тип данных элементов буфера отправки
	// op: Выполняемая операция сокращения
	// root: Ранг принимающего процесса
	// comm: Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
    MPI_Reduce(&avg_time, &global_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        global_avg /= size;

        ofstream fout("mpi_results.txt");
        fout << fixed << setprecision(9);
        for (int i = 0; i < size; i++) {
            fout << "Process " << i << ":" << endl;
            for (int r = 0; r < repeats; r++) {
                fout << "  Repeat " << setw(2) << r + 1
                    << ": " << all_repeat_times[i * repeats + r] << " s" << endl;
            }
            fout << "  Average: " << avg_buf[i] << " s" << endl << endl;
        }

        fout << "Average send/recv time = " << global_avg
            << " seconds (averaged across " << size << " processes)" << endl;
        fout << "Message size = " << sizeof(double) << " bytes" << endl;
        fout << "Iterations per process = " << iterations << endl;
        fout.close();

        cout << fixed << setprecision(9);
        cout << "Average send/recv time = " << global_avg
            << " seconds (averaged across " << size << " processes)" << endl;
        cout << "Message size = " << sizeof(double) << " bytes" << endl;
        cout << "Iterations per process = " << iterations << endl;

        cout << "Results saved to mpi_results.txt" << endl;

        delete[] avg_buf;
        delete[] all_repeat_times;
    }

    MPI_Finalize();
    return 0;
}