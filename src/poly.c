/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/
#include "poly.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Kod zakończenia.
 * Jeśli wystąpi błąd krytyczny, np. zabraknie pamięci, program powinien zakończyć się awaryjnie kodem 1.
*/
#define ERROR_EXIT_STATUS 1
/** Minimalna wartość wykładnika wielomianu */
#define EXP_MIN (-1)


void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; ++i) {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
        p->arr = NULL;
    }
}

/**
 * Alokuje pamięć dla nowego wielomianu o @p size jednomianach.
 * @param[in] size : liczba jednomianów
 * @return nowy wielomian
 */
static Poly PolyInit(size_t size) {
    Mono *init_arr = (Mono *) malloc(sizeof(struct Mono) * size);
    if (init_arr == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    return (Poly) {.size = size, .arr = init_arr};
}

Poly PolyClone(const Poly *p) {
    if (PolyIsCoeff(p)) {
        /* tutaj zostanie zwrócona kopia *p*/
        return *p;
    } else {
        Poly new_poly = PolyInit(p->size);
        for (size_t i = 0; i < p->size; ++i) {
            new_poly.arr[i] = MonoClone(&p->arr[i]);
        }
        return new_poly;
    }
}

/**
 * Porównuje dwa jenomiany według wykładników.
 * @param[in] lhs : wskaźnik na jednomian
 * @param[in] rhs : wskaźnik na jednomian
 * @return wynik porównania:
 *        - dodatnia wartość całkowita, jeśli pierwszy argument jest większy niż drugi,
 *        - ujemna wartość całkowita, jeśli pierwszy argument jest mniejszy niż drugi,
 *        - zero, jeśli argumenty są równoważne
 */
static poly_exp_t MonoCompare(const void *lhs, const void *rhs) {
    return ((Mono *) lhs)->exp - ((Mono *) rhs)->exp;
}

/**
 * Sprawdza, czy jednomian jest tożsamościowo równy zeru.
 * @param[in] m : jednomian
 * @return Czy jednomian jest równy zeru?
 */
static bool MonoIsZero(const Mono *m) {
    return PolyIsZero(&m->p);
}

/**
 * Sprawdza, czy wielomian składa się z jednego jednomianu (czy wielomian jest jednomianem).
 * @param[in] p : wielomian
 * @return Czy wielomian jest jednomianem?
 */
static bool PolyIsMono(Poly *p) {
    return p->size == 1;
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem w formie nie uproszczonej.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem w formie nie uproszczonej?
 */
static bool PolyIsMonoCoef(Poly *p) {
    return (PolyIsMono(p) && MonoGetExp(&p->arr[0]) == 0 && PolyIsCoeff(&p->arr[0].p));
}

/**
 * Sprawdza, czy wielomian w formie nie uproszczonej jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian w formie nie uproszczonej jest tożsamościowo równy zeru?
 */
static bool PolyIsMonoZero(Poly *p) {
    return (PolyIsMono(p) && PolyIsZero(&p->arr[0].p));
}

/**
 * Upraszcza wielomian.
 * @param[in] p : wielomian
 * @return wielomian uproszczony
 */
static void PolyOptimize(Poly *p) {
    if (PolyIsMonoCoef(p) || PolyIsMonoZero(p)) {
        p->coeff = p->arr[0].p.coeff;
        free(p->arr);
        p->arr = NULL;
    }
}

/**
 * Dodaje dwa jednomiany o tym samym wykładniku.
 * @param[in] lhs : wielomian @f$lhs@f$
 * @param[in] rhs : wielomian @f$rhs@f$
 * @return @f$lhs + rhs@f$
 */
static Mono MonoAdd(const Mono *lhs, const Mono *rhs) {
    assert(lhs->exp == rhs->exp);
    return (Mono) {.p = PolyAdd(&lhs->p, &rhs->p), .exp = lhs->exp};
}

Poly PolyOwnMonos(size_t count, Mono *monos){
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }
    /* Od tego momentu wszystkie jednomiany w wielomianie będą sortowane według wykładnika.
     * Funkcje takie jak mnożenie, które psują porządek, będą musiały go przywrócić*/
    qsort(monos, count, sizeof(struct Mono), MonoCompare);

    /* Od tego momentu dla każdego wykładnika istnieje dokładnie jeden jednomian w wielomianie z takim wykładnikiem*/
    size_t current = 0;
    for (size_t next = 1; next < count; ++next) {
        /* nowy unikalny wykładnik*/
        if (MonoGetExp(&monos[next]) != MonoGetExp(&monos[current])) {
            if (MonoIsZero(&monos[current])) {
                /* pomijamy zero (zamieniamy na nowy jednomian)*/
                monos[current] = monos[next];
            } else {
                /* dodaj nowy jednomian*/
                monos[++current] = monos[next];
            }
        } else {
            Mono sum = MonoAdd(&monos[current], &monos[next]);
            MonoDestroy(&monos[current]);
            MonoDestroy(&monos[next]);
            /* może pojawić się zero, pozbędziemy się go w następnej iteracji*/
            monos[current] = sum;
        }
    }
    /* Musimy pozbyć się ostatniego zera. Jeśli ostatnie zero jest jedynym elementem zostawiamy go*/
    if (current != 0 && MonoIsZero(&monos[current])) {
        current--;
    }

    Poly result = (Poly) {.size = ++current, .arr = monos};
    PolyOptimize(&result);
    return result;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }
    /* Przejmujemy na własność zawartość tablicy monos(NIE TABLICĘ!).
     * Więc potrzebujemy kopii tablicy*/
    Mono *monos_copy = (Mono *) malloc(sizeof(struct Mono) * count);
    if (monos_copy == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    memcpy(monos_copy, monos, sizeof(struct Mono) * count);
    return PolyOwnMonos(count, monos_copy);
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL) {
        return PolyZero();
    }
    /* Przejmujemy na własność zawartość tablicy monos(NIE TABLICĘ!).
     * Więc potrzebujemy kopii tablicy*/
    Mono *monos_copy = (Mono *) malloc(sizeof(struct Mono) * count);
    if (monos_copy == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    for (size_t i = 0; i < count; ++i) {
        monos_copy[i] = MonoClone(&monos[i]);
    }
    return PolyOwnMonos(count, monos_copy);
}

/**
 * Zwraca większy z dwóch wykładników.
 * @param[in] lhs : wykładnik @f$lhs@f$
 * @param[in] rhs : wykładnik @f$rhs@f$
 * @return największy wykładnik z @f$lhs@f$ i @f$rhs@f$. Jeśli oba są równoważne, zwracane jest @f$rhs@f$.
 */
static poly_exp_t max(poly_exp_t lhs, poly_exp_t rhs) {
    return (lhs > rhs) ? lhs : rhs;
}

/**
 * Zwraca stopień jednomianu.
 * @param[in] m : jednomian
 * @return stopień jednomianu @p m
 */
static poly_exp_t MonoDeg(const Mono *m) {
    return (PolyDeg(&m->p) + MonoGetExp(m));
}

/**
 * Zwraca stopień współczynnika jednomianu ze względu na zadaną zmienną.
 * @param[in] m : jednomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień współczynnika jednomianu @p m z względu na zmienną o indeksie @p var_idx
 */
static poly_exp_t MonosCoefDegBy(const Mono *m, size_t var_idx) {
    return PolyDegBy(&m->p, var_idx);
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsCoeff(p)) {
        return PolyIsZero(p) ? -1 : 0;
    }
    if (var_idx == 0) {
        /* tablica jednomianów jest posortowana(tworzenie wielomianu z tablicy jednomianów),
         * ostatni element ma największy wykładnik*/
        return MonoGetExp(&p->arr[p->size - 1]);
    } else {
        poly_exp_t result = EXP_MIN;
        for (size_t i = 0; i < p->size; ++i) {
            result = max(MonosCoefDegBy(&p->arr[i], var_idx - 1), result);
        }
        return result;
    }
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsCoeff(p)) {
        return PolyIsZero(p) ? -1 : 0;
    } else {
        poly_exp_t result = EXP_MIN;
        for (size_t i = 0; i < p->size; ++i) {
            result = max(MonoDeg(&p->arr[i]), result);
        }
        return result;
    }
}

