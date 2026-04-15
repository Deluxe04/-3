#ifndef SLAU_SOLVER_H
#define SLAU_SOLVER_H

#include "square_matrix.h"
#include "dynamic_array.h"
#include "exceptions.h"
#include <cmath>
// absval уже определена в square_matrix.h — не нужно дублировать!

template <typename T>
DynamicArray<T> solveSLAU(const SquareMatrix<T>& A, const DynamicArray<T>& b) {
    int n = A.GetSize();
    if (b.GetSize() != n) 
        throw InvalidArgumentException("solveSLAU: size mismatch");

    // Работаем с копиями, чтобы не менять исходные данные
    SquareMatrix<T> mat(A);
    DynamicArray<T> rhs(b);

    const double EPS = 1e-12;

    // ===== ПРЯМОЙ ХОД (Гаусс с выбором главного элемента) =====
    for (int col = 0; col < n; ++col) {
        // 1. Поиск строки с максимальным по модулю элементом
        int pivotRow = col;
        double maxVal = absval(mat.Get(col, col));  // ← absval из square_matrix.h
        for (int row = col + 1; row < n; ++row) {
            double val = absval(mat.Get(row, col));
            if (val > maxVal) {
                maxVal = val;
                pivotRow = row;
            }
        }

        // 2. Проверка на вырожденность
        if (maxVal < EPS) 
            throw InvalidOperationException("solveSLAU: matrix is singular");

        // 3. Перестановка строк
        if (pivotRow != col) {
            mat.SwapRows(col, pivotRow);
            T tmp = rhs.Get(col);
            rhs.Set(col, rhs.Get(pivotRow));
            rhs.Set(pivotRow, tmp);
        }

        // 4. Нормировка строки (использует operator/ из complex.h)
        T pivot = mat.Get(col, col);
        T invPivot = T(1) / pivot;
        
        for (int j = col; j < n; ++j) 
            mat.Set(col, j, mat.Get(col, j) * invPivot);
        rhs.Set(col, rhs.Get(col) * invPivot);

        // 5. Исключение ниже диагонали
        for (int row = col + 1; row < n; ++row) {
            T factor = mat.Get(row, col);
            if (absval(factor) < EPS) continue;
            mat.AddRow(row, col, T() - factor);
            rhs.Set(row, rhs.Get(row) - factor * rhs.Get(col));
        }
    }

    // ===== ОБРАТНЫЙ ХОД =====
    DynamicArray<T> x(n);
    for (int i = n - 1; i >= 0; --i) {
        T sum = rhs.Get(i);
        for (int j = i + 1; j < n; ++j)
            sum = sum - mat.Get(i, j) * x.Get(j);
        x.Set(i, sum);  // Диагональ уже = 1
    }

    return x;
}

#endif // SLAU_SOLVER_H