#ifndef LIST_SEQUENCE_H
#define LIST_SEQUENCE_H

#include "sequence.h"
#include "linked_list.h"

template <class T>
class ListSequenceBase : public Sequence<T> 
{
private:
    template <class U>
    friend class ListSequenceBuilder;

protected:
    LinkedList<T>* list;

    virtual Sequence<T>* instance(const T* arr, int count) const = 0;
    
    //Вспом-ный метод для преобразования списка в массив
    T* ListToArray() const 
    {
        T* arr = new T[GetLength()];
        for (int i = 0; i < GetLength(); ++i) 
        {
            arr[i] = list->Get(i);
        }
        return arr;
    }

    Sequence<T>* buildFromArray(T* tempArray, int count) const
    {
        Sequence<T>* result = nullptr;
        try 
        {
            result = instance(tempArray, count);
        } catch (...) 
        {
            delete[] tempArray;
            throw;
        }
        delete[] tempArray;
        return result;
    }

public:
    using Sequence<T>::GetFirst;
    using Sequence<T>::GetLast;

    //Конструкторы
    ListSequenceBase(const T* items, int count) : list(new LinkedList<T>(items, count)) {}
    ListSequenceBase() : list(new LinkedList<T>()) {}
    ListSequenceBase(const ListSequenceBase<T>& other) : list(new LinkedList<T>(*other.list)) {}

    ListSequenceBase<T>& operator=(const ListSequenceBase<T>& other) 
    {
        if (this != &other) 
        {
            delete list;
            list = new LinkedList<T>(*other.list); 
        }
        return *this;
    }

    //Деструктор
    ~ListSequenceBase() override 
    {
        delete list;
    }

    const T& GetFirst() const override 
    {
        return list->GetFirst();
    }

    const T& GetLast() const override 
    {
        return list->GetLast();
    }

    const T& Get(int index) const override 
    {
        return list->Get(index);
    }

