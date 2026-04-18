#ifndef BUILDER_H
#define BUILDER_H

#include "array_sequence.h"
#include "list_sequence.h"

template <class T>
class ArraySequenceBuilder 
{
private:
    DynamicArray<T>* data;

    T* ToRawArray(int& outSize) const
    {
        outSize = data->GetSize();
        T* arr = new T[outSize];
        for (int i = 0; i < outSize; ++i)
        {
            arr[i] = data->Get(i);
        }
        return arr;
    }

public:
    ArraySequenceBuilder() : data(new DynamicArray<T>(0)) {}
    
    ~ArraySequenceBuilder() 
    { 
        delete data; 
    }
    
    //Запрет копирования
    ArraySequenceBuilder(const ArraySequenceBuilder&) = delete;
    ArraySequenceBuilder& operator=(const ArraySequenceBuilder&) = delete;

    ArraySequenceBuilder& Add(const T& item) 
    {
        data->Resize(data->GetSize() + 1);
        data->Set(data->GetSize() - 1, item);
        return *this;
    }

    ArraySequenceBuilder& AddRange(const T* items, int count) 
    {
        int start = data->GetSize();
        data->Resize(start + count);
        for (int i = 0; i < count; ++i) 
        {
            data->Set(start + i, items[i]);
        }
        return *this;
    }

    MutableArraySequence<T>* BuildMutable() 
    {
        int size;
        T* arr = ToRawArray(size);
        auto* result = new MutableArraySequence<T>(arr, size);
        delete[] arr;
        return result;
    }
    
    ImmutableArraySequence<T>* BuildImmutable() 
    {
        int size;
        T* arr = ToRawArray(size);
        auto* result = new ImmutableArraySequence<T>(arr, size);
        delete[] arr;
        return result;
    }
    
    void Clear() 
    {
        delete data;
        data = new DynamicArray<T>(0);
    }
};

//Билдер для ListSequence
template <class T>
class ListSequenceBuilder 
{
private:
    LinkedList<T>* data;

    T* ToRawArray(int& outSize) const
    {
        outSize = data->GetLength();
        T* arr = new T[outSize];
        for (int i = 0; i < outSize; ++i)
        {
            arr[i] = data->Get(i);
        }
        return arr;
    }

public:
    ListSequenceBuilder() : data(new LinkedList<T>()) {}
    
    ~ListSequenceBuilder() 
    { 
        delete data; 
    }

    //Запрет копирования
    ListSequenceBuilder(const ListSequenceBuilder&) = delete;
    ListSequenceBuilder& operator=(const ListSequenceBuilder&) = delete;

    ListSequenceBuilder& Add(const T& item) 
    {
        data->Append(item);  
        return *this;
    }

    ListSequenceBuilder& AddRange(const T* items, int count) 
    {
        for (int i = 0; i < count; ++i) 
        {
            data->Append(items[i]);
        }
        return *this;
    }
    
    MutableListSequence<T>* BuildMutable() 
    {
        int size;
        T* arr = ToRawArray(size);
        auto* result = new MutableListSequence<T>(arr, size);
        delete[] arr;
        return result;
    }
    
    ImmutableListSequence<T>* BuildImmutable()
    {
        int size;
        T* arr = ToRawArray(size);
        auto* result = new ImmutableListSequence<T>(arr, size);
        delete[] arr;
        return result;
    }

    void Clear() 
    {
        delete data;
        data = new LinkedList<T>();
    }
};

#endif // BUILDER_H