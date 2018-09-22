/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int currLine;
FILE *fp;
int buffSize = 4096; //4KB block size 
int begin, fwd; //global vars
char* buffer; //global pointer

typedef enum{
	ASSIGNOP, COMMENT, FUNID, ID, NUM, RNUM, STR, END, INT, REAL, STRING, MATRIX, MAIN, SQO, SQC, OP, CL, SEMICOLON,
	COMMA, IF, ELSE, ENDIF, READ, PRINT, FUNCTION, PLUS, MINUS, MUL, DIV, SIZE, AND, OR, NOT, LT, LE, EQ, GT, GE, NE, EPS, $
}token; //tokens and other symbols

char* Tlist[] = {"ASSIGNOP", "COMMENT", "FUNID", "ID", "NUM", "RNUM", "STR", "END", "INT", "REAL", "STRING", "MATRIX", "MAIN", "SQO", "SQC", "OP", "CL", "SEMICOLON",
	"COMMA", "IF", "ELSE", "ENDIF", "READ", "PRINT", "FUNCTION", "PLUS", "MINUS", "MUL", "DIV", "SIZE", "AND", "OR", "NOT", "LT", "LE", "EQ", "GT", "GE", "NE", "EPS", "$"};

typedef struct{
	int line;
	char* val;
	int name;
	int num;
	float rnum;
}tokenInfo;

//lookup table
char *k[] = {"_main", "int", "end", "real", "string", "matrix", "if", "else", "endif", "print" , "read", "function"};
char *v[] = {"MAIN", "INT", "END", "REAL", "STRING", "MATRIX", "IF", "ELSE", "ENDIF", "PRINT", "READ", "FUNCTION"};