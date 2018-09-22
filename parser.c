/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "parser.h"

/* --------------------------------------------------aux functions-------------------------------------- */
//prints bits of a number
void printBits(long long a){
	int i;
	int flag;
	for(i = 40; i >=0; i--){
		if(a&(long long)pow(2,i))flag = 1;
		else flag = 0;  
		printf("%d", flag);
	}
}

//prints tokens from bit vector
void printTks(long long a){
	int i;
	for(i = 0; i < 41; i++){
		if(a&(long long)pow(2,i)){
			printf(" %s", Tlist[i]);
		}
	}
}

//returns index of NT in enum/array
int findNT(char* str){
	int i;
	for(i = 0; i < 44; i++){
		if(strcmp(str,NTlist[i]) == 0){
			//printf("%s ", NTlist[i]);
			return i;
		}
	}
}
//returns index of T in enum
int findT(char* str){
	int i;
	for(i = 0; i < 40; i++){
		if(strcmp(str,Tlist[i]) == 0){
			//printf("%s\n", Tlist[i]);
			return i;
		}
	}
}

int hasNullRule(lhs* currLHS){
	rhs* currRHS = currLHS->rule;
	while(currRHS){
		if(currRHS->sym->val == EPS && currRHS->sym->type == 1){
			return 1;
		}
		currRHS = currRHS->next;
	}
	return 0;
}

//val is the enum value of NT/T, type is 0 for NT, gr is pointer to grammar returns bit vector
long long getFirst(int val, int type, lhs* gr, FirstAndFollow* ff){
	lhs* currLHS;
	rhs* currRHS;
	symbol* currSym;
	symbol* nextSym;

	if(type){
		//Terminal has itself in first set
		return (long long)pow(2,val);
	}
	
	if(ff->first[val] != 0) return ff->first[val]; //precomputed value returned

	currLHS = &gr[val];
	currRHS = currLHS->rule; //first rule
	while(currRHS && !(currRHS->sym->val == EPS && currRHS->sym->val == 1)){
		int allEPS = 1; //assuming all symbols of rule have epsilon in first set
		currSym = currRHS->sym;
		while(currSym){
			long long firstCurrSym = getFirst(currSym->val, currSym->type, gr, ff);
			if(currSym->type || (firstCurrSym & (long long)pow(2,EPS)) == 0){
				// symbol is terminal or doesn't have epsilon in it's first
				ff->first[val] = ff->first[val] | firstCurrSym;
				allEPS = 0; //not all symbols have epsilon now
				break;
			}
			else{
				firstCurrSym  = firstCurrSym & ~(long long)pow(2, EPS); //remove epsilon
				ff->first[val] = ff->first[val] | firstCurrSym;
			}
			currSym = currSym->nextSym;			
		}
		
		if(allEPS) ff->first[val] = ff->first[val] | (long long)pow(2, EPS);		
		currRHS = currRHS->next;
	}
	if(hasNullRule(currLHS)){
		ff->first[val] = ff->first[val] | (long long)pow(2, EPS);
		//printTks(ff->first[val]);
	} 	
	return ff->first[val];
}

