/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "codeGenerator.h"

int tempCount = 0;

int getTemp(){
	tempCount++;
	return tempCount;
}

symbolTableNode* insertTemp(symbolTable* st, int type){
	//create token
	tokenInfo* tk = (tokenInfo*)malloc(sizeof(tokenInfo));
	tk->val = (char*)malloc(sizeof(char)*10);
	tk->line = 0;
	char tempNum[4] = "\0\0\0\0";
	sprintf(tempNum, "%d", getTemp());
	char tempT[3] = "_t\0";
	strcpy(tk->val, tempT);
	strcat(tk->val, tempNum);
	tk->name = type;
	int len = 0;
	if(type == INT)len = 2;
	else if(type == REAL)len = 4;
	addToSymbolTable(st, tk, type, st->lastAddr, len, NULL);
	return querySymbolTable(st, tk->val);
}

void createTemporaries(symbolTable* st, parseTree* root, int type){
	if(!root)return;
	if(root->val == assignmentStmt_type1){
		symbolTableNode* stNode = querySymbolTable(st, root->child->token->val);
		if(!stNode){
			printf("Code generation not possible for semantically incorrect code\n");
			exit(0);
		}
		else type = stNode->type;
		createTemporaries(st, root->child->next, type);
	}
	else if(root->val == PLUS || root->val== MINUS || root->val == DIV || root->val == MUL){			
		root->tempVar = insertTemp(st, type); //assuming semantic correctness
		if(root->child->val!=ID)createTemporaries(st, root->child, type);
		if(root->child->next->val!=ID)createTemporaries(st, root->child->next, type);
	}
	//keep traversing
	else if(root->child){
		parseTree* nextChild;
		createTemporaries(st, root->child, type);
		if(root->child && root->child->next){
			nextChild = root->child->next;
			while(nextChild){
				createTemporaries(st, nextChild, type);
				nextChild = nextChild->next;
			}
		}
	}
	return;
}

void codeGenerator(parseTree* root, symbolTable* st, FILE* fp){
	createTemporaries(st,root, 0);
}
