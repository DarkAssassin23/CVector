#ifndef CVECTOR_H
#define CVECTOR_H

#include <stddef.h>

/**
 * @struct vector
 * @brief Components that make up the vector data structure
 */
typedef struct
{
    void *arr;     /* The data housed in the vector */
    size_t len;    /* The number of elements currently in the vector */
    size_t size;   /* Size, in bytes, allocated to the vector */
    size_t t_size; /* Size, in bytes, of the type of the vector */
} vector;

/**
 * @brief Initalize a vector with a given size and return it
 * @param size The number of elements to initially create the vector with
 * @param t_size The size of the type being used in the vector
 * @return The newly created vector
 */
vector vect_init(size_t size, size_t t_size);

/**
 * @brief Assign an array of values to a vector
 * @param v The vector to assign values to
 * @param arr The array to assign
 * @param size The size of the array being assigned
 * @param t_size The size of each individual element in the new array
 * @return 0 on success
 * @attention All elements in the provided vector will be deleted
 */
int vect_assign(vector *v, void *arr, size_t size, size_t t_size);

/**
 * @brief Resize the given vector to the `new_size`
 * @param v The vector to be resized
 * @param new_size The new size to resize the vector to
 * @return 0 on success
 */
int vect_resize(vector *v, size_t new_size);

/**
 * @brief Resize the capacity of the vector to be the size of the length
 * @param v The vector to shrink
 * @param 0 on success
 */
int vect_shrink_to_fit(vector *v);

/**
 * @brief Ensure the vector is at least as big as `new_cap`
 * @param v The vector to reserve capacity in
 * @param new_cap The new capacity
 * @return 0 on success
 */
int vect_reserve(vector *v, size_t new_cap);

/**
 * @brief Return the size of the vector
 * @param v The vector to get the size of
 * @return The size of the vector
 * @note Same as vect_len()
 * @see vect_len()
 */
size_t vect_size(vector *v);

/**
 * @brief Return the length of the vector
 * @param v The vector to get the length of
 * @return The length of the vector
 * @note Same as vect_size()
 * @see vect_size()
 */
size_t vect_len(vector *v);

/**
 * @brief Return if the vector is empty
 * @param v The vector to check
 * @return If the given vector is empty
 */
int vect_empty(vector *v);

/**
 * @brief Return the current max capacity of the vector
 * @param v The vector to get the capacity of
 * @return The current capacity of the vector
 */
size_t vect_capacity(vector *v);

/**
 * @brief Add an element to the end of the vector
 * @param v The vector to add the element to
 * @param val The value to insert into the vector
 * @return 0 on success
 */
int vect_push_back(vector *v, const void *val);

/**
 * @brief Remove the last element
 * @param v The vector to remove the last element of
 * @return 0 on success
 */
int vect_pop_back(vector *v);

/**
 * @brief Swap the contents of v1, with the contents of v2
 * @param v1 First vector to swap
 * @param v2 Second vector to swap
 * @return 0 On success
 */
int vect_swap(vector *v1, vector *v2);

/**
 * @brief Insert an element into the vector
 * @param v The vector to insert the element into
 * @param val The value to insert
 * @param index The index in the vector to insert the element
 * @return 0 on success
 */
int vect_insert(vector *v, const void *val, size_t index);

/**
 * @brief Return the element at the given index
 * @param v The vector to get the element from
 * @param index The index of the element to get
 * @return Pointer to the element
 * @note If the element does not exist, NULL is returned
 */
void *vect_at(vector *v, size_t index);

/**
 * @brief Return the pointer to the array of elements
 * @param v The vector to get the array from
 * @return The pointer to the array in the vector
 */
void *vect_data(vector *v);

/**
 * @brief Return the first element in the vector
 * @param v The vector to get the element from
 * @return Pointer to the element
 * @note If the vector is empty, NULL is returned
 */
void *vect_front(vector *v);

/**
 * @brief Return the last element in the vector
 * @param v The vector to get the element from
 * @return Pointer to the element
 * @note If the vector is empty, NULL is returned
 */
void *vect_back(vector *v);

/**
 * @brief Remove a range of elements from the vector
 * @param v The vector to remove elements from
 * @param start The starting index
 * @param end The ending index
 * @note The range is [start, end), meaning the elements are removed starting
 * at, and including, start and going to, but not including, end
 */
void vect_erase(vector *v, size_t start, size_t end);

/**
 * @brief Remove all elements from the vector
 * @param v The vector to clear the elements of
 */
void vect_clear(vector *v);

/**
 * @brief Free the memory allocated by the vector
 * @param v The vector to free the memory of
 */
void vect_free(vector *v);

#endif /* CVECTOR_H */
