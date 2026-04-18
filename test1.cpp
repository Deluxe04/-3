#include <iostream>
#include <string>
#include <cmath>
#include <chrono>    //для времени

#include "dynamic_array.h"
#include "linked_list.h"
#include "array_sequence.h"
#include "list_sequence.h"
#include "adaptive_sequence.h"
#include "builder.h"
#include "square_matrix.h"
#include "complex.h"
#include "exceptions.h"
#include "utils.h"
#include "slau_solver.h"

//Тест-раннер 
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
    }
    else
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
    }
    catch (const ExceptionType&)
    {
        std::cout << "  [ПРОЙДЕН]  " << name << "\n";
        ++totalPassed;
    }
    catch (...)
    {
        std::cout << "  [ПРОВАЛЕН] " << name << " (неверный тип исключения)\n";
        ++totalFailed;
    }
}

void printSummary()
{
    std::cout << "\n═══════════════════════════════════════════════\n";
    std::cout << "  ИТОГО: пройдено " << totalPassed
              << ", провалено " << totalFailed
              << ", всего " << (totalPassed + totalFailed) << "\n";
    std::cout << "═══════════════════════════════════════════════\n";
}

//Вспомогательные функции для функциональных операций
static int doubleInt(const int& x) 
{ 
    return x * 2; 
}
static bool isEven(const int& x) 
{ 
    return x % 2 == 0; 
}
static int sumInts(int acc, const int& x) 
{ 
    return acc + x; 
}

//Универсальный тест для DynamicArray любого типа
template <typename T>
void testDynamicArrayTemplate(const std::string& suiteName, const T* initData, int size, const T& setValue)
{
    beginSuite(suiteName);
    DynamicArray<T> da(initData, size);
    
    check("GetSize() == " + std::to_string(size), da.GetSize() == size);
    check("Get(0) == first", da.Get(0) == initData[0]);
    check("Get(last) == last", da.Get(size - 1) == initData[size - 1]);

    da.Set(1, setValue);
    check("Set(1) изменён", da.Get(1) == setValue);

    da.Resize(size + 2);
    check("Resize(+) size", da.GetSize() == size + 2);
    check("Resize сохраняет старые данные", da.Get(0) == initData[0]);

    da.Resize(size);
    check("Resize(-) size", da.GetSize() == size);

    //Глубокое копирование
    DynamicArray<T> copy(da);
    copy.Set(0, setValue);
    check("Копирующий конструктор", da.Get(0) == initData[0]);

    DynamicArray<T> assigned(1);
    assigned = da;
    assigned.Set(0, setValue);
    check("operator= (глубокая копия)", da.Get(0) == initData[0]);

    //Исключения
    checkThrows<IndexOutOfRangeException>("Get(-1)", [&]{ da.Get(-1); });
    checkThrows<IndexOutOfRangeException>("Get(100)", [&]{ da.Get(100); });
    checkThrows<InvalidArgumentException>("Resize(-1)", [&]{ da.Resize(-1); });
}

//Тесты для LinkedList
void testLinkedListInt()
{
    beginSuite("LinkedList<int>");
    
    int arr[] = {1, 2, 3, 4, 5};
    LinkedList<int> ll(arr, 5);
    
    check("GetLength()==5", ll.GetLength() == 5);
    check("GetFirst()==1", ll.GetFirst() == 1);
    check("GetLast()==5", ll.GetLast() == 5);
    check("Get(2)==3", ll.Get(2) == 3);
    
    ll.Append(6);
    check("Append(6) GetLast()==6", ll.GetLast() == 6);
    check("Append(6) GetLength()==6", ll.GetLength() == 6);
    
    ll.Prepend(0);
    check("Prepend(0) GetFirst()==0", ll.GetFirst() == 0);
    
    ll.InsertAt(99, 3);
    check("InsertAt(99,3) Get(3)==99", ll.Get(3) == 99);
    
    ll.RemoveAt(3);
    check("RemoveAt(3) Get(3)==3", ll.Get(3) == 3);
    
    LinkedList<int> sub = ll.GetSubList(1, 3);
    check("GetSubList(1,3) length==3", sub.GetLength() == 3);
    
    int arr2[] = {10, 20};
    LinkedList<int> ll2(arr2, 2);
    LinkedList<int> cat = ll.Concat(ll2);
    check("Concat→GetLast()==20", cat.GetLast() == 20);
    
    checkThrows<IndexOutOfRangeException>("Get(-1) исключение", [&]{ ll.Get(-1); });
    checkThrows<EmptyStructureException>("GetFirst() на пустом", [&]{ LinkedList<int> e; e.GetFirst(); });
}

