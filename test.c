#include <assert.h>
#include <stdio.h>

#include "vector.h"

#define START_TEST(test) printf("%s Test: ", test)
#define TEST_PASS printf("PASS\n");

void pop_back_test(vector *v)
{
    vector tmp = vect_init(10, sizeof(float));
    /* Assuming vector of ints */
    size_t curr = vect_size(v);
    int element = *(int *) vect_back(v);
    int prev = *(int *) vect_at(v, curr - 2);

    START_TEST("Pop Back");
    assert(vect_pop_back(&tmp) == 1);
    assert(vect_pop_back(v) == 0);
    assert(vect_pop_back(NULL) == 1);

    assert((curr - 1) == vect_size(v));
    assert(element != *(int *) vect_back(v));
    assert(prev == *(int *) vect_back(v));

    vect_free(&tmp);
    TEST_PASS;
}

void insert_test(vector *v, int val, size_t index)
{
    /* Assumes vector of ints */
    size_t size = vect_size(v);
    int curr = *(int *) vect_at(v, index);

    START_TEST("Insert");
    assert(vect_insert(v, (const void *) &val, index) == 0);

    assert((size + 1) == vect_size(v));
    assert(curr != *(int *) vect_at(v, index));
    assert(val == *(int *) vect_at(v, index));
    assert(vect_insert(NULL, NULL, 0) == 1);
    TEST_PASS;
}

void resize_test(void)
{
    vector v = vect_init(4, sizeof(int));
    vector v2 = vect_init(10, sizeof(double));
    size_t size = 10;
    int x;
    int final_arr[] = { 0, 3, 4 };

    START_TEST("Resize");

    assert(vect_resize(NULL, size) == 1);

    vect_resize(&v, size);
    assert(vect_capacity(&v) == size);
    assert(vect_empty(&v) == 1);

    vect_resize(&v, 0);
    vect_resize(&v, 3);
    for (x = 0; x < (int) vect_capacity(&v); x++)
        vect_push_back(&v, &x);

    vect_resize(&v, 1);
    assert(vect_size(&v) == 1);

    vect_erase(&v, 0, size);
    vect_resize(&v, size);
    for (x = 0; x < (int) size / 2; x++)
        vect_push_back(&v, &x);

    assert(vect_shrink_to_fit(&v2) == 0);
    assert(vect_shrink_to_fit(NULL) == 1);
    assert(vect_data(&v2) == NULL);
    assert(vect_shrink_to_fit(&v) == 0);
    assert(vect_len(&v) == vect_capacity(&v));
    assert(vect_len(&v) == (size / 2));

    vect_erase(&v, 1, 3);
    for (x = 0; x < (int) vect_len(&v); x++)
        assert(final_arr[x] == *(int *) vect_at(&v, x));

    vect_free(&v);
    vect_free(&v2);
    TEST_PASS;
}

void clear_test(void)
{
    vector v;
    int a = 10, b = 5;

    printf("Clear Test: ");
    vect_clear(NULL);
    v = vect_init(0, sizeof(int));
    vect_push_back(&v, &a);
    vect_clear(&v);
    vect_push_back(&v, &b);
    assert(5 == *(int *) vect_at(&v, 0));
    vect_free(&v);
    TEST_PASS;
}

void data_test(void)
{
    int x;
    int *arr;
    vector v = vect_init(10, sizeof(int));

    printf("Data Test: ");
    for (x = 0; x < 10; x++)
        vect_push_back(&v, &x);

    arr = (int *) vect_data(&v);
    assert(vect_data(NULL) == NULL);

    for (x = 0; x < (int) vect_size(&v); x++)
        assert(arr[x] == *(int *) vect_at(&v, x));

    vect_free(&v);
    TEST_PASS;
}