    int GetLength() const override 
    {
        return list->GetLength();
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override 
    {
        if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        {
            throw IndexOutOfRangeException();
        }

        int subLength = endIndex - startIndex + 1;
        T* tempArray = new T[subLength];

        for (int i = startIndex; i <= endIndex; ++i)
        {
            tempArray[i - startIndex] = this->Get(i);
        }
        return buildFromArray(tempArray, subLength);
    }

    //Базовые операции 
    Sequence<T>* Append(const T& item) override 
    {
        return InsertAt(item, GetLength());
    }

    Sequence<T>* Prepend(const T& item) override 
    {
        return InsertAt(item, 0);
    }

    Sequence<T>* InsertAt(const T& item, int index) override 
    {
        if (index < 0 || index > GetLength())
        {
            throw IndexOutOfRangeException();
        }

        const int oldLen = GetLength();
        T* tempArray = ListToArray();
        T* newArray = nullptr;
        try 
        {
            newArray = new T[oldLen + 1];
        } catch (...) 
        {
            delete[] tempArray;
            throw;
        }

        for (int i = 0; i < index; ++i)
        {            
            newArray[i] = tempArray[i];
        }

        newArray[index] = item;
        for (int i = index; i < oldLen; ++i)
        {
            newArray[i + 1] = tempArray[i];
        }
        delete[] tempArray;

        return buildFromArray(newArray, oldLen + 1);
    }

    Sequence<T>* RemoveAt(int index) override 
    {
        if (index < 0 || index >= GetLength())
        {
            throw IndexOutOfRangeException();
        }

        const int oldLen = GetLength();
        T* tempArray = ListToArray();

        T* newArray = nullptr;
        try 
        {
            newArray = new T[oldLen - 1];
        } catch (...) 
        {
            delete[] tempArray;
            throw;
        }
        for (int i = 0, j = 0; i < oldLen; ++i)
        {
            if (i != index) newArray[j++] = tempArray[i];
        }
        delete[] tempArray;

        return buildFromArray(newArray, oldLen - 1);
    }

    Sequence<T>* Concat(Sequence<T>* other) override 
    {
        if (!other)
        {
            throw InvalidArgumentException();
        }
        const int len = GetLength();
        const int otherLen = other->GetLength();

        T* tempArray = ListToArray();
        T* newArray = nullptr;
        try 
        {
            newArray = new T[len + otherLen];
        } catch (...) 
        {
            delete[] tempArray;
            throw;
        }
        for (int i = 0; i < len; ++i)
        {
            newArray[i] = tempArray[i];
        }

        for (int i = 0; i < otherLen; ++i)
        {
            newArray[len + i] = other->Get(i);
        }
        delete[] tempArray;

        return buildFromArray(newArray, len + otherLen);
    }
    
    const T& operator[](int index) const 
    {
    return list->Get(index);
    }

    Sequence<T>* Map(T (*f)(const T&)) const override 
    {
        const int len = GetLength();
        if (len == 0) 
        {
            return instance(nullptr, 0);
        }

        T* tempArray = new T[len];
        for (int i = 0; i < len; ++i)
        {
            tempArray[i] = f(this->Get(i));
        }
        return buildFromArray(tempArray, len);
    }

    Sequence<T>* Where(bool (*predicate)(const T&)) const override 
    {
        const int len = GetLength();
        int count = 0;

        for (int i = 0; i < len; ++i)
        {
            if (!predicate || predicate(this->Get(i))) ++count;
        }

        if (count == 0)
        {
            return instance(nullptr, 0);
        }

        T* tempArray = new T[count];
        int j = 0;
        for (int i = 0; i < len; ++i)
        {
            const T& item = this->Get(i);
            if (!predicate || predicate(item))
            {
                tempArray[j++] = item;
            }
        }
        return buildFromArray(tempArray, count);
    }

    IEnumerator<T>* GetEnumerator() const override 
    {
    return list->GetEnumerator();
    }
};

//Mutable версия
template <class T>
class MutableListSequence : public ListSequenceBase<T> 
{
protected:
    using ListSequenceBase<T>::list;
    Sequence<T>* instance(const T* arr, int count) const override 
    {
        return new MutableListSequence<T>(arr, count);
    }

public:
    using ListSequenceBase<T>::GetLength;
    //Конструкторы
    MutableListSequence(const T* items, int count) : ListSequenceBase<T>(items, count) {}
    MutableListSequence() : ListSequenceBase<T>() {}
    MutableListSequence(const MutableListSequence<T>& other) : ListSequenceBase<T>(other) {}

    Sequence<T>* InsertAt(const T& item, int index) override 
    {
        if (index < 0 || index > GetLength()) 
        {
            throw IndexOutOfRangeException();
        }
        list->InsertAt(item, index);
        return this;
    }

    Sequence<T>* RemoveAt(int index) override 
    {
        if (index < 0 || index >= GetLength()) 
        {
            throw IndexOutOfRangeException();
        }
        list->RemoveAt(index);
        return this;
    }

    Sequence<T>* Concat(Sequence<T>* other) override 
    {
        if (!other)
        {
            throw InvalidArgumentException();
        }
        for (int i = 0; i < other->GetLength(); ++i) 
        {
            list->Append(other->Get(i));
        }
        return this;
    }

    T& operator[](int index) 
    {
    return const_cast<T&>(this->list->Get(index));
    }
};

//Immutable версия
template <class T>
class ImmutableListSequence : public ListSequenceBase<T> 
{
protected:
    Sequence<T>* instance(const T* arr, int count) const override 
    {
        return new ImmutableListSequence<T>(arr, count);
    }

public:
    //Конструкторы
    ImmutableListSequence(const T* items, int count) : ListSequenceBase<T>(items, count) {}
    ImmutableListSequence() : ListSequenceBase<T>() {}
    ImmutableListSequence(const ImmutableListSequence<T>& other) : ListSequenceBase<T>(other) {}
};

#endif // LIST_SEQUENCE_H