//Шаблонный тест для любой mutable-последовательности целых чисел
template <template<typename> class SeqType>
void testMutableSequenceTemplate(const std::string& suiteName)
{
    beginSuite(suiteName);
    int arr[] = {10, 20, 30, 40, 50};
    SeqType<int> seq(arr, 5);

    check("GetLength()==5", seq.GetLength() == 5);
    check("GetFirst()==10", seq.GetFirst() == 10);
    check("GetLast()==50", seq.GetLast() == 50);

    Sequence<int>* r = seq.Append(60);
    check("Append возвращает this", r == &seq);
    check("Append(60) GetLast()==60", seq.GetLast() == 60);

    seq.Prepend(5);
    check("Prepend(5) GetFirst()==5", seq.GetFirst() == 5);

    seq.InsertAt(25, 3);
    check("InsertAt(25,3) Get(3)==25", seq.Get(3) == 25);

    seq.RemoveAt(3);
    check("RemoveAt(3) отработал", seq.GetLength() == 7);

    //Операции (Map, Where, Reduce, Concat)
    int arr2[] = {100, 200};
    SeqType<int> seq2(arr2, 2);
    Sequence<int>* cat = seq.Concat(&seq2);
    check("Concat GetLast()==200", cat->GetLast() == 200);
    if (cat != &seq) 
    {
        delete cat;
    }

    SeqType<int> forMap(arr, 5);
    Sequence<int>* mapped = forMap.Map(doubleInt);
    check("Map(*2) Get(0)==20", mapped->Get(0) == 20);
    delete mapped;

    int arr3[] = {1, 2, 3, 4, 5, 6};
    SeqType<int> forWhere(arr3, 6);
    Sequence<int>* filtered = forWhere.Where(isEven);
    check("Where(isEven)->length==3", filtered->GetLength() == 3);
    delete filtered;

    int arr4[] = {1, 2, 3, 4, 5};
    SeqType<int> forReduce(arr4, 5);
    check("Reduce(sum)==15", forReduce.Reduce(sumInts, 0) == 15);

    checkThrows<IndexOutOfRangeException>("Get(-1) исключение", [&]{ forReduce.Get(-1); });
}

//Тесты для ImmutableArraySequence
void testImmutableArraySequenceInt()
{
    beginSuite("ImmutableArraySequence<int>");
    
    int arr[] = {1, 2, 3};
    ImmutableArraySequence<int> seq(arr, 3);
    
    check("GetLength()==3", seq.GetLength() == 3);
    
    Sequence<int>* n = seq.Append(4);
    check("Append новый объект", n != &seq);
    check("Append старый length==3", seq.GetLength() == 3);
    check("Append новый length==4", n->GetLength() == 4);
    check("Append новый GetLast()==4", n->GetLast() == 4);
    
    ImmutableArraySequence<int> original(arr, 3);
    std::cout << "  Исходная:     " << seq << "\n";
    std::cout << "  После Append: " << *n << "\n";
    delete n;
    
    Sequence<int>* n2 = seq.Prepend(0);
    check("Prepend новый GetFirst()==0", n2->GetFirst() == 0);
    check("Prepend старый не изменён", seq.GetFirst() == 1);
    delete n2;
}

