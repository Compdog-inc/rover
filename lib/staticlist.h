#ifndef STATIC_LIST_H
#define STATIC_LIST_H

#include <stdint.h>
#include <stdlib.h>

template <typename T>
struct StaticList
{
    /// @brief Allocates a new list with a fixed size
    /// @param size The fixed size of the list
    StaticList(uint8_t size)
    {
        this->count = 0;
        this->size = size;
        this->array = (T **)malloc(sizeof(T *) * size);
    }

    /// @brief Frees the list
    ~StaticList()
    {
        free(this->array);
    }

    /// @brief Adds an item to the list
    /// @param item The item to add
    /// @return Returns the index of the new element
    uint8_t Add(T *item)
    {
        if (count < size) // if there is space left
        {
            array[count] = item;
            return count++; // return index and add count
        }

        return -1;
    }

    /// @brief Returns the current number of items in the list
    uint8_t Count()
    {
        return count;
    }

    /// @brief Finds an item in the list
    /// @param item The item to match with
    /// @return Returns the index of the item
    uint8_t Find(T *item)
    {
        for (uint8_t i = 0; i < count; i++)
        {
            if (array[i] == item)
                return i;
        }
        return -1;
    }

    /// @brief Removes an element from the list
    /// @param index The index at which the element to remove is located
    /// @return Returns the item if it was successfully removed or nullptr if it failed
    T *Remove(uint8_t index)
    {
        if (count > 0)
        {
            T *item = array[index];
            count--;
            // shift all items after index by one
            for (uint8_t i = index; i < count; i++)
            {
                array[i] = array[i + 1];
            }
            return item;
        }
        return nullptr;
    }

    /// @brief Returns an element at an index
    T *Get(uint8_t index)
    {
        return array[index];
    }

private:
    uint8_t count;
    uint8_t size;
    T **array;
};

#endif