/** @file
  Interfejs wykonawcy poleceń

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "stack.h"
#include "poly.h"

/**
 * Dostępne polecenia.
 */
typedef enum Commands {
    ZERO,
    IS_COEFF,
    IS_ZERO,
    CLONE,
    ADD,
    MUL,
    NEG,
    SUB,
    IS_EQ,
    DEG,
    DEG_BY,
    AT,
    PRINT,
    POP,
    COMPOSE
} Commands;

/**
 * Unia przechowująca parametry polecenia.
 */
typedef union CommandParams {
    size_t var_idx; ///< parametr polecenia DEG_BY
    size_t count; ///< parametr polecenia COMPOSE
    poly_coeff_t x; ///< parametr polecenia AT
} CommandParams;

/**
 * Wykonuje polecenie o podanym parametrze. W razie potreby modyfikuje stos.
 * Jeśli wykryje błąd, wypisuje komunikat o błędzie na standardowe wyjście błędów.
 * @param[in] command : polecenie
 * @param[in] param : parametr polecenia
 * @param[in, out] stack : stos
 * @param[in] line_number : numer wiersza, w którym znajduje się polecenie
 */
void ExecuteCommand(Commands command, CommandParams param, Stack *stack, int line_number);

#endif //EXECUTOR_H
