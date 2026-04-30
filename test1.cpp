#include <iostream>
#include <string>
#include <cmath>
#include <chrono>

#include "square_matrix.h"
#include "diagonal_matrix.h"
#include "complex.h"
#include "exceptions.h"
#include "benchmark.h"
#include "dynamic_array.h"
#include "slau_solver.h"

static int totalPassed = 0;
static int totalFailed = 0;

void beginSuite(const std::string& name) 
{
    std::cout << "\n┌─────────────────────────────────────────────\n";
    std::cout << "│  " << name << "\n";
    std::cout << "└─────────────────────────────────────────────\n";
}

void check(const std::string& name, bool condition) 
{
    if (condition) 
    {
        std::cout << "  [ПРОЙДЕН]  " << name << "\n";
        ++totalPassed;
    } else 
    {
        std::cout << "  [ПРОВАЛЕН] " << name << "\n";
        ++totalFailed;
    }
}

template <typename ExceptionType, typename Func>
void checkThrows(const std::string& name, Func func) 
{
    try 
    {
        func();
        std::cout << "  [ПРОВАЛЕН] " << name << " (исключение не брошено)\n";
        ++totalFailed;
    } catch (const ExceptionType&) 
    {
        std::cout << "  [ПРОЙДЕН]  " << name << "\n";
        ++totalPassed;
    } catch (...) 
    {
        std::cout << "  [ПРОВАЛЕН] " << name << " (неверный тип исключения)\n";
        ++totalFailed;
    }
}

void printSummary() {
    std::cout << "\n═══════════════════════════════════════════════\n";
    std::cout << "  ИТОГО: пройдено " << totalPassed
              << ", провалено " << totalFailed
              << ", всего " << (totalPassed + totalFailed) << "\n";
    std::cout << "═══════════════════════════════════════════════\n";
}


//Square Matrix
void testSquareMatrixInt() 
{
    beginSuite("SquareMatrix<int>");
    int a[] = {1, 2, 3, 4}, b[] = {5, 6, 7, 8};
    SquareMatrix<int> m1(a, 2), m2(b, 2);

    //Базовые операции
    check("GetSize()=2", m1.GetSize() == 2);
    check("Get(1,0)=3", m1.Get(1, 0) == 3);

    //Арифметика
    auto sum = m1 + m2;
    check("Add (0,0)=6", sum.Get(0, 0) == 6);
    
    auto sc = m2 * 2;
    check("MulScalar (1,1)=16", sc.Get(1, 1) == 16);

    SquareMatrix<int> m1_orig(a, 2);
    auto prod = m1_orig * m2;
    check("MulMatrix (0,0)=19", prod.Get(0, 0) == 19);
    check("MulMatrix (1,1)=50", prod.Get(1, 1) == 50);

    //Норма
    check("Norm()=sqrt(30)", std::abs(m1_orig.Norm() - std::sqrt(30.0)) < 1e-9);
}

void testSquareMatrixDouble()
{
    beginSuite("SquareMatrix<double>");
    double a[] = {1.5, 2.5, 3.5, 4.5};
    SquareMatrix<double> m(a, 2);
    
    check("Get(0,1)=2.5", std::abs(m.Get(0, 1) - 2.5) < 1e-9);
    
    auto sc = m * 2.0;
    check("MulScalar (1,0)=7.0", std::abs(sc.Get(1, 0) - 7.0) < 1e-9);
    
    check("Norm корректна", std::abs(m.Norm() - std::sqrt(1.5*1.5 + 2.5*2.5 + 3.5*3.5 + 4.5*4.5)) < 1e-9);
}

void testSquareMatrixComplex() 
{
    beginSuite("SquareMatrix<Complex>");
    SquareMatrix<Complex> m(2);
    m.Set(0, 0, Complex(1, 1)); m.Set(0, 1, Complex(0, 2));
    m.Set(1, 0, Complex(3, 0)); m.Set(1, 1, Complex(2, -1));

    check("Get(0,1)=0+2i", m.Get(0, 1) == Complex(0, 2));

    //Умножение на скаляр
    auto sc = m * Complex(0, 2); // 0+2i
    check("MulScalar (0,0)=-2+2i", sc.Get(0, 0) == Complex(-2, 2));
    check("MulScalar (1,1)=2+4i", sc.Get(1, 1) == Complex(2, 4));

    //Норма
    //|1+i|^2 + |0+2i|^2 + |3+0i|^2 + |2-1i|^2 = 2 + 4 + 9 + 5 = 20
    check("Norm()=sqrt(20)", std::abs(m.Norm() - std::sqrt(20.0)) < 1e-9);
}

