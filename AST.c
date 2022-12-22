//Christian Cyr
/*Edited 4/8/2022


*/
// ast function
#include<stdlib.h>
#include<stdio.h>
#include "AST.h"
//pre AST_Node_Type
//post ptr to ASTnod from heap  all links set to null
struct ASTnode * AST_Create_Node(enum AST_Node_Type my_type){
	struct ASTnode *p;
	p = (struct ASTnode *)malloc(sizeof( struct ASTnode));
	p->MyType = my_type;
	p->size=0;
	p->s1=NULL;
	p->s2=NULL;
	p->next=NULL;
	return (p);
	
; 

}
//pre: num to print space
//post: printed n spaces
void PT(int N){
	for(int i = 0;i<N;i++){
	printf(" ");

	}
}
/*
void AST_Print_Operator(enum Operator myOp){
	
}
*/
//pre: enum Operator type
//post: print operator
void AST_Print_Operator(enum A_Operator operator){
	switch(operator){
		case A_PLUS: 
			printf("EXPR +\n");
			break;
		case A_MINUS:
			printf("EXPR -\n");
			break;
		case A_TIMES: 
			printf("EXPR *\n");
			break;
		case A_DIVIDEDBY:
			printf("EXPR /\n");
			break;
		case A_AND: 
			printf("EXPR AND\n");
			break;
		case A_OR:
			printf("EXPR OR\n");
			break;
		case A_LEQ: 
			printf("EXPR <=\n");
			break;
		case A_LESS:
			printf("EXPR <\n");
			break;
		case A_GREATER: 
			printf("EXPR >\n");
			break;
		case A_GEQ:
			printf("EXPR >=\n");
			break;
		case A_EQ: 
			printf("EXPR ==\n");
			break;
		case A_NEQ:
			printf("EXPR !=\n");
			break;
		
		default:
			printf("you shouldn't be here. Problem with printing Operator.");
	}
}
//pre: AST_Declared_Type enum
//post: print string
//we have three different declared types: int, void, and boolean
void Print_Declared_Type(enum AST_Declared_Type mine){
	switch(mine){
		case  A_INT_TYPE:
			printf(" int ");
			break;
		case A_VOID_TYPE:
			printf(" void ");
			break;
		case A_BOOLEAN_TYPE:
			printf(" boolean ");
			break;
		
		default:
			printf("unknown type. ");

	}

}
//pre ptr to astnode
//post formatted output of the ast
void AST_Print (struct ASTnode *p, int level){
 
	if(p==NULL) return;
	//at this point we know p is healthy
	switch(p->MyType){
		//what to do if we have A_VARDEC which is a node type
		//printing the name of the variable as well as size. If array
			//print number in brackets, else print 1 for single var
		case A_VARDEC:
			Print_Declared_Type(p->declared_type);
			printf("vardec ");
			if(p->size>0){
				printf(p->name);
				printf("[%d]",p->size);
			}
			else{
				printf(p->name);
			}
			printf(" offset = %d\n",p->symbol->offset);
			
			AST_Print(p->s1,level+1);
			AST_Print(p->next, level);
			break;
		//case for funtion declaration which is a node type
		case A_FUNDEC:
			Print_Declared_Type(p->declared_type);
			printf(" fundec %s",p->name);
			printf(" offset = %d\n",p->symbol->offset);

			
			AST_Print(p->s1,level+1);	//for param
			AST_Print(p->s2,level+1);	//for compound
			AST_Print(p->next,level+1);  //
			break;
		case A_PARAM:
			Print_Declared_Type(p->declared_type);
			printf("param %s",p->name);
			printf(" offset = %d\n",p->symbol->offset);

			
			AST_Print(p->s1,level+1);
			AST_Print(p->next,level+1);
			break;
		case A_BLOCK:
			PT(level);
			printf("BEGIN\n");
			AST_Print(p->s1, level+1);
			AST_Print(p->s2, level+1);
			PT(level);
			printf("END\n");
			break;
		case A_READ:
			PT(level);
			printf("READ\n");
			AST_Print(p->s1,level+1);//supposed to go to var
			AST_Print(p->next,level);
			break;
		case A_WRITE:
			PT(level);
			printf("WRITE\n");
			AST_Print(p->s1,level+1);//supposed to go to var
			AST_Print(p->next,level+1);
			break;
           	case A_NUM:
			PT(level);
			printf("NUMBER with value %d\n",p->size);
			break;
			/*
		case A_IDEN:
            PT(level);			
            printf("IDENTIFIER named %s\n", p->name);
			if(p->s1!=NULL){
			    PT(level);
			    printf("ARRAY\n");
                            PT(level+1);
			    printf("[\n");
			    AST_Print(p->s1,level+2);
			    PT(level+1);
			    printf("]\n");	
			 }
                        break;
						*/

		case A_EXPR:
			PT(level);
			AST_Print_Operator(p->operator);
			AST_Print(p->s1, level+1);
			AST_Print(p->s2,level+1);			
			break;
		case A_RETURN:
			PT(level);
			AST_Print_Operator(p->operator);
			AST_Print(p->s1, level+1);
			AST_Print(p->next, level-1);
			break;
		case A_IF:
			PT(level);
			if(p->s2->s2!=NULL){
				printf("if \n");			
			
			AST_Print(p->s1,level+1);
			PT(level);
			printf("then\n");
			AST_Print(p->s2,level+1);	
			printf("else\n");	
			}
			else{
			printf("if \n");			
			}
			AST_Print(p->s1,level+1);
			PT(level);
			printf("then\n");
			AST_Print(p->s2,level+1);
			break;
		case A_THEN:
			PT(level);
			AST_Print(p->s1,level+1);
			AST_Print(p->s2,level+1);
			
			break;
			
		case A_ASSIGN:
			PT(level);
		//	printf("ASSIGNMENT\n");
		//	AST_Print(p->s1,level+1);
		//	AST_Print(p->s2,level+1);
			break;
			
		case A_WHILE:
			PT(level);
			printf("WHILE STATEMENT\n");
			AST_Print(p->s1,level+1);
			AST_Print(p->s2,level+1);
			AST_Print(p->next,level+1);
			break;
		case A_BOOL:
			PT(level);
			printf("Boolean\n");
			if(p->size==1){
			PT(level+1);
			printf("TRUE\n");
			}
			else{
			printf("FALSE\n");			
			}
			break;
		case A_CALL:
			PT(level);
			printf("CALL %s\n",p->name);
			AST_Print(p->s1,level+1);
			break;
		case A_ARGLIST:
			PT(level);
			printf("arglist \n");
			AST_Print(p->s1,level+1);
			AST_Print(p->s2,level+1);
			break;
		case A_EXPRSTMT:
			PT(level);
			printf("expresion statement \n");
			if(p->s1==NULL){
				PT(level);
				printf(";\n");
			}
			AST_Print(p->s1,level+1);
			break;
			
		default:
			fprintf(stderr,"warning unknown type in AST_print\n");
	}

}


