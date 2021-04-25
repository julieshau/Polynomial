#include "poly.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define ERROR_EXIT_STATUS 1
#define EXP_MIN (-1)

//todo comments
//todo style
//todo gitlab tests
//todo polyaddmonos copy ask

void PolyDestroy(Poly *p){
    if (!PolyIsCoeff(p)){
        for (size_t i = 0; i < p->size; ++i) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
        p->arr = NULL;
    }
}

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

static bool PolyIsMonoZero(Poly *p){
    return (PolyIsMono(p) && PolyIsZero(&p->arr[0].p));
}

static void PolyOptimize(Poly *p){
    if (PolyIsMonoCoef(p) || PolyIsMonoZero(p)){
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
    /*We don't own the monos array, only its contents. So we need copy of the array*/
    Mono* monos_copy = (Mono*)malloc(sizeof(struct Mono) * count);
    if (monos_copy == NULL){
        exit(ERROR_EXIT_STATUS);
    }
    memcpy(monos_copy, monos, sizeof(struct Mono) * count);

    /* From this moment all monos in poly will be sorted by exp. Functions like multiplication
     * that break the order will be required to restore it*/
    qsort(monos_copy, count, sizeof(struct Mono), MonoCompare);

    /*From this moments for each exp there is only one Mono in Poly with such exp*/
    size_t current = 0;
    for (size_t next = 1; next < count; ++next) {
        if (MonoGetExp(&monos_copy[next]) != MonoGetExp(&monos_copy[current])){ //new unique exp
            if (MonoIsZero(&monos_copy[current])){
                monos_copy[current] = monos_copy[next]; //skip zero(replace with new mono)
            }
            else {
                monos_copy[++current] = monos_copy[next]; //add new mono
            }
        }
        else{
            Mono sum = MonoAdd(&monos_copy[current], &monos_copy[next]);
            MonoDestroy(&monos_copy[current]);
            MonoDestroy(&monos_copy[next]);
            monos_copy[current] = sum; //zero may appear, we'll get rid of it in next iteration
        }
    }
    if (current != 0 && MonoIsZero(&monos_copy[current])){ //we need to get rid of last zero. If last zero is the only element we leave it
        current--;
    }

    Poly result= (Poly) {.size = ++current, .arr = monos_copy};
    PolyOptimize(&result);
    return result;
}

static poly_exp_t max(poly_exp_t lhs, poly_exp_t rhs){
    return (lhs > rhs) ? lhs : rhs;
}

static poly_exp_t MonosCoefDegBy(const Mono *m, size_t var_idx){
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
            result = max(MonosCoefDegBy(&p->arr[i], var_idx - 1), result);
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
        if (PolyIsCoeff(p)){ //p and q are both coeffs
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

static Poly PolyMulCoef(const Poly *p, poly_coeff_t q){
    if (q == 0) {
        return PolyZero();
    }
    assert(p->size > 0);
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
        if (!PolyIsZero(&mul)){ //skip zero
            Mono mono = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]));
            result.arr[result.size++] = mono;
        }
        /* Note: we don't destroy anything here.
         * 1. mono owns mul -> no need to destroy mul
         * 2.1(mul isn't zero) result owns mono
         * 2.2(mul is zero) -> no arrays -> nothing to destroy*/
    }
    if (result.size == 0){ //there were only zeros, nothing was written to the result
        free(result.arr); //we don't use destroy function, because result is uninitialized value
        return PolyZero();
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
        for (size_t j = 0; j < q->size; ++j){
            Poly mul = PolyMul(&p->arr[i].p, &q->arr[j].p);
            if (!PolyIsZero(&mul)) { //skip zero
                init_arr[current] = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]) + MonoGetExp(&q->arr[j]));
                current++;
            }
        }
    }
    Poly result = PolyAddMonos(init_size, init_arr);//we use this function to restore broken order and uniqueness of exp
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
            return PolyMulCoef(q, p->coeff);
        }
        else{
            return PolyMulCoef(p, q->coeff);
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
            sum = CoefAddCoef(p->arr[0].p.coeff, q);
        }
        else {
            sum = PolyAddCoef(&p->arr[0].p, q);
        }
        if (!PolyIsZero(&sum)){
            Mono mono = MonoFromPoly(&sum, MonoGetExp(&p->arr[0]));
            result = PolyInit(p->size);
            result.arr[current++] = mono;
        }
        else {
            result = PolyInit(p->size - 1); //skip zero
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
        poly_exp_t q_exp = MonoGetExp(&q->arr[j]);
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
            /* Note: we don't destroy anything here.
             * 1. mono owns sum -> no need to destroy sum
             * 2.1(Mono isn't zero) result owns mono
             * 2.2(Mono is zero) Mono's coef(Poly) is zero -> no arrays -> nothing to destroy*/
            i++;
            j++;
        }
    }
    /*Case i > p->size && j = q->size)*/
    while (i < p->size) {
        result.arr[current] = MonoClone(&p->arr[i]);
        i++;
        current++;
    }
    /*Case i = p->size && j > q->size)*/
    while (j < q->size) {
        result.arr[current] = MonoClone(&q->arr[j]);
        j++;
        current++;
    }
    if (current == 0){ //there were only zeros, nothing was written to the result
        free(result.arr); //we don't use destroy function, because result is uninitialized value
        return PolyZero();
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
            return PolyAddCoef(q, p->coeff);
        }
        else{
            return PolyAddCoef(p, q->coeff);
        }
    }
}

//Source: https://www.geeksforgeeks.org/modular-exponentiation-power-in-modular-arithmetic/
static poly_coeff_t Power(poly_coeff_t base, poly_exp_t exp){
    poly_coeff_t result = 1;
    while (exp > 0){
        // If exp is odd, multiply base with result
        if ((exp & 1) != 0) {
            result *= base;
        }
        // exp must be even now
        exp = exp >> 1; // exp = exp/2
        base *= base;  // Change base to base^2
    }
    return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x){
    if (PolyIsCoeff(p)) {
        return PolyClone(p);
    }
    /* Monos in poly are always sorted by exp. To avoid counting x^n for every mono in poly,
     * we count x^n for current mono and difference m-n (next mono has greater exp m).
     * So x^m = x^n * x^(m-n)  */
    poly_coeff_t x_power_value = 1;
    poly_exp_t current_exp = 0;
    Poly result = PolyZero();
    for (size_t i = 0; i < p->size; ++i) {
        poly_exp_t previous_exp = current_exp;
        current_exp = MonoGetExp(&p->arr[i]);
        poly_exp_t exp_difference = current_exp - previous_exp;//m-n

        x_power_value *= Power(x, exp_difference); //x^n * x^(m-n)

        Poly current_mono_evaluated;
        if (PolyIsCoeff(&p->arr[i].p)){
            current_mono_evaluated = CoefMulCoef(p->arr[i].p.coeff, x_power_value);
        }
        else {
            current_mono_evaluated = PolyMulCoef(&p->arr[i].p, x_power_value);
        }
        Poly new_result = PolyAdd(&current_mono_evaluated, &result);

        PolyDestroy(&result);
        PolyDestroy(&current_mono_evaluated);

        result = new_result;
    }
    return result;
}