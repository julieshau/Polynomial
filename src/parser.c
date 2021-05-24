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

#define COMMAND_MAX_LENGTH 8
#define NUMBER_MAX_LENGTH 20
#define EXP_MAX_LENGTH 10
#define NO_PARAMS 0
#define ERROR errno != 0
#define NO_MATCH end_ptr == line
#define PARTLY_READ *end_ptr != '\0'
#define DECIMAL_BASE 10

typedef enum Type {
    INNER,
    OUTER
} Type;


void PrintError(int line_number, Errors error){
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
        default:
            break;
    }
}

void SkipLine(){
    int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
    if (c == EOF){
        ungetc(c, stdin);
    }
}

int PeekChar(){
    int next = getchar();
    ungetc(next, stdin);
    return next;
}

bool ReadVariable(size_t* var_idx){
    if (PeekChar() != '+'){
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        //pomijamy zera(maybe with peek)
        while ((c = getchar()) == '0');
        if (c == '\n' || c == EOF){ //jedno zero
            line[i++] = '0';
        }

        //wczytujemy co się da
        while (i < NUMBER_MAX_LENGTH && isdigit(c)){
            line[i++] = (char)c;
            c = getchar();
        }
        line[i] = '\0';

        if (c == '\n' || c == EOF) {
            if (c == EOF) {
                ungetc(c, stdin);
            }

            char* end_ptr;
            errno = 0;
            unsigned long long result;
            result = strtoull(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ){
//            if (errno != 0 || end_ptr == line || *end_ptr != '\0'){
                return false;
            }
            else{
                *var_idx = result;
                return true;
            }

        }
        //przekroczenie max długośći
        else{
            SkipLine();
            return false;
        }
    }
    //nie dozwolony + przed liczbą
    else{
        SkipLine();
        return false;
    }
}

bool ReadValue(poly_coeff_t* x){
    //-9223372036854775808 lub większa od 9223372036854775807.
    if (PeekChar() != '+'){
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        if (PeekChar() == '-'){
            line[i++] = (char)getchar();
        }

        //pomijamy zera(maybe with peek)
        bool find_zero = false;
        while ((c = getchar()) == '0'){
            find_zero = true;
        }
        if (find_zero && (c == '\n' || c == EOF)){ //jedno zero
            line[i++] = '0';
        }

        //wczytujemy co się da
        while (i < NUMBER_MAX_LENGTH && isdigit(c)){
            line[i++] = (char)c;
            c = getchar();
        }
        line[i] = '\0';

        if (c == '\n' || c == EOF) {
            if (c == EOF) {
                ungetc(c, stdin);
            }

            char* end_ptr;
            errno = 0;
            long result;
            result = strtol(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ){
//            if (errno != 0 || end_ptr == line || *end_ptr != '\0'){
                return false;
            }
            else{
                *x = result;
                return true;
            }

        }
        //przekroczenie max długośći
        else{
            SkipLine();
            return false;
        }
    }
    //nie dozwolony + przed liczbą
    else{
        SkipLine();
        return false;
    }

}

bool ParseCommand(int line_number, Commands* command, CommandParams* param){
    char line[COMMAND_MAX_LENGTH + 1];
    param->x = NO_PARAMS;
    int i = 0;
    int c;

    c = getchar();
//    while (i < COMMAND_MAX_LENGTH && c != '\n' && c != ' ' && c != EOF){
    while (i < COMMAND_MAX_LENGTH && c != '\n' && !isspace(c) && c != EOF){
        line[i++] = (char)c;
        c = getchar();
    }
    line[i] = '\0';

    if (c == '\n' || c == EOF){
        if (c == EOF){
            ungetc(c, stdin);
        }

        if (strcmp(line, "ZERO") == 0){
            *command = ZERO;
        }
        else if (strcmp(line, "IS_COEFF") == 0){
            *command = IS_COEFF;
        }
        else if (strcmp(line, "IS_ZERO") == 0){
            *command = IS_ZERO;
        }
        else if (strcmp(line, "CLONE") == 0){
            *command = CLONE;
        }
        else if (strcmp(line, "ADD") == 0){
            *command = ADD;
        }
        else if (strcmp(line, "MUL") == 0){
            *command = MUL;
        }
        else if (strcmp(line, "NEG") == 0){
            *command = NEG;
        }
        else if (strcmp(line, "SUB") == 0){
            *command = SUB;
        }
        else if (strcmp(line, "IS_EQ") == 0){
            *command = IS_EQ;
        }
        else if (strcmp(line, "DEG") == 0){
            *command = DEG;
        }
        else if (strcmp(line, "PRINT") == 0){
            *command = PRINT;
        }
        else if (strcmp(line, "POP") == 0){
            *command = POP;
        }
        else if (strcmp(line, "DEG_BY") == 0){
            PrintError(line_number, WRONG_VARIABLE);
            return false;
        }
        else if (strcmp(line, "AT") == 0){
            PrintError(line_number, WRONG_VALUE);
            return false;
        }
        else{
            PrintError(line_number, WRONG_COMMAND);
            return false;
        }
    }
    else{
        if (strcmp(line, "DEG_BY") == 0){
            *command = DEG_BY;
            if (!ReadVariable(&param->var_idx) || c != ' '){ //todo check if it works
                PrintError(line_number, WRONG_VARIABLE);
                return false;
            }
        }
        else if (strcmp(line, "AT") == 0){
            *command = AT;
//            if (c != ' '){
//                SkipLine();
//                PrintError(line_number, WRONG_VALUE);
//                return false;
//            }
//            else if (!ReadValue(&param->x)){
//                PrintError(line_number, WRONG_VALUE);
//                return false;
//            }
            if (!ReadValue(&param->x) || c != ' '){ //todo
                PrintError(line_number, WRONG_VALUE);
                return false;
            }
        }
        else{
            PrintError(line_number, WRONG_COMMAND);
            SkipLine();
            return false;
        }
    }
    return true;
}

