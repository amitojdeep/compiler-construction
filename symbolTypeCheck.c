/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/

//this code creates symbol table and does type checking as well as semantic analysis in same pass
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "symbolTypeCheck.h"

void calculateNodes(parseTree* root, double* count){
	if(!root)return;
	(*count)++; //count self
	if(root->child)calculateNodes(root->child, count);
	parseTree* nextChild;
	if(root->child && root->child->next) 
		nextChild = root->child->next;//next child
	else return;
	while(nextChild){
		calculateNodes(nextChild, count);
		//printSimpleTreeNode(nextChild);
		nextChild = nextChild->next;
	}
}

void printSymbolTable(symbolTable* st, int level,char* funName, char* parentName){
	int i;
	for(i = 0; i < 50; i++){
		symbolTableNode* currNode = st->table[i];
		while(currNode){
			if(currNode->type == FUNID){
				//printf("Symbol Table of %s\n",currNode->lexeme);
				printSymbolTable(currNode->child,level+1,currNode->lexeme, funName);
			}
			else printSymbolTableNode(currNode,funName, parentName , level);
			currNode = currNode->next;
		}
	}	
}

void printSymbolTableNode(symbolTableNode* node, char* funName, char* parentName, int level){
	if(node->type == MATRIX) printf("%s\t%s\t%d\t%s\t%s,%d,%d\t%d\t%d\n",node->lexeme, funName, level, parentName, Tlist[node->type], node->dim.m, node->dim.n, node->width, node->offset);
	else if(node->type!=FUNID) printf("%s\t%s\t%d\t%s\t%s\t%d\t%d\n",node->lexeme, funName, level, parentName, Tlist[node->type], node->width, node->offset);
	//if(node->type == MATRIX)printf("%d\n", node->width);
}

int getHash(char* str, int size){
	//horner's rule for hashing
	//printf("%s\t", str);
	int len = strlen(str);
	//printf("%d\t", len);
	int i = 0;
	int h = 0;
	int x = 5;
	//if(str[i] == '_')i++;
	for(i = i; i < len; i++){
		h = (int)(h + str[i]*(int)(pow(x,i)))%50;
	}
	//printf("%d\n", h);
	return h;
}

symbolTableNode* querySymbolTable(symbolTable* st, char* str){
	if(!st)return NULL;
	int hash = getHash(str,50);
	symbolTableNode* currNode = st->table[hash];
	if(!currNode)return querySymbolTable(st->parent, str); //not found
	while(currNode){
		if(!strcmp(currNode->lexeme, str))return currNode;
		currNode = currNode->next;
	}
	return querySymbolTable(st->parent, str);;
}


symbolTableNode* getSymbolTableNode(tokenInfo* tk, int type, int offset, int len, symbolTable* child){
	symbolTableNode* node = (symbolTableNode*) malloc(sizeof(symbolTableNode));
	node->lexeme = (char*)malloc((strlen(tk->val))*sizeof(char));
	strcpy(node->lexeme,tk->val);
	node->type = type; //data type/funid
	node->offset = offset;
	node->width = len; //for funid, decide below for other
	node->assigned = 0; //var hasn't been assigned a value	
	if(child)node->child = child;
	else node->child = NULL;
	node->next = NULL;
	return node;
}

symbolTable* getSymbolTable(symbolTable* parent, parseTree* root){
	symbolTable* st = (symbolTable*)malloc(sizeof(symbolTable));
	st->table = (symbolTableNode**)malloc(sizeof(symbolTableNode*)*50); //calloc sets values to  null
	if(parent)st->parent = parent; //static parent
	else st->parent = NULL;
	st->root = root;
	st->lastAddr = 0;
	return st;
}

symbolTable* addToSymbolTable(symbolTable* st, tokenInfo* tk, int type, int offset, int len, parseTree* root){
	int hash = getHash(tk->val,50);
	symbolTableNode* currNode = st->table[hash];
	symbolTable* child = NULL;
	if(type == FUNID)child = getSymbolTable(st, root);
	if(!currNode){
		st->table[hash] = getSymbolTableNode(tk, type, offset, len, child);
	}
	else{
		while(currNode->next){ //loop till last node in bucket
			currNode = currNode->next;
		}
		currNode->next = getSymbolTableNode(tk, type, offset, len, child);
	}
	st->lastAddr+=len;
	return child;
}

