#ifndef DIAGONAL_MATRIX_H
#define DIAGONAL_MATRIX_H

#include <cmath>

#include "dynamic_array.h"
#include "exceptions.h"
#include "complex.h"

template <typename T>
class DiagonalMatrix
{
private:
    DynamicArray<T> data;
    int n;

    void checkIndex(int i, int j) const
    {
        if (i < 0 || i >= n || j < 0 || j >= n)
            throw IndexOutOfRangeException("DiagonalMatrix: index out of range");
    }

public:
    explicit DiagonalMatrix(int size) : data(size), n(size)
    {
        if (size <= 0) throw InvalidArgumentException("DiagonalMatrix: size must be > 0");
    }

    DiagonalMatrix(const T* arr, int size) : data(arr, size), n(size)
    {
        if (size <= 0) throw InvalidArgumentException("DiagonalMatrix: size must be > 0");
        if (!arr && size > 0) throw InvalidArgumentException("DiagonalMatrix: null array");
    }

    DiagonalMatrix(const DiagonalMatrix&) = default;
    DiagonalMatrix& operator=(const DiagonalMatrix&) = default;
    ~DiagonalMatrix() = default;

    int GetSize() const { return n; }

    //Возвращаем по значению, чтобы не ссылаться на временный T()
    T Get(int i, int j) const
    {
        checkIndex(i, j);
        return (i == j) ? data.Get(i) : T();
    }

    void Set(int i, int j, const T& val)
    {
        checkIndex(i, j);
        if (i != j) throw InvalidArgumentException("DiagonalMatrix: off-diagonal assignment forbidden");
        data.Set(i, val);
    }

    DiagonalMatrix Add(const DiagonalMatrix& other) const
    {
        if (n != other.n) throw InvalidArgumentException("DiagonalMatrix::Add: sizes differ");
        DiagonalMatrix res(n);
        for (int i = 0; i < n; ++i) res.data.Set(i, data.Get(i) + other.data.Get(i));
        return res;
    }

    DiagonalMatrix MulScalar(const T& scalar) const
    {
        DiagonalMatrix res(n);
        for (int i = 0; i < n; ++i) res.data.Set(i, data.Get(i) * scalar);
        return res;
    }

    double Norm() const
    {
        double sum = 0.0;
        for (int i = 0; i < n; ++i)
        {
            double a = elemAbs(data.Get(i)); 
            sum += a * a;
        }
        return std::sqrt(sum);
    }

    DynamicArray<T> MulVector(const DynamicArray<T>& v) const
    {
        if (n != v.GetSize()) throw InvalidArgumentException("DiagonalMatrix::MulVector: size mismatch");
        DynamicArray<T> res(n);
        for (int i = 0; i < n; ++i) res.Set(i, data.Get(i) * v.Get(i));
        return res;
    }

    DiagonalMatrix operator+(const DiagonalMatrix& other) const { return Add(other); }
    DiagonalMatrix operator*(const T& scalar) const { return MulScalar(scalar); }
};

#endif // DIAGONAL_MATRIX_H