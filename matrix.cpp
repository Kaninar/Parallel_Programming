#include <iostream>
#include <cstdlib>
#include <windows.h>
using namespace std;

// #define WIDTH

// void fill_matrix_rand(int m[WIDTH][WIDTH])
// {
//     for (int i = 0; i < WIDTH * WIDTH; i++)
//     {
//         int row = i / WIDTH;
//         int col = i % WIDTH;
//         m[row][col] = std::rand() % 256;
//     }
// }

int main()
{
    // try
    // {
    //     int m1[WIDTH][WIDTH];
    //     int m2[WIDTH][WIDTH];
    //     try
    //     {
    //         fill_matrix_rand(m1);
    //         fill_matrix_rand(m2);
    //     }
    //     catch (exception ext)
    //     {
    //         std::cout << "fill error" << std::endl;
    //     }

    //     int result[WIDTH][WIDTH];
    //     int itr = 0;
    //     try
    //     {
    //         for (unsigned long long n = 0; n < WIDTH * WIDTH * WIDTH; n++)
    //         {

    //             int i = n / (WIDTH * WIDTH);
    //             int j = (n % (WIDTH * WIDTH)) / WIDTH;
    //             int k = n % WIDTH;

    //             result[i][j] += m1[i][k] * m2[k][j];

    //             // for (int j = 0; j < WIDTH; j++)
    //             // {
    //             //     result[i][j] = 0;
    //             //     for (int k = 0; k < WIDTH; k++)
    //             //     {
    //             //         result[i][j] += m1[i][k] * m2[k][j];
    //             //     }
    //             // }
    //             itr++;
    //         }
    //     }
    //     catch (exception ext)
    //     {
    //         std::cout << itr << std::endl;
    //     }
    // }
    // catch (exception ext)
    // {
    //     return 1;
    // }
    int result[100][100];

    // #define WIDTH 10000
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            result[i][j] = i + j;
        }
    }
    return 0;
}
