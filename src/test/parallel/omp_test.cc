#include "core/tt_test.h"
#include "core/tt_tictoc.h"
#include <iostream>
#include <omp.h>

// JUST_RUN_TEST(omp, test)
TEST(omp, test)
{
    const int size = 1e5;
    int array[size];

    for (int i = 0; i < size; ++i) 
        array[i] = i;

    TicToc tt;
    tt.Tic();
    #pragma omp parallel for
    for (int i = 0; i < size; ++i) 
        array[i] = array[i] * array[i];
    std::cout << "time: " << tt.Toc() << std::endl;

    // 没有使用 OpenMP 的情况
    for (int i = 0; i < size; ++i) 
        array[i] = i;
    
    tt.Tic();
    for (int i = 0; i < size; ++i) 
        array[i] = array[i] * array[i];
    std::cout << "time: " << tt.Toc() << std::endl;
}

/*
这里实际上测试发现，使用 OpenMP 并没有加速，反而更慢了，可能是因为 OpenMP 的开销比较大，
而这里的计算量比较小，所以 OpenMP 的开销比较大，导致反而更慢了。
*/