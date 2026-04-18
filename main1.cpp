#include <iostream>
#include <string>
#include <sstream>
#include <cmath>

#include "dynamic_array.h"
#include "linked_list.h"
#include "array_sequence.h"
#include "list_sequence.h"
#include "adaptive_sequence.h"
#include "builder.h"
#include "square_matrix.h"
#include "complex.h"
#include "exceptions.h"
#include "slau_solver.h"

template <typename T>
class MatrixHolder
{
public:
    SquareMatrix<T>* mat;
    MatrixHolder() : mat(nullptr) {}
    ~MatrixHolder() 
    { 
        delete mat; 
    }

    //Запрет копирования
    MatrixHolder(const MatrixHolder&) = delete;
    MatrixHolder& operator=(const MatrixHolder&) = delete;

    void set(SquareMatrix<T>* m)
    {
        delete mat;  //Освобождаем старую матрицу
        mat = m;
    }

    bool isValid() const 
    { 
        return mat != nullptr; 
    }
    void clear()
    {
        delete mat;
        mat = nullptr;
    }
};

//Глобальные переменные
MatrixHolder<int>     currentIntMat;
MatrixHolder<double>  currentDoubleMat;
MatrixHolder<Complex> currentComplexMat;
int currentMatType = 0;

//Ввод и вывод
template <class T>
int readArray(T* arr, int maxSize)
{
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    int count = 0;
    std::string token;
    while (count < maxSize && iss >> token)
    {
        std::istringstream tokenStream(token);
        T value;
        if (tokenStream >> value) 
        {
            arr[count++] = value;
        }
    }
    return count;
}

template <>
int readArray<Complex>(Complex* arr, int maxSize)
{
    std::string line;
    std::getline(std::cin, line);
    std::istringstream iss(line);
    int count = 0;
    double re, im;
    char sign, i_char;
    while (count < maxSize && (iss >> re >> sign >> im >> i_char))
    {
        if (sign == '-') im = -im;
        arr[count++] = Complex(re, im);
    }
    return count;
}

bool readInt(int& val)
{
    if (!(std::cin >> val)) 
    {
        std::cin.clear(); 
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка: введите целое число\n"; 
        val = -1; 
        return false;
    }
    std::cin.ignore(10000, '\n'); 
    return true;
}

bool readDouble(double& val)
{
    if (!(std::cin >> val)) 
    {
        std::cin.clear(); 
        std::cin.ignore(10000, '\n');
        std::cout << "Ошибка: введите число\n"; 
        val = 0; 
        return false;
    }
    std::cin.ignore(10000, '\n'); 
    return true;
}

template <class T>
void printSequence(const Sequence<T>* seq, const char* name)
{
    if (!seq) 
    { 
        std::cout << name << " is null\n";
        return; 
    }
    std::cout << name << " (len=" << seq->GetLength() << "): [";
    for (int i = 0; i < seq->GetLength(); ++i) 
    {
        if (i > 0) std::cout << ", ";
        std::cout << seq->Get(i);
    }
    std::cout << "]\n";
}

template <>
void printSequence<std::string>(const Sequence<std::string>* seq, const char* name)
{
    if (!seq) 
    { 
        std::cout << name << " is null\n"; 
        return; 
    }
    std::cout << name << " (len=" << seq->GetLength() << "): [";
    for (int i = 0; i < seq->GetLength(); ++i) 
    {
        if (i > 0) std::cout << ", ";
        std::cout << "\"" << seq->Get(i) << "\"";
    }
    std::cout << "]\n";
}

template <typename T>
void printMatrix(const SquareMatrix<T>& mat, const char* name)
{
    std::cout << name << " (" << mat.GetSize() << "x" << mat.GetSize() << "):\n";
    for (int i = 0; i < mat.GetSize(); ++i) 
    {
        std::cout << "  ";
        for (int j = 0; j < mat.GetSize(); ++j) std::cout << mat.Get(i, j) << "\t";
        std::cout << "\n";
    }
}

