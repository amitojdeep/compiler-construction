/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "symbolTypeCheckDef.h"

int getHash(char* str, int size);
symbolTableNode* querySymbolTable(symbolTable* st, char* str);
symbolTableNode* getSymbolTableNode(tokenInfo* tk, int type, int offset, int len, symbolTable* child);
symbolTable* getSymbolTable(symbolTable* parent, parseTree* root);
symbolTable* addToSymbolTable(symbolTable* st, tokenInfo* tk, int type, int offset, int len, parseTree* root);
int createSymbolTable(symbolTable* st,parseTree* root, int offset);
void printSymbolTable(symbolTable* st, int level,char* funName, char* parentName);
void printSymbolTableNode(symbolTableNode* node, char* funName, char* parentName, int level);
void calculateNodes(parseTree* root, double* count);
tuple getMatrixDim(parseTree* root);
int arithCheck(int type, parseTree* root, symbolTable* st);
int stringCheck(parseTree* root, symbolTable* st);
tuple matrixCheck(parseTree* root, symbolTable* st);
