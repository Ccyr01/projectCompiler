%{
/*
Edited by: Christian Cyr
            4/20/2022
			New addition of T_Write T_Quoted_String rules 
			 and store string as name.
			4/8/2022
			Symbol table requires semantic action in C. It keeps track of
			the variables we declare as well as the functions, calls, and arguments.
			The offset starts at 0 for global level, and the level increments with every 
			compound block statement. Type checking is also checking for declared_type, if they match
			or else barf.
			
		
*/
/*
 *			**** CALC ****
 *
 * This routine will function like a desk calculator
 * There are 26 integer registers, named 'a' thru 'z'
 *
 */

/* This calculator depends on a LEX description which outputs either VARIABLE or INTEGER.
   The return type via yylval is integer

   When we need to make yylval more complicated, we need to define a pointer type for yylval
   and to instruct YACC to use a new type so that we can pass back better values

   The registers are based on 0, so we substract 'a' from each single letter we get.

   based on context, we  have YACC do the correct memmory look up or the storage depending
   on position

   Shaun Cooper
    Janua9ry 2015

   problems  fix unary minus, fix parenthesis, add multiplication
   problems  make it so that verbose is on and off with an input argument instead of compiled in
*/


	/* begin specs */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "AST.h"
extern int lineno;
#include "symtable.h"
#include <string.h>
#include "emit.h"
static int mydebug;
int maxoffset;
int level =0;
int offset =0;
int goffset =0;
#define FUNCTION_MIN_SIZE 2

struct ASTnode * Program=NULL;//gloval var set to null


int yylex ();//prototype for yylex


int numVars = 0;

void yyerror (s)  /* Called by yyparse on error */
     char *s;
{
  printf ("%s at line number %d\n", s,lineno );
}


%}
/*  defines the start symbol, what values come back from LEX and how the operators are associated  */
//p is the starting point
//union allows ints and char *
%start program

%union{
	int value;
	char *string;
	enum AST_Declared_Type declared_type;
	enum A_Operator operator;
	struct ASTnode * astnode;
}

%token <value> T_NUM
%token <string> T_ID T_QUOTED_STRING
%token T_INT T_VOID T_END T_BEGIN T_OF T_READ T_BOOLEAN T_IF T_THEN T_ELSE
%token T_WHILE T_DO T_TRUE T_FALSE T_AND T_OR T_NOT T_RETURN
%token T_LEQ T_EQ T_GEQ T_NEQ T_WRITE 
%type<astnode> declaration_list declaration var_declaration var_list fun_declaration statement statement_list var
%type<astnode> param param_list params return_stmt read_stmt write_stmt assignment_stmt expression_stmt compound_stmt local_declarations 
%type<astnode> term factor expression simple_expression additive_expression selection_stmt iteration_stmt arg_list call args
%type<declared_type> type_specifier 
%type<operator> addop multop relop


%%	/* end specs, begin rules */
program : declaration_list {Program =$1;}
  		;

declaration_list : declaration 			{$$=$1;}
	| declaration declaration_list 		{$$ = $1; $1->next = $2;}
	;
declaration : var_declaration 				{$$ = $1;}
	| fun_declaration 				{$$=$1;}
	;
var_declaration : type_specifier var_list ';'
								{$$=$2;
								struct ASTnode * p;
								p=$2;
								//travers list 
									while(p != NULL){
										p->declared_type=$1;
										p=p->s1;
									}
								}
	;

