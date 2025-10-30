#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <omp.h>
#include <iostream>

#define N 100

struct Max
{
    double value;
    double row;
    double col;
};

void glavelem(int k, double a[][N + 1], int n, double x[]);

void print2d(int arrayLength, double array[][N + 1]);

int main()
{

    double t, x[N];
    int i, j, k, n, count = 0;
    n = N;
    srand(time(0));
    int rows = N;
    int cols = N + 1;
    double a[N][N + 1];

/*
    collapse используется для вложенных циклов, вместо создания rows итераций, в котором будут циклы по j,
    создаётся rows * cols итераций, которые распараллеливаются.
*/
#pragma omp parallel for collapse(2)
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            a[i][j] = 1 + (rand() % 100);
    do
    {
        printf("Введите размер матрицы:\n"); // Размер системы
        scanf("%d", &n);
        if (n > N)
            printf("Слишком большое число уравнений. Повторите ввод.\n"); // Задано значение 6 уравнений максимум(#define N 6)
        else
            printf("N = %d\n", n); // Повторный ввод системы, удовлетворяющий кол-ву уравнений
    } while (n > N);

    printf("Введите СЛАУ:\n"); // ввод построчно исходных данных

    for (j = 0; j < n; j++)
    {
        for (i = 0; i < n + 1; i++)
        {
            printf("a[%d][%d] = ", j, i);
            scanf("%f", &a[j][i]);
            count++;

            if (count == (n + 1))
            {
                printf("\n", count = 0);
            }
        }
    }

    printf("Исходная матрица:\n");

    for (j = 0; j < n; j++)
    {
        for (i = 0; i < n + 1; i++)
        {
            printf("%6.3f\t", a[j][i]);
        }

        printf("\n");
    }

    for (k = 0; k < n; k++)
    {
        // На какой позиции должен стоять главный элемент
        double time = omp_get_wtime();
        glavelem(k, a, n, x); // Установка главного элемента

        if (fabs(a[k][k]) < 0.0001)
        {
            printf("Система не имеет единственного решения"); // Вызов функции по выбору главного элемента

            break;
        }

        double divisor = a[k][k];

#pragma omp parallel for
        for (int m = n; m >= k; m--)
            a[k][m] /= divisor;

#pragma omp parallel for
        for (i = k + 1; i < n; i++)
            for (j = n; j >= k; j--)
                a[i][j] -= a[k][j] * a[i][k];

#pragma omp parallel for
        for (i = 0; i < n; i++)
            x[i] = a[i][n];

#pragma omp for
        for (i = n - 2; i >= 0; i--)
        {
            float temp = 0.0;
/*
    Базовый подсчёт суммы с использованием reduction.
    Гонки данных нет, переменная temp выделенна специально для выполнения суммы и инициализиуется в каждом потоке отдельно
*/
#pragma omp parallel for reduction(+ : temp)
            for (j = i + 1; j < n; j++)
                temp = -x[j] * a[i][j];

            x[i] = -temp;
        }

        time = omp_get_wtime() - time;
        printf("Elapsed secs: %lf\n", time);
        printf("Ticks: %lf\n", omp_get_wtick());

        printf("Матрица приведенная к треугольному виду:\n");

        print2d(n, a);

        printf("Корни СЛАУ:\n");

        for (i = 0; i < n; i++)
        {
            printf("x[%d] = %6.3f\n", i, x[i]);
        }
    }

    printf("\n\nНажмите любую клавишу для выхода...");
    _getch();
}

void glavelem(int k, double a[][N + 1], int n, double x[])
{

/*
    Функция для поиска максимального значения с использованием структуры.
    <название_оператора> : <тип> : <функция_возвращающая_omp_out>
    omp_out - возвращаемое значение
    omp_in - входное значение
    как правило представляют a и b, как, например, a + b
*/
#pragma omp declare reduction(maximo : struct Max : omp_out = omp_in.value > omp_out.value ? omp_in : omp_out)
    int i, j, i_max = k, j_max = k;
    int temp;
    struct Max maxStruct;
    maxStruct.value = 0;
    maxStruct.row = 0;
    maxStruct.col = 0;

/*
    Применение декларированной функции, ничего особенного
*/
#pragma omp parallel for reduction(maximo : maxStruct)
    for (i = k; i < n; i++)
        for (j = k; j < n; j++)
            if (fabs(a[i_max][j_max]) < fabs(a[i][j]))
            {
                maxStruct.value = a[i][j];
                maxStruct.row = i;
                maxStruct.col = j;
            }

    i_max = maxStruct.row;
    j_max = maxStruct.col;

/*
    Нет необходимости в дополнительных директивах, просто изменение значений в памяти не связанной на прямую
*/
#pragma omp parallel for
    for (j = k; j < n + 1; j++) // Переставляем строки
    {
        double temp = a[k][j];
        a[k][j] = a[i_max][j];
        a[i_max][j] = temp;
    }

/*
    Аналогично прошлому методу
    Нет необходимости в дополнительных директивах, просто изменение значений в памяти не связанной на прямую
*/
#pragma omp parallel for
    for (i = 0; i < n; i++) // Переставляем столбцы
    {
        double temp = a[i][k];
        a[i][k] = a[i][j_max];
        a[i][j_max] = temp;
    }

    i = x[k];
    x[k] = x[j_max];
    x[j_max] = i;
}

void print2d(int arrayLength, double array[][N + 1])
{
    for (int i = 0; i < arrayLength; i++)
    {
        for (int j = 0; j < arrayLength + 1; j++)
        {
            printf("%6.3lf\t", array[i][j]);
        }
        printf("\n");
    }
}