tuple getMatrixDim(parseTree* root){
	parseTree* rowNode = root;
	parseTree* ele;
	int m = 0;
	int n = 0;
	tuple dim;
	dim.m = 0;
	dim.n = 0;
	while(rowNode){
		ele = rowNode->child;
		int currN = 0;
		while(ele){
			currN++;
			ele = ele->next;
		}
		if(m && (currN != n)){
			printf("line %d: Matrix rows are not of same size\n", root->child->child->token->line);
			return dim; // 0*0 to indicate error
		}
		else{
			n = currN;
		}
		m++;
		rowNode = rowNode->next;
	}
	dim.m = m;
	dim.n = n;
	//printf("%d %d\n",m,n);
	return dim;
}

//type and declaration check for arith exp
int arithCheck(int type, parseTree* root, symbolTable* st){
	if(!root)return 1;
	//check for itself
	if(root->val == PLUS || root->val == MINUS || root->val == MUL ){
		int res1 = arithCheck(type, root->child, st);
		int res2 = arithCheck(type, root->child->next, st);
		return res1&&res2;
		/*return arithCheck(type, root->child, st) && arithCheck(type, root->child->next, st);*/
	}
	else if(root->val == DIV){
		if(type == INT)return 0;//int can't be on lhs
		int resInt1 = arithCheck(INT, root->child, st);
		int resInt2 = arithCheck(INT, root->child->next, st);
		int resReal1 = arithCheck(type, root->child, st);
		int resReal2 = arithCheck(type, root->child->next, st);
		return (resInt1 && resInt2) || (resReal1 && resReal2);
	}
	else if(root->val == NUM){
		if(!(type == INT || type==0)){
			//printf("line %d: Type mismatch\n",root->token->line);
			return 0;
		}
		return 1;
	}
	else if(root->val == RNUM){
		if(!(type == REAL || type==0)){
			//printf("line %d: Type mismatch\n",root->token->line);
			return 0;
		}
		return 1;
	}
	else{
		symbolTableNode* stNode = querySymbolTable(st, root->token->val);
		if(!stNode){
			printf("line %d: Variable %s not declared\n", root->token->line, root->token->val);
			return 1; //to continue checking
		}
		else{
			int declaredType = stNode->type;
			/*if(!stNode->assigned){
				//printf("line %d: Variable %s not assigned a value before use\n", root->token->line, root->token->val);
				return 1;
			}*/
			if(type){
				if(declaredType == MATRIX && root->next && root->next->next)declaredType = INT; //matrix element
				if(declaredType != type){
					//printf("line %d: Type mismatch\n",root->token->line);
					return 0;
				}
				else type = declaredType;

			}

		}
	}
	return 1;
}

int stringCheck(parseTree* root, symbolTable* st){
	if(root->val == STR){
		return strlen(root->token->val);
	}
	else if(root->val == ID){
		symbolTableNode* stNode = querySymbolTable(st,root->token->val);
		if(!stNode){
			printf("line %d: Variable %s is not declared\n", root->token->line, root->token->val);
			return 0;
		}
		else if(!(stNode->assigned)){
			//printf("line %d: Variable %s not assigned a value before use\n", root->token->line, root->token->val);
			return 0;
		} 
		else return stNode->width;
	} 
	else if(root->val == PLUS){
		return stringCheck(root->child,st) + stringCheck(root->child->next, st);
	}
	else{
		//operation not allowed
		printf("line %d: operation %s is not allowed on strings\n", root->token->line, root->token->val);
		return 0;
	}
}

tuple matrixCheck(parseTree* root, symbolTable* st){
	//to do
	tuple dim;
	dim.m = 0;
	dim.n = 0;
	if(root->val == ID){
		symbolTableNode* stNode = querySymbolTable(st,root->token->val);
		if(!st){
			printf("line %d: Variable %s is not declared\n", root->token->line, root->token->val);
			return dim;
		}
		else if(!(stNode->assigned)){
			//printf("line %d: Variable %s not assigned a value before use\n", root->token->line, root->token->val);
			return dim;
		} 
		else{
			return stNode->dim;
		}

	}

	else if(root->val == PLUS || root->val == MINUS){
		tuple dimL = matrixCheck(root->child,st);
		tuple dimR = matrixCheck(root->child->next,st);
		if((dimL.m == dimR.m) && dimR.n == dimL.n){
			return dimL;
		}
		else{
			printf("line %d: matrix addition is between matrices of incompatible sizes\n", root->token->line);
		}
	}
	return dim;
}

