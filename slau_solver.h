#ifndef SLAU_SOLVER_H
#define SLAU_SOLVER_H
#include "square_matrix.h"
#include "dynamic_array.h"
#include "exceptions.h"

//Метод Гаусса
template <typename T>
static DynamicArray<T> gaussSolve(SquareMatrix<T> mat, DynamicArray<T> rhs)
{
    int n = mat.GetSize();
    const double EPS = 1e-12;

    //Прямой ход
    for (int col = 0; col < n; ++col) 
    {
        //Выбор ведущего элемента
        int pivotRow = col;
        double maxVal = elemAbs(mat.Get(col, col));
        for (int row = col + 1; row < n; ++row) 
        {
            double val = elemAbs(mat.Get(row, col));
            if (val > maxVal) 
            { 
                maxVal = val; 
                pivotRow = row; 
            }
        }
        if (maxVal < EPS) 
            throw InvalidOperationException("solveSLAU: singular matrix");

        //Перестановка строк 
        if (pivotRow != col) 
        {
            mat.SwapRows(col, pivotRow);
            T tmp = rhs.Get(col);
            rhs.Set(col, rhs.Get(pivotRow));
            rhs.Set(pivotRow, tmp);
        }

        //Нормализация ведущей строки
        T pivot = mat.Get(col, col);
        T invPivot = T(1) / pivot;
        for (int j = col; j < n; ++j) 
        {
            mat.Set(col, j, mat.Get(col, j) * invPivot);
        }
        rhs.Set(col, rhs.Get(col) * invPivot);

        //Исключение эл-ов ниже диагонали
        for (int row = col + 1; row < n; ++row) 
        {
            T factor = mat.Get(row, col);
            if (elemAbs(factor) < EPS)
            {
                continue;
            }
            mat.AddRow(row, col, factor * (-1));
            rhs.Set(row, rhs.Get(row) - factor * rhs.Get(col));
        }
    }

    //Обратный ход
    DynamicArray<T> x(n);
    for (int i = n - 1; i >= 0; --i) 
    {
        T sum = rhs.Get(i);
        for (int j = i + 1; j < n; ++j) 
        {
            sum = sum - mat.Get(i, j) * x.Get(j);
        }
        x.Set(i, sum);
    }
    return x;
}

//Шаболн для double, Complex
template <typename T>
DynamicArray<T> solveSLAU(const SquareMatrix<T>& A, const DynamicArray<T>& b)
{
    if (A.GetSize() != b.GetSize()) 
        throw InvalidArgumentException("solveSLAU: size mismatch");
    return gaussSolve(A, b);
}

//Для int (обход целочисленного деления)
template <>
DynamicArray<int> solveSLAU(const SquareMatrix<int>& A, const DynamicArray<int>& b)
{
    if (A.GetSize() != b.GetSize()) 
        throw InvalidArgumentException("solveSLAU: size mismatch");
    int n = A.GetSize();

    //Конвертация в double 
    SquareMatrix<double> matD(n);
    DynamicArray<double> bD(n);
    for (int i = 0; i < n; ++i) 
    {
        bD.Set(i, static_cast<double>(b.Get(i)));
        for (int j = 0; j < n; ++j) 
        {
            matD.Set(i, j, static_cast<double>(A.Get(i, j)));
        }
    }

    DynamicArray<double> xD = gaussSolve(matD, bD);

    //Конвертация в int
    DynamicArray<int> x(n);
    for (int i = 0; i < n; ++i) 
    {
        double val = xD.Get(i);
        int rounded = (val >= 0.0) ? static_cast<int>(val + 0.5) : static_cast<int>(val - 0.5);
        x.Set(i, rounded);
    }
    return x;
}

#endif // SLAU_SOLVER_H