void testSquareMatrixElementary() 
{
    beginSuite("SquareMatrix: элементарные преобразования");
    int a[] = {1, 2, 3, 4};
    SquareMatrix<int> m(a, 2);
    //Исходное: [1 2]
    //          [3 4]

    m.SwapRows(0, 1);          
    check("SwapRows (0,0)=3", m.Get(0, 0) == 3);

    m.MulRow(0, 10);           
    check("MulRow (0,0)=30", m.Get(0, 0) == 30);

    m.AddRow(1, 0, 1);        
    check("AddRow (1,0)=31", m.Get(1, 0) == 31);

    m.SwapCols(0, 1);       
    check("SwapCols (0,0)=40", m.Get(0, 0) == 40);

    m.MulCol(1, 2);           
    check("MulCol (0,1)=60", m.Get(0, 1) == 60);

    m.AddCol(1, 0, -1);     
    check("AddCol (0,1)=20", m.Get(0, 1) == 20);
}

void testDiagonalMatrixComplex() 
{
    beginSuite("DiagonalMatrix<Complex>");
    Complex d[] = {Complex(1,1), Complex(0,2), Complex(3,0)};
    DiagonalMatrix<Complex> dm(d, 3);

    check("Get(0,0)=1+1i", dm.Get(0, 0) == Complex(1, 1));

    DynamicArray<Complex> v(3);
    v.Set(0, Complex(1,0)); 
    v.Set(1, Complex(1,1)); 
    v.Set(2, Complex(0,1));
    auto res = dm.MulVector(v);

    //d[1]*v[1] = (0+2i)*(1+1i) = (0*1 - 2*1) + (0*1 + 2*1)i = -2 + 2i
    check("MulVector (1)=-2+2i", res.Get(1) == Complex(-2, 2));
}

//Тесты Diagonal Matrix
void testDiagonalMatrixInt() 
{
    beginSuite("DiagonalMatrix<int>");
    int d[] = {1, 2, 3, 4};
    DiagonalMatrix<int> dm(d, 4);
    
    check("GetSize()=4", dm.GetSize() == 4);
    check("Get(2,2)=3", dm.Get(2, 2) == 3);
    check("Get(0,1)=0", dm.Get(0, 1) == 0);

    //Add
    DiagonalMatrix<int> dm2(d, 4);
    auto sum = dm + dm2;
    check("Add (1,1)=4", sum.Get(1, 1) == 4);

    //MulScalar
    auto sc = dm * 3;
    check("MulScalar (3,3)=12", sc.Get(3, 3) == 12);

    //Norm
    check("Norm()=sqrt(30)", std::abs(dm.Norm() - std::sqrt(30.0)) < 1e-9);
}

void testDiagonalMatrixDouble() 
{
    beginSuite("DiagonalMatrix<double>");
    double d[] = {1.5, 2.5, 3.5};
    DiagonalMatrix<double> dm(d, 3);
    check("Get(1,1)=2.5", std::abs(dm.Get(1, 1) - 2.5) < 1e-9);
    
    //MulVector
    DynamicArray<double> v(3);
    v.Set(0, 2.0); 
    v.Set(1, 3.0); 
    v.Set(2, 4.0);
    auto res = dm.MulVector(v);
    check("MulVector (0)=3.0", std::abs(res.Get(0) - 3.0) < 1e-9);
    check("MulVector (1)=7.5", std::abs(res.Get(1) - 7.5) < 1e-9);
}

//Тесты исключений
void testMatrixExceptions() 
{
    beginSuite("Matrix: Исключения");
    
    //Конструкторы
    checkThrows<InvalidArgumentException>("SquareMatrix(0)", []{ SquareMatrix<int>(0); });
    checkThrows<InvalidArgumentException>("DiagonalMatrix(-5)", []{ DiagonalMatrix<double>(-5); });

    //Индексы
    SquareMatrix<int> sm(2);
    checkThrows<IndexOutOfRangeException>("SquareMatrix Get(-1,0)", [&]{ sm.Get(-1, 0); });
    checkThrows<IndexOutOfRangeException>("SquareMatrix Set(2,2)", [&]{ sm.Set(2, 2, 5); });

    DiagonalMatrix<int> dm(3);
    checkThrows<IndexOutOfRangeException>("DiagonalMatrix Get(0,5)", [&]{ dm.Get(0, 5); });
    checkThrows<InvalidArgumentException>("DiagonalMatrix Set(0,1) off-diag", [&]{ dm.Set(0, 1, 5); });

    //Размерности операций
    SquareMatrix<int> sm1(2), sm2(3);
    checkThrows<InvalidArgumentException>("SquareMatrix Add size mismatch", [&]{ sm1 + sm2; });
    checkThrows<InvalidArgumentException>("SquareMatrix MulMatrix size mismatch", [&]{ sm1 * sm2; });

    DiagonalMatrix<int> dm1(2), dm2(3);
    checkThrows<InvalidArgumentException>("DiagonalMatrix Add size mismatch", [&]{ dm1 + dm2; });
    
    DynamicArray<int> v_wrong(4);
    checkThrows<InvalidArgumentException>("DiagonalMatrix MulVector mismatch", [&]{ dm1.MulVector(v_wrong); });
}

