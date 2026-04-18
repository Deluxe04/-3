#ifndef UTILS_H
#define UTILS_H

#include <ostream>
#include "array_sequence.h"

template <typename T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq)
{
    os << "[";
    for (int i = 0; i < seq.GetLength(); ++i)
    {
        if (i > 0) os << ", ";
        os << seq.Get(i);
    }
    os << "]";
    return os;
}

//Специализация для string
template <>
inline std::ostream& operator<<(std::ostream& os, const Sequence<std::string>& seq)
{
    os << "[";
    for (int i = 0; i < seq.GetLength(); ++i)
    {
        if (i > 0) os << ", ";
        os << "\"" << seq.Get(i) << "\"";
    }
    os << "]";
    return os;
}

template<typename T>
Sequence<T>* operator+(const Sequence<T>& lhs, const Sequence<T>& rhs)
{
    int lLen = lhs.GetLength();
    int rLen = rhs.GetLength();

    T* arr = new T[lLen + rLen];

    for (int i = 0; i < lLen; ++i)
    {
        arr[i] = lhs.Get(i);
    }
    for (int i = 0; i < rLen; ++i)
    {
        arr[lLen + i] = rhs.Get(i);
    }

    Sequence<T>* result = new MutableArraySequence<T>(arr, lLen + rLen);
    delete[] arr;
    return result;
}


inline Sequence<int>* Range(int l, int h)
{
    int count = (h >= l) ? (h - l + 1) : 0;
    int* arr = new int[count];
    for (int i = 0; i < count; ++i)
    {
        arr[i] = l + i;
    } 
    MutableArraySequence<int>* result = new MutableArraySequence<int>(arr, count);
    delete[] arr;
    return result;
}

//поиск первого вхождения sub в seq (-1, если не найдено)
template<typename T>
int findSubsequence(const Sequence<T>* seq, const Sequence<T>* sub)
{
    if (!seq || !sub) 
    {
        return -1;
    }

    int n = seq->GetLength();
    int m = sub->GetLength();

    if (m == 0) 
    {
        return 0; 
    } 
    if (m > n)  
    {
        return -1;
    }

    for (int i = 0; i <= n - m; ++i)
    {
        bool match = true;
        for (int j = 0; j < m && match; ++j)
        {
            if (!(seq->Get(i + j) == sub->Get(j)))
            {
                match = false;
            }
        }
        if (match) 
        {
            return i;
        }   
    }
    return -1;
}

#endif // UTILS_H