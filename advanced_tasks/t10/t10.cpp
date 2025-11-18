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
	int Iam, p; // rank — ранг (id) текущего процесса, size — общее число процессов
	double h, a, b, pi;
	int n, master = 0;
	double my_result, result;
	double start_time, end_time, elapsed_time;
	MPI_Status status; // структура для получения статуса приёма сообщений
	MPI_Request req[1]; // массив для хранения запросов на отправку/приём сообщений

	// Инициализация MPI — обязателен вызов перед любыми другими MPI-функциями
	// Передаём argc/argv, чтобы MPI обрабатывал опции командной строки
	MPI_Init(&argc, &argv);
	// Получаем ранг текущего процесса в коммуникаторе MPI_COMM_WORLD
	MPI_Comm_rank(MPI_COMM_WORLD, &Iam);
	// Получаем общее число процессов в коммуникаторе MPI_COMM_WORLD
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
	start_time = MPI_Wtime();
	// Собираем результаты от всех процессов в переменную result на процессе master
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

	// Завершение работы MPI — обязателен вызов после всех MPI-функций
	MPI_Finalize();
	return 0;
}