long long getFollow(int val, lhs* gr, FirstAndFollow* ff){
	//base cases
	if(val == mainFunction)return pow(2,$); //FOLLOW(S) = { $ }

	if(ff->follow[val]!=0)return ff->follow[val]; //pre computed

	//compute follow set
	lhs* currLHS;
	rhs* currRHS;
	symbol* currSym;
	symbol* nextSym;
	int lhsNum;
	long long firstNext;
	long long firstEPS = (long long)pow(2,EPS);
	int allEPS;
	for(lhsNum = 0; lhsNum < 44; lhsNum++){
		currLHS = &gr[lhsNum]; 
		currRHS = currLHS->rule;
		while(currRHS){
			currSym = currRHS->sym;
			while(currSym){
				if(currSym->val == val && currSym->type == 0){
					//matched
					nextSym = currSym->nextSym;
					if(nextSym){
						//printf("\nNext is %s", Tlist[nextSym->val]);
						allEPS = 1;
						while(nextSym){
							//printf("\nEnters next sym while loop");
							firstNext = getFirst(nextSym->val, nextSym->type, gr, ff);
							if((firstNext & firstEPS) == 0){
								//doesn't have eps in first set
								//printf("\n doesn't have eps in first set");
								ff->follow[val] = ff->follow[val] |= firstNext;
								allEPS = 0;
								break; //no need to check further symbols of this rule
							}
							else{
								ff->follow[val] = ff->follow[val] |= (firstNext & ~firstEPS);
							}
							nextSym = nextSym->nextSym;
						}
						if(allEPS){
							//all rules on right of symbol have eps in first set
							//ff->follow[val] = 0;
							ff->follow[val] = ff->follow[val] |= getFollow(lhsNum, gr,ff); //try getFollow
						}
					}
					else{
						//doesn't have a symbol after it
						ff->follow[val] = ff->follow[val] |= getFollow(lhsNum, gr,ff);
					}
				}
				currSym = currSym->nextSym;
			}
			currRHS = currRHS->next;
		}
	}
	return ff->follow[val];	
}

//add rule to reqd columns of a given row
void addRule(int row, long long colVector, rhs* rule, table* T){
	int col;
	for(col = 0; col < 41; col++){
		if(colVector & (long long)pow(2,col)){
			//current col's terminal is present in bit vector
			if(T->arr[row][col] != 0){
				printf("\nClash at (%s, %s) ", NTlist[row], Tlist[col]);
				printf("Prev rule: %s = ",NTlist[row]);
				printRule(T->arr[row][col]);
				printf("Curr rule: %s = ", NTlist[row]);
				printRule(rule);
				printf("\n");
			}
			T->arr[row][col] = rule; //add rule
		}
	}
}

//prints a rule, used for debugging parse table clashes
void printRule(rhs* currRHS){
	symbol* currSym = currRHS->sym;
	while(currSym){
		if(currSym->type == 0){
			printf("%s ", NTlist[currSym->val]);
		}
		else{
			printf("%s ", Tlist[currSym->val]);
		}
		currSym = currSym->nextSym;
	}
}

void printParseTable(table* T){
	int i,j;
	for(i = 0; i < 44; i++){
		printf("Row %s: ", NTlist[i]);
		for(j = 0; j < 41; j++){
			printf("Column %s: ", Tlist[j]);
			if(!T->arr[i][j]){
				printf("Error");
			}
			else{
				printRule(T->arr[i][j]);
			}
			printf("\n");
		}
	}
}

//Stack functions
stackNode* newNode(symbol* data){
	stackNode* node = (stackNode*)malloc(sizeof(stackNode));
	node->data = data;
	return node;
}

int isEmpty(stackNode* root){
	return !root;
}

void push(stackNode** root, symbol* data){
	stackNode* node = newNode(data);
	node->next = *root;
	*root = node;
}

symbol* pop(stackNode** root){
	if(isEmpty(*root)){
		printf("\nSTACK UNDERFLOW");
		return NULL;
	} //underflow error
	stackNode* temp = *root;//temporary node
	*root = (*root)->next;
	symbol* popped = temp->data;
	free(temp);
	return popped;
}

symbol* top(stackNode* root){
	if(isEmpty(root)){
		printf("\nSTACK UNDERFLOW");
		return NULL;
	} //underflow error
	return root->data;
}
//produces symbol of given NT or T
symbol* Symbol(int val, int type){
	symbol* sym = (symbol*) malloc(sizeof(symbol));
	sym->type = type;
	sym->val = val;
	return sym;
}

parseTree* newParseTreeNode(symbol* sym, parseTree* parent){
	parseTree* node = (parseTree*)malloc(sizeof(parseTree));
	node->type = sym->type;
	node->val = sym->val;
	node->parent = parent;
	node->child = NULL;
	node->next = NULL;
	if(sym->type){
		node->token = (tokenInfo*)malloc(sizeof(tokenInfo));
	}
	else node->token = NULL;
	return node;
}