template <typename T>
void readMatrix(SquareMatrix<T>& mat)
{
    int n = mat.GetSize();
    std::cout << "Введите " << n << "x" << n << " элементов матрицы:\n";
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j) 
        {
            T val; std::cin >> val; 
            mat.Set(i, j, val);
        }
    std::cin.ignore(10000, '\n');
}

template <>
void readMatrix<Complex>(SquareMatrix<Complex>& mat)
{
    int n = mat.GetSize();
    std::cout << "Введите " << n << "x" << n << " элементов (формат: a+bi или a-bi):\n";
    for (int i = 0; i < n; ++i) 
    {
        for (int j = 0; j < n; ++j) 
        {
            double re, im; char sign, i_char;
            std::cin >> re >> sign >> im >> i_char;
            if (sign == '-') 
            {
                im = -im;
            }
            mat.Set(i, j, Complex(re, im));
        }
    }
    std::cin.ignore(10000, '\n');
}

//Шаблонные операции
template <typename T> void doNorm(const SquareMatrix<T>& m) 
{ 
    std::cout << "Норма: " << m.Norm() << "\n"; 
}
template <typename T> void doPrint(const SquareMatrix<T>& m) 
{ 
    printMatrix(m, "Текущая матрица"); 
}

template <typename T> void doMulScalar(const SquareMatrix<T>& m) 
{
    T s; std::cout << "Введите скаляр: ";
    if (std::cin >> s) 
    { 
        std::cin.ignore(10000, '\n'); 
        printMatrix(m * s, "Результат умножения на скаляр"); 
    } else 
    { 
        std::cin.clear(); 
        std::cin.ignore(10000, '\n'); 
        std::cout << "Ошибка ввода\n"; 
    }
}

template <typename T> void doAdd(const SquareMatrix<T>& m) 
{
    int n; std::cout << "Размер второй матрицы: ";
    if (!readInt(n) || n <= 0) 
    {
        return;
    }
    if (n != m.GetSize()) 
    { 
        std::cout << "Размеры не совпадают\n"; 
        return; 
    }
    SquareMatrix<T> other(n); readMatrix(other);
    printMatrix(m + other, "Сумма матриц");
}

template <typename T> void doMulMatrix(const SquareMatrix<T>& m) 
{
    int n; 
    std::cout << "Размер второй матрицы: ";
    if (!readInt(n) || n <= 0) 
    {
        return;
    }
    if (n != m.GetSize()) 
    { 
        std::cout << "Размеры не совпадают\n"; 
        return; 
    }
    SquareMatrix<T> other(n); 
    readMatrix(other);
    printMatrix(m * other, "Произведение матриц");
}

template <typename T> void doSwapRows(SquareMatrix<T>& m) 
{
    int i, k; 
    std::cout << "Номера строк: "; 
    std::cin >> i >> k; 
    std::cin.ignore(10000, '\n');
    m.SwapRows(i, k); printMatrix(m, "После SwapRows");
}
template <typename T> void doMulRow(SquareMatrix<T>& m) 
{
    int r; T s; std::cout << "Номер строки: "; std::cin >> r;
    std::cout << "Скаляр: "; 
    std::cin >> s; 
    std::cin.ignore(10000, '\n');
    m.MulRow(r, s); 
    printMatrix(m, "После MulRow");
}
template <typename T> void doAddRow(SquareMatrix<T>& m) 
{
    int t, s; T c; std::cout << "Целевая: "; 
    std::cin >> t;
    std::cout << "Исходная: "; 
    std::cin >> s; 
    std::cout << "Коэфф: "; 
    std::cin >> c; 
    std::cin.ignore(10000, '\n');
    m.AddRow(t, s, c); 
    printMatrix(m, "После AddRow");
}
template <typename T> void doSwapCols(SquareMatrix<T>& m) 
{
    int j, k; 
    std::cout << "Номера столбцов: "; 
    std::cin >> j >> k; 
    std::cin.ignore(10000, '\n');
    m.SwapCols(j, k); 
    printMatrix(m, "После SwapCols");
}
template <typename T> void doMulCol(SquareMatrix<T>& m) 
{
    int c; T s; 
    std::cout << "Номер столбца: "; 
    std::cin >> c;
    std::cout << "Скаляр: ";
    std::cin >> s; 
    std::cin.ignore(10000, '\n');
    m.MulCol(c, s); 
    printMatrix(m, "После MulCol");
}
template <typename T> void doAddCol(SquareMatrix<T>& m) 
{
    int t, s;
    T c; 
    std::cout << "Целевой: "; 
    std::cin >> t;
    std::cout << "Исходный: "; 
    std::cin >> s; 
    std::cout << "Коэфф: ";
    std::cin >> c; 
    std::cin.ignore(10000, '\n');
    m.AddCol(t, s, c); 
    printMatrix(m, "После AddCol");
}

