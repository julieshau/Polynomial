/** @file
  Implementacja parsera

  @authors Yuliya Shauchuk <ys423496@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "parser.h"
#include "poly.h"
#include "executor.h"
#include "vector.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

/** Maksymalna długość(ilość znaków) polecenia(bez parametrów) */
#define COMMAND_MAX_LENGTH 8
/** Maksymalna długość(ilość znaków) poprawnej liczby */
#define NUMBER_MAX_LENGTH 20
/** Maksymalna długość(ilość znaków) wykładnika jednomianu */
#define EXP_MAX_LENGTH 10
/** Polecenie nie wymaga parametrów */
#define NO_PARAMS 0
/** Błąd przy parsowaniu */
#define ERROR errno != 0
/** Napis nie zawiera liczb*/
#define NO_MATCH end_ptr == line
/** Napis częśćiowo sparsowany*/
#define PARTLY_READ *end_ptr != '\0'
/** Baza dziesiętna */
#define DECIMAL_BASE 10

/**
 * Typ wielomianu
 */
typedef enum PolyType {
    INNER, ///< zwykły wielomian
    OUTER ///< wielomian w jednomianie
} PolyType;

void PrintError(int line_number, Errors error) {
    switch (error) {
        case WRONG_COMMAND:
            fprintf(stderr, "ERROR %d WRONG COMMAND\n", line_number);
            break;
        case WRONG_VARIABLE:
            fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", line_number);
            break;
        case WRONG_VALUE:
            fprintf(stderr, "ERROR %d AT WRONG VALUE\n", line_number);
            break;
        case STACK_UNDERFLOW:
            fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", line_number);
            break;
        case WRONG_POLY:
            fprintf(stderr, "ERROR %d WRONG POLY\n", line_number);
            break;
        case WRONG_PARAMETER:
            fprintf(stderr, "ERROR %d COMPOSE WRONG PARAMETER\n", line_number);
            break;
        default:
            break;
    }
}

void SkipLine() {
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
    if (c == EOF) {
        ungetc(c, stdin);
    }
}

int PeekChar() {
    int next = getchar();
    ungetc(next, stdin);
    return next;
}

/**
 * W zależności od typu wielomianu sprawdza czy @p c
 * kończy wczytywanie wielomianu
 * @param[in] c : symbol
 * @param[in] type : typ wielomianu
 * @return Czy @p c kończy wczytywanie wielomianu?
 */
static bool IsPolyEnd(int c, PolyType type) {
    bool result;
    switch (type) {
        case OUTER:
            result = (c == '\n' || c == EOF);
            if (c == EOF) {
                ungetc(c, stdin);
            }
            break;
        case INNER:
            result = (c == ',');
            break;
        default:
            result = false;
    }
    return result;
}

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wartość parametru polecenia DEG_BY.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[out] var_idx : wartość parametru polecenia DEG_BY
 * @return Czy się udało sparsować wiersz?
 */
static bool ParseVariable(size_t *var_idx) {
    /* liczba nie może się zaczynać od + */
    if (PeekChar() != '+') {
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        /* pomijamy zera */
        bool find_zero = false;
        while ((c = getchar()) == '0') {
            find_zero = true;
        }
        /* pojedyncze zero */
        if (find_zero && (c == '\n' || c == EOF)) {
            line[i++] = '0';
        }

        /* wczytujemy liczbę */
        while (i < NUMBER_MAX_LENGTH && isdigit(c)) {
            line[i++] = (char) c;
            c = getchar();
        }
        line[i] = '\0';

        /* cała liczba wczytana*/
        if (c == '\n' || c == EOF) {
            if (c == EOF) {
                ungetc(c, stdin);
            }

            char *end_ptr;
            errno = 0;
            unsigned long long result;
            result = strtoull(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ) {
                return false;
            } else {
                *var_idx = result;
                return true;
            }

        }
        /* przekroczenie max długośći lub błędny znak */
        else {
            SkipLine();
            return false;
        }
    } else {
        SkipLine();
        return false;
    }
}

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wartość parametru polecenia AT.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[out] x : wartość parametru polecenia AT
 * @return Czy się udało sparsować wiersz?
 */
