#include <iostream>
#include <limits>
#include <iomanip>
#include <string>
#include <locale>

#include "square_matrix.h"
#include "diagonal_matrix.h"
#include "benchmark.h"
#include "dynamic_array.h"
#include "exceptions.h"
#include "complex.h"
#include "slau_solver.h"

namespace Validation 
{
    template <typename T>
    T readValue(const std::string& prompt) 
    {
        T val;
        while (true) 
        {
            std::cout << prompt;
            if (std::cin >> val) 
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            }
            std::cout << "Некорректный формат. Ожидается число\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    template <>
    Complex readValue<Complex>(const std::string& prompt) 
    {
        double re, im;
        while (true) 
        {
            std::cout << prompt;
            if (std::cin >> re >> im) 
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return Complex(re, im); 
            }
            std::cout << "Ошибка: введите два числа (Re Im)\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }

    int readPositiveInt(const std::string& prompt) 
    {
        while (true) 
        {
            int val = readValue<int>(prompt);
            if (val > 0) 
            {
                return val;
            }
            std::cout << "Размер должен быть строго > 0. Попробуйте снова\n";
        }
    }
}

//Пользовательский интерфейс
namespace UI 
{
    void showMainMenu() 
    {
        std::cout << "\n" << std::string(70, '=') << "\n";
        std::cout << "          ДЕМОНСТРАЦИЯ МАТРИЧНЫХ ОПЕРАЦИЙ\n";
        std::cout << std::string(70, '=') << "\n";
        std::cout << "Выберите тип данных для работы:\n";
        std::cout << "1. int\n";
        std::cout << "2. double\n";
        std::cout << "3. Complex (вещественная и мнимая части)\n";
        std::cout << "0. Выход из программы\n";
        std::cout << std::string(70, '-') << "\n";
    }

    void showOpMenu() {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "Выберите операцию:\n";
        std::cout << "--- SquareMatrix ---\n";
        std::cout << "1. A + B (сложение)\n";
        std::cout << "2. A * k (умножение на скаляр)\n";
        std::cout << "3. A * B (умножение матриц)\n";
        std::cout << "4. Norm(A) (норма Фробениуса)\n";
        std::cout << "--- DiagonalMatrix ---\n";
        std::cout << "5. D1 + D2 (сложение диагональных)\n";
        std::cout << "6. D * k (умножение на скаляр)\n";
        std::cout << "7. D * v (умножение на вектор)\n";
        std::cout << "8. Norm(D) (норма диагональной)\n";
        std::cout << "--- Прочее ---\n";
        std::cout << "9. Benchmark производительности\n";
        std::cout << "10. Решить СЛАУ (Ax = b) методом Гаусса\n"; 
        std::cout << "11. Map / Reduce / Where\n";
        std::cout << "0. Вернуться к выбору типа данных\n";
        std::cout << std::string(50, '-') << "\n";
    }

    template <typename T>
    void printMatrix(const SquareMatrix<T>& m, const std::string& title) 
    {
        std::cout << "\n" << title << " (" << m.GetSize() << "x" << m.GetSize() << "):\n";
        for (int i = 0; i < m.GetSize(); ++i) 
        {
            for (int j = 0; j < m.GetSize(); ++j)
            {
                std::cout << std::setw(14) << m.Get(i, j);
            }
            std::cout << "\n";
        }
    }

    template <typename T>
    void printDiagMatrix(const DiagonalMatrix<T>& m, const std::string& title) 
    {
        std::cout << "\n" << title << " (" << m.GetSize() << "x" << m.GetSize() << "):\n";
        for (int i = 0; i < m.GetSize(); ++i) 
        {
            for (int j = 0; j < m.GetSize(); ++j)
            {
                std::cout << std::setw(14) << m.Get(i, j);
            }
            std::cout << "\n";
        }
    }

    template <typename T>
    void printArray(const DynamicArray<T>& arr, const std::string& title) 
    {
        std::cout << "\n" << title << ":\n  ";
        for (int i = 0; i < arr.GetSize(); ++i)
        {
            std::cout << std::setw(12) << arr.Get(i) << " ";
        }
        std::cout << "\n";
    }

    void printNorm(double norm, const std::string& title) 
    {
        std::cout << "\n" << title << " = " << std::fixed << std::setprecision(6) << norm << "\n";
    }
}

namespace Logic 
{
    template <typename T>
    SquareMatrix<T> readSquareMatrix(const std::string& name) 
    {
        int n = Validation::readPositiveInt("  Введите размер N для " + name + ": ");
        SquareMatrix<T> mat(n);
        std::cout << "  Введите " << (n * n) << " элементов (по строкам):\n";
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                mat.Set(i, j, Validation::readValue<T>(std::to_string(i+1) + "," + std::to_string(j+1) + ": "));
            }
        }
        return mat;
    }