var_list: T_ID   				{
                                struct SymbTab *p;
                                if(Search($1, level,0)==NULL){
								$$=AST_Create_Node(A_VARDEC);
								$$->symbol = Insert($1,0,0,level,1,offset);
								$$->name =$1;
								offset += 1;
								}
								else{
								yyerror($1);
								yyerror("symbol already used");
								exit(1);
								}
								}
	| T_ID '[' T_NUM ']'  		{
								struct SymbTab *p;
									if(Search($1,level,0)==NULL){
									$$ =AST_Create_Node(A_VARDEC);
									$$->name=$1;
									$$->size=$3;
									$$->symbol=Insert($1,0,2,level,$3,offset);
									offset += $3;

									}
									else{
									yyerror($1);
									yyerror("symbol already used");
									exit(1);
									}	
								}			
	| T_ID ',' var_list			{
								struct SymbTab *p;
								if(Search($1,level,0)==NULL){
									$$=AST_Create_Node(A_VARDEC);
									$$->name =$1;
									$$->s1=$3;
									$$->size =1;
									$$->symbol=Insert($1,0,0,level,1,offset);
									offset+=1;
					
								}
								else{
									yyerror($1);
									yyerror("symbol already used");
									exit(1);
								}
								}		
	| T_ID '[' T_NUM ']' ',' var_list	{struct StmbTab *p;
											if(Search($1,level,0)==NULL){
											$$ = AST_Create_Node(A_VARDEC);
											$$->name=$1;
											$$->size=$3;
											$$->s1=$6;
											$$->symbol=Insert($1,0,2,level,$3,offset);
											offset += $3;
											}
											else{
											yyerror($1);
											yyerror("symbol already used ");
											exit(1);
											}	
										}		
	;

type_specifier : T_INT		{$$=A_INT_TYPE;}//label the type inside of the box we call the node since it is the type specifier
	| T_VOID				{$$=A_VOID_TYPE;}
	| T_BOOLEAN				{$$=A_BOOLEAN_TYPE;}
	;
fun_declaration : type_specifier T_ID '(' { if(Search($2,level,0)==NULL){
											Insert($2,$1,1,level,0,0);
											goffset = offset;
											offset=FUNCTION_MIN_SIZE;
											maxoffset = 0;
											}
											else{
												yyerror($2);
												yyerror("name already used. ");
												exit(1);
											}
										}
params{(Search($2,0,0))->fparms = $5;}

 ')' compound_stmt  {
									$$=AST_Create_Node(A_FUNDEC);
									$$->declared_type=$1;
									
									$$ ->name =$2;
									$$->s1=$5;
									$$->s2=$8;
									offset=goffset;
									$$->symbol=Search($2,0,0);
									$$->symbol->mysize=maxoffset;
									//might need $$->size/val = maxoffset
									}
	;

params : T_VOID						{$$ = NULL;}
	| param_list						{$$=$1;}
	;
param_list: param						{$$=$1;}
	| param ',' param_list 				{$$=$1;
										$$->next =$3;
										}
	;
param: T_ID T_OF type_specifier			{if(Search($1,level+1,0)==NULL){
										$$= AST_Create_Node(A_PARAM);//parameter deserves box of its own with labels
										$$->name=$1;
										$$->size=0;
										$$->declared_type=$3;
										$$->symbol=Insert($1,$3,0,level+1,1,offset);
					
										offset+=1;
										
										}
										else{
											yyerror($1);
											yyerror("symbol already used ");
											exit(1);
										}
										}
	| T_ID'['']' T_OF type_specifier	{if(Search($1,level+1,0)==NULL){
											$$=AST_Create_Node(A_PARAM);
											$$->name=$1;
											$$->size=-1;
											$$->declared_type=$5;
											$$->symbol=Insert($1,$5,0,level+1,1,offset);
											offset+=1;
	
											}
											else{
												yyerror($1);
												yyerror("symbol already used");
												exit(1);
											}
										}
	;

compound_stmt: T_BEGIN {level++;}local_declarations statement_list T_END
					{$$ = AST_Create_Node(A_BLOCK);//block gets box of its own with branches to decls and stmt list 
					$$->s1=$3;
					$$->s2=$4;
					if(mydebug) Display();
					if(offset>maxoffset) maxoffset=offset;
					offset -= Delete(level);
					level--;
					}
	;
local_declarations: /* empty */			{$$ = NULL;}
	|	var_declaration local_declarations
										{$$=$1;
										$$->next=$2;
											
										}
	;
statement_list : /*empty*/				{$$=NULL;}
	| statement statement_list			{$$=$1;
										$$->next=$2;
										}
	;
statement : expression_stmt				{$$=$1;}
	| compound_stmt						{$$=$1;}
	| selection_stmt					{$$=$1;}
	| iteration_stmt					{$$=$1;}
	| assignment_stmt					{$$=$1;}
	| return_stmt						{$$=$1;}
	| read_stmt							{$$=$1;}
	| write_stmt						{$$=$1;}
	;
