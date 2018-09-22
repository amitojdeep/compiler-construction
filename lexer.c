/*Name : Amitojdeep Singh
      ID: 2014B3A70615P*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

/* --------------------------------------------------buffer control-------------------------------------- */
void getBuffer(FILE* fp){
	//allocate mem to global pointer of buffer
	buffer = (char*)malloc(2*buffSize*sizeof(char));
	 //fill the first half
	getStream(fp, buffer, buffSize);
	begin = 0; //intialize indices
	fwd = -1; //check
}

//function to increment forward
void incFwd(){
	if(fwd == buffSize-1){
		//load in second half when mid is reached
		getStream(fp, (buffer+buffSize), buffSize); 
		fwd++;
	}
	else if(fwd == 2*buffSize - 1){
		//end reached
		getStream(fp, buffer, buffSize);
		fwd = 0;
	}
	else fwd++;
}

//decrement forward when retracting, handle possible buffer overwrite later
void decFwd(){
	if(fwd == 0){
		fwd = 2*buffSize-1; //take pointer to end of last used buffer
	}
	
	else fwd--;
}
/* --------------------------------------------------aux functions-------------------------------------- */
void error(){
	printf("\n: Lexical error at %d", currLine);
}

//returns string between begin & fwd (both inclusive)
int getVal(char* str){
	int i,j=0,sz = 0;
	//Case 1: fwd is in front buffer, begin is in end buffer
	if(fwd<begin){
		for(i = begin; i < 2*buffSize; i++){
			str[j++] = buffer[i];
			sz++;
		}
		for(i = 0; i <=fwd; i++){
			str[j++] = buffer[i];
			sz++;
		}
	}
	// Case 2: Normal
	else{
		for(i = begin; i <=fwd; i++){
			str[j++] = buffer[i];
			sz++;
		}
	}
	return sz;
}

/* --------------------------------------------------core functions--------------------------------------*/


// Read source code file to buffer, call initially in main
FILE *getStream(FILE *fp, char* buff, int size){
	int bytesRead = fread((buff),1, size,fp); //fill the first half
	//printf("Successfully read %d bytes\n", bytesRead);
	if(bytesRead < size)buff[bytesRead] = '\0';
	return fp;
}

