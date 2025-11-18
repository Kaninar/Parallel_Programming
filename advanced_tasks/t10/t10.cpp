#include <mpi.h>
#include <iostream>
#include <cmath>
#include <iomanip>
using namespace std;

double integral(double a, int i, double h, int n)
{
	auto fct = [](double x)
	{
		return cos(x);
	};
	double sum = 0.0;
	double h2 = h / 2.0;

#pragma omp parallel for reduction(+ : sum)
	for (int j = 0; j < n; ++j)
	{
		double aij = a + (i * n + j) * h;
		sum += fct(aij + h2) * h;
	}

	return sum;
}

int main(int argc, char *argv[])
{
	int n, size;
	double h, a, b, pi;
	int rank, master = 0;
	double inter_result, final_result = 0.0;
	double start_time, end_time, elapsed_time;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	pi = acos(-1.0);
	a = 0.0;
	b = pi / 2.0;
	n = 500;
	h = (b - a) / n / size;
	inter_result = integral(a, rank, h, n);

	MPI_Barrier(MPI_COMM_WORLD);
	start_time = MPI_Wtime();

	MPI_Allreduce(&inter_result, &final_result, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	end_time = MPI_Wtime();
	elapsed_time = end_time - start_time;

	cout << "Process " << rank
		 << ", final_result = " << inter_result
		 << ", elapsed time = " << elapsed_time << endl;

	if (rank == master)
	{
		cout << "Final final_result = " << std::fixed << std::setprecision(32) << final_result
			 << endl;
	}

	MPI_Finalize();

	return 0;
}