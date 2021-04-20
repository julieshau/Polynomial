#include "poly.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR_EXIT_STATUS 1

void PolyDestroy(Poly *p){
    if (!PolyIsCoeff(p)){
        for (size_t i = 0; i < p->size; ++i) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
        p->arr = NULL;
    }
}

//todo comment
static Poly PolyInit(size_t size){
    Mono* init_arr = (Mono*)malloc(sizeof(struct Mono) * size);
    if (init_arr == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    return (Poly) {.size = size, .arr = init_arr};
}

Poly PolyClone(const Poly *p){
    if (PolyIsCoeff(p)){
        return *p; // A copy of *p will be returned here
    }
    else {
        Poly new_poly = PolyInit(p->size);
        for (size_t i = 0; i < p->size; ++i) {
            new_poly.arr[i] = MonoClone(&p->arr[i]);
        }
        return new_poly;
    }
}