tokenInfo* getNextToken(){

	tokenInfo* tk = (tokenInfo*)malloc(sizeof(tokenInfo));
	tk->val = NULL;
	tk->name = -1;
	char* str = (char*)malloc(100*sizeof(char));//placeholder for token name
	//lookup table
	char* k[] = {"_main", "int", "end", "real", "string", "matrix", "if", "else", "endif", "print" , "read", "function"};
	int v[] = {MAIN, INT, END, REAL, STRING, MATRIX, IF, ELSE, ENDIF, PRINT, READ, FUNCTION}; //according to enum
	
	incFwd();
	int found = 0;
	while(!found){
		switch(buffer[fwd]){
		case '\t':{
			//tab
			
			incFwd();
			begin++;
			//incFwd();
			//printf("\n Begin is at %c, Fwd is at %c", buffer[begin], buffer[fwd]);
			break;
		}
		case ' ':{
			//whitespace
			
			incFwd();
			begin++;
			/*incFwd();
			if(buffer[fwd] == '\t'){
				begin++;
				break;
			}
			decFwd();*/
			//printf("\n Begin is at %c, Fwd is at %c", buffer[begin], buffer[fwd]);
			break;
		}
	
		case '\n':{			
			begin++;
			incFwd();
			currLine++;
			break;	
		}
		case 13:{
			//^M			
			begin++;
			incFwd();
			currLine++;
			break;	
		}
		case '#':{
			while(buffer[fwd]!='\n'){
				incFwd();
				begin++;
			} //skip comments
				
			break;
		}
		case '=':{
			incFwd();
			if(buffer[fwd] == '='){
				tk->name = EQ;
			}
			else if(buffer[fwd] == '/'){
				incFwd();
				if(buffer[fwd] == '='){
					tk->name = NE;
				}
				else{
					getVal(str);
					printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
				}
			}
			else{
				tk->name = ASSIGNOP;
				decFwd(); //retract			
			}
			found++;
			break;
		}
		
		case '_':{
			incFwd();
			while(isalpha(buffer[fwd]))incFwd();
			//lookup main
			decFwd();
			getVal(str);
			if(strcmp(str,k[0]) == 0){
				tk->name = MAIN; 
			}			
			else tk->name = FUNID;
			found++;
			break;
		}
		case '"':{
			//strings can have blanks
			incFwd();
			while((buffer[fwd]>='a' && buffer[fwd]<='z') || buffer[fwd] == ' ')incFwd();
			if(buffer[fwd] == '"') tk->name = STR;
			else {
				getVal(str);
				printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
			}
			found++;
			break;
		}
		case '[':{
			tk->name = SQO;
			found++;
			break;
		}
		case ']':{
			tk->name = SQC;
			found++;
			break;
		}
		case '(':{
			tk->name = OP;
			found++;
			break;
		}
		case ')':{
			tk->name = CL;
			found++;
			break;
		}
		case ';':{
			tk->name = SEMICOLON;
			found++;
			break;
		}
		case ',':{
			tk->name = COMMA;
			found++;
			break;
		}
		case '+':{
			tk->name = PLUS;
			found++;
			break; 
		}
		case '-':{
			tk->name = MINUS;
			found++;
			break;
		}
		case '*':{
			tk->name = MUL;
			found++;
			break;
		}
		case '/':{
			tk->name = DIV;
			found++;
			break;
		}
		case '@':{
			tk->name = SIZE;
			found++;
			break;
		}
		case '.':{
			incFwd();
			if(buffer[fwd] == 'a'){
				incFwd();
				if(buffer[fwd] == 'n'){
					incFwd();
					if(buffer[fwd] == 'd'){
						incFwd();
						if(buffer[fwd] == '.'){
							tk->name = AND;
						}	
						else{
							getVal(str);
							printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
						}
					}
					else{
						getVal(str);
						printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
					}
				}
				else{
					getVal(str);
					printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
				}				
			}
			else if(buffer[fwd] == 'o'){
				incFwd();
				if(buffer[fwd] == 'r'){
					incFwd();
					if(buffer[fwd] == '.'){
						tk->name = OR;
					}
					else{
						getVal(str);
						printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
					}
					
				}
				else{
					getVal(str);
					printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
				}	
			}
			else if(buffer[fwd] == 'n'){
				incFwd();
				if(buffer[fwd] == 'o'){
					incFwd();
					if(buffer[fwd] == 't'){
						incFwd();
						if(buffer[fwd] == '.'){
							tk->name = NOT;
						}
						else{
							getVal(str);
							printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
						}
					}
					else{
						getVal(str);
						printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
					}
				}
				else{
					getVal(str);
					printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
				}	
			}
			else{
					getVal(str);
					printf("\n%d: Lexical error: Unknown pattern %s",currLine, str);
				}
			
			found++;
			break;

		}
		case '<':{
			incFwd();
			if(buffer[fwd] == '=')
				tk->name = LE;
			else{
				tk->name = LT;
				decFwd();
			}
			found++;
			break;
		}
		case '>':{
			incFwd();
			if(buffer[fwd] == '=')
				tk->name = GE;
			else{
				tk->name = GT;
				decFwd();
			}
			found++;
			break;
		}
		
		case '\0':{
			//printf("End of File!");
			tk->name = -2;
			return tk;
		}

		default:{
			if(isalpha(buffer[fwd])){
				//ID
				while(isalpha(buffer[fwd]))incFwd();
				if(isdigit(buffer[fwd]))incFwd();
				decFwd();
					//lookup
				getVal(str);
				int loc, flag = 0;
				for(loc = 1; loc < 12; loc++){
					if(strcmp(str,k[loc]) == 0){
						tk->name = (token)(v[loc]);
						flag = 1;
						break;
					}
				}
						
				if(!flag)tk->name = ID; 
			}
			else if(isdigit(buffer[fwd])){
				//NUM or RNUM
				while(isdigit(buffer[fwd]))incFwd();
				//incFwd();
				if(buffer[fwd] == '.'){
					//RNUM, only 2 digits after decimal
					incFwd();
					if(isdigit(buffer[fwd]))incFwd();
					if(isdigit(buffer[fwd]))incFwd();
					tk->name = RNUM;
					decFwd();
				}
				else{
					tk->name = NUM;
					decFwd();
				}
				
			}
			else{
				
				getVal(str);
				printf("\n%d: Lexical error: Unknown Symbol %c", currLine, buffer[fwd]);
				//unknown character appears
				begin = fwd + 1;
				return tk;
			}
			found++;
		}
		}
	}
	tk->line = currLine;
	int tkSize = getVal(str);
	//length checks
	if(tk->name== ID && tkSize >20){
		printf("\n%d: Lexical error: Identifier is longer than the prescribed length", currLine);
		begin = fwd + 1;
		return tk;
	}
	
	if(tk->name== STR && tkSize >22){
		printf("\n%d: Lexical error: Identifier is longer than the prescribed length", currLine);
		begin = fwd + 1;
		return tk;
	}

	if(tk->name == NUM){
		tk->num = atoi(str);
	}
	else if(tk->name == RNUM){
		tk->rnum = atof(str);
	}
	
	tk->val = (char*)malloc(tkSize*sizeof(char));
	strcpy(tk->val,str);

	if(tk->name == STR){
		tk->val = (char*)malloc(tkSize-1*sizeof(char));
		int x;
		for(x = 0; x < tkSize-2; x++){
			tk->val[x] = str[x+1];
		}
		tk->val[tkSize-2] = '\0';
	}
		
	begin = fwd + 1;
	return tk;		
}

char* removeComments(){
	fseek(fp, 0L, SEEK_END); //seek to end of input file
	int fileSize = ftell(fp); //get the total size of all chars in file
	rewind(fp);// rewind to beginning
	char* testcaseFile = (char*)malloc(fileSize*sizeof(char));
	char* cleanFile = (char*)malloc(fileSize*sizeof(char));
	getStream(fp, testcaseFile, fileSize);
	rewind(fp);// rewind to beginning
	int i = 0, j = 0;
	while(testcaseFile[i]!='\0'){
		if(testcaseFile[i] == '#'){
			while(testcaseFile[i]!='\n')i++;
			i++; //to avoid new line character
			continue;
		}
		else{
			cleanFile[j] = testcaseFile[i];
			j++;
			i++;
		}
	}
	cleanFile[j] = '\0';
	return cleanFile;
}
