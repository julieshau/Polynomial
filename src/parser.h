/** @file
  Interfejs parsera

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef PARSER_H
#define PARSER_H

#include "executor.h"
#include "poly.h"
#include <stdbool.h>

/**
 * Możliwe błędy
 */
typedef enum Errors {
    WRONG_COMMAND,
    WRONG_VARIABLE,
    WRONG_VALUE,
    STACK_UNDERFLOW,
    WRONG_POLY
} Errors;

/**
 * Odczytuje następny znak bez pobierania go.
 * @return następny znak w sekwencji wejścia
 */
int PeekChar();

/**
 * Pomija biezący wiersz. Jeśli nie ma więcej wierszów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 */
void SkipLine();

/**
 * Wypisuje komunikaty o błędach na standardowe wyjście błędów.
 * @param[in] line_number : numer wiersza, w którym znajduje się błąd
 * @param[in] error : rodzaj błędu
 */
void PrintError(int line_number, Errors error);

/**
 * Przetwarza wiersz ze standardowego wejśćia. Zwraca polecenie
 * i parametry polecenia. Jeśli wykryje błąd, wypisuje komunikat o błędzie
 * na standardowe wyjście błędów i zwraca false.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[in] line_number : numer wiersza, w którym znajduje się błąd
 * @param[out] command : polecenie
 * @param[out] param : parametr polecenia
 * @return Czy się udało sparsować wiersz?
 */
bool ParseCommand(int line_number, Commands *command, CommandParams *param);

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wielomian.
 * Jeśli wykryje błąd, wypisuje komunikat o błędzie
 * na standardowe wyjście błędów i zwraca false.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[in] line_number : numer wiersza, w którym znajduje się błąd
 * @param[out] poly : wielomian
 * @return Czy się udało sparsować wiersz?
 */
bool ParsePoly(int line_number, Poly *poly);

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca jednomian.
 * Jeśli wykryje błąd, wypisuje komunikat o błędzie
 * na standardowe wyjście błędów i zwraca false.
 * @param[in] line_number : numer wiersza, w którym znajduje się błąd
 * @param[out] mono : jednomian
 * @return Czy się udało sparsować wiersz?
 */
bool ParseMono(int line_number, Mono *mono);

#endif