void pushRule(stackNode** S, rhs* rule){
	//push symbols one by one in reverse order s.t leftmost is top
	symbol* currSym = rule->sym; //first symbol of rule
	if(currSym->val == EPS && currSym->type == 1){
		//don't push epsilon!
		return;
	}
	symbol* arr[20];
	int i = 0;
	//add symbols to array
	/*if(top(*S)->type == 0)printf("\nTop before pushing rule is  %s",NTlist[top(*S)->val]);
	else printf("\nTop before pushing rule is %s",Tlist[top(*S)->val]);*/
	while(currSym){
		arr[i] = currSym;
		i++;
		currSym = currSym->nextSym;
	}
	for(i--; i >= 0; i--){
		push(S, arr[i]); //push in reverse order
	}
}

void parsingError(){
	printf("\n PARSING ERROR \n");
}
//prints contents of stack
void printStack(stackNode* S){
	printf("\nStack Contents:");
	stackNode* curr = S;
	symbol* sym;
	while(curr){
		sym = curr->data;
		if(sym->type == 0)printf("\n| %s |",NTlist[sym->val]);
		else printf("\n| %s |",Tlist[sym->val]);
		curr = curr->next;
	}
	printf("\n|_____________|");
}

void addToParseTree(parseTree* parent, rhs* rule){
	symbol* currSym = rule->sym;
	//add first symbol
	parseTree* currNode = newParseTreeNode(currSym,parent);
	parent->child = currNode;
	currSym = currSym->nextSym;
	//add more symbols as it's siblings
	while(currSym){
		currNode->next = newParseTreeNode(currSym,parent);
		currNode = currNode->next;
		currSym = currSym->nextSym;
	}
}

parseTree* getNextRightNode(parseTree* currNode){
	if(currNode->next)return currNode->next;
	if(currNode->parent)return getNextRightNode(currNode->parent);
	//printf("\nUNABLE TO FIND NEXT RIGHT NODE");
}
//prints node of parse tree
void printTreeNode(parseTree* node){
	char* dummy = "Dummy,no token found";
	if(node->type && node->val == EPS){
		fprintf(outfile,"\t\t    ---- \t %25s \t %32s   yes   \t\t\t\n",  Tlist[node->val],NTlist[node->parent->val]);
		//return;
	}
	else if(node->type && !node->token->val){
		fprintf(outfile,"%24s %8d %24s \t %32s   yes   \t\t\t\n",  dummy, node->token->line,  Tlist[node->val], NTlist[node->parent->val]);
	}
	else if(node->type && !(node->val == NUM || node->val == RNUM)){//leaf T
		fprintf(outfile,"%24s %8d %24s \t %32s   yes   \t\t\t\n",  node->token->val, node->token->line,  Tlist[node->val], NTlist[node->parent->val]);
	}
	else if(node->type && (node->val == NUM)){
		//leaf T with num
		fprintf(outfile,"%24s %8d %24s%8d%32s  yes  \t\t\t\n",  node->token->val, node->token->line, Tlist[node->val],  node->token->num,  NTlist[node->parent->val]);
	}
	else if(node->type && (node->val == RNUM)){
		//leaf T with num
		fprintf(outfile,"%24s %8d %24s%8.2f%32s  yes   \t\t\t\n", node->token->val, node->token->line,  Tlist[node->val], node->token->rnum,  NTlist[node->parent->val]);
	}
	else if(node->parent && !node->child){
		//non root and leaf NT
		fprintf(outfile,"\t\t    ---- \t \t\t\t\t %32s   yes    %28s\n", NTlist[node->parent->val], NTlist[node->val]);
	}
	else if(node->parent && node->child){
		//non root and non leaf NT
		fprintf(outfile,"\t\t    ---- \t \t\t\t\t %32s    no    %28s\n", NTlist[node->parent->val], NTlist[node->val]);
	}
	else if(!node->parent && !node->child){
		//leaf root
		fprintf(outfile,"\t\t    ---- \t \t\t\t\t  \t\t\t    ROOT    yes   %28s\n", NTlist[node->val]);
	}
	else{
		//non leaf root
		fprintf(outfile,"\t\t    ---- \t \t\t\t\t  \t\t\t    ROOT     no   %28s\n", NTlist[node->val]);
	}
	//add for leaf NTs
	fflush(outfile);
	
}

