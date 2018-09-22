/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "lexer.c"
#include "parserDef.h"

int findNT(char* str);//returns index of NT in enum/array
int findT(char* str);//returns index of T in enum
lhs* createGrammar();//returns grammar structure, input the pointer of file with grammar
int hasNullRule(lhs* currLHS); //checks for null rule in rhs of the lhs non terminal
long long getFirst(int val, int type, lhs* gr, FirstAndFollow* ff); //computes first set
long long getFollow(int val, lhs* gr, FirstAndFollow* ff); //computes follow set
FirstAndFollow* computeFirstAndFollowSets(lhs* gr);
table* createParseTable(FirstAndFollow* F, lhs* gr);
void printBits(long long a);
void printTks(long long a);
void printParseTable(table* T);
void printRule(rhs* currRHS);
stackNode* newNode(symbol* data);
int isEmpty(stackNode* root);
void push(stackNode** root, symbol* data);
symbol* pop(stackNode** root);
symbol* top(stackNode* root);
void pushRule(stackNode** S, rhs* rule);
void parsingError();
void printStack(stackNode* S);
void addToParseTree(parseTree* parent, rhs* rule);
parseTree* newParseTreeNode(symbol* sym, parseTree* parent);
parseTree* getNextRightNode(parseTree* currNode);
parseTree* parseInputSourceCode(table* T, FirstAndFollow* F);
void printSimpleParseTree(parseTree* root);
void printSimpleTreeNode(parseTree* node);