expression_stmt : expression ';'		{$$=AST_Create_Node(A_EXPRSTMT);
								
										$$->s1=$1;
										}
	| ';'								{$$=AST_Create_Node(A_EXPRSTMT);
										
										}
	;

selection_stmt : T_IF expression T_THEN statement					{$$=AST_Create_Node(A_IF);
																	$$->s1=$2;
																	$$->s2=AST_Create_Node(A_THEN);
																	$$->s2->s1=$4;
																	}
	| T_IF expression T_THEN statement  T_ELSE statement										
                        { $$=AST_Create_Node(A_IF);
						$$->s1=$2;
						$$->s2=AST_Create_Node(A_THEN);
						$$->s2->s1=$4;
						$$->s2->s2=$6;
						}
	;
iteration_stmt : T_WHILE expression T_DO statement			{$$=AST_Create_Node(A_WHILE);
									$$->s1=$2;
									$$->s2=$4;
									
									}
	;
return_stmt : T_RETURN ';'			{$$=AST_Create_Node(A_RETURN);
						
						}
	| T_RETURN expression ';'		{$$=AST_Create_Node(A_RETURN);
									$$->s1=$2;
						}
	;
read_stmt : T_READ var ';'			{$$=AST_Create_Node(A_READ);
									$$->s1=$2;				//s1 goes to var
									}
	;
write_stmt : T_WRITE expression ';'	{$$=AST_Create_Node(A_WRITE);
									$$->s1=$2;

									}
    | T_WRITE T_QUOTED_STRING ';'   {$$=AST_Create_Node(A_WRITE);
                                    $$->label=CreateLabel();
                                    $$->name=$2;
                                    }
	;
assignment_stmt : var '=' simple_expression ';'{$$=AST_Create_Node(A_ASSIGN);
						$$->s1=$1;
						$$->s2=$3;
                        $$->name=CreateTemp();
                        $$->symbol=Insert($$->name,A_INT_TYPE,0,level,1,offset);
                        offset++;
                }
	;
expression : simple_expression { $$= $1;}
	;
var : T_ID 						{struct SymbTab *p;
									p=Search($1,level,1);
									if(p!=NULL && p->IsAFunc==0){
									$$=AST_Create_Node(A_IDEN);
									$$->name=$1;
									$$->symbol=p;
									$$->isType=p->Type;
									}
									else{
										if(p!=NULL && p->IsAFunc==1)
										{
										yyerror($1);
										yyerror("symbol is declared as a function not scalar, syntax error.");
										exit(1);
										}
										yyerror($1);
										yyerror("undeclared var");
										exit(1);
										}
									
								}
	| T_ID '['expression ']'	{struct SymbTab *p;
								p=Search($1,level,1);
								if(p!=NULL && p->IsAFunc==2){
								$$=AST_Create_Node(A_IDEN);
								$$->name=$1;
								$$->s1=$3;//expression gets branched to 
								$$->symbol=p;
								$$->isType=p->Type;
								}
								else{
									printf("undeclared variable, syntax error");

								}
								}

	;

simple_expression : additive_expression { $$= $1;}
	| simple_expression  relop additive_expression {
													if($1->isType != $3->isType){
														yyerror(" type mismatch for additive");
														exit(1);
													}
													$$=AST_Create_Node(A_EXPR);
													$$->s1=$1;
													$$->s2=$3;
                                                    $$->isType=$3->isType;
													$$->operator=$2;
													$$->name=CreateTemp();
													$$->symbol=Insert($$->name,A_INT_TYPE,0,level,1,offset);
													offset++;
													
													}
	;
relop :	T_LEQ					{$$=A_LEQ;}
	| '<'						{$$=A_LESS;}
	| '>'						{$$=A_GREATER;}
	| T_GEQ					{$$=A_GEQ;}
	| T_EQ						{$$=A_EQ;}
	| T_NEQ					{$$=A_NEQ;}
	;