void printSimpleTreeNode(parseTree* node){
	if(node->type){
		//T
		/*if(node->val == ID){
			printf("%s ", node->token->val);
		}
		if(node->val == NUM){
			printf("%d ", node->token->num);
		}*/
		printf("%s\n",  Tlist[node->val]);
	}
	else{
		//NT
		printf("%s\n",  NTlist[node->val]);
	}
}

/* --------------------------------------------------core functions--------------------------------------*/

//returns grammar structure, input the pointer of file with grammar
lhs* createGrammar(){
	FILE* grFp = fopen("grammar.txt","r");
	lhs* gr = (lhs*)malloc(sizeof(lhs)*44); //array for grammar 
	char* grBuff = (char*)malloc(4096);
	fread((grBuff),1, 4096,grFp); //grammar fits in 4kb
	//using strtok of string.h
	char *tok = strtok(grBuff, " "); //grammar input is space separated
	int init = 1; //first rule starts
	int index = -1; //index of NT in grammar array/enum
	lhs* currLHS; //current LHS element of array
	rhs* currRHS; //current RHS of rule
	symbol* currSym; //current symbol of RHS
	int newSym = 0; //1 means space needs to be alloc for new lhs symbol of rule as rhs has more than 1 symbols
	while(tok!=NULL){
		
		if(init){
			index++;
			currLHS = &gr[index];
			init = 0;
		}
		else if(tok[0] == '\n'){
			//printf("\nnewline");
			//tok = strtok(NULL, " "); //first token of new rule
			//printf("%s ",tok);
			index++;
			currLHS = &gr[index];
		}
		else if(tok[0] == '='){
			//first rule begins
			currLHS->rule = (rhs*)malloc(sizeof(rhs));
			currRHS = currLHS->rule;
			currRHS->sym = (symbol*)malloc(sizeof(symbol));
			currSym = currRHS->sym;
			newSym = 0;
		}
		else if(tok[0] == '|'){
			//next rule of same NT
			currRHS->next = (rhs*)malloc(sizeof(rhs));
			currRHS = currRHS->next;
			currRHS->sym = (symbol*)malloc(sizeof(symbol));
			currSym = currRHS->sym;
			newSym = 0;
		}
		else if(tok[0] == '<'){
			//non terminal on RHS
			if(newSym){
				currSym->nextSym = (symbol*)malloc(sizeof(symbol));
				currSym = currSym->nextSym;
			}
			currSym->type = 0;
			currSym->val = findNT(tok);
			newSym = 1;

		}
		else{
			//terminal
			if(newSym){
				currSym->nextSym = (symbol*)malloc(sizeof(symbol));
				currSym = currSym->nextSym;
			}
			currSym->type = 1;
			currSym->val = findT(tok);
			newSym = 1;
		}
		//printf("%s ",tok);
		tok = strtok(NULL, " "); //next token
	}
	return gr;
}

FirstAndFollow* computeFirstAndFollowSets(lhs* gr){
	FirstAndFollow* ff = (FirstAndFollow*)malloc(sizeof(FirstAndFollow));
	memset(ff, 0, sizeof(FirstAndFollow)); //initialize first and follow to zero
	int i;
	long long f;
	for(i = 0; i < 44; i++){
		f = getFirst(i, 0, gr, ff); //getFirst() is in aux functions
		//uncomment to print first when driver is run
		/*printf("%d %s =", i+1, NTlist[i]);
		printTks(f);
		printf("\n");*/
		
	}
	//printf("\n");
	for(i = 0; i < 44; i++){
		f = getFollow(i, gr,ff); //getFollow() is in aux functions
		//uncomment to print follow when driver is run
		/*printf("%d %s =", i+1, NTlist[i]);
		printTks(f);
		printf("\n")*/;
	}
	return ff;
}

