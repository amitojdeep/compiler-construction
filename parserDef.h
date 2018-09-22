/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

typedef struct{
	long long first[44];//bit vectors of computed first, 0 if not computed 
	long long follow[44]; //bit vectors of computed follow, 0 if not computed
}FirstAndFollow;

FILE* outfile;

typedef enum{
	mainFunction, stmtsAndFuntionDefs, newStmts , stmtOrFunctionDef, stmt , functionDef, parameter_list, type, remainingList, declarationStmt, var_list, 
	more_ids, assignmentStmt_type1 , assignmentStmt_type2, leftHandSide_singleVar, leftHandSide_listVar, rightHandSide_type1, rightHandSide_type2,
	 sizeExpression, ifStmt , newIf, otherStmts, ioStmt, funCallStmt, inputParameterList, listVar, arithmeticExpression, newarithmeticExpression, arithmeticTerm,
	  newarithmeticTerm , factor, operator_lowPrecedence, operator_highPrecedence, booleanExpression, constrainedVars, var, matrix, rows, newRows, row,
	   remainingColElements, matrixElement , logicalOp , relationalOp
}NT; //non terminals

typedef struct symb{//implemented as a union
	int type; //0 is non terminal, 1 is terminal
	int val; //number in enum of NT or T 
	struct symb* nextSym; //next symbol in rule, null for last symbol
}symbol;

typedef struct rh{
	symbol* sym; 
	struct rh* next; //pointer to next rule with same lhs
}rhs;

typedef struct{
	//NT nt;
	rhs* rule; 
}lhs;

typedef struct{
	rhs* arr[44][41]; //44 NTs, 41 Ts(including EPS and $)
}table;

typedef struct stacknode{
	symbol* data;
	struct stacknode* next;
}stackNode;

typedef struct tup{
	int m;
	int n;
}tuple;

typedef struct STnod{
	char* lexeme; //ID name
	int type; //data type or FUNID, use T enum
	int width;
	int offset;
	tuple dim; //used for matrix
	struct st* child; //symbol table of hashed function name, NULL for IDs
	struct STnod* next; //overflow chain
	int assigned;
}symbolTableNode;

typedef struct tree{//implemented as a union
	int type;//0 for NT, 1 for T (isLeaf)
	int val;//enum of NT or T
	tokenInfo* token;
	struct tree* parent; //for traversal
	struct tree* child; //leftmost child
	struct tree* next; //sibling on the right
	symbolTableNode* tempVar; //temporaries of operators
}parseTree;

char* NTlist[] = {"<mainFunction>", "<stmtsAndFuntionDefs>", "<newStmts>" , "<stmtOrFunctionDef>", "<stmt>" , "<functionDef>", "<parameter_list>", "<type>",
 "<remainingList>", "<declarationStmt>", "<var_list>", "<more_ids>", "<assignmentStmt_type1>" , "<assignmentStmt_type2>", "<leftHandSide_singleVar>",
  "<leftHandSide_listVar>", "<rightHandSide_type1>" , "<rightHandSide_type2>" , "<sizeExpression>", "<ifStmt>" ,"<newIf>", "<otherStmts>", "<ioStmt>", "<funCallStmt>",
   "<inputParameterList>", "<listVar>", "<arithmeticExpression>", "<newarithmeticExpression>", "<arithmeticTerm>", "<newarithmeticTerm>" , "<factor>",
    "<operator_lowPrecedence>", "<operator_highPrecedence>", "<booleanExpression>", "<constrainedVars>", "<var>", "<matrix>", "<rows>", "<newRows>", "<row>",
     "<remainingColElements>", "<matrixElement>" , "<logicalOp>" , "<relationalOp>"}; //aux list, used for reading grammar and printing