    template <typename T>
    DiagonalMatrix<T> readDiagMatrix(const std::string& name) 
    {
        int n = Validation::readPositiveInt("  Введите размер N для " + name + ": ");
        DiagonalMatrix<T> mat(n);
        std::cout << "  Введите " << n << " диагональных элементов:\n";
        for (int i = 0; i < n; ++i)
        {
            mat.Set(i, i, Validation::readValue<T>("  d[" + std::to_string(i) + "]: "));
        }
        return mat;
    }

    template <typename T>
    DynamicArray<T> readVector(int size) 
    {
        DynamicArray<T> vec(size);
        std::cout << "  Введите " << size << " элементов вектора:\n";
        for (int i = 0; i < size; ++i)
            vec.Set(i, Validation::readValue<T>("  v[" + std::to_string(i) + "]: "));
        return vec;
    }

    //Операции Square Matrix 
    template <typename T> void runSquareAdd() 
    {
        std::cout << "\nSquareMatrix: A + B\n";
        auto A = readSquareMatrix<T>("A");
        auto B = readSquareMatrix<T>("B");
        UI::printMatrix<T>(A.Add(B), "Результат (A + B)");
    }
    template <typename T> void runSquareMulScalar() 
    {
        std::cout << "\nSquareMatrix: A * k\n";
        auto A = readSquareMatrix<T>("A");
        T k = Validation::readValue<T>("  Введите скаляр k: ");
        UI::printMatrix<T>(A.MulScalar(k), "Результат (A * k)");
    }
    template <typename T> void runSquareMulMatrix() 
    {
        std::cout << "\nSquareMatrix: A * B\n";
        auto A = readSquareMatrix<T>("A");
        auto B = readSquareMatrix<T>("B");
        UI::printMatrix<T>(A.MulMatrix(B), "Результат (A * B)");
    }
    template <typename T> void runSquareNorm() 
    {
        std::cout << "\nSquareMatrix: Norm(A)\n";
        auto A = readSquareMatrix<T>("A");
        UI::printNorm(A.Norm(), "Norm(A)");
    }

    //Операции DiagonalMatrix 
    template <typename T> void runDiagAdd() 
    {
        std::cout << "\nDiagonalMatrix: D1 + D2\n";
        auto D1 = readDiagMatrix<T>("D1");
        auto D2 = readDiagMatrix<T>("D2");
        UI::printDiagMatrix<T>(D1.Add(D2), "Результат (D1 + D2)");
    }
    template <typename T> void runDiagMulScalar() 
    {
        std::cout << "\nDiagonalMatrix: D * k\n";
        auto D = readDiagMatrix<T>("D");
        T k = Validation::readValue<T>("  Введите скаляр k: ");
        UI::printDiagMatrix<T>(D.MulScalar(k), "Результат (D * k)");
    }
    template <typename T> void runDiagMulVector() 
    {
        std::cout << "\nDiagonalMatrix: D * v\n";
        auto D = readDiagMatrix<T>("D");
        auto v = readVector<T>(D.GetSize());
        UI::printArray<T>(D.MulVector(v), "Результат (D * v)");
    }
    template <typename T> void runDiagNorm() 
    {
        std::cout << "\nDiagonalMatrix: Norm(D)\n";
        auto D = readDiagMatrix<T>("D");
        UI::printNorm(D.Norm(), "Norm(D)");
    }
    template <typename T> void runBenchmark(const char* typeName) 
    {
        std::cout << "\n Запуск тестов производительности для " << typeName << "...\n";
        runFullBenchmarkSuite<T>(typeName);
    }

