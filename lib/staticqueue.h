#ifndef STATIC_QUEUE_H
#define STATIC_QUEUE_H

#include <stdint.h>
#include <stdlib.h>

template <typename T>
struct StaticQueue
{
    /// @brief Allocates a new queue with a fixed size
    /// @param size The fixed size of the queue
    StaticQueue(int size)
    {
        this->head = 0;
        this->tail = 0;
        this->size = size;
        this->array = (T *)malloc(sizeof(T) * size);
    }

    /// @brief Frees the queue
    ~StaticQueue()
    {
        free(this->array);
    }

    /// @brief Enqueues an item to the queue
    /// @param item The item to enqueue
    /// @return Returns true on success
    bool Enqueue(T item)
    {
        if (tail == head - 1 || (tail == size - 1 && head == 0))
            return false;

        array[tail++] = item;
        if (tail >= size)
        {
            tail = 0;
        }

        return true;
    }

    /// @brief Returns the current number of items in the queue (distance between head and tail)
    int Count()
    {
        return abs(head - tail);
    }

    /// @brief Removes an element from the queue
    /// @return Returns the item if it was successfully removed or null if it failed
    T Dequeue()
    {
        if (Count() == 0)
            return nullptr;

        T ret = array[head++];
        if (head >= size)
        {
            head = 0;
        }
        return ret;
    }

    /// @brief Returns the next item
    T &Peek()
    {
        if (Count() == 0)
            return nullptr;

        return &array[head];
    }

private:
    int head;
    int tail;
    int size;
    T *array;
};

#endif