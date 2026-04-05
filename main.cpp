#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

// Подключение заголовочных файлов проекта (bit_sequence удалён)
#include "complex.h"
#include "square_matrix.h"
#include "dynamic_array.h"
#include "linked_list.h"
#include "array_sequence.h"
#include "list_sequence.h"
#include "adaptive_sequence.h"
#include "exceptions.h"

// ---------------------------------------------------------------------------
// RAII-обёртка для безопасного управления памятью матриц
// ---------------------------------------------------------------------------
template <typename T>
class MatrixManager {
private:
    SquareMatrix<T>* matrix;
    std::string typeLabel;
public:
    MatrixManager() : matrix(nullptr), typeLabel("none") {}
    ~MatrixManager() { delete matrix; } // Автоматическая очистка

    MatrixManager(const MatrixManager&) = delete;
    MatrixManager& operator=(const MatrixManager&) = delete;

    void create(size_t size, const std::string& label) {
        delete matrix;
        matrix = new SquareMatrix<T>(size);
        typeLabel = label;
    }

    SquareMatrix<T>* get() { return matrix; }
    const SquareMatrix<T>* get() const { return matrix; }
    bool isValid() const { return matrix != nullptr; }
    const std::string& getLabel() const { return typeLabel; }
};

// Глобальные менеджеры
MatrixManager<int>     intMat;
MatrixManager<double>  doubleMat;
MatrixManager<Complex> complexMat;

// ---------------------------------------------------------------------------
// Вспомогательные шаблонные функции (заменяют runtime-проверки типов)
// ---------------------------------------------------------------------------
template <typename T> T getDemoScalar() { return T(2); }
template <> Complex getDemoScalar<Complex>() { return Complex(0, 2); }

template <typename T> T getDemoFill() { return T(1); }
template <> Complex getDemoFill<Complex>() { return Complex(1, 1); }

template <typename T> void initDemoMatrix(SquareMatrix<T>& mat);
template <> void initDemoMatrix<int>(SquareMatrix<int>& mat) {
    mat.Set(0,0,1); mat.Set(0,1,2); mat.Set(1,0,3); mat.Set(1,1,4);
}
template <> void initDemoMatrix<double>(SquareMatrix<double>& mat) {
    mat.Set(0,0,1.5); mat.Set(0,1,2.5); mat.Set(1,0,3.5); mat.Set(1,1,4.5);
}
template <> void initDemoMatrix<Complex>(SquareMatrix<Complex>& mat) {
    mat.Set(0,0, Complex(1, 1)); mat.Set(0,1, Complex(2, 0));
    mat.Set(1,0, Complex(0,-1)); mat.Set(1,1, Complex(3, 2));
}

