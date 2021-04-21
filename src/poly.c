#include "poly.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_EXIT_STATUS 1
#define EXP_MIN (-1)

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
        return MonoGetExp(&p->arr[p->size - 1]);//array of mono is sorted(creation of poly from mono array), last elt has largest exp
    }
    else{
        poly_exp_t result = EXP_MIN;
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
        poly_exp_t result = EXP_MIN;
        for (size_t i = 0; i < p->size; ++i){
            result = max(MonoDeg(&p->arr[i]), result);
        }
        return result;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q){
    if (PolyIsCoeff(p) == PolyIsCoeff(q)){
        if (PolyIsCoeff(p)){
            return p->coeff == q->coeff;
        }
        else if (p->size != q->size){
            return false;
        }
        else {
            for (size_t i = 0; i < p->size; ++i){
                if (!MonoIsEq(&p->arr[i], &q->arr[i])){
                    return false;
                }
            }
            return true;
        }
    }
    else {
        return false;
    }
}

Poly PolySub(const Poly *p, const Poly *q){
    Poly temp = PolyNeg(q);
    Poly result = PolyAdd(p, &temp);
    PolyDestroy(&temp);
    return result;
}

Poly PolyNeg(const Poly *p){
    Poly neg = PolyFromCoeff(-1);
    return PolyMul(p, &neg);
}

static Poly CoefMulCoef(poly_coeff_t p, poly_coeff_t q){
    return PolyFromCoeff(p * q);
}

//?add monoiscoef
static Poly PolyMulCoef(const Poly *p, poly_coeff_t q){
    if (q == 0) {
        return PolyZero();
    }
    Poly result = PolyInit(p->size);
    result.size = 0;
    for (size_t i = 0; i < p->size; ++i){
        Poly mul;
        if (PolyIsCoeff(&p->arr[i].p)){
            mul = CoefMulCoef(p->arr[i].p.coeff, q);
        }
        else {
            mul = PolyMulCoef(&p->arr[i].p, q);
        }
        Mono mono = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]));
        if (!MonoIsZero(&mono)){
            result.arr[result.size++] = mono;
        }
    }
    PolyOptimize(&result);
    return result;
}

static Poly PolyMulPoly(const Poly *p,const Poly *q){
    size_t init_size = p->size * q->size;
    Mono* init_arr = (Mono*)malloc(sizeof(struct Mono) * init_size);
    if (init_arr == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    size_t current = 0;
    for (size_t i = 0; i < p->size; ++i) {
        for (size_t j = 0; i < q->size; ++j){
            Poly mul = PolyMul(&p->arr[i].p, &q->arr[j].p);
            init_arr[current] = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]) * MonoGetExp(&q->arr[j]));
            current++;
        }
    }
    Poly result = PolyAddMonos(init_size, init_arr);
    free(init_arr);
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q){
    if (PolyIsCoeff(p) == PolyIsCoeff(q)){
        if (PolyIsCoeff(p)){
            return CoefMulCoef(p->coeff, q->coeff);
        }
        else{
            return PolyMulPoly(p, q);
        }
    }
    else{
        if (PolyIsCoeff(p)){
            return PolyMulCoef(p, q->coeff);
        }
        else{
            return PolyMulCoef(q, p->coeff);
        }
    }
}

static Poly CoefAddCoef(poly_coeff_t p, poly_coeff_t q){
    return PolyFromCoeff(p + q);
}

static Poly PolyAddCoef(const Poly *p, poly_coeff_t q){
    if (q == 0) {
        return PolyClone(p);
    }
    assert(p->size > 0);
    Poly result;
    if (MonoGetExp(&p->arr[0]) == 0){
        size_t current = 0;

        Poly sum;
        if (PolyIsCoeff(&p->arr[0].p)){
            sum = CoefMulCoef(p->arr[0].p.coeff, q);
        }
        else {
            sum = PolyMulCoef(&p->arr[0].p, q);
        }
        Mono mono = MonoFromPoly(&sum, MonoGetExp(&p->arr[0]));
        if (!MonoIsZero(&mono)){
            result = PolyInit(p->size);
            result.arr[current++] = mono;
        }
        else {
            result = PolyInit(p->size - 1);
        }
        for (size_t i = 1; i < p->size; ++i) {
            result.arr[current++] = MonoClone(&p->arr[i]);
        }
    }
    else {
        result = PolyInit(p->size + 1);
        Poly poly = PolyFromCoeff(q);
        result.arr[0] = MonoFromPoly(&poly,0);
        for (size_t i = 0; i < p->size; ++i) {
            result.arr[i + 1] = MonoClone(&p->arr[i]);
        }
    }
    return result;
}

static Poly PolyAddPoly(const Poly *p, const Poly *q){
    Poly result = PolyInit(p->size + q ->size);
    size_t i = 0;
    size_t j = 0;
    size_t current = 0;
    while (i < p->size && j < q->size){
        poly_exp_t p_exp = MonoGetExp(&p->arr[i]);
        poly_exp_t q_exp = MonoGetExp(&q->arr[i]);
        if (p_exp < q_exp){
            result.arr[current] = MonoClone(&p->arr[i]);
            i++;
            current++;
        }
        else if (p_exp > q_exp){
            result.arr[current] = MonoClone(&q->arr[j]);
            j++;
            current++;
        }
        else {
            Mono sum = MonoAdd(&p->arr[i], &q->arr[j]);
            if (!MonoIsZero(&sum)){
                result.arr[current] = sum;
                current++;
            }
            i++;
            j++;
        }
    }
    while (i < p->size) {
        result.arr[current] = MonoClone(&p->arr[i]);
        i++;
        current++;
    }
    while (j < q->size) {
        result.arr[current] = MonoClone(&q->arr[j]);
        j++;
        current++;
    }
    result.size = current;
    PolyOptimize(&result);
    return result;
}

Poly PolyAdd(const Poly *p, const Poly *q){
    if (PolyIsCoeff(p) == PolyIsCoeff(q)){
        if (PolyIsCoeff(p)){
            return CoefAddCoef(p->coeff, q->coeff);
        }
        else{
            return PolyAddPoly(p,q);
        }
    }
    else{
        if (PolyIsCoeff(p)){
            return PolyAddCoef(p, q->coeff);
        }
        else{
            return PolyAddCoef(q, p->coeff);
        }
    }
}