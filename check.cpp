#include <stdio.h>

#include "check.h"

void dump(Stack* stk, const char* file, const char* func, const int code_str)
{
    stack_element_t* start_ptr = stk->data;
    size_t size = stk->size;
    size_t capacity = stk->capacity;

    printf("Stack [%x]\n", (size_t)stk);

    printf("called from %s: %d (%s)\n", file, code_str, func);
    printf("name stk born at %s: %d (%s)\n", stk->origin_file, stk->origin_str, stk->origin_func);

    printf("\nleft canary = %x\n", (size_t)start_ptr[0]);
    printf("right canary = %x\n\n", (size_t)start_ptr[capacity + LEFT_CANARY_ADD]);

    printf("array data address %p\n", start_ptr);
    printf("capacity = %d\n", capacity);
    printf("size = %d\n", size);
    printf("array data [%x]\n{\n", (size_t)start_ptr);
    print_stk_elements(start_ptr, capacity, size);
    
}

void print_stk_elements(stack_element_t* start_ptr, size_t capacity, size_t size)
{
    for (size_t i = 0; i < capacity; i++)
    {
        if(i < size)
            printf(" * ");
        else
            printf("   ");
        if (start_ptr[i + LEFT_CANARY_ADD] == poison)
            printf("[%d] = %lg (POISON)\n", i, poison);
        else
            printf("[%d] = %lf\n", i, start_ptr[i + LEFT_CANARY_ADD]);
    }
    printf(" }\n}");
}

StkErrors check(Stack* stk)
{
    assert(stk != nullptr);
    assert(stk->data != nullptr);

    size_t size = stk->size;
    size_t capacity = stk->capacity;
    stack_element_t* start_ptr = stk->data;

    if (stk == nullptr)
    {
        printf("no place for arrays\n");
        return NO_PLACE;
    }

    if (stk->size > stk->capacity)
    {
        printf("program crossed the line\n");
        return BUFFER_OVERFLOW;
    }
        
    for (size_t i = 0; i < size; i++)
    {
        if (start_ptr[i] == poison)
        {
            printf("elements were not add\n");
            return VALUE_PROBLEM;
        }
    }

    if(start_ptr[0] != left_canary_value)
    {
        printf("problem in left canary\n");
        return PROBLEM;
    }

    if(start_ptr[capacity + LEFT_CANARY_ADD] != right_canary_value)
    {
        printf("problem in right canary\n");
        return PROBLEM;
    }

    if (stk->hash != stk_hash(stk))
    {
        printf("strange rearrangement in the array, hash is incorrect\n");
        return HASH_PROBLEM;
    }

    return ALL_RIGHT;
}


unsigned long long stk_hash(Stack* stk)
{
    unsigned long long hash = start_hash;
    stack_element_t* start_ptr = stk->data;
    size_t size = stk->size;
    size_t elem_num = 0;

    while (elem_num < size)
    {
        hash = hash * 33  + (unsigned long long)start_ptr[elem_num  + LEFT_CANARY_ADD];
        elem_num++;
    }
    return hash;
}
