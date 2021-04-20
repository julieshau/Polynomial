#include "poly.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

static poly_exp_t MonoCompare(const void *lhs, const void *rhs){
    return ((Mono*)lhs)->exp - ((Mono*)rhs)->exp;
}

static bool PolyIsMono(Poly *p){
    return p->size == 1;
}

static bool PolyIsMonoCoef(Poly *p){
    return (PolyIsMono(p) && MonoGetExp(&p->arr[0]) == 0 && PolyIsCoeff(&p->arr[0].p));
}

static void PolyOptimize(Poly *p){
    if (PolyIsMonoCoef(p)){
        p->coeff = p->arr[0].p.coeff;
        free(p->arr);
        p->arr = NULL;
    }
    else {
        p->arr = realloc(p->arr, sizeof(struct Mono) * p->size);
        if (p->arr == NULL){
            exit(ERROR_EXIT_STATUS);
        }
    }
}

Poly PolyAddMonos(size_t count, const Mono monos[]){
    if (count == 0){
        return PolyZero();
    }
    Mono* monos_copy = (Mono*)malloc(sizeof(struct Mono) * count);
    if (monos_copy == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    memcpy(monos_copy, monos, sizeof(struct Mono) * count);

    qsort(monos_copy, count, sizeof(struct Mono), MonoCompare);

    size_t current = 0;
    for (size_t next = 1; next < count; ++next) {
        if (MonoGetExp(&monos_copy[next]) != MonoGetExp(&monos_copy[current])){
            monos_copy[++current] = monos_copy[next];
        }
        else{
            Mono sum = MonoAdd(&monos_copy[current], &monos_copy[next]);
            MonoDestroy(&monos_copy[current]);
            MonoDestroy(&monos_copy[next]);
            monos_copy[current] = sum;
            if (MonoIsZero(&sum)){
                current--;
            }
        }
    }
    Poly result= (Poly) {.size = ++current, .arr = monos_copy};
    PolyOptimize(&result);
    return result;
}

static poly_exp_t max(poly_exp_t lhs, poly_exp_t rhs){
    return (lhs > rhs) ? lhs : rhs;
}

static poly_exp_t MonoDegBy(const Mono *m, size_t var_idx){
    return PolyDegBy(&m->p, var_idx);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx){
    if (PolyIsCoeff(p)){
        return PolyIsZero(p) ? -1 : 0;
    }
    if (var_idx == 0){
        return MonoGetExp(&p->arr[p->size - 1]);
    }
    else{
        poly_exp_t result = -1;
        for (size_t i = 0; i < p->size; ++i){
            result = max(MonoDegBy(&p->arr[i], var_idx - 1), result);
        }
        return result;
    }
}

poly_exp_t PolyDeg(const Poly *p){
    if (PolyIsCoeff(p)){
        return PolyIsZero(p) ? -1 : 0;
    }
    else {
        poly_exp_t result = -1;
        for (size_t i = 0; i < p->size; ++i){
            result = max(MonoDeg(&p->arr[i]), result);
        }
        return result;
    }
}