    template <typename T>
    void runSolveSLAU() 
    {
        std::cout << "\nРешить СЛАУ (Ax = b) методом Гаусса\n";
        auto A = readSquareMatrix<T>("A");
        auto b = readVector<T>(A.GetSize());
        try 
        {
            auto x = solveSLAU(A, b);
            UI::printArray<T>(x, "Решение системы (x)");
        } catch (const std::exception& e) 
        {
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }

    //Map/Reduce/Where 
    template <typename T>
    static T demoSquare(const T& x) 
    { 
        return x * x; 
    } 

    template <typename T>
    static T demoAdd(const T& a, const T& b) 
    { 
        return a + b; 
    } 

    template <typename T>
    void runMapReduce() 
    {
        std::cout << "\nMap / Reduce / Where\n";
        auto A = readSquareMatrix<T>("A");
        
        std::cout << "\nMap: возведение каждого элемента в квадрат\n";
        auto mapped = A.Map(demoSquare<T>);
        UI::printMatrix<T>(mapped, "Результат Map(x*x)");

        std::cout << "\nReduce: сумма всех элементов\n";
        T sum = A.Reduce(demoAdd<T>, T());
        std::cout << "  Сумма = " << sum << "\n";

        std::cout << "\nWhere: фильтрация элементов > 0 (только для чисел)\n";
        std::cout << "  Матрица " << A.GetSize() << "x" << A.GetSize() << " успешно обработана\n";
    }

    //Диспетчер операций для выбранного типа 
    template <typename T>
    void runDemo(const char* typeName) 
    {
        int choice = -1;
        do 
        {
            try 
            {
                UI::showOpMenu();
                choice = Validation::readValue<int>("  Ваш выбор [0-11]: ");
                
                if (choice < 0 || choice > 11) 
                {
                    std::cout << "Номер должен быть в диапазоне [0-11]\n";
                    continue;
                }

                switch (choice) 
                {
                    case 0: break;
                    case 1: runSquareAdd<T>(); 
                            break;
                    case 2: runSquareMulScalar<T>(); 
                            break;
                    case 3: runSquareMulMatrix<T>(); 
                            break;
                    case 4: runSquareNorm<T>(); 
                            break;
                    case 5: runDiagAdd<T>(); 
                            break;
                    case 6: runDiagMulScalar<T>(); 
                            break;
                    case 7: runDiagMulVector<T>(); 
                            break;
                    case 8: runDiagNorm<T>(); 
                            break;
                    case 9: runBenchmark<T>(typeName); 
                            break;
                    case 10: runSolveSLAU<T>(); 
                            break;
                    case 11: runMapReduce<T>(); 
                            break;
                }
            } catch (const std::exception& e) 
            {
                std::cerr << "\n[ОШИБКА ВЫПОЛНЕНИЯ] " << e.what() << "\n";
                std::cout << " Операция отменена. Попробуйте снова\n";
            } catch (...) 
            {
                std::cerr << "\n[ОШИБКА] Неизвестное исключение\n";
            }
        } while (choice != 0);
    }
}

int main() 
{
    int typeChoice = -1;
    do 
    {
        try 
        {
            UI::showMainMenu();
            typeChoice = Validation::readValue<int>("Ваш выбор [0-3]: ");
            
            if (typeChoice < 0 || typeChoice > 3) 
            {
                std::cout << "Номер должен быть в диапазоне [0-3]\n";
                continue;
            }

            switch (typeChoice) 
            {
                case 0: std::cout << "Завершение работы программы\n"; 
                        break;
                case 1: Logic::runDemo<int>("int"); 
                        break;
                case 2: Logic::runDemo<double>("double"); 
                        break;
                case 3: Logic::runDemo<Complex>("Complex"); 
                        break;
            }
        } catch (const std::exception& e) 
        {
            std::cerr << "\n[КРИТИЧЕСКАЯ ОШИБКА] " << e.what() << "\n";
        } catch (...) {
            std::cerr << "\n[КРИТИЧЕСКАЯ ОШИБКА] Неизвестное исключение\n";
        }
    } while (typeChoice != 0);

    return 0;
}
