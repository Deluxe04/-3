#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "exceptions.h"
#include "iterator.h"  

template <class T>
class LinkedListEnumerator;

template <class T>
class LinkedList 
{
public:
    struct Node 
    {
        T data;
        Node* next;
        Node(const T& value, Node* nextNode = nullptr) : data(value), next(nextNode) {}
    };

private:
    Node* head;
    Node* tail;
    int length;

    //Создаёт узел, оборачивает исключения памяти
    Node* createNode(const T& value, Node* next = nullptr)
    {
        Node* node = nullptr;
        try { 
            node = new Node(value, next); 
        } catch (...) 
        { 
            throw MemoryAllocationException(); 
        }
        return node;
    }

    void clear() 
    {
        Node* current = head;
        while (current != nullptr) 
        {
            Node* toDelete = current;
            current = current->next;
            delete toDelete;
        }
        head = tail = nullptr;
        length = 0;
    }

    //Копирует узлы из другого списка
    void copyFrom(const LinkedList<T>& other)
    {
        Node* current = other.head;
        while (current != nullptr)
        {
            Append(current->data);
            current = current->next;
        }
    }

    const Node* getNodeAt(int index) const
    {
        if (index < 0 || index >= length)
        {
            throw IndexOutOfRangeException();
        }

        const Node* current = head;
        for (int i = 0; i < index; ++i)
        {
            current = current->next;
        }
        return current;
    }

    Node** getNodePtrAt(int index)
    {
        if (index < 0 || index > length)
        {
            throw IndexOutOfRangeException();
        }
        Node** current = &head;
        for (int i = 0; i < index; ++i)
        {
            current = &((*current)->next);
        }
        return current;
    }

    Node* getNodeAt(int index)       
    { 
        return const_cast<Node*>(static_cast<const LinkedList<T>&>(*this).getNodeAt(index));
    }

public:
    LinkedList(const T* items, int count) : head(nullptr), tail(nullptr), length(0) 
    {
        if (count < 0) 
        {
            throw InvalidArgumentException();
        }

        if (count > 0 && items == nullptr) 
        {
            throw InvalidArgumentException();
        }
        
        for (int i = 0; i < count; ++i) 
        {
            Append(items[i]);
        }
    }

    LinkedList() : head(nullptr), tail(nullptr), length(0) {}

    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), length(0) 
    {
        copyFrom(other);
    }

    ~LinkedList() 
    {
        clear();
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) 
    {
        if (this != &other) 
        {
            clear();
            copyFrom(other);
        }
        return *this;
    }

    const T& GetFirst() const 
    {
        if (head == nullptr) 
        {
            throw EmptyStructureException();
        }
        return getNodeAt(0)->data;
    }

    T& GetFirst() 
    {
        return const_cast<T&>(static_cast<const LinkedList<T>&>(*this).GetFirst());
    }

    const T& GetLast() const
    {
        if (tail == nullptr) 
        {
            throw EmptyStructureException();
        }
        return tail->data;  
    }

    T& GetLast()
    {
        return const_cast<T&>(static_cast<const LinkedList<T>&>(*this).GetLast());
    }

    const T& Get(int index) const 
    {
        return getNodeAt(index)->data;
    }

    T& Get(int index)
    {
        return const_cast<T&>(static_cast<const LinkedList<T>&>(*this).Get(index));
    }

    LinkedList<T> GetSubList(int startIndex, int endIndex) const
    {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex)
        {
            throw IndexOutOfRangeException();
        }

        LinkedList<T> result;
        const Node* current = getNodeAt(startIndex);
        for (int i = startIndex; i <= endIndex; ++i)
        {
            result.Append(current->data);
            current = current->next;
        }
        return result;
    }

    int GetLength() const 
    {
        return length;
    }

    void Append(const T& item) 
    {
        InsertAt(item, length);  
    }

    void Prepend(const T& item) 
    {
        InsertAt(item, 0);  
    }

    void InsertAt(const T& item, int index)
    {
        if (index < 0 || index > length)
        {
            throw IndexOutOfRangeException();
        }

        Node** current = getNodePtrAt(index);

        Node* newNode = createNode(item, *current);
        *current = newNode;

        if (index == length)
        {
            tail = newNode;
        }
        ++length;
    }

    void RemoveAt(int index)
    {
        if (index < 0 || index >= length)
        {
            throw IndexOutOfRangeException();
        }
        Node** current = getNodePtrAt(index);

        Node* toDelete = *current;
        *current = toDelete->next;  

        if (toDelete->next == nullptr)
        {
            tail = (length > 1) ? getNodeAt(length - 2) : nullptr;
        }

        delete toDelete;
        --length; 
    }

    LinkedList<T> Concat(const LinkedList<T>& list) const 
    {
        LinkedList<T> result(*this);
        Node* current = list.head;
        while (current != nullptr) 
        {
            result.Append(current->data);
            current = current->next;
        }
        return result;
    }

    IEnumerator<T>* GetEnumerator() const 
    {
        return new LinkedListEnumerator<T>(this);
    }
    
    friend class LinkedListEnumerator<T>;
};

//IEnumerator
template <class T>
class LinkedListEnumerator : public IEnumerator<T>
{
private:
    const LinkedList<T>* list; 
    const typename LinkedList<T>::Node* current;       
    bool started;                                
    
public:
    LinkedListEnumerator(const LinkedList<T>* lst) 
        : list(lst), current(nullptr), started(false) {}
    
    bool MoveNext() override 
    {
        if (!started) 
        {
            current = list->head;
            started = true;
        } else if (current) 
        {
            current = current->next;
        }
        return current != nullptr;
    }
    
    const T& Current() const override
    {
        if (!current) 
        {
            throw IndexOutOfRangeException();
        }
        return current->data;
    }
    
    void Reset() override 
    {
        current = nullptr;
        started = false;
    }
};

#endif // LINKED_LIST_H 