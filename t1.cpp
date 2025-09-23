#include <iostream>
#include <omp.h>
using namespace std;

#define WIDTH 410

void fill_matrix_rand(int m[WIDTH][WIDTH])
{
#pragma omp parallel for schedule(dynamic, 1)
    for (int i = 0; i < WIDTH; i++)
    {
        for (int j = 0; j < WIDTH; j++)
        {
            m[i][j] = std::rand() % 256;
        }
    }
}

int main()
{
    int n = WIDTH;
    int k[n][n];
    int a1[WIDTH][WIDTH];
    int a2[WIDTH][WIDTH];
    fill_matrix_rand(a1);
    fill_matrix_rand(a2);
    int i, j, m;
#pragma omp parallel for schedule(dynamic, 1) collapse(3) private(i, j, m)
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {

            for (m = 0; m < n; m++)
            {
                k[i][j] += a1[i][m] * a2[m][j];
            }
        }
    }
}