// ---------------------------------------------------------------------------
// Универсальные функции вывода и ввода
// ---------------------------------------------------------------------------
template <typename T>
void printMatrix(const SquareMatrix<T>& mat, const std::string& header) {
    size_t n = mat.GetSize();
    std::cout << "\n[" << header << " (" << n << "x" << n << ")]\n";
    for (size_t i = 0; i < n; ++i) {
        std::cout << "  ";
        for (size_t j = 0; j < n; ++j) {
            std::cout << mat.Get(i, j) << "\t";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

template <typename T>
void fillMatrixFromConsole(SquareMatrix<T>& mat) {
    size_t n = mat.GetSize();
    std::cout << "Введите " << n << "x" << n << " элементов:\n";
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            T val;
            std::cin >> val; // Работает для int, double и Complex (перегружен в complex.h)
            mat.Set(i, j, val);
        }
    }
    std::cin.ignore(10000, '\n');
}

// ---------------------------------------------------------------------------
// Демонстрация работы матрицы (полностью type-safe)
// ---------------------------------------------------------------------------
template <typename T>
void demonstrateMatrix(MatrixManager<T>& mgr, const std::string& typeName) {
    std::cout << "\n=== Демонстрация SquareMatrix<" << typeName << "> ===\n";
    
    mgr.create(2, typeName);
    auto* mat = mgr.get();

    // Типобезопасная инициализация через специализации
    initDemoMatrix(*mat);
    printMatrix(*mat, "Исходная");

    std::cout << "Норма Фробениуса: " << mat->Norm() << "\n";

    // Умножение на скаляр
    T scalar = getDemoScalar<T>();
    printMatrix((*mat) * scalar, "Умноженная на скаляр");

    // Сложение
    SquareMatrix<T> other(mat->GetSize());
    for(size_t i=0; i<mat->GetSize(); ++i)
        for(size_t j=0; j<mat->GetSize(); ++j)
            other.Set(i, j, getDemoFill<T>());
            
    printMatrix((*mat) + other, "Сумма с заполненной матрицей");
    printMatrix((*mat) * other, "Произведение матриц");

    // Элементарные преобразования
    mat->SwapRows(0, 1);
    printMatrix(*mat, "После SwapRows(0, 1)");
    
    mat->MulCol(0, scalar);
    printMatrix(*mat, "После MulCol(0, scalar)");
}

// ---------------------------------------------------------------------------
// Интерактивный режим (сфокусировано на Complex)
// ---------------------------------------------------------------------------
void runInteractiveComplex() {
    std::cout << "\n=== Интерактивный режим: SquareMatrix<Complex> ===\n";
    int choice = -1;
    
    while (choice != 0) {
        std::cout << "\n1. Создать новую матрицу\n"
                  << "2. Показать текущую\n"
                  << "3. Вычислить норму\n"
                  << "4. Умножить на скаляр\n"
                  << "5. Сложить с другой матрицей\n"
                  << "6. Умножить на другую матрицу\n"
                  << "7. Элементарные операции (строки/столбцы)\n"
                  << "0. Выход в главное меню\n"
                  << "Выбор: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear(); std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(10000, '\n');

        if (!complexMat.isValid() && choice >= 2 && choice <= 7) {
            std::cout << "[Ошибка] Сначала создайте матрицу (пункт 1).\n";
            continue;
        }

        try {
            auto* m = complexMat.get();
            switch (choice) {
                case 1: {
                    size_t n;
                    std::cout << "Размер (n x n): ";
                    std::cin >> n;
                    std::cin.ignore(10000, '\n');
                    complexMat.create(n, "Complex");
                    fillMatrixFromConsole(*complexMat.get());
                    printMatrix(*complexMat.get(), "Создана");
                    break;
                }
                case 2:
                    printMatrix(*m, "Текущая");
                    break;
                case 3:
                    std::cout << "Норма: " << m->Norm() << "\n";
                    break;
                case 4: {
                    Complex sc;
                    std::cout << "Введите комплексный скаляр (a+bi): ";
                    std::cin >> sc;
                    std::cin.ignore(10000, '\n');
                    printMatrix((*m) * sc, "Результат умножения на скаляр");
                    break;
                }
                case 5: {
                    size_t n = m->GetSize();
                    SquareMatrix<Complex> other(n);
                    std::cout << "Введите вторую матрицу " << n << "x" << n << ":\n";
                    fillMatrixFromConsole(other);
                    printMatrix((*m) + other, "Сумма матриц");
                    break;
                }
                case 6: {
                    size_t n = m->GetSize();
                    SquareMatrix<Complex> other(n);
                    std::cout << "Введите вторую матрицу " << n << "x" << n << ":\n";
                    fillMatrixFromConsole(other);
                    printMatrix((*m) * other, "Произведение матриц");
                    break;
                }
                case 7: {
                    int op, idx1, idx2;
                    std::cout << "1-SwapRows 2-SwapCols 3-MulRow 4-MulCol 5-AddRow 6-AddCol\nОперация: ";
                    std::cin >> op;
                    
                    if (op == 1 || op == 2) {
                        std::cout << "Индексы (через пробел): ";
                        std::cin >> idx1 >> idx2;
                        std::cin.ignore(10000, '\n');
                        (op == 1) ? m->SwapRows(idx1, idx2) : m->SwapCols(idx1, idx2);
                    } else {
                        Complex k;
                        std::cout << "Индекс строки/столбца: "; std::cin >> idx1;
                        std::cout << "Коэффициент (a+bi): ";   std::cin >> k;
                        std::cin.ignore(10000, '\n');
                        
                        if (op == 3) m->MulRow(idx1, k);
                        else if (op == 4) m->MulCol(idx1, k);
                        else {
                            std::cout << "Индекс второй строки/столбца: "; std::cin >> idx2;
                            std::cin.ignore(10000, '\n');
                            (op == 5) ? m->AddRow(idx1, idx2, k) : m->AddCol(idx1, idx2, k);
                        }
                    }
                    printMatrix(*m, "После операции");
                    break;
                }
                case 0: break;
                default: std::cout << "[Неверный выбор]\n";
            }
        } catch (const IndexOutOfRangeException& e) {
            std::cerr << "[IndexError] " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "[Error] " << e.what() << "\n";
        }
    }
}

// ---------------------------------------------------------------------------
// Точка входа
// ---------------------------------------------------------------------------
int main() {
    std::cout << "========================================\n"
              << "   Лабораторная работа №3: SquareMatrix\n"
              << "========================================\n";

    // Автоматическая демонстрация работы со всеми типами
    demonstrateMatrix(complexMat, "Complex");
    demonstrateMatrix(intMat,     "int");
    demonstrateMatrix(doubleMat,  "double");

    // Запуск интерактивного режима (упор на Complex)
    runInteractiveComplex();

    std::cout << "\nПрограмма завершена. Память освобождена автоматически.\n";
    return 0;
}