static bool ParseValue(poly_coeff_t *x) {
    /* liczba nie może się zaczynać od + */
    if (PeekChar() != '+') {
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        if (PeekChar() == '-') {
            line[i++] = (char) getchar();
        }

        /* pomijamy zera */
        bool find_zero = false;
        while ((c = getchar()) == '0') {
            find_zero = true;
        }
        /* pojedyncze zero */
        if (find_zero && (c == '\n' || c == EOF)) {
            line[i++] = '0';
        }

        /* wczytujemy liczbę */
        while (i < NUMBER_MAX_LENGTH && isdigit(c)) {
            line[i++] = (char) c;
            c = getchar();
        }
        line[i] = '\0';

        /* cała liczba wczytana*/
        if (c == '\n' || c == EOF) {
            if (c == EOF) {
                ungetc(c, stdin);
            }

            char *end_ptr;
            errno = 0;
            long result;
            result = strtol(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ) {
                return false;
            } else {
                *x = result;
                return true;
            }

        }
        /* przekroczenie max długośći lub błędny znak */
        else {
            SkipLine();
            return false;
        }
    } else {
        SkipLine();
        return false;
    }

}

bool ParseCommand(int line_number, Commands *command, CommandParams *param) {
    char line[COMMAND_MAX_LENGTH + 1];
    param->x = NO_PARAMS;
    int i = 0;
    int c;

    c = getchar();
    while (i < COMMAND_MAX_LENGTH && (isupper(c) || c == '_')) {
        line[i++] = (char) c;
        c = getchar();
    }
    line[i] = '\0';

    if (c == '\n' || c == EOF) {
        if (c == EOF) {
            ungetc(c, stdin);
        }

        if (strcmp(line, "ZERO") == 0) {
            *command = ZERO;
        } else if (strcmp(line, "IS_COEFF") == 0) {
            *command = IS_COEFF;
        } else if (strcmp(line, "IS_ZERO") == 0) {
            *command = IS_ZERO;
        } else if (strcmp(line, "CLONE") == 0) {
            *command = CLONE;
        } else if (strcmp(line, "ADD") == 0) {
            *command = ADD;
        } else if (strcmp(line, "MUL") == 0) {
            *command = MUL;
        } else if (strcmp(line, "NEG") == 0) {
            *command = NEG;
        } else if (strcmp(line, "SUB") == 0) {
            *command = SUB;
        } else if (strcmp(line, "IS_EQ") == 0) {
            *command = IS_EQ;
        } else if (strcmp(line, "DEG") == 0) {
            *command = DEG;
        } else if (strcmp(line, "PRINT") == 0) {
            *command = PRINT;
        } else if (strcmp(line, "POP") == 0) {
            *command = POP;
        } else if (strcmp(line, "DEG_BY") == 0) {
            PrintError(line_number, WRONG_VARIABLE);
            return false;
        } else if (strcmp(line, "AT") == 0) {
            PrintError(line_number, WRONG_VALUE);
            return false;
        } else if (strcmp(line, "COMPOSE") == 0) {
            PrintError(line_number, WRONG_PARAMETER);
            return false;
        } else {
            PrintError(line_number, WRONG_COMMAND);
            return false;
        }
    } else {
        if (strcmp(line, "DEG_BY") == 0) {
            *command = DEG_BY;
            if (!ParseVariable(&param->var_idx) || c != ' ') {
                PrintError(line_number, WRONG_VARIABLE);
                return false;
            }
        } else if (strcmp(line, "AT") == 0) {
            *command = AT;
            if (!ParseValue(&param->x) || c != ' ') {
                PrintError(line_number, WRONG_VALUE);
                return false;
            }
        } else if (strcmp(line, "COMPOSE") == 0) {
            *command = COMPOSE;
            if (!ParseVariable(&param->count) || c != ' ') {
                PrintError(line_number, WRONG_PARAMETER);
                return false;
            }
        } else {
            PrintError(line_number, WRONG_COMMAND);
            SkipLine();
            return false;
        }
    }
    return true;
}

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wartość współczynnika jednomianu.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[out] coeff : wartość współczynnika jednomianu
 * @param[in] type : typ wielomianu
 * @return Czy się udało sparsować wiersz?
 */
static bool ParseCoeff(poly_coeff_t *coeff, PolyType type) {
    /* liczba nie może się zaczynać od + */
    if (PeekChar() != '+') {
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        if (PeekChar() == '-') {
            line[i++] = (char) getchar();
        }

        /* pomijamy zera */
        bool find_zero = false;
        while ((c = getchar()) == '0') {
            find_zero = true;
        }
        /* pojedyncze zero */
        if (find_zero && IsPolyEnd(c, type)) {
            line[i++] = '0';
        }

        /* wczytujemy liczbę */
        while (i < NUMBER_MAX_LENGTH && isdigit(c)) {
            line[i++] = (char) c;
            c = getchar();
        }
        line[i] = '\0';

        /* cała liczba wczytana*/
        if (IsPolyEnd(c, type)) {
            char *end_ptr;
            errno = 0;
            long result;
            result = strtol(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ) {
                if (type == INNER) {
                    SkipLine();
                }
                return false;
            } else {
                *coeff = result;
                return true;
            }

        }
        /* przekroczenie max długośći lub błędny znak */
        else {
            if (c != EOF && c != '\n') {
                SkipLine();
            } else if (c == EOF) {
                ungetc(c, stdin);
            }
            return false;
        }
    } else {
        SkipLine();
        return false;
    }
}

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wielomian.
 * Jeśli wykryje błąd, wypisuje komunikat o błędzie
 * na standardowe wyjście błędów i zwraca false.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[in] line_number : numer wiersza, w którym znajduje się błąd
 * @param[out] poly : wielomian
 * @param[in] type : typ wielomianu
 * @return Czy się udało sparsować wiersz?
 */