int createSymbolTable(symbolTable* st,parseTree* root, int offset){
	parseTree* nextChild;
	if(!root)return offset;
	if(root->val == assignmentStmt_type1){
		//basic type checks for assignment
		symbolTableNode* stNode = querySymbolTable(st, root->child->token->val);
		 //undeclared, checked far below
		if(stNode && (stNode->type == MATRIX)){
			if(!(root->child->next->val == row || root->child->next->val == PLUS || root->child->next->val == MINUS)){
				printf("line %d: type mismatch error\n", root->child->token->line);
				return offset;
			}
		}
		else if(stNode && stNode->type == STRING){
			if(!(root->child->next->val == STR || root->child->next->val == PLUS)){
				printf("line %d: type mismatch error\n", root->child->token->line);
				return offset;
			}

		}
		
	}
	if((root->val == declarationStmt) && (root->child->val == INT || root->child->val == REAL)){
		int len, type;
		parseTree* currNode;
		currNode = root->child;
		type = currNode->val;
		if(type == INT)len = 2;
		else len = 4;
		currNode = currNode->next;
		while(currNode){
			//redeclared check
			if(querySymbolTable(st, currNode->token->val)){
				printf("line %d: The variable %s is declared more than once\n",currNode->token->line, currNode->token->val);
				return offset;
			}
			addToSymbolTable(st, currNode->token, type, offset, len, NULL);
			offset+=len;
			currNode = currNode->next;
		}
		return offset;
	}
	//String and matrix are inserted into table with 0 size and 0 offset
	else if(root->val == declarationStmt){
		int type;
		parseTree* currNode;
		currNode = root->child;
		type = currNode->val;
		currNode = currNode->next;
		while(currNode){
			//redeclared check
			if(querySymbolTable(st, currNode->token->val)){
				printf("line %d: The variable %s is declared more than once\n",root->child->token->line, root->child->token->val);
				return offset;
			}
			addToSymbolTable(st, currNode->token, type, 0, 0, NULL);
			currNode = currNode->next;
		}
		//printf("%d\n",querySymbolTable(st,root->child->next->token->val)->width);
		return offset;
	}

	//function call
	else if((root->val == assignmentStmt_type1 || root->val == assignmentStmt_type2) && (!root->child->next->type && root->child->next->val == funCallStmt)){ 
		//check if called before def
		symbolTableNode* fnNode = querySymbolTable(st, root->child->next->child->token->val);
		if(!fnNode){
			printf("line %d: Function %s is not defined\n", root->child->next->child->token->line, root->child->next->child->token->val);
			return offset;
		}
		//check for recursive call
		if(fnNode->child == st){
			printf("line %d:  recursive function definitions are not allowed\n",root->child->next->child->token->line);
		}
		symbolTable* fnST = fnNode->child;
		parseTree* fnDefRoot = fnST->root;
		parseTree* fnCallRoot = root->child->next;

		//check for input params
		parseTree* currActualParam = fnCallRoot->child->next;
		parseTree* currFormalType = fnDefRoot->child->next->child;
		int type;
		symbolTableNode* currActualNode;
		while(currActualParam && currFormalType){
			//type checking
			type = currFormalType->val;
			if(currActualParam->val == ID){
				currActualNode = querySymbolTable(st, currActualParam->token->val);
				if(!currActualNode)printf("line %d: Variable %s is not declared\n",currActualParam->token->line,currActualParam->token->val);
				else
					if(type!=currActualNode->type)printf("line %d: types of actual and formal input parameters do not match\n", currActualParam->token->line);
			}
			//for NUM/RNUM
			else if(currActualParam->val == NUM){
				if(type != INT)printf("line %d: types of actual and formal input parameters do not match\n", currActualParam->token->line);
			}
			else if(currActualParam->val == RNUM){
				if(type != REAL)printf("line %d: types of actual and formal input parameters do not match\n", currActualParam->token->line);
			}		
			
			//increment pointers
			if(currFormalType->next->next){
				//check if paramlist exists
				currFormalType = currFormalType->next->next->child;
				//more actual params should be there

				if(!currActualParam->next){
					printf("line %d: number of input parameters in function call to function %s does not match with the number of formal input parameters\n", root->child->next->child->token->line, root->child->next->child->token->val);;
					break;
				}
				else currActualParam = currActualParam->next;
			}
			else{
				//check if more actual params are there => number of params don't match
				if(currActualParam->next){
					printf("line %d: number of input parameters in function call to function %s does not match with the number of formal input parameters\n", root->child->next->child->token->line, root->child->next->child->token->val);
					//check declarations
					while(currActualParam){
						currActualNode = querySymbolTable(st, currActualParam->token->val);
						if(currActualParam->val == ID){
							if(!currActualNode)printf("line %d: Variable %s is not declared\n",currActualParam->token->line,currActualParam->token->val);
						}		
						
						currActualParam = currActualParam->next;
					}
					break;
				}
				break;
			}
		}

		// checking for output params
		//Case 1: single o/p param
		if(root->val == assignmentStmt_type1){
			currFormalType = fnDefRoot->child->child;
			type = currFormalType->val;
			currActualParam = root->child;
			currActualNode = querySymbolTable(st, currActualParam->token->val);
			if(!currActualNode)printf("line %d: Variable %s is not declared\n",currActualParam->token->line,currActualParam->token->val);
			else{
					if(type!=currActualNode->type)printf("line %d: Type mismatch error\n", currActualParam->token->line);
					currActualNode->assigned = 1;
				}
		
		}
		//Case 2: leftHandSide_listVar
		else{
			currFormalType = fnDefRoot->child->child;
			currActualParam = root->child->child;
			while(currActualParam && currFormalType){
				type = currFormalType->val;
				if(currActualParam->val == ID){
					currActualNode = querySymbolTable(st, currActualParam->token->val);
					if(!currActualNode){
						printf("line %d: Variable %s is not declared\n",currActualParam->token->line,currActualParam->token->val);
						return offset; //??
					}
					else{
						if(type!=currActualNode->type)printf("line %d: types of actual and formal output parameters do not match\n", currActualParam->token->line);
						currActualNode->assigned = 1;
					}
				}
				else if(currActualParam->val == NUM && type !=INT)printf("line %d: types of actual and formal output parameters do not match\n", currActualParam->token->line);
				else if(currActualParam->val == RNUM && type !=REAL)printf("line %d: types of actual and formal output parameters do not match\n", currActualParam->token->line);


				//increment pointers
				if(currFormalType->next->next){
				//check if paramlist exists
				currFormalType = currFormalType->next->next->child;
				//more actual params should be there
				if(!currActualParam->next){
					printf("line %d: number of output parameters in function call to function %s does not match with the number of formal output parameters\n", root->child->next->child->token->line, root->child->next->child->token->val);
					break;
				}
				else currActualParam = currActualParam->next;
				}
				else{
				//check if more actual params are there => number of params don't match
					if(currActualParam->next){
						printf("line %d: number of output parameters in function call to function %s does not match with the number of formal output parameters\n", currActualParam->token->line, fnDefRoot->token->val);
						//check declarations
						while(currActualParam){
							currActualNode = querySymbolTable(st, currActualParam->token->val);
							if(!currActualNode)printf("line %d: Variable %s is not declared\n",currActualParam->token->line,currActualParam->token->val);
							currActualParam = currActualParam->next;
						}
						break;
					}
					break;
				}

			}

		}
	}

	
	//string assignment from STR
	else if(root->val == assignmentStmt_type1 && root->child->next->val == STR){
		if(!querySymbolTable(st, root->child->token->val)){
			printf("line %d: Variable %s is not declared\n", root->child->token->line,root->child->token->val);
		}
		else{
			symbolTableNode* strNode = querySymbolTable(st, root->child->token->val);
			strNode->width = strlen(root->child->next->token->val);
			strNode->offset = offset;
			strNode->assigned = 1;
			offset+=strlen(root->child->next->token->val);
			st->lastAddr+=strlen(root->child->next->token->val);
		}
		return offset;
	}

	//give size, offset and dim  to matrix
	else if(root->val == assignmentStmt_type1 && root->child->next->val == row){
		parseTree* currNode = root->child->next;
		if(!querySymbolTable(st, root->child->token->val)){
			printf("line %d: Variable %s is not declared\n", root->child->token->line,root->child->token->val);
		}
		else{
			symbolTableNode* matNode = querySymbolTable(st, root->child->token->val);
			tuple dim = getMatrixDim(currNode);
			//printf("%d x %d\n", dim.m, dim.n);
			if(dim.m > 10 || dim.n >10){
				printf("line %d: Matrix larger than 10x10 is not allowed in language", root->child->token->line);
				return offset;
			}
			int len = (dim.m * dim.n)*2;
			matNode->width = len;
			matNode->offset = offset;
			matNode->dim.m = dim.m;
			matNode->dim.n = dim.n;
			matNode->assigned = 1;
			offset+=len;
			st->lastAddr+=len;
			//printf("%d\n", matNode->width);
		}
		return offset;
	}
	//function definition
	else if(root->val == FUNID && root->parent->val!=funCallStmt){
		//check for overloaded def(redeclared)
		if(querySymbolTable(st, root->token->val)){
			printf("line %d: function overloading is not allowed\n",root->token->line);
			return offset;
		}
		//insert funid to parent's symbol table and get pointer to it's st
		symbolTable* childST = addToSymbolTable(st, root->token, root->val, 0, 0, root);
		parseTree* opParamNode = root->child;
		int childOffset = 0;
		int len, type;
		while(opParamNode){
			type = opParamNode->child->val;
			if(type == INT)len = 2;
			else if(type == REAL)len = 4;
			else len = 0;
			addToSymbolTable(childST, opParamNode->child->next->token, type, childOffset, len, NULL);
			childOffset+=len;
			opParamNode = opParamNode->child->next->next;
		}
		//populate input params in function's symbol table
		parseTree* inParamNode = root->child->next;
		while(inParamNode){
			//offset = createSymbolTable(st, opParamNode, offset);
			type = inParamNode->child->val;
			if(type == INT)len = 2;
			else if(type == REAL)len = 4;
			else len = 0;
			addToSymbolTable(childST, inParamNode->child->next->token, type, childOffset, len, NULL);
			childOffset+=len;
			inParamNode = inParamNode->child->next->next;
		}
		createSymbolTable(childST,root->child->next->next, childOffset);
		// check if all o/p params in a fn are assigned value after return
		inParamNode = root->child->child->next;
		symbolTableNode* inParamStNode;
		while(inParamNode){
			inParamStNode = querySymbolTable(childST, inParamNode->token->val);
			if(!inParamStNode->assigned)printf("line %d: output parameter %s in function %s is not assigned any value throughout the function definition \n",
				inParamNode->token->line, inParamNode->token->val, root->token->val);
			if(inParamNode->next && inParamNode->next->child && inParamNode->next->child->next)inParamNode = inParamNode->next->child->next;
			else break;
		}
		return offset;
	}
	//size expr for non matrix
	else if(root->val == assignmentStmt_type1 && root->child->next->val == sizeExpression){
		//check for lhs var
		int type = 0;
		symbolTableNode* stNode = querySymbolTable(st, root->child->token->val);
		if(!stNode){
			printf("line %d: Variable %s not declared\n", root->child->token->line, root->child->token->val); //check lhs
		}
		else {
			type = stNode->type;
			stNode->assigned = 1; //will be assigned after end of arith exp
		}
		if(type!=INT)printf("line %d: Variable %s is not an integer\n", root->child->token->line, root->child->token->val);

		//check for rhs	
		parseTree* rhsID = root->child->next->child;
		symbolTableNode* rhsNode = querySymbolTable(st, rhsID->token->val);
		if(!rhsNode)printf("line %d: Variable %s not declared\n", rhsID->token->line, rhsID->token->val);
		else{
			if(rhsNode->type == MATRIX){
				printf("line %d: Size of matrix %s must be stored in two output variables\n", rhsID->token->line, rhsID->token->val);
			}
		}
		return offset;
	}
	//size expr for matrix
	else if(root->val == assignmentStmt_type2 && root->child->next->val == sizeExpression){
		//check for lhs
		
		parseTree* val1 = root->child->child;
		symbolTableNode* val1node = querySymbolTable(st, val1->token->val);
		if(!val1node)printf("line %d: Variable %s not declared\n", val1->token->line, val1->token->val);
		else{
			if(val1node->type != INT)printf("line %d: Variable %s is not an integer\n", val1->token->line, val1->token->val);
		}
		if(val1->next){
			parseTree* val2 = val1->next;
			symbolTableNode* val2node = querySymbolTable(st, val2->token->val);
			if(!val2node)printf("line %d: Variable %s not declared\n", val2->token->line, val2->token->val);
			else{
				if(val2node->type != INT)printf("line %d: Variable %s is not an integer\n", val2->token->line, val2->token->val);
			}
			if(val2->next)printf("line %d: Size must be stored in single variable or two output variables\n", val1->token->line);
		}
		else{
			printf("line %d: Size must be stored in single variable or two output variables\n", val1->token->line);
		}


		//check for RHS
		parseTree* rhsID = root->child->next->child;		
		symbolTableNode* rhsNode = querySymbolTable(st, rhsID->token->val);
		if(!rhsNode)printf("line %d: Variable %s not declared\n", rhsID->token->line, rhsID->token->val);
		else{
			if(rhsNode->type == MATRIX){
				//if(!rhsNode->assigned)printf("line %d: Variable %s not assigned a value before use\n", rhsID->token->line, rhsID->token->val);
			}
			else{
				printf("line %d: Variable %s is not a matrix and expects single output variable for size\n", rhsID->token->line, rhsID->token->val);
			}
		}
		return offset;		
	}
	
	//arith exp, string, matrix ops
	else if(root->val == assignmentStmt_type1){
		int type = 0;
		symbolTableNode* stNode = querySymbolTable(st, root->child->token->val);
		if(!stNode){
			printf("line %d: Variable %s not declared\n", root->child->token->line, root->child->token->val); //check lhs
		}
		else {
			type = stNode->type;
			stNode->assigned = 1;
			
		}
		if(type == INT || type == REAL || !type){
			int check = arithCheck(type, root->child->next, st);//pass head of artithexp
			if(!check)printf("line %d: type mismatch error\n", root->child->token->line);
		}
		else if(type == MATRIX&&root->child->next){
			//also include matrix ele
			int check = arithCheck(INT, root->child->next, st);//pass head of artithexp
			if(!check)printf("line %d: type mismatch error\n", root->child->token->line);

		}
		else if(type == STRING){
			/*if(root->child->next->val == INT || root->child->next->val == REAL){
				printf("line %d: type mismatch error", root->child->token->line);
			}*/
			int strSize = stringCheck(root->child->next,st);
			if(strSize > 20)printf("line %d: String %s is longer than 20\n", root->child->token->line,root->child->token->val);			
			else{
				stNode->width = strSize;
				stNode->offset = offset;
				offset+=strSize;
				st->lastAddr+=strSize;
			}
			
		}
		else if(type == MATRIX){
			
			tuple matDim = matrixCheck(root->child->next,st);
			stNode->dim = matDim;
			stNode->offset = offset;
			int matLen = (matDim.m * matDim.n)*2;
			stNode->width = matLen;
			offset+=matLen;
			st->lastAddr+=matLen;
		}
		return offset;
	}

	else if(root->val == booleanExpression && (root->child->val == ID || root->child->val == NUM || root->child->val == RNUM)){
		symbolTableNode* stNode;
		if(!(root->child->val == NUM || root->child->val == RNUM)){
			stNode = querySymbolTable(st, root->child->token->val);
			if(!stNode){
				printf("line %d: Variable %s not declared\n",root->child->token->line, root->child->token->val);
			}
			else{
				if(!(stNode->type == INT || stNode->type == REAL))printf("line %d: Relational operators are allowed only on integer or real variables\n",root->child->token->line);
			}
		}
		if(!(root->child->next->next->val == NUM || root->child->next->next->val == RNUM)){
			stNode = querySymbolTable(st, root->child->next->next->token->val);
			if(!stNode){
				printf("line %d: Variable %s not declared\n",root->child->next->next->token->line, root->child->next->next->token->val);
			}
			else{
				if(!(stNode->type == INT || stNode->type == REAL))printf("line %d: Relational operators are allowed only on integer or real variables\n",root->child->next->next->token->line);
			}
		}		
		
		return offset; 
	}
	//to keep traversing
	else if(root->child){
		offset = createSymbolTable(st, root->child, offset);
		if(root->child && root->child->next){
			nextChild = root->child->next;
			while(nextChild){
				offset = createSymbolTable(st, nextChild, offset);
				nextChild = nextChild->next;
			}
		}
	}
	return offset;
}