#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <omp.h>

#define N 6

void glavelem(int k, float a[][N + 1], int n, float x[]);

int main()
{
    float t, a[N][N + 1], x[N]; // корни системы в виде массива + сам массив с неизвестными
    int i, j, k, n, count = 0;

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

    printf("Исходная матрица:\n"); // вывод исходных данных на экран

    for (j = 0; j < n; j++)
    {
        for (i = 0; i < n + 1; i++)
        {
            printf("%6.3f\t", a[j][i]);
        }

        printf("\n");
    }

    // прямой ход
    for (k = 0; k < n; k++)
    { // На какой позиции должен стоять главный элемент
        double time = omp_get_wtime();
        glavelem(k, a, n, x); // Установка главного элемента

        if (fabs(a[k][k]) < 0.0001)
        {
            printf("Система не имеет единственного решения"); // Вызов функции по выбору главного элемента

            return (0);
        }
        // #pragma omp parallel for
        for (j = n; j >= k; j--)
        {
            a[k][j] /= a[k][k];
        }

        // #pragma omp parallel for
        for (i = k + 1; i < n; i++)
        {
            for (j = n; j >= k; j--)
            {
                a[i][j] -= a[k][j] * a[i][k];
            }
        }

        // обратный ход

        // #pragma omp parallel for
        for (i = 0; i < n; i++)
        {
            x[i] = a[i][n];
        }

        // #pragma omp parallel for collapse(2)
        for (i = n - 2; i >= 0; i--)
        {
            for (j = i + 1; j < n; j++)
            {
                x[i] = x[i] - x[j] * a[i][j];
            }
        }
        time -= omp_get_wtime();
        printf("Elapsed secs: %lf\n", time);
        // находим корни
        // вывод матрицы квазитреугольного вида на экран
        printf("Матрица приведенная к треугольному виду:\n");

        // нельзя делать параллельно, как минимум написать секцию к которой нужно будет своевременно обращаться
        for (j = 0; j < n; j++)
        {
            for (i = 0; i < n + 1; i++)
            {
                printf("%6.3f\t", a[j][i]);
            }
            printf("\n");
        }
        // вывод ответа на экран

        // тут тоже нельзя
        printf("Корни СЛАУ:\n");

        for (i = 0; i < n; i++)
        {
            printf("x[%d] = %6.3f\n", i, x[i]);
        }

        getch();
    }
}

void glavelem(int k, float a[][N + 1], int n, float x[]) // функция по выбору главноего элемента
{
    int i, j, i_max = k, j_max = k;
    int temp;

    // #pragma omp parallel for schedule(dynamic, 1) collapse((i, j, m)

    for (i = k; i < n; i++) // Ищем максимальный по модулю элемент
    {
        for (j = k; j < n; j++)
        {
            if (fabs(a[i_max][j_max]) < fabs(a[i][j]))
            {
                i_max = i;
                j_max = j;
            }
        }
    }

    // #pragma omp parallel for
    for (j = k; j < n + 1; j++) // Переставляем строки
    {
        temp = a[k][j];
        a[k][j] = a[i_max][j];
        a[i_max][j] = temp;
    }

    // #pragma omp parallel for
    for (i = 0; i < n; i++) // Переставляем столбцы
    {
        temp = a[i][k];
        a[i][k] = a[i][j_max];
        a[i][j_max] = temp;
    }
    i = x[k];
    x[k] = x[j_max];
    x[j_max] = i;
}