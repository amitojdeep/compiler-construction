/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "ast.h"

parseTree* createAST(parseTree* root){
	//using child pointer for addr
	if(root->type == 0){
		//NT
		parseTree* currChild;
		if(root->val == mainFunction || root->val == stmtOrFunctionDef || root->val == stmt || root->val == leftHandSide_listVar || root->val == rightHandSide_type1
			|| root->val == rightHandSide_type2 ||root->val == matrix || root->val == factor || root->val == newStmts || root->val == more_ids || root->val == newRows 
			|| root->val == remainingList || root->val == listVar){
			//just addr pulling from first NT child and EPS if applicable
			currChild = root->child;
			if(currChild->type && currChild->val == EPS){
				//free(currChild);
				//free(root);
				return NULL;
			}
			while(currChild->type){
				currChild = currChild->next;
			}
			root->child = createAST(currChild);
			//remove the terminals like SEMICOLON, END
			if(root->val == mainFunction || root->val == stmt){
				currChild = root->child;
				while(currChild->next && ((currChild->type && currChild->val == ID) || !currChild->next->type)){
					currChild = currChild->next;
				}
				currChild->next = NULL;
			}		
			return root->child;

		}
		else if(root->val ==  var_list || root->val ==  otherStmts || root->val == inputParameterList || root->val == rows){
			//add link from addr of more_ids/otherStmts(second child) to ID/stmt(first child)
			if(root->child->type && root->child->val == EPS){
				return root->child = NULL;
			}
			currChild = root->child->next;
			root->child = createAST(root->child);
			root->child->next = createAST(currChild);
			//free(currChild);
			return root->child;					
		}
		else if(root->val == row){
			root->child->next = createAST(root->child->next);
			return root;
		}
		else if(root->val == type || root->val == leftHandSide_singleVar || root->val == operator_lowPrecedence || root->val == operator_highPrecedence
			|| root->val == constrainedVars || root->val == logicalOp || root->val == relationalOp){
			//just addr pulling from first T child
			currChild = root->child;
			return currChild;
		}
		else if(root->val == stmtsAndFuntionDefs || root->val == declarationStmt || root->val == assignmentStmt_type1 
			 || root->val == newIf || root->val == booleanExpression || root->val == functionDef){
			//new node using addr from all NT children
			//better alter same node keeping reqd children as linked list
			parseTree* funidNode;
			if(root->val == functionDef)funidNode = root->child->next->next->next->next->next;
			currChild = root->child;
			while(currChild->type){
				currChild = currChild->next;
			}
			//make first NT child as the first child
			root->child = createAST(currChild);
			parseTree* prevChild = root->child;
			currChild = currChild->next;
			//free(tempChild);
			while(currChild){
			//loop over all children to find more NT children
				while(currChild && currChild->type){
					currChild = currChild->next;
				}
				if(!currChild)break;
				if(!prevChild)break; //param list was empty
				prevChild->next = createAST(currChild); //add to ll
				if(prevChild->next)prevChild = prevChild->next;
				currChild = currChild->next;
				//free(tempChild);
			}
			//remove the terminals like SEMICOLON, END
			if(root->val == functionDef){
				currChild = root->child;
				while(currChild->next && ((currChild->type && currChild->val == ID) || !currChild->next->type)){
					currChild = currChild->next;
				}
				currChild->next = NULL;
				root->token = funidNode->token;
				root->val = funidNode->val;
				root->type = funidNode->type;
			}
			return root; //return the "new" node
		}
		else if(root->val == ifStmt){
			parseTree* newIfNode = root->child->next->next->next->next->next->next;
			parseTree* otherStmtsNode = root->child->next->next->next->next->next;
			root->child = createAST(root->child->next->next); //boolExp
			root->child->next = createAST(root->child->next->next); //stmt
			root->child->next->next = createAST(otherStmtsNode); //otherStmts
			// go to end of linked list of stmts derived from other stmts
			currChild = root->child; //first in the overall linked list
			while(currChild->next)currChild = currChild->next;
			currChild->next = createAST(newIfNode);
			return root;
		}

		else if(root->val == assignmentStmt_type2){
			root->child->next = createAST(root->child->next->next); //rhstype2
			root->child->child = createAST(root->child->child->next);//var_list
			return root;
		}
		
		else if(root->val == sizeExpression){
			//new node from addr of all T children
			root->child = root->child->next; //remove SIZE
			return root; //nothing to do as new node is same
		}
		else if(root->val ==  ioStmt){
			//make "new" node with first child PRINT, second ID
			//PRINT is already first child
			root->child->next = root->child->next->next; //make ID next of PRINT
			root->child->next->next = NULL;
			return root;
		}
		else if(root->val ==  funCallStmt){
			//FUNID is already first child
			parseTree* OP = root->child->next; 
			root->child->next = createAST(root->child->next->next); //called on inputParameterList making it second child
			return root;
			//free(OP);
			root->child->next->next = NULL;
			////free(root->child->next->next); ////free CL
		}
		else if(root->val == remainingColElements){
			if(root->child->type && root->child->val == EPS){
				return root->child = NULL;
			}
			currChild = root->child->next;
			//free(root->child); //remove COMMA
			root->child = currChild; //make NUM the first child
			currChild = root->child->next;
			root->child->next = createAST(root->child->next);
			//free(currChild);
			return root->child;		

		}
		else if(root->val == var){
			if(root->child->type && root->child->val == ID)	{
				currChild = root->child->next;
				root->child->next = createAST(currChild);
			}
			if(!root->child->type && root->child->val == matrix){
				return createAST(root->child);
			}
			return root->child;

		}
		else if(root->val == matrixElement){
			if(root->child->type && root->child->val == EPS){
				return NULL;
			}
			root->child = root->child->next; //first NUM is first child
			root->child->next = root->child->next->next; //second NUM
			root->child->next->next = NULL;
			return root->child;			
		}
		else if(root->val == arithmeticExpression || root->val == arithmeticTerm){
			//using parent for inherited address
			//printf("is this newArith: %s\n",NTlist[root->child->next->val]);
			root->child->next->parent = createAST(root->child); //set inh_addr
			return createAST(root->child->next); //return address of new arith
		}
		else if(root->val == newarithmeticExpression || root->val == newarithmeticTerm){
			//if(!root->child)return root->parent;//fix for CL case
			currChild = root->child;
			if(currChild->type && currChild->val == EPS){
				// /printf("\nEPS child of newArith");
				return root->parent; //return inh_addr
			}
			parseTree* inh = root->parent; //inh_addr
			currChild = root->child->next; //arith
			/*symbol sym;
			sym.type = root->child->child->type;
			sym.val = root->child->child->val;*/
			//parseTree* node = newParseTreeNode(&sym, root);			
			parseTree* node = createAST(root->child);
			//printf("Address copied %d %d\n", inh->type, inh->val);
			node->child = inh; //inh
			node->child->next = createAST(currChild);
			return node; 
		}
		else if(root->val == parameter_list){
			parseTree* currChild = root->child->next; //ID
			root->child = createAST(root->child);//type
			root->child->next = currChild;
			currChild = root->child->next->next;//remList
			root->child->next->next = createAST(currChild);
			//root->next = createAST(currChild);
			return root;
		}
		else return root; //rules not yet defined
	}
	else{
		//T
		return root;
	}
}