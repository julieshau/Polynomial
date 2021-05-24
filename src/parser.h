#ifndef PARSER_H
#define PARSER_H

#include "executor.h"
#include <stdbool.h>

typedef enum Errors {
    WRONG_COMMAND,
    WRONG_VARIABLE,
    WRONG_VALUE,
    STACK_UNDERFLOW,
    WRONG_POLY
} Errors;

bool ParseCommand(int line_number, Commands* command, CommandParams* param);
int PeekChar();
void SkipLine();
void PrintError(int line_number, Errors error);
bool ParsePoly(int line_number, Poly* poly);
bool ParseMono(Mono* mono, int line_number);

#endif