/**
 * Sprawdza równość dwóch jednomianów.
 * @param[in] rhs : jednomian @f$rhs@f$
 * @param[in] lhs : jednomian @f$lhs@f$
 * @return @f$rhs = lhs@f$
 */
static bool MonoIsEq(const Mono *lhs, const Mono *rhs) {
    return MonoGetExp(lhs) == MonoGetExp(rhs) && PolyIsEq(&lhs->p, &rhs->p);
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) == PolyIsCoeff(q)) {
        /* p i q to oba współczynniki*/
        if (PolyIsCoeff(p)) {
            return p->coeff == q->coeff;
        } else if (p->size != q->size) {
            return false;
        } else {
            for (size_t i = 0; i < p->size; ++i) {
                if (!MonoIsEq(&p->arr[i], &q->arr[i])) {
                    return false;
                }
            }
            return true;
        }
    } else {
        return false;
    }
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly temp = PolyNeg(q);
    Poly result = PolyAdd(p, &temp);
    PolyDestroy(&temp);
    return result;
}

Poly PolyNeg(const Poly *p) {
    Poly neg = PolyFromCoeff(-1);
    return PolyMul(p, &neg);
}

/**
 * Mnoży dwa wielomiany stałe (które są współczynnikiem).
 * @param[in] p : współczynnik @f$p@f$
 * @param[in] q : współczynnik @f$q@f$
 * @return @f$p * q@f$
 */
