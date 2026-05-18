#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <cstdio>
#include <ctime>
#include <cmath>

#include "square_matrix.h"
#include "diagonal_matrix.h"
#include "dynamic_array.h"
#include "complex.h"

//Вспом-ая фу-я (заполнение массива линейной послед-ю)
template <typename T>
void fillLinear(T* arr, int size, T start = T(), T step = T(1))
{
    if (!arr && size > 0) 
    {
        return;
    }
    T val = start;
    for (int i = 0; i < size; ++i)
    {
        arr[i] = val;
        val = val + step;
    }
}

//Умножение плотной матрицы на вектор 
template <typename T>
DynamicArray<T> denseMulVector(const SquareMatrix<T>& mat, const DynamicArray<T>& vec)
{
    int n = mat.GetSize();
    DynamicArray<T> res(n);
    for (int i = 0; i < n; ++i)
    {
        T sum = T();
        for (int j = 0; j < n; ++j)
        {
            sum = sum + mat.Get(i, j) * vec.Get(j);
        }
        res.Set(i, sum);
    }
    return res;
}

//Бенчмарк MulVector 
template <typename T>
void runMulVectorBenchmark(int size, int repeats, const char* typeName)
{
    if (size <= 0 || repeats <= 0) 
    {
        return;
    }

    //Подготовка данных
    T* diagRaw = new T[size];
    T* vecRaw = new T[size];
    fillLinear(diagRaw, size, T(2), T(0)); 
    fillLinear(vecRaw, size, T(1), T(0.1)); 

    SquareMatrix<T> sqMat(size);
    for (int i = 0; i < size; ++i) 
    {
        sqMat.Set(i, i, diagRaw[i]);
    }
    DiagonalMatrix<T> diagMat(diagRaw, size);
    DynamicArray<T> vec(vecRaw, size);

    //Прогрев кэша (один прогон без замера)
    (void)denseMulVector(sqMat, vec);
    (void)diagMat.MulVector(vec);

    //Замер SquareMatrix 
    clock_t startSq = clock();
    for (int r = 0; r < repeats; ++r)
    {
        denseMulVector(sqMat, vec);
    }
    clock_t endSq = clock();
    double timeSq = static_cast<double>(endSq - startSq) / CLOCKS_PER_SEC;

    //Замер DiagonalMatrix 
    clock_t startDiag = clock();
    for (int r = 0; r < repeats; ++r)
    {
        diagMat.MulVector(vec);
    }
    clock_t endDiag = clock();
    double timeDiag = static_cast<double>(endDiag - startDiag) / CLOCKS_PER_SEC;

    //Вывод результатов
    printf("\n=== Benchmark: MulVector <%s> ===\n", typeName);
    printf("Size: %d x %d | Repeats: %d\n", size, size, repeats);
    printf("SquareMatrix  : %.4f sec  [O(n²)]\n", timeSq);
    printf("DiagonalMatrix: %.4f sec  [O(n)]\n", timeDiag);
    if (timeDiag > 1e-9 && timeSq > 1e-9)
    {
        double speedup = timeSq / timeDiag;
        printf(">>> Speedup: %.1fx faster\n", speedup);
        if (speedup > 10.0) printf("Диагональная оптимизация эффективна\n");
    }
    printf("================================\n");

    delete[] diagRaw;
    delete[] vecRaw;
}

//Запуск тестов для разных размеров
template <typename T>
void runFullBenchmarkSuite(const char* typeName)
{
    printf("\n=================================\n");
    printf("  DiagonalMatrix Benchmark <%s>  \n", typeName);
    printf("\n=================================\n");

    int sizes[] = {100, 500, 1000, 2000};
    int repeats[] = {100, 50, 20, 10}; 

    for (int i = 0; i < 4; ++i)
        runMulVectorBenchmark<T>(sizes[i], repeats[i], typeName);
}

#endif // BENCHMARK_H