static bool ParsePolyHelper(int line_number, Poly *poly, PolyType type) {
    int c = PeekChar();
    if (isdigit(c) || c == '-') {
        poly_coeff_t coeff;
        if (ParseCoeff(&coeff, type)) {
            *poly = PolyFromCoeff(coeff);
            return true;
        } else {
            PrintError(line_number, WRONG_POLY);
            return false;
        }
    } else if (c == '(') {
        Vector v = VectorCreate();
        bool end = false;
        while (!end) {
            Mono mono;
            if (ParseMono(line_number, &mono)) {
                VectorAdd(&v, &mono);
                int ch = PeekChar();
                if (ch == '+') {
                    /* wczytujemy następny jednomian */
                    ch = getchar();
                    end = false;
                } else if (IsPolyEnd(ch, type)) {
                    /* ostatni jednomian został wczytany */
                    ch = getchar();
                    end = true;
                } else {
                    SkipLine();
                    /* Usuwamy z pamięci wszystkie dotychczas wczytane jednomiany */
                    VectorDestroy(&v);
                    PrintError(line_number, WRONG_POLY);
                    return false;
                }
            } else {
                /* Usuwamy z pamięci wszystkie dotychczas wczytane jednomiany */
                VectorDestroy(&v);
                return false;
            }
        }
        *poly = PolyAddMonos(v.size, v.array);
        /* poly jest właścicielem jednomianów z wektora */
        free(v.array);
        return true;
    } else {
        SkipLine();
        PrintError(line_number, WRONG_POLY);
        return false;
    }

}

/**
 * Przetwarza wiersz ze standardowego wejśćia i zwraca wartość wykładnika jednomianu.
 * Jeśli nie ma więcej znakówów do odczytania
 * (EOF na końcu wiersza), umieszcza EOF z powrotem w strumieniu wejściowym.
 * @param[out] exp : wartość wykładnika jednomianu
 * @return Czy się udało sparsować wiersz?
 */
static bool ParseExp(poly_exp_t *exp) {
    /* liczba nie może się zaczynać od + */
    if (PeekChar() != '+') {
        char line[EXP_MAX_LENGTH + 1];
        int i = 0;
        int c;

        /* pomijamy zera */
        bool find_zero = false;
        while ((c = getchar()) == '0') {
            find_zero = true;
        }
        /* pojedyncze zero */
        if (find_zero && c == ')') {
            line[i++] = '0';
        }

        /* wczytujemy liczbę */
        while (i < EXP_MAX_LENGTH && isdigit(c)) {
            line[i++] = (char) c;
            c = getchar();
        }
        line[i] = '\0';

        /* cała liczba wczytana*/
        if (c == ')') {
            char *end_ptr;
            errno = 0;
            unsigned long result;
            result = strtoul(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ || result > INT_MAX) {
                SkipLine();
                return false;
            } else {
                *exp = result;
                return true;
            }
        }
        /* przekroczenie max długośći lub błędny znak */
        else {
            if (c != EOF && c != '\n') {
                SkipLine();
            } else if (c == EOF) {
                ungetc(c, stdin);
            }
            return false;
        }
    } else {
        SkipLine();
        return false;
    }
}

bool ParseMono(int line_number, Mono *mono) {
    int c = PeekChar();
    if (c == '(') {
        c = getchar();
        Poly poly;
        if (ParsePolyHelper(line_number, &poly, INNER)) {
            /* , wczytany w ParsePolyHelper */
            poly_exp_t exp;
            if (ParseExp(&exp)) {
                /* ) wczytany w ParseExp */
                *mono = MonoFromPoly(&poly, exp);
                return true;
            } else {
                PolyDestroy(&poly);
                PrintError(line_number, WRONG_POLY);
                return false;
            }
        } else {
            return false;
        }
    } else {
        SkipLine();
        PrintError(line_number, WRONG_POLY);
        return false;
    }

}

bool ParsePoly(int line_number, Poly *poly) {
    return ParsePolyHelper(line_number, poly, OUTER);
}