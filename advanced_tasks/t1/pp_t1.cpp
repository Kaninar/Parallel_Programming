#include <iostream>
#include <cmath>
#include <functional>
#include <omp.h>
#include <windows.h>

const double a = 0;
const double b = 10;
const int n = 1000000;

double trapezoidalIntegral(double a, double b, int n, const std::function<double(double)> &f)
{
    const double width = (b - a) / n;

    double trapezoidal_integral = 0;

#pragma omp parallel for reduction(+ : trapezoidal_integral)
    for (int step = 0; step < n; step++)
    {
        const double x1 = a + step * width;
        const double x2 = a + (step + 1) * width;

        trapezoidal_integral += 0.5 * (x2 - x1) * (f(x1) + f(x2));
    }

    return trapezoidal_integral;
}

double simpsonIntegral(double a, double b, int n, const std::function<double(double)> &f)
{
    const double width = (b - a) / n;

    double simpson_integral = 0;
#pragma omp parallel for reduction(+ : simpson_integral)
    for (int step = 0; step < n; step++)
    {
        const double x1 = a + step * width;
        const double x2 = a + (step + 1) * width;

        simpson_integral += (x2 - x1) / 6.0 * (f(x1) + 4.0 * f(0.5 * (x1 + x2)) + f(x2));
    }

    return simpson_integral;
}

double rectangles(double left, double right, double step, const std::function<double(double)> &f)
{
    double result = 0, h = (b - a) / n;
#pragma omp parallel for reduction(+ : result)

    for (int i = 0; i < n; i++)
    {
        result += f(a + h / 2 + i * h);
    }

    result *= h;
    return result;
}

double f(double x)
{
    return (double)sin(x);
}

void benchmark(const std::function<double(double)> &integral_function)
{
    double itime, ftime, exec_time;
    itime = omp_get_wtime();

    ftime = omp_get_wtime();
    exec_time = ftime - itime;
    printf("\n\nTime taken is %f", exec_time);
}

int main(int argc, char const *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    double itime, ftime, exec_time, tick;
    itime = omp_get_wtime();

    std::cout << "Результат: " << trapezoidalIntegral(a, b, n, f) << std::endl;

    ftime = omp_get_wtime();
    tick = omp_get_wtick();
    exec_time = ftime - itime;
    printf("Занятое время %f\n", exec_time);
    printf("Тики %f\n", tick);

    std::cout << std::endl
              << "Метод Симпсона" << std::endl;
    itime = omp_get_wtime();

    std::cout << "Результат: " << simpsonIntegral(a, b, n, f) << std::endl;

    ftime = omp_get_wtime();
    tick = omp_get_wtick();
    exec_time = ftime - itime;
    printf("Занятое время %f\n", exec_time);
    printf("Тики %f\n", tick);

    std::cout << std::endl
              << "Метод прямоугольника" << std::endl;
    itime = omp_get_wtime();

    std::cout << "Результат: " << rectangles(a, b, n, f) << std::endl;

    ftime = omp_get_wtime();
    tick = omp_get_wtick();
    exec_time = ftime - itime;
    printf("Занятое время %f\n", exec_time);
    printf("Тики %f\n", tick);

    std::cin.get();
    return 0;
}
