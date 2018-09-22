/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "symbolTypeCheck.c"

int getTemp();
symbolTableNode* insertTemp(symbolTable* st, int type);
void createTemporaries(symbolTable* st, parseTree* root, int type);
void codeGenerator(parseTree* root, symbolTable* st, FILE* fp);
