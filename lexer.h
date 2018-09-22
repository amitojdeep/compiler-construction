/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexerDef.h"

void getBuffer(FILE* fp);

void incFwd();

void decFwd();

void error();

int getVal();

FILE *getStream(FILE *fp, char* buff, int size);

char* removeComments();

tokenInfo* getNextToken();