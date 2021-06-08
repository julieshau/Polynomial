/** @file
  Interfejs wektora jednomianów

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef VECTOR_H
#define VECTOR_H

#include "poly.h"

/**
 * Struktura wektora.
 */
typedef struct Vector {
    size_t size; ///< ilość elementów
    size_t capacity; ///< pojemność
    Mono *array; ///< tablica przechowująca elementy wektora
} Vector;

/**
* Tworzy wektor jednomianów.
* @return wektor
*/
Vector VectorCreate();

/**
 * Usuwa wektor z pamięci.
 * @param[in,out] vector : wektor
 */
void VectorDestroy(Vector *vector);

/**
 * Wstawia jednomian do wektora.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p mono.
 * @param[in,out] vector : wektor
 * @param[in] mono : jednomian
 */
void VectorAdd(Vector *vector, Mono *mono);

#endif //VECTOR_H
