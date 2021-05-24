/** @file
  Implementacja stosu wielomianów

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/
#include "stack.h"
#include <stdlib.h>

#define STACK_INIT_CAPACITY 4
#define ERROR_EXIT_STATUS 1
#define CAPACITY_CHANGE 2

Stack StackCreate() {
    Poly* init_arr = (Poly*) malloc(sizeof(Poly) * STACK_INIT_CAPACITY);
    if (init_arr == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    return (Stack) {.size = 0, .capacity = STACK_INIT_CAPACITY, .array = init_arr};
};

bool StackIsEmpty(const Stack* s){
    return s->size == 0;
}

bool StackIsFull(const Stack* s){
    return s->size == s->capacity;
}

size_t StackGetSize(Stack* s){
    return s->size;
}

Poly Pop(Stack* s){
    assert(!StackIsEmpty(s));
    return s->array[--(s->size)];
}

Poly Top(const Stack* s){
    assert(!StackIsEmpty(s));
    return s->array[s->size - 1];
}

/**
 * Realokuje pamięć w stosie.
 * @param[in] s : stos
 */
static void StackResize(Stack* s){
    s->capacity *= CAPACITY_CHANGE;
    Poly* new_array = (Poly*) realloc(s->array, sizeof(Poly) * s->capacity);
    if (new_array == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    s->array = new_array;
}

void Push(Stack* s, Poly* p){
    if (StackIsFull(s)){
        StackResize(s);
    }
    s->array[s->size++] = *p; //no clone
}

void StackDestroy(Stack* s){
    for(size_t i = 0; i < s->size; ++i) {
        PolyDestroy(&s->array[i]);
    }
    free(s->array); //no free s
}


