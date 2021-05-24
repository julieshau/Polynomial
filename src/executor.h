#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "stack.h"
#include "poly.h"


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
    POP
} Commands;

typedef union CommandParams {
    size_t var_idx;
    poly_coeff_t x;
} CommandParams;

void ExecuteCommand(Commands command, CommandParams param, Stack* stack, int line_number);

#endif //EXECUTOR_H
