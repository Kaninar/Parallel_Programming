#include <iostream>
#include <omp.h>

int main()
{
#pragma omp parallel num_threads(4)
    {
        std::cout << "Hello World!" << omp_get_thread_num() << std::endl;
    }

    return 0;
}