static Poly CoefMulCoef(poly_coeff_t p, poly_coeff_t q) {
    return PolyFromCoeff(p * q);
}

/**
 * Mnoży dwa wielomiany: nie stały i stały (który jest współczynnikiem).
 * @param[in] p : wielomian nie stały @f$p@f$
 * @param[in] q : współczynnik @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulCoef(const Poly *p, poly_coeff_t q) {
    if (q == 0) {
        return PolyZero();
    }
    assert(p->size > 0);
    Poly result = PolyInit(p->size);
    result.size = 0;
    for (size_t i = 0; i < p->size; ++i) {
        Poly mul;
        if (PolyIsCoeff(&p->arr[i].p)) {
            mul = CoefMulCoef(p->arr[i].p.coeff, q);
        } else {
            mul = PolyMulCoef(&p->arr[i].p, q);
        }
        /* pomijamy zero*/
        if (!PolyIsZero(&mul)) {
            Mono mono = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]));
            result.arr[result.size++] = mono;
        }
        /* Uwaga: tu nie używamy destroy
         * 1. mono jest właścicielem mul -> nie używamy destroy dla mul
         * 2.1(mul nie zero) result jest właścicielem mono
         * 2.2(mul zero) -> nie ma tablicy jednomianów -> nie używamy destroy*/
    }
    /* były tylko zera, nic nie zostało zapisane do wyniku*/
    if (result.size == 0) {
        /* nie używamy funkcji destroy, ponieważ wynik jest niezainicjalizowaną wartością*/
        free(result.arr);
        return PolyZero();
    }
    PolyOptimize(&result);
    return result;
}

/**
 * Mnoży dwa wielomiany nie stałe.
 * @param[in] p : wielomian nie stały @f$p@f$
 * @param[in] q : wielomian nie stały @f$q@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulPoly(const Poly *p, const Poly *q) {
    size_t init_size = p->size * q->size;
    Mono *init_arr = (Mono *) malloc(sizeof(struct Mono) * init_size);
    if (init_arr == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    size_t current = 0;
    for (size_t i = 0; i < p->size; ++i) {
        for (size_t j = 0; j < q->size; ++j) {
            Poly mul = PolyMul(&p->arr[i].p, &q->arr[j].p);
            /* pomijamy zero*/
            if (!PolyIsZero(&mul)) {
                init_arr[current] = MonoFromPoly(&mul, MonoGetExp(&p->arr[i]) + MonoGetExp(&q->arr[j]));
                current++;
            }
        }
    }
    Poly result;
    if (current == 0) {
        result = PolyZero();
    } else {
        /* używamy tej funkcji, aby przywrócić zepsuty porządek i unikalność każdego wykładnika*/
        result = PolyAddMonos(current, init_arr);
    }
    free(init_arr);
    return result;
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) == PolyIsCoeff(q)) {
        if (PolyIsCoeff(p)) {
            return CoefMulCoef(p->coeff, q->coeff);
        } else {
            return PolyMulPoly(p, q);
        }
    } else {
        if (PolyIsCoeff(p)) {
            return PolyMulCoef(q, p->coeff);
        } else {
            return PolyMulCoef(p, q->coeff);
        }
    }
}

/**
 * Dodaje dwa wielomiany stałe (które są współczynnikiem).
 * @param[in] p : współczynnik @f$p@f$
 * @param[in] q : współczynnik @f$q@f$
 * @return @f$p + q@f$
 */
static Poly CoefAddCoef(poly_coeff_t p, poly_coeff_t q) {
    return PolyFromCoeff(p + q);
}