//Тест Slau Solver
void testGaussSolver()
{
    beginSuite("SLAU SOLVER");
    //double 3x3 
    {
        SquareMatrix<double> A(3);
        A.Set(0,0,2);  
        A.Set(0,1,1);  
        A.Set(0,2,-1);
        A.Set(1,0,-3); 
        A.Set(1,1,-1); 
        A.Set(1,2,2);
        A.Set(2,0,-2); 
        A.Set(2,1,1);  
        A.Set(2,2,2);
        DynamicArray<double> b(3);
        b.Set(0, 8); 
        b.Set(1, -11); 
        b.Set(2, -3);
        auto x = solveSLAU(A, b);
        check("double x[0]=2.0", std::abs(x.Get(0) - 2.0) < 1e-9);
        check("double x[1]=3.0", std::abs(x.Get(1) - 3.0) < 1e-9);
        check("double x[2]=-1.0", std::abs(x.Get(2) + 1.0) < 1e-9);
    }

    //int 3x3 (проверка конвертации в double и округления)
    {
        SquareMatrix<int> A(3);
        A.Set(0,0,1); 
        A.Set(0,1,1); 
        A.Set(0,2,1);
        A.Set(1,0,1); 
        A.Set(1,1,2); 
        A.Set(1,2,3);
        A.Set(2,0,1); 
        A.Set(2,1,4); 
        A.Set(2,2,9);
        DynamicArray<int> b(3);
        b.Set(0, 6); 
        b.Set(1, 14); 
        b.Set(2, 36);
        auto x = solveSLAU(A, b);
        check("int x[0]==1", x.Get(0) == 1);
        check("int x[1]==2", x.Get(1) == 2);
        check("int x[2]==3", x.Get(2) == 3);
    }

    //Проверка невязки ||Ax - b|| < eps для 4x4
    {
        int n = 4;
        SquareMatrix<double> A(n);
        DynamicArray<double> b(n);
        for (int i = 0; i < n; ++i) 
        {
            for (int j = 0; j < n; ++j) 
            {
                A.Set(i, j, (i==j) ? 10.0 : 1.0);
            }
            b.Set(i, static_cast<double>(i+1));
        }
        auto x = solveSLAU(A, b);
        double maxResidual = 0.0;
        for (int i = 0; i < n; ++i) 
        {
            double sum = 0.0;
            for (int j = 0; j < n; ++j) 
            {
                sum += A.Get(i,j) * x.Get(j);
            }
            double residual = std::abs(sum - b.Get(i));
            if (residual > maxResidual) 
            {
                maxResidual = residual;
            }
        }
        check("Норма невязки ||Ax-b|| < 1e-9", maxResidual < 1e-9);
    }

    //Вырожденная матрица
    {
        SquareMatrix<double> A(2);
        A.Set(0,0,1); 
        A.Set(0,1,2);
        A.Set(1,0,2); 
        A.Set(1,1,4);
        DynamicArray<double> b(2);
        b.Set(0, 3); 
        b.Set(1, 6);
        checkThrows<InvalidOperationException>("Вырожденная матрица (исключение)", [&]{solveSLAU(A, b);});
    }
}

void testSLAU_Complex()
{
    beginSuite("solveSLAU<Complex>");
    //Система: x + y = 2+0i, x - y = 0+0i Решение: x=1+0i, y=1+0i
    SquareMatrix<Complex> A(2);
    A.Set(0,0, Complex(1.0, 0.0)); 
    A.Set(0,1, Complex(1.0, 0.0));
    A.Set(1,0, Complex(1.0, 0.0)); 
    A.Set(1,1, Complex(-1.0, 0.0));
    DynamicArray<Complex> b(2);
    b.Set(0, Complex(2.0, 0.0));
    b.Set(1, Complex(0.0, 0.0));

    auto x = solveSLAU(A, b);
    const double TOL = 1e-7; 
    bool x0_ok = std::abs(x.Get(0).re - 1.0) < TOL && std::abs(x.Get(0).im - 0.0) < TOL;
    bool x1_ok = std::abs(x.Get(1).re - 1.0) < TOL && std::abs(x.Get(1).im - 0.0) < TOL;
    check("x[0] == 1+0i", x0_ok);
    check("x[1] == 1+0i", x1_ok);
}

//Бенчмарк
void runBenchmarks() 
{
    beginSuite("Benchmark: производительность MulVector");
    runFullBenchmarkSuite<double>("double");
    runFullBenchmarkSuite<Complex>("Complex");
}

int main() 
{
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   АВТОМАТИЧЕСКИЕ ТЕСТЫ: Square & Diagonal Matrix\n";
    std::cout << "═══════════════════════════════════════════════\n";
    
    //Square Matrix
    testSquareMatrixInt();
    testSquareMatrixDouble();
    testSquareMatrixComplex();
    testSquareMatrixElementary();
    
    //Diagonal Matrix
    testDiagonalMatrixInt();
    testDiagonalMatrixDouble();
    testDiagonalMatrixComplex();
    
    //Exceptions
    testMatrixExceptions();
    
    //Slau Solver
    testGaussSolver();
    testSLAU_Complex();

    //Benchmark
    runBenchmarks();
    printSummary();
    return (totalFailed == 0) ? 0 : 1;
}
