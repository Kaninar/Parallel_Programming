#include <mpi.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
using namespace std;

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size % 2 != 0)
    {
        if (rank == 0)
            cout << "Error: number of processes must be even." << endl;
        MPI_Finalize();
        return 0;
    }

    int partner = (rank + size / 2) % size;
    double send_val = rank * 1.0;
    double recv_val = 0.0;

    const int iterations = 100000 / size;
    const int repeats = 10;

    double start_time, end_time;
    double elapsed_time = 0.0, avg_time = 0.0, global_avg = 0.0;

    vector<double> repeat_times(repeats, 0.0);

    for (int r = 0; r < repeats; r++)
    {
        MPI_Barrier(MPI_COMM_WORLD);
        start_time = MPI_Wtime();

        for (int i = 0; i < iterations; i++)
        {
            MPI_Sendrecv(&send_val, 1, MPI_DOUBLE, partner, 0,
                         &recv_val, 1, MPI_DOUBLE, partner, 0,
                         MPI_COMM_WORLD, &status);
        }

        end_time = MPI_Wtime();
        repeat_times[r] = end_time - start_time;
        elapsed_time += repeat_times[r];
    }

    avg_time = elapsed_time / repeats;

    double *avg_buf = nullptr;
    if (rank == 0)
        avg_buf = new double[size];
    MPI_Gather(&avg_time, 1, MPI_DOUBLE,
               avg_buf, 1, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    double *all_repeat_times = nullptr;
    if (rank == 0)
        all_repeat_times = new double[size * repeats];
    MPI_Gather(repeat_times.data(), repeats, MPI_DOUBLE,
               all_repeat_times, repeats, MPI_DOUBLE,
               0, MPI_COMM_WORLD);

    MPI_Reduce(&avg_time, &global_avg, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        global_avg /= size;

        ofstream fout("mpi_results.txt");
        fout << fixed << setprecision(9);
        for (int i = 0; i < size; i++)
        {
            fout << "Process " << i << ":" << endl;
            for (int r = 0; r < repeats; r++)
            {
                fout << "  Repeat " << setw(2) << r + 1
                     << ": " << all_repeat_times[i * repeats + r] << " s" << endl;
            }
            fout << "  Average: " << avg_buf[i] << " s" << endl
                 << endl;
        }

        fout << "Average send/recv time = " << global_avg
             << " seconds (averaged across " << size << " processes)" << endl;
        << "Message size = " << sizeof(double) << " bytes\n";
        << "Iterations per process = " << iterations << endl;
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