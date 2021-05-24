#include "executor.h"
#include "parser.h"
#include "poly.h"
#include "stack.h"
#include <stdio.h>
#include <string.h>

void ZeroCommand(Stack* stack){
    Poly p = PolyZero();
    Push(stack, &p);
}

void IsCoeffCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        printf("%d\n", PolyIsCoeff(&p));
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void IsZeroCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        printf("%d\n", PolyIsZero(&p));
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void CloneCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        Poly clone = PolyClone(&p);
        Push(s, &clone);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void AddCommand(Stack* s, int line_number){
    if (StackGetSize(s) > 1) {
        Poly first = Pop(s);
        Poly second = Pop(s);
        Poly sum = PolyAdd(&first, &second);
        Push(s, &sum);
        PolyDestroy(&first);
        PolyDestroy(&second);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void MulCommand(Stack* s, int line_number){
    if (StackGetSize(s) > 1) {
        Poly first = Pop(s);
        Poly second = Pop(s);
        Poly mul = PolyMul(&first, &second);
        Push(s, &mul);
        PolyDestroy(&first);
        PolyDestroy(&second);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void NegCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Pop(s);
        Poly neg = PolyNeg(&p);
        Push(s, &neg);
        PolyDestroy(&p);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void SubCommand(Stack* s, int line_number){
    if (StackGetSize(s) > 1) {
        Poly first = Pop(s);
        Poly second = Pop(s);
        Poly sub = PolySub(&first, &second);
        Push(s, &sub);
        PolyDestroy(&first);
        PolyDestroy(&second);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void IsEqCommand(Stack* s, int line_number){
    if (StackGetSize(s) > 1) {
        Poly first = Pop(s);
        Poly second = Top(s);
        Push(s, &first);
        printf("%d\n", PolyIsEq(&first, &second));
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void DegCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        printf("%d\n", PolyDeg(&p));
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void DegByCommand(Stack* s, int line_number, size_t var_idx){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        printf("%d\n", PolyDegBy(&p, var_idx));
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void AtCommand(Stack* s, int line_number, poly_coeff_t x){
    if (!StackIsEmpty(s)) {
        Poly p = Pop(s);
        Poly at = PolyAt(&p, x);
        Push(s, &at);
        PolyDestroy(&p);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void PrintCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Top(s);
        PolyPrint(&p);
        printf("\n");
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void PopCommand(Stack* s, int line_number){
    if (!StackIsEmpty(s)) {
        Poly p = Pop(s);
        PolyDestroy(&p);
    }
    else {
        PrintError(line_number, STACK_UNDERFLOW);
    }
}

void ExecuteCommand(Commands command, CommandParams param, Stack* stack, int line_number){
    switch (command) {
        case ZERO:
            ZeroCommand(stack);
            break;
        case IS_COEFF:
            IsCoeffCommand(stack, line_number);
            break;
        case IS_ZERO:
            IsZeroCommand(stack, line_number);
            break;
        case CLONE:
            CloneCommand(stack, line_number);
            break;
        case ADD:
            AddCommand(stack, line_number);
            break;
        case MUL:
            MulCommand(stack, line_number);
            break;
        case NEG:
            NegCommand(stack, line_number);
            break;
        case SUB:
            SubCommand(stack, line_number);
            break;
        case IS_EQ:
            IsEqCommand(stack, line_number);
            break;
        case DEG:
            DegCommand(stack, line_number);
            break;
        case DEG_BY:
            DegByCommand(stack, line_number, param.var_idx);
            break;
        case AT:
            AtCommand(stack, line_number, param.x);
            break;
        case PRINT:
            PrintCommand(stack, line_number);
            break;
        case POP:
            PopCommand(stack, line_number);
            break;
        default:
            break;
    }
}

