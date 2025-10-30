#include <iostream>
#include <omp.h>

int main(int argc, char const *argv[])
{
    double *a; // нижняя диагональ
    double *b; // центральная
    double *c; // верхняя
    double *d; // вектор свободных членов
    double *A; // прогоночные коэффициенты
    double *B;
    double *x; // корни

    A[0] = c[0] / b[0];
    B[0] = d[0] / b[0];

    for (int i = 1; i < n - 1; i++)
    {
        A[i] = c[i] / (b[i] - a[i - 1] * A[i - 1]);
        B[i] = (d[i] - a[i - 1] * B[i - 1]) / (b[i] - a[i - 1] * A[i - 1]);
    }

    x[n - 1] = B[n - 1];

    for (int i = n - 1 - 1; i >= 0; --i)
    {
        x[i] = B[i] - A[i] * x[i + 1];
    }

    return 0;
}
