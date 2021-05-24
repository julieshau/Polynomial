#include "vector.h"

#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 4
#define ERROR_EXIT_STATUS 1
#define CAPACITY_CHANGE 2

Vector VectorCreate() {
    Mono* init_arr = (Mono*) malloc(sizeof(Mono) * VECTOR_INIT_CAPACITY);
    if (init_arr == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    return (Vector) {.size = 0, .capacity = VECTOR_INIT_CAPACITY, .array = init_arr};
};

void VectorDestroy(Vector* vector){
    for(size_t i = 0; i < vector->size; ++i) {
        MonoDestroy(&vector->array[i]);
    }
    free(vector->array);
}

static void VectorResize(Vector* vector){
    vector->capacity *= CAPACITY_CHANGE;
    Mono* new_array = (Mono*) realloc(vector->array, sizeof(Mono) * vector->capacity);
    if (new_array == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    vector->array = new_array;
}

void VectorAdd(Vector* vector, Mono* mono){
    if (vector->size == vector->capacity){
        VectorResize(vector);
    }
    vector->array[vector->size++] = *mono;
}
