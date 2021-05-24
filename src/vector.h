#ifndef VECTOR_H
#define VECTOR_H

#include "poly.h"

typedef struct Vector {
    size_t size;
    size_t capacity;
    Mono* array;
} Vector;

Vector VectorCreate();
void VectorDestroy(Vector* vector);
void VectorAdd(Vector* vector, Mono* mono);

#endif //VECTOR_H