table* createParseTable(FirstAndFollow* F, lhs* gr){
	table* T = (table*)malloc(sizeof(table));
	memset(T, 0, sizeof(table)); //set all entries to error
	int ntNum;
	lhs* currLHS;
	rhs* currRHS;
	symbol* currSym;
	long long firstRHS;
	long long firstTemp;
	long long followNT;
	long long firstEPS = (long long)pow(2,EPS);
	for(ntNum = 0; ntNum < 44; ntNum++){//constructing the table row wise
		currLHS = &gr[ntNum];
		currRHS = currLHS->rule; //start from first rule of the NT
		while(currRHS){//loop across all rules
			currSym = currRHS->sym;
			//find first of the RHS
			int allEPS = 1; //assuming first has EPS
			firstRHS = 0;
			while(currSym){
				firstTemp = getFirst(currSym->val, currSym->type, gr, F);
				if((firstTemp & firstEPS) == 0){
					//doesn't have EPS
					firstRHS |= firstTemp;
					allEPS = 0;
					break;
				}
				else{
					firstTemp &= (~firstEPS);
					firstRHS |= firstTemp;
				}
				currSym = currSym->nextSym;				
			}
			if(allEPS){
				//add follow of NT as RHS has EPS in it's first
				firstRHS |= getFollow(ntNum, gr, F);
			}
			addRule(ntNum, firstRHS, currRHS, T); //add rule to reqd columns of a given row
			currRHS = currRHS->next; //next rule of same NT
		}
	}
	//corrections
	rhs* epsRule = T->arr[newStmts][END];
	T->arr[newarithmeticExpression][CL] = epsRule;
	T->arr[more_ids][SQC] = epsRule;
	return T;
}

