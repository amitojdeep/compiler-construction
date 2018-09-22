/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "codeGenerator.c"

int main(int argc, char *argv[]){
	int ctrl = 1;
	currLine = 1;
	if(argc != 2){
		printf("Please Execute as ./stage2exe testcase.txt\n");
		return 0;
	}
	fp = fopen(argv[1],"r");
	if(!fp){
		printf("User program file not found, please verify filename\n");
		return 0;
	}
	//do the pre requisites
	getBuffer(fp);
	lhs* gr = createGrammar();
	FirstAndFollow* F = computeFirstAndFollowSets(gr);
	table* T = createParseTable(F, gr);
	int i;
	long long f;
	parseTree* PT;
	double PTnodes, ASTnodes, compress, PTsize, ASTsize, PTmem, ASTmem;
	printf("LEVEL 4: Symbol table/ AST/ Semantic Rules/ Type Checking modules work, Code generation partly implemented\n");
	while(ctrl){
		printf("\nChoose an option to continue:\n0: Exit\n1: Print Tokens\n2: Parsing\n3: AST\n4: Memory Comparison\n5: Print Symbol Table\n6: Compiling for Syntactical and Semantic Errors\n7: Assembly Code Generation\n");
		scanf("%d", &ctrl);
		switch(ctrl){
			case 0:{
				printf("\nExit Successful\n");
				break;
			}
			case 1:{
				printf("\n------------------------------------------------Token list------------------------------------------------\n");
				currLine = 1;
				rewind(fp);// rewind to beginning
				getBuffer(fp);//initialize twin buffer
				printf("\n   Token\t\t\t\tLexeme\t\t\t\tLine\n");
				while(1){
					tokenInfo* tk = getNextToken();
					if(tk->name == -2)break; //EOF
					if(tk->val == NULL)continue;
					printf("\n%*s%*s\t\t%d", 9,Tlist[tk->name], 35, tk->val, tk->line);
				}
				break;
			}
			case 2:{
				printf("\n-------------------------------------------------Parsing--------------------------------------------------\n");
				PT = parseInputSourceCode(T,F);//first and follow is passed for error recovery
				printSimpleParseTree(PT); //print on console
				break;
			}
			case 3:{
				PT = parseInputSourceCode(T,F);
				createAST(PT);
				printf("\nAST printed inorder : First child, Parent, Other children\n");
				printSimpleParseTree(PT);//print ast on console
				break;
			}
			case 4:{
				PT = parseInputSourceCode(T,F);
				PTnodes = 0;
				calculateNodes(PT, &PTnodes);
				ASTsize = PTsize = sizeof(parseTree*); //using same struct for both, modifying semantics of pointers
				printf("Parse tree Number of nodes = %0f\t",PTnodes);
				PTmem = PTnodes*PTsize;
				printf("Allocated Memory = %.0f\n",PTmem);
				createAST(PT);
				ASTnodes = 0;
				calculateNodes(PT, &ASTnodes);
				printf("AST Number of nodes = %.0f\t",ASTnodes);
				ASTmem = ASTnodes*ASTsize;
				printf("Allocated Memory = %.0f\n",ASTmem);
				compress = ((PTmem - ASTmem)/PTmem)*100;
				printf("Compression percentage = %.2f\n",compress);
				break;
			}
			case 5:{//also does type checking when creating symbol table
				parseTree* PT = parseInputSourceCode(T,F);
				createAST(PT);
				symbolTable* st = getSymbolTable(NULL, PT);
				createSymbolTable(st, PT, 0);
				printf("\n Symbol Table \n");
				printSymbolTable(st,0,"_main","none");
				printf("\nNote: 1.Semantic and type errors are also printed as they are generated in same pass as creation of symbol table\n");
				printf("      2.Unassigned matrices and strings, if any have 0 width and 0 offset");
				break;
			}
			case 6:{

				parseTree* PT = parseInputSourceCode(T,F);
				createAST(PT);
				symbolTable* st = getSymbolTable(NULL, PT);
				printf("\nType Checking\n");
				createSymbolTable(st, PT, 0);//also checks type
				break;
			}
			case 7:{
				parseTree* PT = parseInputSourceCode(T,F);
				createAST(PT);
				printSimpleParseTree(PT);
				symbolTable* st = getSymbolTable(NULL, PT);
				createSymbolTable(st, PT, 0);//also checks type
				printf("\nCode Generation partly implemented, Symbol table with temporaries generated:\n");
				FILE* fp1;
				codeGenerator(PT, st, fp1);
				printSymbolTable(st,0,"_main","none");
				break;
			}
			default:{
				printf("\nInvalid input, please retry\n");
				break;
			}
		}	

	}
	
}