#ifndef STATIC_LIST_H
#define STATIC_LIST_H

#include <stdint.h>
#include <stdlib.h>

template <typename T>
struct StaticList
{
    /// @brief Allocates a new list with a fixed size
    /// @param size The fixed size of the list
    StaticList(int size)
    {
        this->count = 0;
        this->size = size;
        this->array = (T *)malloc(sizeof(T) * size);
    }

    /// @brief Frees the list
    ~StaticList()
    {
        free(this->array);
    }

    /// @brief Adds an item to the list
    /// @param item The item to add
    /// @return Returns the index of the new element
    int Add(T item)
    {
        if (count < size) // if there is space left
        {
            array[count] = item;
            return count++; // return index and add count
        }

        return -1;
    }

    /// @brief Returns the current number of items in the list
    int Count()
    {
        return count;
    }

    /// @brief Finds an item in the list
    /// @param item The item to match with
    /// @return Returns the index of the item or -1 if it does not exist
    int Find(T item)
    {
        for (int i = 0; i < count; i++)
        {
            if (array[i] == item)
                return i;
        }
        return -1;
    }

    /// @brief Removes an element from the list
    /// @param index The index at which the element to remove is located
    /// @return Returns the item if it was successfully removed or null if it failed
    T *Remove(int index)
    {
        if (count > 0)
        {
            T item = array[index];
            count--;
            // shift all items after index by one
            for (int i = index; i < count; i++)
            {
                array[i] = array[i + 1];
            }
            return &item;
        }
        return nullptr;
    }

    /// @brief Returns an element at an index
    T &Get(int index)
    {
        return array[index];
    }

private:
    int count;
    int size;
    T *array;
};

#endif