parseTree* parseInputSourceCode(table* T,FirstAndFollow* F){
	currLine = 1; //line number of file in lexer
	rewind(fp);// rewind to beginning
	getBuffer(fp);//renew twin buffer
	stackNode* S = newNode(Symbol($, 1)); //create stack with $ already pushed
	//printStack(S);
	push(&S, Symbol(mainFunction, 0));//push start symbol
	tokenInfo* L = getNextToken(); //lookahead symbol
	parseTree* root = newParseTreeNode(Symbol(mainFunction, 0), NULL); //create parse tree
	parseTree* currNode = root;
	rhs* rule;
	int err = 0;
	while(L && L->name != -2){//till tokens are received and token is not end of input(-2)
		//rule = NULL;
		if(L->val == NULL)L = getNextToken();//error token skipped
		/*if(currNode->type)printf("\n\nCurrentTreeNode is %s",Tlist[currNode->val]);
		else printf("\nCurrentTreeNode %s",NTlist[currNode->val]);
		printf("\n\nCurrent token name = %s lexeme = %s line = %d",Tlist[L->name], L->val, L->line);
		if(top(S)->type == 0)printf("\nCurrent top %s",NTlist[top(S)->val]);
		else printf("\nCurrent top %s",Tlist[top(S)->val]);*/
		//printStack(S);
		/*if(top(S)->type == 1 && top(S)->val == $){
			//stack has $ but input is not over
			printf("\n%d: Syntax error: Stack is empty but input is not over, expected less tokens", L->line);
			//parsingError();
			err++;
			break;
		}*/
		if(top(S)->type == 0){//NT
			if(T->arr[top(S)->val][(int)L->name] != NULL){//rule exists in parse table
				rule = T->arr[top(S)->val][L->name]; //rule to be pushed
				//printf("\nRule to be applied %s", NTlist[top(S)->val]);
				//printRule(rule);
				//printStack(S);
				pop(&S); //pop NT to be expanded
				/*if(top(S)->type == 0)printf("\nNew top %s",NTlist[top(S)->val]);
				else printf("\nNew top %s",Tlist[top(S)->val]);*/
				pushRule(&S, rule);//push symbols one by one
				addToParseTree(currNode, rule);//add as children of currNode
				if(rule->sym->type && rule->sym->val == EPS)currNode = getNextRightNode(currNode); //update currnode
				else currNode = currNode->child;
			}
			else if(top(S)->val == newarithmeticExpression && L->name == CL){
				pop(&S);
				currNode = getNextRightNode(currNode);		
			}
			else{//panic mode recovery
				printf("\n%d: Syntax error: Non terminal %s does not have any rule for the token %s",L->line, NTlist[top(S)->val], Tlist[L->name]);
				err++;
				long long followTop = F->follow[top(S)->val];
				long long firstTop = F->follow[top(S)->val];
				//int nullable = hasNullRule(&gr[top(S)->val]);
				while(L && L->name != -2){
					//printf("\n\tSkipped token %s",Tlist[L->name]);
					L = getNextToken();
					if(L->val == NULL)L = getNextToken(); //error token from lexer is skipped
					if(followTop & (long long)pow(2,L->name)){
						//token in follow is found
						pop(&S);
						currNode = getNextRightNode(currNode);
						//printf("\n\tPopped %s as token %s is found in follow set",NTlist[top(S)->val],Tlist[L->name]);
						break;
					}
					if(firstTop & (long long)pow(2,L->name)){
						//token in follow is found
						//printf("\n\tToken %s is found in first set",Tlist[L->name]);
						break;
					}

				}
			}
		}
		else{//Terminal at top
			if(top(S)->val == (int)L->name){
				//printf("\nPopped terminal at top %s == lookahead %s", Tlist[top(S)->val], Tlist[L->name]);
				pop(&S);
				currNode->token = L;
				currNode = getNextRightNode(currNode); //update currnode
				if(!currNode)break; //new addition!
				L = getNextToken();
			}
			else{//panic mode recovery
				printf("\n%d: Syntax error: The token %s for lexeme %s does not match at line %d. The expected token here is %s", L->line, Tlist[L->name], L->val, L->line, Tlist[top(S)->val]);
				//printf("\n\tPopped terminal %s at top, inserted dummy node in parse tree", Tlist[top(S)->val]);
				pop(&S);
				if(!currNode)break; //new addition!
				currNode->token->line = currLine;
				currNode = getNextRightNode(currNode);				
				L = getNextToken();
			} 
		}
	}
	if(top(S)->type == 1 && top(S)->val == $ && !err)printf("\nParsing successful\n");
	else if(top(S)->type == 1 && top(S)->val == $){
		printf("\nParsing completed with errors\n");
		exit(0); //don't go for semantic analysis
	}
	else{
		printf("\n%d: Syntax error: Stack not empty but input is consumed, expected more tokens",currLine);
		exit(0); //don't go for semantic analysis
	} 
	return root;
}

void printParseTree(parseTree* root){
	if(!root)return;
	if(root->child) printParseTree(root->child);//first child
	printTreeNode(root);//self
	parseTree* nextChild;
	if(root->child && root->child->next) 
		nextChild = root->child->next;//next child
	else return;
	while(nextChild){
		printParseTree(nextChild);
		//printSimpleTreeNode(nextChild);
		nextChild = nextChild->next;
	}
}

void printSimpleParseTree(parseTree* root){
	if(!root)return;
	if(root->child) printSimpleParseTree(root->child);//first child
	printSimpleTreeNode(root);//self
	parseTree* nextChild;
	if(root->child && root->child->next) 
		nextChild = root->child->next;//next child
	else return;
	while(nextChild){
		printSimpleParseTree(nextChild);
		nextChild = nextChild->next;
	}
}