additive_expression : term { $$= $1;}
	| additive_expression addop term		{//type check
											if($1->isType != $3->isType){
												yyerror("type mismatch in multiplication. ");
												exit(1);
											}
											$$=AST_Create_Node(A_EXPR);
											$$->s1 = $1;
											$$->operator=$2;
											$$->s2=$3;
											$$->isType=A_INT_TYPE;
											$$->name=CreateTemp();
											$$->symbol=Insert($$->name,A_INT_TYPE,0,level,1,offset);
											offset++;
											}
	;
addop : '+'									{$$=A_PLUS;}
	| '-'									{$$=A_MINUS;}
	;
term : factor { $$= $1;}
	| term multop factor					{//type check
											if($1->isType != $3->isType){
												yyerror("type mismatch in multiplication. ");
												exit(1);
											}
											$$=AST_Create_Node(A_EXPR);
											$$->s1=$1;
											$$->operator=$2;
											$$->s2=$3;
											$$->isType=A_INT_TYPE;
											$$->name=CreateTemp();
											$$->symbol=Insert($$->name,A_INT_TYPE,0,level,1,offset);
											offset++;
											}
	;	
multop : '*'						{$$=A_TIMES;}
	| '/'						{$$=A_DIVIDEDBY;}
	| T_AND					{$$=A_AND;}
	| T_OR						{$$=A_OR;}
	;
factor : '(' expression ')'   	{$$=$2;}
	| T_NUM		{$$=AST_Create_Node(A_NUM);
				$$->size=$1;
				$$->isType=A_INT_TYPE;				
				}
	| var		{$$=$1;}
	| call 		{$$=$1;}
	| T_TRUE		{$$=AST_Create_Node(A_BOOL);
					$$->size=1;	
					$$->isType=A_BOOLEAN_TYPE;		
					}
	| T_FALSE		{$$=AST_Create_Node(A_BOOL);
					$$->size=0;
					$$->isType=A_BOOLEAN_TYPE;
					}
	| T_NOT factor		{$$=AST_Create_Node(A_EXPR);
                        $$->operator=A_NOT;
                        $$->s1=$2;
                        }
	;
call : T_ID '(' args ')'	{
							struct SymbTab *p;
							if((p=Search($1,0,0)) != NULL){
								if(p->IsAFunc != 1){
									yyerror($1);
									yyerror("Function name not defined ");
									exit(1);
								}
								
								if(CompareFormals(p->fparms,$3) != 1){
									yyerror($1);
									yyerror("parameter type or length mismatch between formal and actuals");
									exit(1);
								}
								
								$$=AST_Create_Node(A_CALL);
								$$->name=$1;
								$$->s1=$3;
								$$->isType=p->Type;
							}
							else{

							}
							
							
							}		
	;
args : arg_list				{$$=$1;
							}
	| /*empty*/				{$$=NULL;}
	;
arg_list : expression 		{$$=AST_Create_Node(A_ARGLIST);
							$$->s1=$1;
							$$->isType=$1->isType;
							$$->next=NULL;
							$$->name=CreateTemp();
							$$->symbol=Insert($$->name,A_INT_TYPE,0,level, 1, offset);
							offset++;
							}
	| expression ',' arg_list {$$=AST_Create_Node(A_ARGLIST);
								$$->s1=$1;
								$$->isType=$1->isType;
								$$->next=$3;
								$$->name=CreateTemp();
								$$->symbol=Insert($$->name,A_INT_TYPE,0,level, 1, offset);
								offset++;
								}
%%	/* end of rules, start of program */

int main(int argc, char *argv[])          //added a return type int
{ 
	int c=1;
	FILE *fp = NULL;
	char s[100];
	while(c<argc){
		if(strcmp(argv[c], "-d")==0) mydebug=1;

		if(strcmp(argv[c], "-o")==0){
			strcpy(s, argv[c+1]);
			strcat(s, ".asm");
			fp=fopen(s,"w");
			if(fp==NULL){
				printf("can't open %s \n",s);
				exit(1);
			}
		}
		c++;
	}
	if(fp==NULL){
		printf("must provide filename\n");
		exit(1);
	}
	yyparse();
	if(mydebug) Display();
	EMIT_header(Program,fp);
}
