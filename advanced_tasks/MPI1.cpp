#include <mpi.h>
#include <iostream>
#include <locale>
#include <windows.h>
using namespace std;

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int rank, size;
    int value;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (true) {
        if (rank == 0) {
            cout << "\nInput integer (negative for out): ";
            cin >> value;

            for (int i = 1; i < size; i++) {
                MPI_Send(&value, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        }
        else {
            MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        }

        if (value < 0) {
            break;
        }

        cout << "Proc " << rank << " received value " << value << endl;
    }

    MPI_Finalize();
    return 0;
}
