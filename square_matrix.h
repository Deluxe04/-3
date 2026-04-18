#ifndef SQUARE_MATRIX_H
#define SQUARE_MATRIX_H

#include <cmath>

#include "dynamic_array.h"
#include "exceptions.h"
#include "complex.h"

template <typename T>
inline double absval(const T& val) 
{ 
    return elemAbs(val); 
}

template <typename T>
class SquareMatrix 
{
private:
    DynamicArray<T> data; 
    int n;

    void checkIndex(int i, int j) const 
    {
        if (i < 0 || i >= n || j < 0 || j >= n)
            throw IndexOutOfRangeException("SquareMatrix: index out of range");
    }

    //Доступ без проверок
    const T& uncheckedGet(int i, int j) const 
    {
        return data.Get(i * n + j);  
    }

public:
    //Конструкторы
    explicit SquareMatrix(int size) : data(size * size), n(size) 
    {
        if (size <= 0) 
            throw InvalidArgumentException("SquareMatrix: size must be > 0");
    }

    SquareMatrix(const T* arr, int size) : data(arr, size * size), n(size) 
    {
        if (size <= 0) 
            throw InvalidArgumentException("SquareMatrix: size must be > 0");
        if (!arr && size > 0) 
            throw InvalidArgumentException("SquareMatrix: null array");
    }
    
    SquareMatrix(const SquareMatrix&) = default;
    SquareMatrix& operator=(const SquareMatrix&) = default;
    ~SquareMatrix() = default; 

    const T& Get(int i, int j) const 
    {
        checkIndex(i, j);
        return data.Get(i * n + j); 
    }

    void Set(int i, int j, const T& val) 
    {
        checkIndex(i, j);
        data.Set(i * n + j, val);
    }

    int GetSize() const 
    { 
        return n; 
    }

    //Арифметические операции
    SquareMatrix operator+(const SquareMatrix& other) const 
    { 
        return Add(other); 
    }
    SquareMatrix operator*(const T& scalar) const 
    { 
        return MulScalar(scalar);
    }
    SquareMatrix operator*(const SquareMatrix& other) const 
    { 
        return MulMatrix(other); 
    }

    SquareMatrix Add(const SquareMatrix& other) const 
    {
        if (n != other.n) 
            throw InvalidArgumentException("SquareMatrix::Add: sizes differ");
        SquareMatrix res(n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                res.Set(i, j,uncheckedGet(i, j) + other.uncheckedGet(i, j));
            }
        return res;
    }

    SquareMatrix MulScalar(const T& scalar) const 
    {
        SquareMatrix res(n);
        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
            {
                res.Set(i, j, uncheckedGet(i, j) * scalar);
            }
        return res;
    }

    SquareMatrix MulMatrix(const SquareMatrix& other) const 
    {
        if (n != other.n) 
            throw InvalidArgumentException("SquareMatrix:: MulMatrix: sizes differ");
        SquareMatrix res(n);
        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                T sum = T();
                for (int k = 0; k < n; ++k) 
                {
                    sum = sum + uncheckedGet(i, k) * other.uncheckedGet(k, j);
                }
                res.Set(i, j, sum);  
            }
        }
        return res;
    }

    double Norm() const 
    {
        double sum = 0.0;
        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                double a = absval(uncheckedGet(i, j));
                sum += a * a;
            }
        }
        return std::sqrt(sum);
    }

    //Элементарные преобразования
    void SwapRows(int i, int k) 
    {
        checkIndex(i, 0);  
        checkIndex(k, 0);
        if (i == k) 
        {
            return;
        }
        for (int j = 0; j < n; ++j) 
        {
            T tmp = uncheckedGet(i, j);
            Set(i, j, uncheckedGet(k, j));  
            Set(k, j, tmp);
        }
    }

    void MulRow(int i, const T& scalar) 
    {
        checkIndex(i, 0);
        for (int j = 0; j < n; ++j)
            Set(i, j, uncheckedGet(i, j) * scalar);
    }

    void AddRow(int target, int source, const T& scalar) 
    {
        checkIndex(target, 0); 
        checkIndex(source, 0);
        for (int j = 0; j < n; ++j)
        {
            Set(target, j, uncheckedGet(target, j) + scalar * uncheckedGet(source, j));
        }
    }

    void SwapCols(int j, int k) 
    {
        checkIndex(0, j); 
        checkIndex(0, k);
        if (j == k) 
        {
            return;
        }
        for (int i = 0; i < n; ++i) 
        {
            T tmp = uncheckedGet(i, j);
            Set(i, j, uncheckedGet(i, k));
            Set(i, k, tmp);
        }
    }

    void MulCol(int j, const T& scalar) 
    {
        checkIndex(0, j);
        for (int i = 0; i < n; ++i)
        {
            Set(i, j, uncheckedGet(i, j) * scalar);
        }
    }

    void AddCol(int target, int source, const T& scalar) 
    {
        checkIndex(0, target); 
        checkIndex(0, source);
        for (int i = 0; i < n; ++i)
        {
            Set(i, target, uncheckedGet(i, target) + scalar * uncheckedGet(i, source));
        }
    }
};

#endif // SQUARE_MATRIX_H