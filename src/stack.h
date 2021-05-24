/** @file
  Interfejs stosu wielomianów

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/
#ifndef STACK_H
#define STACK_H

#include "poly.h"

/**
 * Struktura stosu
 */
typedef struct Stack {
    size_t size; ///< komentarz
    size_t capacity;
    Poly* array;
} Stack;

/**
* Tworzy stos.
* @param[in] c : wartość współczynnika
* @return stos
*/
Stack StackCreate();

/**
* Sprawdza, czy stos jest pusty.
* @param[in] s : stos
* @return Czy stos jest pusty?
*/
bool StackIsEmpty(const Stack* s);

/**
* Sprawdza, czy stos jest pełny.
* @param[in] s : stos
* @return Czy stos jest pełny?
*/
bool StackIsFull(const Stack* s);

/**
 * Zwraca rozmiar stosu.
 * @param[in] s : stos
 * @return rozmiar stosu
 */
size_t StackGetSize(Stack* s);

/**
 * Usuwa wielomian z wierzchołka stosu
 * @param[in] s : stos
 * @return wielomian z wierzchołka stosu
 */
Poly Pop(Stack* s);

/**
 * Zwraca wielomian z wierzchołka stosu
 * @param[in] s : stos
 * @return wielomian z wierzchołka stosu
 */
Poly Top(const Stack* s);

/**
 * Wstawia wielomian na stos
 * @param[in] s : stos
 * @param[in] p : wielomian
 */
void Push(Stack* s, Poly* p);

/**
 * Usuwa stos z pamięci.
 * @param[in] s : stos
 */
void StackDestroy(Stack* s);

#endif
