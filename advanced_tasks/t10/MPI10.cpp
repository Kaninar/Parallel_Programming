#include <mpi.h>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

double integral(double a, int i, double h, int n) {
	auto fct = [](double x) {
		return cos(x);
		};
	double sum = 0.0;
	double h2 = h / 2.0;

	for (int j = 0; j < n; j++) {
		double aij = a + (i * n + j) * h;
		sum += fct(aij + h2) * h;
	}
	return sum;
}

int main(int argc, char* argv[]) {
	int Iam, p;
	double h, a, b, pi;
	int n, master = 0;
	double my_result, result;
	double start_time, end_time, elapsed_time;
	MPI_Status status;
	MPI_Request req[1]; // Массив для хранения запросов на отправку/приём сообщений

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &Iam);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	pi = acos(-1.0);
	a = 0.0;
	b = pi / 2.0;
	n = 500;

	int dest = master;
	int tag = 123;

	h = (b - a) / n / p;
	my_result = integral(a, Iam, h, n);
	// Синхронизируем все процессы перед началом измерения времени
	MPI_Barrier(MPI_COMM_WORLD);
	// Замеряем время выполнения функции MPI_Allreduce
	// MPI_Wtime(void) - Возвращает затраченное время на вызывающем процессоре
	start_time = MPI_Wtime();
	// Собираем результаты от всех процессов в переменную result на процессе master
	// MPI_Allreduce - Объединяет значения из всех процессов
	// и распределяет результат по всем процессам.
	// MPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm)
	// Параметры функции:
	// sendbuf - Начальный адрес буфера отправки (на выбор)
	// recvbuf - Начальный адрес буфера приёма (на выбор)
	// count - Количество элементов в буфере отправки (целое число)
	// datatype - Тип данных элементов буфера отправки
	// op - Операция (дескриптор)
	// comm - Коммуникатор (дескриптор, обычно MPI_COMM_WORLD)
	MPI_Allreduce(&my_result, &result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	end_time = MPI_Wtime();
	elapsed_time = end_time - start_time;

	cout << "Process " << Iam
		<< ", result = " << my_result
		<< ", elapsed time = " << elapsed_time << endl;

	if (Iam == master) {
		cout << "Final result = " << std::fixed << std::setprecision(6) << result
			<< endl;
	}

	MPI_Finalize();
	return 0;
}