//Тесты для AdaptiveSequence
void testAdaptiveSequenceInt()
{
    beginSuite("AdaptiveSequence<int>");
    
    int arr[] = {1, 2, 3, 4, 5};
    AdaptiveSequence<int> seq(arr, 5);
    
    check("GetLength()==5", seq.GetLength() == 5);
    check("GetFirst()==1", seq.GetFirst() == 1);
    check("GetLast()==5", seq.GetLast() == 5);
    
    std::cout << "  AdaptiveSequence = " << seq << "\n";
    
    for (int i = 0; i < 20; ++i) 
    {
        seq.Get(i % 5);
    }
    check("После 20 чтений работает", seq.Get(2) == 3);
    
    AdaptiveSequence<int> seq2(arr, 5);
    for (int i = 0; i < 10; ++i) 
    {
        seq2.Append(i);
    }
    check("После 10 вставок length==15", seq2.GetLength() == 15);
    std::cout << "  После 10 вставок = " << seq2 << "\n";
    
    Sequence<int>* sub = seq.GetSubsequence(0, 2);
    check("GetSubsequence(0,2) length==3", sub->GetLength() == 3);
    delete sub;
    
    Sequence<int>* mapped = seq.Map(doubleInt);
    check("Map(*2) Get(0)==2", mapped->Get(0) == 2);
    std::cout << "  Map(*2) = " << *mapped << "\n";
    delete mapped;
    
    checkThrows<IndexOutOfRangeException>("GetSubsequence(-1,2)", [&]{ seq.GetSubsequence(-1, 2); });
}

//Тесты для Builders
void testBuilders()
{
    beginSuite("ArraySequenceBuilder и ListSequenceBuilder");
    
    ArraySequenceBuilder<int> ab;
    ab.Add(10).Add(20).Add(30);
    
    MutableArraySequence<int>* ma = ab.BuildMutable();
    check("ArrayBuilder Mutable length==3", ma->GetLength() == 3);
    check("ArrayBuilder Mutable Get(0)==10", ma->Get(0) == 10);
    std::cout << "  ArrayBuilder Mutable = " << *ma << "\n";
    delete ma;
    
    ImmutableArraySequence<int>* ia = ab.BuildImmutable();
    check("ArrayBuilder Immutable length==3", ia->GetLength() == 3);
    std::cout << "  ArrayBuilder Immutable = " << *ia << "\n";
    delete ia;
    
    ListSequenceBuilder<int> lb;
    lb.Add(1).Add(2).Add(3).Add(4);
    
    MutableListSequence<int>* ml = lb.BuildMutable();
    check("ListBuilder Mutable length==4", ml->GetLength() == 4);
    check("ListBuilder Mutable Get(0)==1", ml->Get(0) == 1);
    std::cout << "  ListBuilder Mutable = " << *ml << "\n";
    delete ml;
}

//Шаблонный тест итератора
template <template<typename> class ContType>
void testIteratorTemplate(const std::string& suiteName)
{
    beginSuite(suiteName);
    int arr[] = {10, 20, 30};
    
    ContType<int> cont(arr, 3);
    IEnumerator<int>* it = cont.GetEnumerator();
    bool ok = true; int idx = 0;
    while (it->MoveNext()) 
    {
        if (it->Current() != arr[idx++]) 
        {
            ok = false;
        }
    }
    check("Итератор: все элементы совпадают", ok && idx == 3);
    it->Release();

    //Пустая коллекция
    int emptyData[] = {};
    ContType<int> empty(emptyData, 0);
    IEnumerator<int>* itEmpty = empty.GetEnumerator();
    check("Итератор пустой: MoveNext()==false", !itEmpty->MoveNext());
    itEmpty->Release();
}

//Тесты для Complex
void testComplexBasics()
{
    beginSuite("Complex");
    
    Complex c1(3, 4), c2(1, -2);
    
    check("re==3", c1.re == 3.0);
    check("im==4", c1.im == 4.0);
    check("Abs()==5", std::abs(c1.Abs() - 5.0) < 1e-9);
    
    Complex sum = c1 + c2;
    check("sum.re==4", sum.re == 4.0);
    check("sum.im==2", sum.im == 2.0);
    
    Complex prod = c1 * c2;
    check("prod.re==11", prod.re == 11.0);
    check("prod.im==-2", prod.im == -2.0);
    
    Complex scaled = c1 * 2.0;
    check("scaled.re==6", scaled.re == 6.0);
    check("scaled.im==8", scaled.im == 8.0);
    
    check("c1==Complex(3,4)", c1 == Complex(3, 4));
    check("c1!=c2", c1 != c2);
}