template <typename T> void doSolveSLAU(const SquareMatrix<T>& m) 
{
    int n = m.GetSize();
    DynamicArray<T> b(n);
    std::cout << "Введите вектор b (" << n << " элементов):\n";
    for (int i = 0; i < n; ++i) 
    {
        std::cout << "b[" << i << "] = ";
        T val; 
        if (!(std::cin >> val)) 
        { 
            std::cin.clear(); 
            std::cin.ignore(10000, '\n'); 
            return; 
        }
        b.Set(i, val);
    }
    std::cin.ignore(10000, '\n');
    auto x = solveSLAU(m, b);
    std::cout << "Решение x:\n";
    for (int i = 0; i < n; ++i) 
    {
        std::cout << "x[" << i << "] = " << x.Get(i) << "\n";
    }
}

//Диспетчеры, которые вызывают шаблоны по типу
void matrixNorm()          
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doNorm(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doNorm(*currentDoubleMat.mat);
        } 
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doNorm(*currentComplexMat.mat);
        } 
        else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    }
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
    
}  
void printCurrentMatrix()  
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doPrint(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doPrint(*currentDoubleMat.mat);
        } 
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doPrint(*currentComplexMat.mat);
        }
        else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        } 
    }
    catch(...){} 
}
void matrixMulScalar()     
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doMulScalar(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doMulScalar(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doMulScalar(*currentComplexMat.mat);
        } else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    }
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixAdd()           
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doAdd(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doAdd(*currentDoubleMat.mat);
        } 
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doAdd(*currentComplexMat.mat);
        } 
        else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixMulMatrix()     
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doMulMatrix(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doMulMatrix(*currentDoubleMat.mat);
        } 
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doMulMatrix(*currentComplexMat.mat);
        } 
        else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n";
    } 
}
void matrixSwapRows()      
{ try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doSwapRows(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doSwapRows(*currentDoubleMat.mat); 
        } 
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doSwapRows(*currentComplexMat.mat);
        } else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixMulRow()        
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doMulRow(*currentIntMat.mat);
        } 
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doMulRow(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doMulRow(*currentComplexMat.mat); 
        }
        else 
        {
            std::cout<<"Нет активной матрицы\n";
        } 
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixAddRow()        
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doAddRow(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doAddRow(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doAddRow(*currentComplexMat.mat);
        } else
        {
            std::cout<<"Нет активной матрицы\n";
        }  
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixSwapCols()      
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doSwapCols(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doSwapCols(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doSwapCols(*currentComplexMat.mat); 
        }
        else 
        {
            std::cout<<"Нет активной матрицы\n";
        }
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixMulCol()        
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doMulCol(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doMulCol(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doMulCol(*currentComplexMat.mat);
        } 
        else 
        {
            std::cout<<"Нет активной матрицы\n"; 
        }
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void matrixAddCol()        
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doAddCol(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doAddCol(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doAddCol(*currentComplexMat.mat); 
        }
        else 
        {
            std::cout<<"Нет активной матрицы\n";
        } 
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}
void solveSLAUInteractive()
{ 
    try 
    { 
        if(currentMatType==1 && currentIntMat.isValid()) 
        {
            doSolveSLAU(*currentIntMat.mat); 
        }
        else if(currentMatType==2 && currentDoubleMat.isValid()) 
        {
            doSolveSLAU(*currentDoubleMat.mat); 
        }
        else if(currentMatType==3 && currentComplexMat.isValid()) 
        {
            doSolveSLAU(*currentComplexMat.mat); 
        }
        else std::cout<<"Нет активной матрицы\n"; 
    } 
    catch(const std::exception& e)
    { 
        std::cerr<<"Ошибка: "<<e.what()<<"\n"; 
    } 
}

//Создание матрицы
template <typename T>
void createMatrix(MatrixHolder<T>& holder)
{
    int n;
    std::cout << "Введите размер матрицы (n для n×n): ";
    if (!readInt(n) || n <= 0) 
    { 
        std::cout << "Ошибка: неверный размер\n"; 
        return; 
    }

    SquareMatrix<T>* mat = new SquareMatrix<T>(n);
    try 
    {
        readMatrix(*mat); 
    } catch (...) 
    {
        delete mat;
        throw;
    }
    holder.set(mat);
    std::cout << "Матрица создана.\n";
    printMatrix(*holder.mat, "Текущая матрица");
}

//Операции с после-ю
template <class T>
void opGet(Sequence<T>* seq)
{
    if (!seq)
    {
        std::cout << "Последовательность не создана\n";
        return;
    }

    int idx;
    std::cout << "Введите индекс: ";
    if (!readInt(idx)) 
    {
        return;
    }

    if (idx < 0 || idx >= seq->GetLength())
    {
        std::cout << "Ошибка: индекс вне диапазона [0, " << seq->GetLength() - 1 << "]\n";
    }
    else
    {
        std::cout << "Get(" << idx << ") = " << seq->Get(idx) << "\n";
    }
}

template <class T>
void opAppend(Sequence<T>*& seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    T val;
    std::cout << "Введите значение: ";
    std::cin >> val;
    std::cin.ignore(10000, '\n');
    
    Sequence<T>* r = seq->Append(val);
    if (r != seq) 
    {
        delete seq;
        seq = r;
    }
    printSequence(seq, "После Append");
}

template <class T>
void opPrepend(Sequence<T>*& seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    T val;
    std::cout << "Введите значение: ";
    std::cin >> val;
    std::cin.ignore(10000, '\n');
    
    Sequence<T>* r = seq->Prepend(val);
    if (r != seq) 
    {
        delete seq;
        seq = r;
    }
    printSequence(seq, "После Prepend");
}

template <class T>
void opInsertAt(Sequence<T>*& seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    int idx;
    std::cout << "Введите индекс: ";
    if (!readInt(idx)) 
    {
        return;
    }
    
    if (idx < 0 || idx > seq->GetLength()) 
    {
        std::cout << "Ошибка: индекс вне диапазона [0, " << seq->GetLength() << "]\n";
        return;
    }
    T val;
    std::cout << "Введите значение: ";
    std::cin >> val;
    std::cin.ignore(10000, '\n');
    
    Sequence<T>* r = seq->InsertAt(val, idx);
    if (r != seq) 
    {
        delete seq;
        seq = r;
    }
    printSequence(seq, "После InsertAt");
}

template <class T>
void opRemoveAt(Sequence<T>*& seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    int idx;
    std::cout << "Введите индекс: ";
    if (!readInt(idx)) 
    {
        return;
    }
    
    if (idx < 0 || idx >= seq->GetLength()) 
    {
        std::cout << "Ошибка: индекс вне диапазона [0, " << seq->GetLength()-1 << "]\n";
        return;
    }
    Sequence<T>* r = seq->RemoveAt(idx);
    if (r != seq) 
    {
        delete seq;
        seq = r;
    }
    printSequence(seq, "После RemoveAt");
}

template <class T>
void opSubsequence(Sequence<T>* seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    int start, end;
    std::cout << "Введите startIndex endIndex: ";
    std::cin >> start >> end;
    std::cin.ignore(10000, '\n');
    
    if (start < 0 || end < start) 
    {
        std::cout << "Ошибка: нужно 0 <= start <= end\n";
        return;
    }
    if (end >= seq->GetLength()) 
    {
        std::cout << "Ошибка: end вне диапазона [0, " << seq->GetLength()-1 << "]\n";
        return;
    }
    Sequence<T>* sub = seq->GetSubsequence(start, end);
    printSequence(sub, "Подпоследовательность");
    delete sub;
}

template <class T>
void opConcat(Sequence<T>*& seq)
{
    if (!seq) 
    {
        std::cout << "Последовательность не создана\n";
        return;
    }
    std::cout << "Введите элементы второй последовательности: ";
    T temp[100];
    int count = readArray<T>(temp, 100);
    if (count == 0) 
    {
        std::cout << "Ошибка: не введено элементов\n";
        return;
    }

    MutableArraySequence<T>* other = new MutableArraySequence<T>(temp, count);
    try 
    {
        Sequence<T>* r = seq->Concat(other);
        delete other;
        if (r != seq) 
        {
            delete seq;
            seq = r;
        }
        printSequence(seq, "После Concat");
    } catch (...) 
    {
        delete other; 
        throw;
    }
}

//Меню
void printMenu()
{
    std::cout << "\n===== Главное меню =====\n"
        << "--- Последовательности (int) ---\n"
        << "1.  MutableArraySequence<int>\n"
        << "2.  ImmutableArraySequence<int>\n"
        << "3.  MutableListSequence<int>\n"
        << "4.  ImmutableListSequence<int>\n"
        << "--- Последовательности (string) ---\n"
        << "5.  MutableArraySequence<string>\n"
        << "6.  ImmutableArraySequence<string>\n"
        << "7.  MutableListSequence<string>\n"
        << "8.  ImmutableListSequence<string>\n"
        << "--- Операции над последовательностью ---\n"
        << "9.  Напечатать текущую\n"
        << "10. Get\n11. GetLength\n12. Append\n13. Prepend\n"
        << "14. InsertAt\n15. RemoveAt\n16. GetSubsequence\n17. Concat\n"
        << "--- Квадратные матрицы ---\n"
        << "18. Создать SquareMatrix<int>\n"
        << "19. Создать SquareMatrix<double>\n"
        << "20. Создать SquareMatrix<Complex>\n"
        << "21. Показать текущую матрицу\n22. Вычислить норму\n"
        << "23. Умножить на скаляр\n24. Сложить матрицы\n25. Умножить матрицы\n"
        << "26. SwapRows\n27. MulRow\n28. AddRow\n"
        << "29. SwapCols\n30. MulCol\n31. AddCol\n"
        << "32. Решить СЛАУ (A*x = b)\n"
        << "--- Прочее ---\n33. AdaptiveSequence<int>\n0.  Выход\n"
        << "Ввод:";
    std::cout.flush();
}

template <class T>
void demonstrateSequence(T* arr, int size, const char* typeName)
{
    std::cout << "\n=== Демонстрация: " << typeName << " ===\n";
    Sequence<T>* seq = new MutableArraySequence<T>(arr, size);
    printSequence(seq, "Исходная");
    Sequence<T>* r = seq->Append(arr[size-1]); 
    if(r!=seq)
    {
        delete seq; 
        seq=r;
    } 
    printSequence(seq,"После Append");
    r = seq->Prepend(arr[0]); 
    if(r!=seq)
    {
        delete seq; seq=r;
    } 
    printSequence(seq,"После Prepend");
    r = seq->RemoveAt(0); 
    if(r!=seq)
    {
        delete seq; seq=r;
    } 
    printSequence(seq,"После RemoveAt(0)");
    Sequence<T>* sub = seq->GetSubsequence(0, seq->GetLength()-1);
    printSequence(sub, "GetSubsequence"); 
    delete sub;
    delete seq;
}
void demonstrateInt() 
{ 
    int arr[]={10,20,30,40,50}; 
    demonstrateSequence(arr,5,"integer"); 
}
void demonstrateString() 
{ 
    std::string arr[]={"apple","banana","cherry","date"}; 
    demonstrateSequence(arr,4,"string"); 
}
void demonstrateMatrices()
{
    std::cout << "\n=== Демонстрация SquareMatrix ===\n";
    int arr[]={1,2,3,4}; 
    SquareMatrix<int> mI(arr,2); 
    printMatrix(mI,"int 2x2");
    std::cout << "Норма: " << mI.Norm() << "\n";
    SquareMatrix<Complex> mC(2);
    mC.Set(0,0,Complex(1,1)); 
    mC.Set(0,1,Complex(2,0));
    mC.Set(1,0,Complex(0,-1)); 
    mC.Set(1,1,Complex(3,2));
    printMatrix(mC,"Complex 2x2");
    std::cout << "Норма: " << mC.Norm() << "\n";
    printMatrix(mC*mC, "Квадрат Complex");
    printMatrix(mC*Complex(0,2), "Умножение на (0+2i)");
}

void runInteractive()
{
    Sequence<int>*         currentInt    = nullptr;
    Sequence<std::string>* currentString = nullptr;
    bool isIntMode = true;
    int choice;
    do 
    {
        try 
        {
            printMenu(); choice = -1; 
            readInt(choice);
            if (choice == -1) 
            {
                continue;
            }
            if (choice >= 1 && choice <= 4) 
            {
                std::cout << "Введите целые числа через пробел: ";
                int temp[100]; 
                int count = readArray<int>(temp, 100);
                if (count == 0) 
                { 
                    std::cout << "Ошибка: нет элементов\n"; 
                    continue; 
                }
                delete currentInt; currentInt = nullptr;
                delete currentString; currentString = nullptr; isIntMode = true;
                switch(choice)
                {
                    case 1: currentInt = new MutableArraySequence<int>(temp, count); 
                            break;
                    case 2: currentInt = new ImmutableArraySequence<int>(temp, count); 
                            break;
                    case 3: currentInt = new MutableListSequence<int>(temp, count); 
                            break;
                    case 4: currentInt = new ImmutableListSequence<int>(temp, count); 
                            break;
                }
                printSequence(currentInt, "Создана");
            }
            else if (choice >= 5 && choice <= 8) 
            {
                std::cout << "Введите строки через пробел: ";
                std::string temp[100]; 
                int count = readArray<std::string>(temp, 100);
                if (count == 0) 
                { 
                    std::cout << "Ошибка: нет элементов\n"; 
                    continue; 
                }
                delete currentInt; 
                currentInt = nullptr;
                delete currentString; 
                currentString = nullptr; 
                isIntMode = false;
                switch(choice){
                    case 5: currentString = new MutableArraySequence<std::string>(temp, count); 
                            break;
                    case 6: currentString = new ImmutableArraySequence<std::string>(temp, count); 
                            break;
                    case 7: currentString = new MutableListSequence<std::string>(temp, count); 
                            break;
                    case 8: currentString = new ImmutableListSequence<std::string>(temp, count); 
                            break;
                }
                printSequence(currentString, "Создана");
            }
            else if (choice >= 9 && choice <= 17) 
            {
                switch(choice)
                {
                    case 9:  if(isIntMode) 
                    {
                        printSequence(currentInt,"Текущая"); 
                    }
                    else 
                    {
                        printSequence(currentString,"Текущая");    
                    }
                    break;

                    case 10: if(isIntMode) 
                    {
                        opGet(currentInt); 
                    }
                    else 
                    {
                        opGet(currentString);             
                    } 
                    break;

                    case 11: if(isIntMode&&currentInt) 
                    {
                        std::cout<<"Length="<<currentInt->GetLength()<<"\n"; 
                    }
                    else if(!isIntMode&&currentString) 
                    {
                        std::cout<<"Length="<<currentString->GetLength()<<"\n"; 
                    }
                    else {
                        std::cout<<"Нет последовательности\n"; 
                    }
                    break;

                    case 12: if(isIntMode) 
                    {
                        opAppend(currentInt); 
                    }
                    else 
                    {
                        opAppend(currentString);
                    }
                    break;

                    case 13: if(isIntMode) 
                    {
                        opPrepend(currentInt); 
                    }
                    else 
                    {
                        opPrepend(currentString); 
                    }
                    break;

                    case 14: if(isIntMode) 
                    {
                        opInsertAt(currentInt); 
                    }
                    else 
                    {
                        opInsertAt(currentString); 
                    }
                    break;

                    case 15: if(isIntMode) 
                    {
                        opRemoveAt(currentInt); 
                    }
                    else 
                    {
                        opRemoveAt(currentString); 
                    }
                    break;

                    case 16: if(isIntMode) 
                    {
                        opSubsequence(currentInt); 
                    }
                    else 
                    {
                        opSubsequence(currentString); 
                    }
                    break;

                    case 17: if(isIntMode) 
                    {
                        opConcat(currentInt); 
                    }
                    else 
                    {
                        opConcat(currentString); 
                    }
                    break;
                }
            }
            else if (choice >= 18 && choice <= 32) 
            {
                switch(choice)
                {
                    case 18: createMatrix(currentIntMat); 
                            currentMatType=1; 
                            break;
                    case 19: createMatrix(currentDoubleMat); 
                            currentMatType=2; 
                            break;
                    case 20: createMatrix(currentComplexMat); 
                            currentMatType=3; 
                            break;
                    case 21: printCurrentMatrix(); 
                            break; 
                    case 22: matrixNorm(); 
                            break;
                    case 23: matrixMulScalar(); 
                            break; 
                    case 24: matrixAdd(); 
                            break;
                    case 25: matrixMulMatrix(); 
                            break; 
                    case 26: matrixSwapRows(); 
                            break;
                    case 27: matrixMulRow(); 
                            break; 
                    case 28: matrixAddRow(); 
                            break;
                    case 29: matrixSwapCols(); 
                            break; 
                    case 30: matrixMulCol(); 
                            break;
                    case 31: matrixAddCol(); 
                            break; 
                    case 32: solveSLAUInteractive(); 
                            break;
                }
            }
            else if (choice == 33) 
            {
                std::cout << "Введите int-элементы для AdaptiveSequence: ";
                int temp[100]; 
                int count = readArray<int>(temp, 100);
                if (count == 0) 
                { 
                    std::cout << "Ошибка: нет элементов\n"; 
                    continue; 
                }
                AdaptiveSequence<int> adaptive(temp, count);
                std::cout << "Реализация: " << adaptive.GetImplType() << "\n";
                printSequence<int>(&adaptive, "AdaptiveSequence");
                for(int i=0;i<20;++i) 
                {
                    adaptive.Get(i%count);
                }
                std::cout << "После 20 чтений: " << adaptive.GetImplType() << "\n";
            }
            else if (choice != 0) 
            {
                std::cout << "Неизвестная команда.\n";
            }
        }
        catch (const IndexOutOfRangeException& e)
        { 
            std::cerr << "Ошибка индекса: " << e.what() << "\n"; 
        }
        catch (const EmptyStructureException& e) 
        { 
            std::cerr << "Пустая структура: " << e.what() << "\n"; 
        }
        catch (const InvalidArgumentException& e) 
        { 
            std::cerr << "Ошибка аргумента: " << e.what() << "\n"; 
        }
        catch (const std::exception& e) 
        { 
            std::cerr << "Ошибка: " << e.what() << "\n"; 
        }
    } while (choice != 0);

    //Очистка
    delete currentInt;
    delete currentString;
}

int main()
{
    std::cout << "========================================\n"
        << "   Лабораторная работа №3: Sequence + SquareMatrix\n"
        << "========================================\n";
    demonstrateInt(); demonstrateString(); demonstrateMatrices();
    std::cout << "\n========================================\n"
        << "   ИНТЕРАКТИВНЫЙ РЕЖИМ\n"
        << "========================================\n";
    runInteractive();
    std::cout << "\nПрограмма завершена. Память освобождена.\n";
    return 0;
}