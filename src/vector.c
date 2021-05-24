/** @file
  Implementacja wektora jednomianów

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "vector.h"

#include <stdlib.h>

/** Początkowa pojemność wektora */
#define VECTOR_INIT_CAPACITY 4
/**
 * Kod zakończenia.
 * Jeśli wystąpi błąd krytyczny, np. zabraknie pamięci, program powinien zakończyć się awaryjnie kodem 1.
*/
#define ERROR_EXIT_STATUS 1
/** Zmiana pojemnośći */
#define CAPACITY_CHANGE 2

Vector VectorCreate() {
    Mono *init_arr = (Mono *) malloc(sizeof(Mono) * VECTOR_INIT_CAPACITY);
    if (init_arr == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    return (Vector) {.size = 0, .capacity = VECTOR_INIT_CAPACITY, .array = init_arr};
};

void VectorDestroy(Vector *vector) {
    for (size_t i = 0; i < vector->size; ++i) {
        MonoDestroy(&vector->array[i]);
    }
    free(vector->array);
}

/**
 * Realokuje pamięć w wektorze.
 * @param[in] vector : wektor
 */
static void VectorResize(Vector *vector) {
    vector->capacity *= CAPACITY_CHANGE;
    Mono *new_array = (Mono *) realloc(vector->array, sizeof(Mono) * vector->capacity);
    if (new_array == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    vector->array = new_array;
}

void VectorAdd(Vector *vector, Mono *mono) {
    if (vector->size == vector->capacity) {
        VectorResize(vector);
    }
    vector->array[vector->size++] = *mono;
}
