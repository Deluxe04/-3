#ifndef ADAPTIVE_SEQUENCE_H
#define ADAPTIVE_SEQUENCE_H

#include "array_sequence.h"
#include "list_sequence.h"

template <class T>
class AdaptiveSequence : public Sequence<T>
{
private:
    Sequence<T>* impl;
    mutable int getCount;
    mutable int insertCount;

    int threshold; 
    
    template <typename NewSequenceType>
    void SwitchTo()
    {
        //Если уже нужный тип, то не пересоз-м
        if (dynamic_cast<NewSequenceType*>(impl))
        {
            return;
        }

        int len = impl->GetLength();
        T* temp = new T[len];

        for (int i = 0; i < len; ++i)
        {
            temp[i] = impl->Get(i);
        }

        Sequence<T>* newImpl = new NewSequenceType(temp, len);
        delete[] temp;

        delete impl;
        impl = newImpl;
    }

    void SwitchToArray()
    {
        SwitchTo<MutableArraySequence<T>>();
    }

    void SwitchToList()
    {
        SwitchTo<MutableListSequence<T>>();
    }
    
    void Rebalance()
    {
        int total = getCount + insertCount;
        if (total == 0) 
        {
            return;
        }

        if (getCount * 100 / total > 70)
        {
            SwitchToArray();
        }
        else if (insertCount * 100 / total > 50)
        {
            SwitchToList();
        }
        
        getCount = 0;
        insertCount = 0;
    }

    //Увеличивает счётчик чтений
    template <typename Func>
    const T& Access(Func func) const
    {
        ++getCount;
        if (getCount > threshold)
        {
            const_cast<AdaptiveSequence<T>*>(this)->Rebalance();
        }

        return func();
    }

public:
    explicit AdaptiveSequence(int t = 100) : impl(new MutableArraySequence<T>()),
        getCount(0), insertCount(0), threshold(t) {}

    AdaptiveSequence(const T* items, int count, int t = 100) : impl(new MutableArraySequence<T>(items, count)),
         getCount(0), insertCount(0), threshold(t){}

    AdaptiveSequence(const AdaptiveSequence<T>& other): getCount(0), insertCount(0), threshold(other.threshold)
    {
        int len = other.impl->GetLength();
        T* temp = new T[len];
        for (int i = 0; i < len; ++i)
        {
            temp[i] = other.impl->Get(i);
        }
        impl = new MutableArraySequence<T>(temp, len);
        delete[] temp;
    }

    ~AdaptiveSequence() override
    {
        delete impl;
    }
    
    // Декомпозиция
    const T& GetFirst() const override
    {
        return Access([this]() -> const T& { return impl->GetFirst(); });
    }
    
    const T& GetLast() const override
    {
        return Access([this]() -> const T& { return impl->GetLast(); });
    }
    
    const T& Get(int index) const override
    {
        return Access([this, index]() -> const T& { return impl->Get(index); });
    }
    
    int GetLength() const override
    {
        return impl->GetLength();
    }
    
    // Операции
   Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override
    {
        if (startIndex < 0 || endIndex >= GetLength() || startIndex > endIndex)
        {
            throw IndexOutOfRangeException();
        }

        int len = endIndex - startIndex + 1;
        T* temp = new T[len];

        for (int i = startIndex; i <= endIndex; ++i)
        {
            temp[i - startIndex] = impl->Get(i);
        }

        AdaptiveSequence<T>* result = new AdaptiveSequence<T>(temp, len, threshold);
        delete[] temp;
        return result;
    }
    
    Sequence<T>* Append(const T& item) override
    {
        ++insertCount;
        impl->Append(item);
        if (insertCount > threshold)
        {
            Rebalance();
        }
        return this;
    }

    Sequence<T>* Prepend(const T& item) override
    {
        ++insertCount;
        impl->Prepend(item);
        if (insertCount > threshold)
        {
            Rebalance();
        }
        return this;
    }

    Sequence<T>* InsertAt(const T& item, int index) override
    {
        ++insertCount;
        impl->InsertAt(item, index);
        if (insertCount > threshold)
        {
            Rebalance();
        }
        return this;
    }

    Sequence<T>* RemoveAt(int index) override
    {
        ++insertCount;
        impl->RemoveAt(index);
        if (insertCount > threshold)
        {
            Rebalance();
        }
        return this;
    }
    
    Sequence<T>* Concat(Sequence<T>* other) override
    {
        int otherLen = other->GetLength();
        for (int i = 0; i < otherLen; ++i)
        {
            ++insertCount;
            impl->Append(other->Get(i));
            if (insertCount > threshold)
            {
                Rebalance();
            }
        }
        return this;
    }

    IEnumerator<T>* GetEnumerator() const override
    {
        return impl->GetEnumerator();
    }

    //Map-Reduce
    Sequence<T>* Map(T (*f)(const T&)) const override
    {
        return impl->Map(f);
    }
    
    Sequence<T>* Where(bool (*predicate)(const T&)) const override
    {
        return impl->Where(predicate);
    }
    
    T Reduce(T (*func)(T, const T&), T initial) const override
    {
        return impl->Reduce(func, initial);
    }
    
    Option<T> GetFirst(bool (*predicate)(const T&)) const override
    {
        return impl->GetFirst(predicate);
    }

    Option<T> GetLast(bool (*predicate)(const T&)) const override
    {
        return impl->GetLast(predicate);
    }
    
    //Вручную настраивает порог переключения
    void SetThreshold(int t)
    {
        threshold = t;
    }

    //Для отладки и тестов
    const char* GetImplType() const
    {
        if (dynamic_cast<MutableArraySequence<T>*>(impl))
        {
            return "ArraySequence";
        }
        if (dynamic_cast<MutableListSequence<T>*>(impl))
        {
            return "ListSequence";
        }
        return "Unknown";
    }
};

#endif // ADAPTIVE_SEQUENCE_H