void testElemAbs()
{
    beginSuite("elemAbs перегрузки");
    
    check("elemAbs(int -5)==5", elemAbs(-5) == 5.0);
    check("elemAbs(double -2.5)==2.5", std::abs(elemAbs(-2.5) - 2.5) < 1e-9);
    check("elemAbs(Complex(3,4))==5", std::abs(elemAbs(Complex(3,4)) - 5.0) < 1e-9);
}

//Тесты для SquareMatrix<int>
void testSquareMatrixIntBasics()
{
    beginSuite("SquareMatrix<int> базовые");
    
    int data[] = {1, 2, 3, 4};
    SquareMatrix<int> mat(data, 2);
    
    check("GetSize()==2", mat.GetSize() == 2);
    check("Get(0,0)==1", mat.Get(0, 0) == 1);
    check("Get(1,1)==4", mat.Get(1, 1) == 4);
    
    mat.Set(0, 1, 99);
    check("Set(0,1,99)→Get(0,1)==99", mat.Get(0, 1) == 99);
    
    check("Norm() корректна", std::abs(mat.Norm() - std::sqrt(9827.0)) < 1e-9);
}

void testSquareMatrixIntElementary()
{
    beginSuite("SquareMatrix<int> элементарные преобразования");
    
    int data[] = {1, 2, 3, 4};
    SquareMatrix<int> mat(data, 2);
    
    //SwapRows
    mat.SwapRows(0, 1);
    check("SwapRows (0,0)==3", mat.Get(0, 0) == 3);
    check("SwapRows (1,1)==2", mat.Get(1, 1) == 2);
    
    //SwapCols
    mat.SwapCols(0, 1);
    check("SwapCols (0,0)==4", mat.Get(0, 0) == 4);
    check("SwapCols (1,1)==1", mat.Get(1, 1) == 1);
    
    //MulRow
    mat.MulRow(0, 10);
    check("MulRow (0,0)==40", mat.Get(0, 0) == 40);
    check("MulRow (0,1)==30", mat.Get(0, 1) == 30); 
    
    //MulCol
    mat.MulCol(1, 5);
    check("MulCol (0,1)==150", mat.Get(0, 1) == 150);  
    check("MulCol (1,1)==5", mat.Get(1, 1) == 5);      
}

//Тесты для SquareMatrix<Complex>
void testSquareMatrixComplexBasics()
{
    beginSuite("SquareMatrix<Complex> базовые");
    
    SquareMatrix<Complex> mat(2);
    mat.Set(0, 0, Complex(1, 1)); mat.Set(0, 1, Complex(0, 0));
    mat.Set(1, 0, Complex(0, 0)); mat.Set(1, 1, Complex(1, 1));
    
    check("GetSize()==2", mat.GetSize() == 2);
    check("Get(0,0)==1+1i", mat.Get(0, 0) == Complex(1, 1));

    check("Norm()==2", std::abs(mat.Norm() - 2.0) < 1e-9);
}

void testSquareMatrixComplexOperations()
{
    beginSuite("SquareMatrix<Complex> операции");
    
    SquareMatrix<Complex> mat(2);
    mat.Set(0, 0, Complex(1, 0)); mat.Set(0, 1, Complex(0, 1));
    mat.Set(1, 0, Complex(1, 1)); mat.Set(1, 1, Complex(0, 0));
    
    //Умножение на комплексный скаляр (0+2i)
    SquareMatrix<Complex> result = mat * Complex(0, 2);
    check("MulComplex (0,0)==0+2i", result.Get(0, 0) == Complex(0, 2));
    check("MulComplex (0,1)==-2+0i", result.Get(0, 1) == Complex(-2, 0));
    
    //Сложение
    SquareMatrix<Complex> other(2);
    other.Set(0, 0, Complex(1, 1)); other.Set(1, 1, Complex(1, 1));
    SquareMatrix<Complex> sum = mat + other;
    check("Add (0,0)==2+1i", sum.Get(0, 0) == Complex(2, 1));
}