/**
 * Dodaje dwa wielomiany: nie stały i stały (który jest współczynnikiem).
 * @param[in] p : wielomian nie stały @f$p@f$
 * @param[in] q : współczynnik @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddCoef(const Poly *p, poly_coeff_t q) {
    if (q == 0) {
        return PolyClone(p);
    }
    assert(p->size > 0);
    Poly result;
    /* wykładniki są posortowane i unikalne -> współczynnik może pojawić się tylko na pierwszej pozycji*/
    if (MonoGetExp(&p->arr[0]) == 0) {
        size_t current = 0;

        Poly sum;
        if (PolyIsCoeff(&p->arr[0].p)) {
            sum = CoefAddCoef(p->arr[0].p.coeff, q);
        } else {
            sum = PolyAddCoef(&p->arr[0].p, q);
        }
        if (!PolyIsZero(&sum)) {
            Mono mono = MonoFromPoly(&sum, MonoGetExp(&p->arr[0]));
            result = PolyInit(p->size);
            result.arr[current++] = mono;
        } else {
            /* pomijamy zero*/
            result = PolyInit(p->size - 1);
        }
        for (size_t i = 1; i < p->size; ++i) {
            result.arr[current++] = MonoClone(&p->arr[i]);
        }
    } else {
        result = PolyInit(p->size + 1);
        Poly poly = PolyFromCoeff(q);
        result.arr[0] = MonoFromPoly(&poly, 0);
        for (size_t i = 0; i < p->size; ++i) {
            result.arr[i + 1] = MonoClone(&p->arr[i]);
        }
    }
    return result;
}

/**
 * Dodaje dwa wielomiany nie stałe.
 * @param[in] p : wielomian nie stały @f$p@f$
 * @param[in] q : wielomian nie stały @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddPoly(const Poly *p, const Poly *q) {
    Poly result = PolyInit(p->size + q->size);
    size_t i = 0;
    size_t j = 0;
    size_t current = 0;
    while (i < p->size && j < q->size) {
        poly_exp_t p_exp = MonoGetExp(&p->arr[i]);
        poly_exp_t q_exp = MonoGetExp(&q->arr[j]);
        if (p_exp < q_exp) {
            result.arr[current] = MonoClone(&p->arr[i]);
            i++;
            current++;
        } else if (p_exp > q_exp) {
            result.arr[current] = MonoClone(&q->arr[j]);
            j++;
            current++;
        } else {
            Mono sum = MonoAdd(&p->arr[i], &q->arr[j]);
            if (!MonoIsZero(&sum)) {
                result.arr[current] = sum;
                current++;
            }
            /* Uwaga: tu nie używamy destroy
            * 1. mono jest właścicielem sum -> nie używamy destroy dla sum
            * 2.1(Mono nie zero) result jest właścicielem mono
            * 2.2(Mono zero)-> wielomian tego jednomianu zero -> nie ma tablicy jednomianów -> nie używamy destroy*/
            i++;
            j++;
        }
    }
    /* Przypadek i > p->size && j = q->size)*/
    while (i < p->size) {
        result.arr[current] = MonoClone(&p->arr[i]);
        i++;
        current++;
    }
    /* Przypadek i = p->size && j > q->size)*/
    while (j < q->size) {
        result.arr[current] = MonoClone(&q->arr[j]);
        j++;
        current++;
    }
    /* były tylko zera, nic nie zostało zapisane do wyniku*/
    if (current == 0) {
        /* nie używamy funkcji destroy, ponieważ wynik jest niezainicjalizowaną wartością*/
        free(result.arr);
        return PolyZero();
    }
    result.size = current;
    PolyOptimize(&result);
    return result;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) == PolyIsCoeff(q)) {
        if (PolyIsCoeff(p)) {
            return CoefAddCoef(p->coeff, q->coeff);
        } else {
            return PolyAddPoly(p, q);
        }
    } else {
        if (PolyIsCoeff(p)) {
            return PolyAddCoef(q, p->coeff);
        } else {
            return PolyAddCoef(p, q->coeff);
        }
    }
}

/**
 * Wylicza @f$exp@f$-tą potęgę współczynnika @f$base@f$.
 * Źródło: https://www.geeksforgeeks.org/modular-exponentiation-PolyPow-in-modular-arithmetic/
 * @param[in] base : współczynnik @f$base@f$
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$base^{exp}@f$
 */
