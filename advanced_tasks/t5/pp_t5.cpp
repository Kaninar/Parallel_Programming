#include <iostream>
#include <omp.h>
#include <math.h>
#include <random>
using namespace std;

#define MIN_RANDOM_BOUND 1
#define MAX_RANDOM_BOUND 3
#define MAX_ITERATIONS 1000

double const eps = 0.01;

double *gaussian(double **a, double *b, int n);

double **generate_random_matrix(int n);

double *generate_y(double **a, int n);

double *iter(double **a, double *y, int n)
{
    double error = 0.0;
    double epsilon = 0.001;
    int iteration = 0;
    double *x = new double[n];
    double *Xn = new double[n];

#pragma omp parallel for
    for (int i = 0; i < n; i++)
        x[i] = y[i];

    do
    {
#pragma omp parallel for schedule(dynamic, 5)
        for (int i = 0; i < n; i++)
        {
            double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
            for (int j = 0; j < n; ++j)
                if (i != j)
                    sum = a[i][j] * x[j];

            Xn[i] = (y[i] - sum) / a[i][i];
        }

        error = 0.0;
#pragma omp parallel for reduction(max : error)
        for (int i = 0; i < n; i++)
            error = abs(Xn[i] - x[i]);

#pragma omp parallel for
        for (int i = 0; i < n; i++)
            x[i] = Xn[i];

    } while (error > epsilon && ++iteration < MAX_ITERATIONS);

    return x;
}

int main()
{
    int n = 100;

    double **A = generate_random_matrix(n);
    double *B = generate_y(A, n);

    double time = omp_get_wtime();

    double *x = iter(A, B, n);

    time = omp_get_wtime() - time;
    printf("Простые итерации\n");
    printf("Elapsed secs: %lf\n", time);
    printf("Ticks: %lf\n", omp_get_wtick());

    cout << endl;

    time = omp_get_wtime();
    double *sx = gaussian(A, B, n);

    time = omp_get_wtime() - time;
    printf("Гаусса\n");
    printf("Elapsed secs: %lf\n", time);
    printf("Ticks: %lf\n", omp_get_wtick());

    system("pause");

    return 0;
}

double *gaussian(double **a, double *b, int n)
{
    double *x = new double[n];
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
        x[i] = 0;

    for (int i = 0; i < n; ++i)
    {
        double divider = a[i][i];

#pragma omp parallel for
        for (int j = i; j < n; ++j)
            a[i][j] /= divider;

        b[i] /= divider;

        for (int j = i + 1; j < n; ++j)
        {
            double multiplier = a[j][i];

            for (int k = i; k < n; ++k)
            {
                a[j][k] -= a[i][k] * multiplier;
            }

            b[j] -= b[i] * multiplier;
        }
    }

    for (int i = n - 1; i >= 0; --i)
    {
        double sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
        for (int j = n - 1; j > i; --j)
        {
            sum = x[j] * a[i][j];
        }
        x[i] = b[i] - sum;
    }

    return x;
}

double **generate_random_matrix(int n)
{
    double **a = new double *[n];

    for (int i = 0; i < n; ++i)
    {
        a[i] = new double[n];

        for (int j = 0; j < n; ++j)
        {
            a[i][j] = (double)(rand() % MAX_RANDOM_BOUND - MIN_RANDOM_BOUND + 1) + (double)MIN_RANDOM_BOUND;
        }
        cout << endl;
    }

    return a;
}

double *generate_y(double **a, int n)
{
    double *x = new double[n];
    double *b = new double[n];

    for (int i = 0; i < n; ++i)
    {
        x[i] = (double)(rand() % MAX_RANDOM_BOUND - MIN_RANDOM_BOUND + 1) + (double)MIN_RANDOM_BOUND;
    }

    for (int i = 0; i < n; ++i)
    {
        b[i] = 0;
        for (int j = 0; j < n; ++j)
        {
            b[i] += x[j] * a[i][j];
        }
    }

    return b;
}