void testSquareMatrixComplexElementary()
{
    beginSuite("SquareMatrix<Complex> элементарные преобразования");
    
    SquareMatrix<Complex> mat(2);
    mat.Set(0, 0, Complex(1, 0)); mat.Set(0, 1, Complex(2, 0));
    mat.Set(1, 0, Complex(3, 0)); mat.Set(1, 1, Complex(4, 0));
    
    //SwapRows с комплексными
    mat.SwapRows(0, 1);
    check("SwapRows (0,0)==3", mat.Get(0, 0).re == 3.0);
    
    //MulRow с комплексным скаляром
    mat.MulRow(0, Complex(0, 2)); 
    check("MulRow (0,0)==0+6i", mat.Get(0, 0) == Complex(0, 6));
}

//Тесты для исключений в SquareMatrix
void testSquareMatrixExceptions()
{
    beginSuite("SquareMatrix исключения");
    
    SquareMatrix<int> mat(2);
    
    checkThrows<IndexOutOfRangeException>("Get(-1,0)", [&]{ mat.Get(-1, 0); });
    checkThrows<IndexOutOfRangeException>("Get(0,5)", [&]{ mat.Get(0, 5); });
    checkThrows<IndexOutOfRangeException>("Set(10,0,1)", [&]{ mat.Set(10, 0, 1); });
    checkThrows<IndexOutOfRangeException>("SwapRows(0,5)", [&]{ mat.SwapRows(0, 5); });
    checkThrows<IndexOutOfRangeException>("MulCol(-1,2)", [&]{ mat.MulCol(-1, 2); });
}

void testSquareMatrixIntOperations()
{
    beginSuite("SquareMatrix<int> операции");
    
    int a[] = {1, 2, 3, 4}, b[] = {5, 6, 7, 8};
    SquareMatrix<int> m1(a, 2), m2(b, 2);
    
    //Сложение
    SquareMatrix<int> sum = m1 + m2;
    check("Add (0,0)==6", sum.Get(0, 0) == 6);
    check("Add (1,1)==12", sum.Get(1, 1) == 12);
    
    //Умножение на скаляр
    SquareMatrix<int> scaled = m1 * 3;
    check("MulScalar (0,1)==6", scaled.Get(0, 1) == 6);
    check("MulScalar (1,0)==9", scaled.Get(1, 0) == 9);
    
    //Умножение матриц
    SquareMatrix<int> prod = m1 * m2;
    check("Mul (0,0)==19", prod.Get(0, 0) == 19);  
    check("Mul (0,1)==22", prod.Get(0, 1) == 22);  
    check("Mul (1,0)==43", prod.Get(1, 0) == 43);  
    check("Mul (1,1)==50", prod.Get(1, 1) == 50);  
}

//Тесты СЛАУ
void testSLAU_Double()
{
    beginSuite("solveSLAU<double>");
    //2x + y = 5, x + 3y = 11 x=0.8, y=3.4
    SquareMatrix<double> A(2);
    A.Set(0,0,2); A.Set(0,1,1);
    A.Set(1,0,1); A.Set(1,1,3);

    DynamicArray<double> b(2);
    b.Set(0, 5.0); b.Set(1, 11.0);

    auto x = solveSLAU(A, b);
    check("x[0] == 0.8", std::abs(x.Get(0) - 0.8) < 1e-9);
    check("x[1] == 3.4", std::abs(x.Get(1) - 3.4) < 1e-9);
}

void testSLAU_Int()
{
    beginSuite("solveSLAU<int>");
    //2x + y = 4, x - y = -1 x=1, y=2
    SquareMatrix<int> A(2);
    A.Set(0,0,2); A.Set(0,1,1);
    A.Set(1,0,1); A.Set(1,1,-1);

    DynamicArray<int> b(2);
    b.Set(0, 4); b.Set(1, -1);

    auto x = solveSLAU(A, b);
    check("x[0] == 1", x.Get(0) == 1);
    check("x[1] == 2", x.Get(1) == 2);
}

