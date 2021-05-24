#include "poly.h"
#include "stack.h"
#include "parser.h"
#include "executor.h"
#include <stdio.h>
#include "ctype.h"

int main(){
    Stack s = StackCreate();
    int line_number = 1;
    int c;
    Commands commands;
    CommandParams params;
    Poly poly;
    while((c = getchar()) != EOF) {
        ungetc(c, stdin);
        if (isalpha(c)){
            if(ParseCommand(line_number, &commands, &params)){
                ExecuteCommand(commands, params, &s, line_number);
            }
        }
        else if (c == '#' || c == '\n' || c == '\0'){
            SkipLine();
        }
        else if(c == '(' || c == '-' || isdigit(c)){
            if(ParsePoly(line_number, &poly)){
                Push(&s, &poly);
            }
        }
        else{
            SkipLine();
            PrintError(line_number, WRONG_POLY);
        }
        ++line_number;
    }
    StackDestroy(&s);
    return 0;
}