void swap_test(void)
{
    vector v1, v2, v3;
    size_t v1_cap = 6, v1_len = 5, v2_cap = 4, v2_len = 3, x;

    printf("Swap Test: ");
    v1 = vect_init(v1_cap, sizeof(int));
    v2 = vect_init(v2_cap, sizeof(int));
    v3 = vect_init(8, sizeof(double));

    for (x = 0; x < v1_len; x++)
    {
        int tmp = x * 2;
        vect_push_back(&v1, &x);
        if (x < v2_len)
            vect_push_back(&v2, &tmp);
    }
    assert(vect_size(&v1) == v1_len);
    assert(vect_size(&v2) == v2_len);

    assert(vect_swap(NULL, NULL) == 1);
    assert(vect_swap(NULL, &v1) == 1);
    assert(vect_swap(&v2, NULL) == 1);
    assert(vect_swap(&v1, &v3) == 1);
    assert(vect_swap(&v1, &v2) == 0);

    assert(vect_size(&v1) == v2_len);
    assert(vect_size(&v2) == v1_len);
    assert(vect_capacity(&v1) == v2_cap);
    assert(vect_capacity(&v2) == v1_cap);

    vect_free(&v1);
    vect_free(&v2);
    vect_free(&v3);

    TEST_PASS;
}

void reserve_test(void)
{
    vector v;
    size_t og = 10, low = 4, high = 15;

    printf("Reserve Test: ");
    v = vect_init(og, sizeof(int));
    assert(vect_reserve(&v, low) == 0);
    assert(vect_capacity(&v) >= low);
    assert(vect_reserve(&v, high) == 0);
    assert(vect_capacity(&v) >= high);
    assert(vect_reserve(NULL, 0) == 1);

    vect_free(&v);
    TEST_PASS;
}

void assign_test(void)
{
    const size_t size = 100;
    int arr[100], x;
    vector v = vect_init(4, sizeof(double));

    printf("Assign Test: ");
    for (x = 0; x < (int) size; x++)
        arr[x] = x;

    assert(vect_assign(NULL, arr, size, sizeof(int)) == 1);
    assert(vect_assign(&v, NULL, size, sizeof(int)) == 1);
    assert(vect_assign(&v, arr, 0, sizeof(int)) == 1);
    assert(vect_assign(&v, arr, size, 0) == 1);
    assert(vect_assign(&v, arr, size, sizeof(int)) == 0);

    for (x = 0; x < (int) size; x++)
        assert(arr[x] == *(int *) vect_at(&v, x));

    vect_free(&v);
    TEST_PASS;
}

void initializer_test(void)
{
    int x = 10;
    vector n = VECT_INITIALIZER;

    START_TEST("Initializer");
    assert(vect_push_back(&n, &x) == 1);
    assert(vect_pop_back(&n) == 1);
    assert(vect_insert(&n, &x, 10) == 1);
    assert(vect_front(&n) == NULL);
    assert(vect_back(&n) == NULL);
    assert(vect_data(&n) == NULL);
    assert(vect_erase(&n, 2, 5) == 0);
    assert(vect_clear(&n) == 0);
    TEST_PASS;
}

int main(void)
{
    int size = 4, x;
    int final_vec[] = { 0, 1, 2, 25, 3, 4, 5, 89, 23 };
    vector v = vect_init(size, sizeof(int));
    vector d = vect_init(size, sizeof(double));

    resize_test();
    clear_test();
    data_test();
    swap_test();
    reserve_test();
    assign_test();
    initializer_test();

    for (x = 0; x < size + 3; x++)
    {
        double tmp = x * 1.25;
        vect_push_back(&v, &x);
        vect_push_back(&d, &tmp);
        assert(x == *(int *) vect_at(&v, x));
    }

    assert(0 == *(int *) vect_front(&v));
    assert((vect_size(&d) - 1) * 1.25 == *(double *) vect_back(&d));

    pop_back_test(&v);
    insert_test(&v, 25, 3);
    insert_test(&v, 89, 10); /* Should call push_back */
    insert_test(&v, 23, -1); /* Should wrap around and call push_back */

    START_TEST("Final");
    assert((sizeof(final_vec) / sizeof(int)) == vect_size(&v));
    for (x = 0; x < (int) vect_size(&v); x++)
        assert(final_vec[x] == *(int *) vect_at(&v, x));

    vect_free(&v);
    vect_free(&d);
    TEST_PASS;

    return 0;
}