static poly_coeff_t Power(poly_coeff_t base, poly_exp_t exp) {
    poly_coeff_t result = 1;
    while (exp > 0) {
        /* Jeśli exp jest nieparzysty, mnożymy base i result*/
        if ((exp & 1) != 0) {
            result *= base;
        }
        /* teraz exp musi być parzysty*/
        exp = exp >> 1;
        base *= base;
    }
    return result;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    if (PolyIsCoeff(p)) {
        return PolyClone(p);
    }
    /* Jednomiany w wielomianie są zawsze sortowane według wykładników. Aby uniknąć liczenia x^n dla
     * każdego jednomianu w wielomianie, liczymy x^n dla bieżącego jednomianu i różnicę m-n(następny
     * jednomian ma większy wykładnik m). Więc x^m = x^n * x^(m-n)*/
    poly_coeff_t x_power_value = 1;
    poly_exp_t current_exp = 0;
    Poly result = PolyZero();
    for (size_t i = 0; i < p->size; ++i) {
        poly_exp_t previous_exp = current_exp;
        current_exp = MonoGetExp(&p->arr[i]);

        /* m - n*/
        poly_exp_t exp_difference = current_exp - previous_exp;

        /* x^n * x^(m - n)*/
        x_power_value *= Power(x, exp_difference);

        Poly current_mono_evaluated;
        if (PolyIsCoeff(&p->arr[i].p)) {
            current_mono_evaluated = CoefMulCoef(p->arr[i].p.coeff, x_power_value);
        } else {
            current_mono_evaluated = PolyMulCoef(&p->arr[i].p, x_power_value);
        }
        Poly new_result = PolyAdd(&current_mono_evaluated, &result);

        PolyDestroy(&result);
        PolyDestroy(&current_mono_evaluated);

        result = new_result;
    }
    return result;
}

void MonoPrint(const Mono *m) {
    printf("(");
    PolyPrint(&m->p);
    printf(",");
    printf("%d", m->exp);
    printf(")");
}

void PolyPrint(const Poly *p){
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
    }
    else{
        assert(p->size > 0);
        MonoPrint(&p->arr[0]);
        for (size_t i = 1; i < p->size; ++i) {
            printf("+");
            MonoPrint(&p->arr[i]);
        }
    }
}

/**
 * Wylicza @f$exp@f$-tą potęgę wielomianu @f$p@f$.
 * @param[in] p : wielomian
 * @param[in] exp : wykładnik @f$exp@f$
 * @return @f$p^{exp}@f$
 */
Poly PolyPow(const Poly *p, poly_exp_t exp){
    Poly temp;
    if( exp == 0)
        return PolyFromCoeff(1);
    temp = PolyPow(p, exp / 2);
    Poly result = PolyMul(&temp, &temp);
    PolyDestroy(&temp);
    if (exp % 2 == 0){
        return result;
    }
    else {
        Poly poly = PolyMul(p, &result);
        PolyDestroy(&result);
        return poly;
    }
}

Poly PolyPow2(const Poly *p, poly_exp_t exp){
    Poly result = PolyFromCoeff(1);
    while (exp > 0) {
        Poly r = PolyMul(&result, p);
        PolyDestroy(&result);
        result = r;
        exp--;
    }
    return result;
}

Poly PolyCompose(const Poly *p, size_t count, const Poly q[]){
    if (PolyIsCoeff(p)) {
        return *p;
    }
    if (count == 0) {
        return PolyAt(p, 0);
    }
    Poly* temp = malloc(p->size * sizeof(Poly));
    if (temp == NULL) {
        exit(ERROR_EXIT_STATUS);
    }
    for (size_t i = 0; i < p->size; i++) {
        Poly inner_compose = PolyCompose(&p->arr[i].p, (count > 1) ? count - 1 : 0, q + (count > 1));
        if (PolyIsZero(&inner_compose)) {
            temp[i] = PolyZero();
        }
        else {
            Poly poly_pow = PolyPow(&q[0], p->arr[i].exp);
            temp[i] = PolyMul(&inner_compose, &poly_pow);
            PolyDestroy(&poly_pow);
        }
        PolyDestroy(&inner_compose);
    }
    Poly result = temp[0];
    for (size_t i = 1; i < p->size; i++) {
        Poly temp_res = PolyAdd(&result, &temp[i]);
        PolyDestroy(&result);
        result = temp_res;
        PolyDestroy(&temp[i]);
    }
    free(temp);
    return result;
}