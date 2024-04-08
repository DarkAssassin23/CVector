#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define MIN(a, b) ((a < b) ? (a) : (b))
#define MAX(a, b) ((a > b) ? (a) : (b))

/**
 * @brief Resize or initalize the vector if it is NULL
 * @param v The vector to resize
 * @return 0 on success 1 on failure
 */
static int resize(vector *v)
{
    size_t new_size = MAX(v->size, 1) * 2;
    void *tmp = NULL;

    if (v->t_size == 0)
        return 1;

    /* Handle uninitalized vector */
    if (v->arr == NULL)
    {
        v->arr = (void *) calloc(new_size, v->t_size);
        if (v->arr == NULL)
        {
            fprintf(stderr, "Error: Calloc failed\n");
            return 1;
        }
        v->size = new_size;
        return 0;
    }

    /* Check if we need to resize */
    if (v->len < v->size)
        return 0;

    /* Resize the vector */
    tmp = (void *) realloc(v->arr, new_size * v->t_size);
    if (tmp == NULL)
    {
        fprintf(stderr, "Error: Realloc failed\n");
        return 1;
    }
    v->arr = tmp;
    v->size = new_size;

    return 0;
}

/**
 * @brief Check if the vector pointer is valid
 * @param v The vector to check
 * @return True (1) if the vector is valid
 */
static int valid_vect(vector *v)
{
    return (v != NULL);
}

/**
 * @brief Default return value if vect_init fails
 * @return Empty vector
 */
static vector vector_error(void)
{
    vector v;
    memset(&v, 0, sizeof(vector));
    return v;
}

vector vect_init(size_t size, size_t t_size)
{
    vector v;

    if (t_size == 0)
        return vector_error();

    v.len = 0;
    v.size = size;
    v.t_size = t_size;
    v.arr = NULL;

    if (size == 0)
        return v;

    v.arr = (void *) calloc(size, t_size);
    if (v.arr == NULL)
    {
        fprintf(stderr, "Error: Calloc failed\n");
        return vector_error();
    }

    return v;
}

int vect_assign(vector *v, void *arr, size_t size, size_t t_size)
{
    if (!valid_vect(v) || arr == NULL || size == 0 || t_size == 0)
        return 1;

    vect_free(v);
    v->arr = (void *) malloc(size * t_size);
    if (v->arr == NULL)
    {
        fprintf(stderr, "Error: Malloc failed\n");
        return 1;
    }

    memcpy(v->arr, arr, size * t_size);
    v->size = size;
    v->len = size;
    v->t_size = t_size;
    return 0;
}

int vect_resize(vector *v, size_t new_size)
{
    void *tmp = NULL;

    if (!valid_vect(v))
        return 1;

    if (v->t_size == 0)
        return 1;

    /* Handle uninitialized vector */
    if (v->arr == NULL && new_size != 0)
    {
        v->arr = (void *) calloc(new_size, v->t_size);
        if (v->arr != NULL)
        {
            v->size = new_size;
            return 0;
        }
        return 1;
    }

    /* Resize the vector */
    if (new_size > 0)
    {
        tmp = (void *) realloc(v->arr, new_size * v->t_size);
        if (tmp == NULL)
        {
            fprintf(stderr, "Error: Realloc failed\n");
            return 1;
        }
        v->arr = tmp;
    }
    else
    {
        if (v->arr != NULL)
        {
            free(v->arr);
            v->arr = NULL;
        }
    }

    v->size = new_size;
    v->len = MIN(v->len, new_size);

    return 0;
}

int vect_shrink_to_fit(vector *v)
{
    if (!valid_vect(v))
        return 1;

    return vect_resize(v, v->len);
}

int vect_reserve(vector *v, size_t new_cap)
{
    if (!valid_vect(v))
        return 1;

    if (v->size < new_cap)
        return vect_resize(v, new_cap);
    return 0;
}

size_t vect_size(vector *v)
{
    return valid_vect(v) ? v->len : 0;
}

size_t vect_len(vector *v)
{
    return valid_vect(v) ? v->len : 0;
}

int vect_empty(vector *v)
{
    return valid_vect(v) ? v->len == 0 : 0;
}

size_t vect_capacity(vector *v)
{
    return valid_vect(v) ? v->size : 0;
}

int vect_push_back(vector *v, const void *val)
{
    if (!valid_vect(v))
        return 1;

    if (resize(v) == 1) /* Something went wrong */
        return 1;

    memcpy((char *) v->arr + (v->len * v->t_size), val, v->t_size);
    v->len++;
    return 0;
}

int vect_pop_back(vector *v)
{
    if (!valid_vect(v))
        return 1;

    if (v->arr != NULL && v->len > 0)
    {
        memset((char *) v->arr + (v->len * v->t_size), 0, v->t_size);
        v->len--;
        return 0;
    }
    return 1;
}

int vect_swap(vector *v1, vector *v2)
{
    void *tmp;
    size_t tmp_s;

    if (!valid_vect(v1) || !valid_vect(v2))
        return 1;
    if (v1->t_size != v2->t_size)
        return 1;

    tmp = v1->arr;
    v1->arr = v2->arr;
    v2->arr = tmp;

    tmp_s = v1->len;
    v1->len = v2->len;
    v2->len = tmp_s;

    tmp_s = v1->size;
    v1->size = v2->size;
    v2->size = tmp_s;

    return 0;
}

int vect_insert(vector *v, const void *val, size_t index)
{
    size_t mv_size;
    void *src, *dest;

    if (!valid_vect(v))
        return 1;

    if (index >= v->len)
        return vect_push_back(v, val);

    if (resize(v) == 1) /* Something wrong happened */
        return 1;

    src = (char *) v->arr + (index * v->t_size);
    dest = (char *) v->arr + ((index + 1) * v->t_size);
    mv_size = v->len - index;

    memmove(dest, src, mv_size * v->t_size);
    memcpy(src, val, v->t_size);

    v->len++;
    return 0;
}

void *vect_at(vector *v, size_t index)
{
    if (!valid_vect(v))
        return NULL;

    if (v->len == 0)
        return NULL;

    return (index >= v->len) ? vect_back(v)
                             : (char *) v->arr + (index * v->t_size);
}

void *vect_data(vector *v)
{
    return valid_vect(v) ? v->arr : NULL;
}

void *vect_front(vector *v)
{
    if (!valid_vect(v))
        return NULL;

    return (v->len == 0) ? NULL : vect_at(v, 0);
}

void *vect_back(vector *v)
{
    if (!valid_vect(v))
        return NULL;

    return (v->len == 0) ? NULL : vect_at(v, v->len - 1);
}

int vect_erase(vector *v, size_t start, size_t end)
{
    size_t mv_size;
    void *src, *dest;

    if (!valid_vect(v))
        return 1;

    if (start > end)
    {
        size_t tmp = start;
        start = end;
        end = tmp;
    }

    if (start >= v->len)
        return 0;

    end = MIN(v->len, end);

    if (end == v->len)
        return vect_resize(v, start);

    src = (char *) v->arr + (end * v->t_size);
    dest = (char *) v->arr + (start * v->t_size);
    mv_size = v->len - end;
    memmove(dest, src, mv_size * v->t_size);

    v->len -= (end - start);
    return 0;
}

int vect_clear(vector *v)
{
    if (!valid_vect(v))
        return 1;

    if (v->arr != NULL)
    {
        free(v->arr);
        v->arr = NULL;
    }
    v->len = 0;
    v->size = 0;
    return 0;
}

void vect_free(vector *v)
{
    vect_clear(v);
    v->t_size = 0;
}