void testSLAU_Complex()
{
    beginSuite("solveSLAU<Complex>");
    //x + y = 2+0i, x - y = 0+0i  x = 1+0i, y = 1+0i
    SquareMatrix<Complex> A(2);
    A.Set(0,0, Complex(1,0)); A.Set(0,1, Complex(1,0));
    A.Set(1,0, Complex(1,0)); A.Set(1,1, Complex(-1,0));

    DynamicArray<Complex> b(2);
    b.Set(0, Complex(2,0));
    b.Set(1, Complex(0,0));

    auto x = solveSLAU(A, b);
    
    bool x0_ok = std::abs(x.Get(0).re - 1.0) < 1e-9 && std::abs(x.Get(0).im - 0.0) < 1e-9;
    bool x1_ok = std::abs(x.Get(1).re - 1.0) < 1e-9 && std::abs(x.Get(1).im - 0.0) < 1e-9;
    
    check("x[0] == 1+0i", x0_ok);
    check("x[1] == 1+0i", x1_ok);
}

void testSLAU_Exceptions()
{
    beginSuite("solveSLAU Исключения");
    //Вырожденная матрица: x+2y=3, 2x+4y=6
    SquareMatrix<double> A(2);
    A.Set(0,0,1); A.Set(0,1,2);
    A.Set(1,0,2); A.Set(1,1,4);

    DynamicArray<double> b(2);
    b.Set(0, 3.0); b.Set(1, 6.0);

    checkThrows<InvalidOperationException>("Вырожденная матрица (det≈0)", [&]{
        solveSLAU(A, b);
    });
}

void testSLAU_Performance_100x100()
{
    beginSuite("solveSLAU Производительность (100x100)");
    int n = 100;
    SquareMatrix<double> A(n);
    DynamicArray<double> b(n);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A.Set(i, j, (i == j) ? 2.0 * n : 1.0);
        }
        b.Set(i, static_cast<double>(i) + 1.0);
    }

    auto start = std::chrono::high_resolution_clock::now();
    try {
        auto x = solveSLAU(A, b);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;

        std::cout << "  Время решения 100x100: " << duration.count() << " мс\n";
        check("Решение найдено без исключений", true);
    } catch (const std::exception& e) {
        std::cerr << " Ошибка при решении: " << e.what() << "\n";
        check("Решение без исключений", false);
    }
}

//Главная функция
int main()
{
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   АВТОМАТИЧЕСКИЕ ТЕСТЫ: Sequence + SquareMatrix\n";
    std::cout << "═══════════════════════════════════════════════\n";
    
    //Последовательности
    //Тесты DynamicArray
    int intArr[] = {10, 20, 30, 40, 50};
    testDynamicArrayTemplate<int>("DynamicArray<int>", intArr, 5, 99);
    std::string strArr[] = {"apple", "banana", "cherry"};
    testDynamicArrayTemplate<std::string>("DynamicArray<std::string>", strArr, 3, "blueberry");
    testLinkedListInt();
    testImmutableArraySequenceInt();
    
    //Инстанцируем шаблон для конкретных классов
    testMutableSequenceTemplate<MutableArraySequence>("MutableArraySequence<int>");
    testMutableSequenceTemplate<MutableListSequence>("MutableListSequence<int>");
    testAdaptiveSequenceInt();
    testBuilders();
    testIteratorTemplate<DynamicArray>("IEnumerator<DynamicArray>");
    testIteratorTemplate<MutableArraySequence>("IEnumerator<MutableArraySequence>");
    
    //Complex
    testComplexBasics();
    testElemAbs();
    
    //SquareMatrix<int>
    testSquareMatrixIntBasics();
    testSquareMatrixIntOperations();
    testSquareMatrixIntElementary();
    
    //SquareMatrix<Complex>
    testSquareMatrixComplexBasics();
    testSquareMatrixComplexOperations();
    testSquareMatrixComplexElementary();
    
    //Исключения
    testSquareMatrixExceptions();


    //СЛАУ
    testSLAU_Double();
    testSLAU_Int();
    testSLAU_Complex();
    testSLAU_Exceptions();
    testSLAU_Performance_100x100();
    
    printSummary();
    return (totalFailed == 0) ? 0 : 1;
}