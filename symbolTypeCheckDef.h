/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ast.c"

//rest of the structs declared in parserDef.h as they are added to tree nodes

typedef struct st{
	symbolTableNode** table; //hashtable populated with entries of symbol table
	struct st* parent;//static parent
	parseTree* root; //function
	int lastAddr;
}symbolTable;
