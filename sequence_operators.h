#ifndef SEQUENCE_OPERATORS_H
#define SEQUENCE_OPERATORS_H

#include <ostream>
#include "sequence.h"
#include "iterator.h" 
#include "bit_sequence.h"

//Выводит последовательность в поток
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>& seq)
{
    os << "[";
    IEnumerator<T>* it = seq.GetEnumerator();
    bool first = true;
    while (it->MoveNext())
    {
        if (!first) os << ", ";
        os << it->Current();
        first = false;
    }
    it->Release();
    os << "]";
    return os;
}

//Перегрузка для указателя
template <class T>
std::ostream& operator<<(std::ostream& os, const Sequence<T>* seq)
{
    if (!seq)
        return os << "[null]";
    return os << *seq; 
}

//Сравнивает две последовательности поэлементно
template <class T>
bool operator==(const Sequence<T>& lhs, const Sequence<T>& rhs)
{
    if (lhs.GetLength() != rhs.GetLength())
        return false;
    for (int i = 0; i < lhs.GetLength(); ++i)
    {
        if (!(lhs.Get(i) == rhs.Get(i)))
            return false;
    }
    return true;
}

template <class T>
bool operator!=(const Sequence<T>& lhs, const Sequence<T>& rhs)
{
    return !(lhs == rhs);
}

#endif // SEQUENCE_OPERATORS_H