bool IsPolyEnd(int c, Type type){
    bool result;
    switch (type) {
        case OUTER:
            result = (c == '\n' || c == EOF);
            if (c == EOF){
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

bool ParseCoeff(poly_coeff_t* coeff, Type type){

    if (PeekChar() != '+'){
        char line[NUMBER_MAX_LENGTH + 1];
        int i = 0;
        int c;

        if (PeekChar() == '-'){
            line[i++] = (char)getchar();
        }

        //pomijamy zera
        bool find_zero = false;
        while ((c = getchar()) == '0'){
            find_zero = true;
        }
        if (find_zero && IsPolyEnd(c, type)){ //jedno zero
            line[i++] = '0';
        }

        //wczytujemy ile się da
        while (i < NUMBER_MAX_LENGTH && isdigit(c)){
            line[i++] = (char)c;
            c = getchar();
        }
        line[i] = '\0';

        if (IsPolyEnd(c, type)) {
            char* end_ptr;
            errno = 0;
            long result;
            result = strtol(line, &end_ptr, DECIMAL_BASE);
            if (ERROR || NO_MATCH || PARTLY_READ){ //todo maybe skipline up
//            if (errno != 0 || end_ptr == line || *end_ptr != '\0'){
                if (type == INNER){
                    SkipLine();
                }
                return false;
            }
            else{
                *coeff = result;
                return true;
            }

        }
        //przekroczenie max długośći lub niepoprawny znak
        else{
            SkipLine();
            return false;
        }
    }
    //nie dozwolony + przed liczbą
    else{
        SkipLine();
        return false;
    }
}

bool ParsePolyHelper(int line_number, Poly* poly, Type type){
    int c = PeekChar();
    if (isdigit(c) || c == '-'){
        poly_coeff_t  coeff;
        if (ParseCoeff(&coeff, type)){
            *poly = PolyFromCoeff(coeff);
            return true;
        }
        else{
            PrintError(line_number, WRONG_POLY);
            return false;
        }
    }
    else if (c == '('){
        Vector v = VectorCreate();
        bool end = false;
        while (!end){
            Mono mono;
            if (ParseMono(&mono, line_number)){ //mono skip line, mono destroy if error?
                VectorAdd(&v, &mono);
                int ch = PeekChar();
                if (ch == '+'){
                    ch = getchar();
                    end = false;
                }
                else if (IsPolyEnd(ch, type)){
                    ch = getchar();
                    end = true;
                }
                else{
                    SkipLine();
                    VectorDestroy(&v);
                    PrintError(line_number, WRONG_POLY);
                    return false;
                }
            }
            else{
                VectorDestroy(&v);
                return false;
            }
        }
        *poly = PolyAddMonos(v.size, v.array);
        free(v.array);
        return true;
    }
    else{
        SkipLine();
        PrintError(line_number, WRONG_POLY);
        return false;
    }

}

bool ReadExp(poly_exp_t* exp){
    if (PeekChar() != '+'){
        char line[EXP_MAX_LENGTH + 1];
        int i = 0;
        int c;

        //pomijamy zera(maybe with peek)
        bool find_zero = false;
        while ((c = getchar()) == '0'){
            find_zero = true;
        }
        if (find_zero && c == ')'){ //jedno zero
            line[i++] = '0';
        }

        //wczytujemy co się da
        while (i < NUMBER_MAX_LENGTH && isdigit(c)){
            line[i++] = (char)c;
            c = getchar();
        }
        line[i] = '\0';

        if (c == ')') {

            char* end_ptr;
            errno = 0;
            unsigned long result;
            result = strtoul(line, &end_ptr, DECIMAL_BASE); //todo
            if (ERROR || NO_MATCH || PARTLY_READ || result > INT_MAX){
//            if (errno != 0 || end_ptr == line || *end_ptr != '\0'){
                SkipLine();
                return false;
            }
            else{
                *exp = result;
                return true;
            }

        }
            //przekroczenie max długośći
        else{
            SkipLine();
            return false;
        }
    }
        //nie dozwolony + przed liczbą
    else{
        SkipLine();
        return false;
    }
}

bool ParseMono(Mono* mono, int line_number){
    int c = PeekChar();
    if (c == '('){
        c = getchar();
        Poly poly;
        if (ParsePolyHelper(line_number, &poly, INNER)){
            //, is checked in poly
            poly_exp_t exp;
            if (ReadExp(&exp)){
                *mono = MonoFromPoly(&poly, exp);
                return true;
            }
            else {
                PolyDestroy(&poly);
                PrintError(line_number, WRONG_POLY);
                return false;
            }
        }
        else{
            return false;
        }
    }
    else{
        SkipLine();
        PrintError(line_number, WRONG_POLY);
        return false;
    }

}

bool ParsePoly(int line_number, Poly* poly){
    return ParsePolyHelper(line_number, poly, OUTER);
}