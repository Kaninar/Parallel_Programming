#include <iostream>
#include <omp.h>

double f(double x, double y)
{
    return y - 2 * x / y;
}

double EulerR(double x, double x1, double y, int n)
{
    double f1, h, y1;
    h = (x1 - x) / (double)n;
    y1 = y;

    double time = omp_get_wtime();
#pragma omp parallel for
    for (int i = 0; i < n; ++i)
    {

        {
            // можно выполнить после изменения x
            f1 = f(x, y); // f_{i-1} = f(x_i-1, y_)
            x += h;       // x_i
            y += h * f1;  // y_{i}
        }
        // y_i = y_{i-1} + h * ( f_{i-1} + f_i ) / 2

        y = y1 + h * (f1 + f(x, y)) / 2.0;

        y1 = y;
    }
    time = omp_get_wtime() - time;
    printf("Elapsed secs: %lf\n", time);
    printf("Ticks: %lf\n", omp_get_wtick());
    return y;
}

int main(int argc, char const *argv[])
{
    double x = 0;
    double y = 2;
    double xn = 10;
    double n = 100;
    double time = omp_get_wtime();
    double res = EulerR(0.0, 10.0, 1.0, 100000);
    time = omp_get_wtime() - time;
    printf("Elapsed secs: %lf\n", time);
    printf("Ticks: %lf\n", omp_get_wtick());
    std::cin.get();
    return 0;
}
