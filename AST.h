//ast.h
//Christian Cyr
/*Edited 4/8/2022
All node type declarations made here Prototypes
*/
#ifndef  AST_H
#define AST_H

enum AST_Node_Type{
	A_VARDEC,
	A_FUNDEC,
	A_PARAM,
	A_BLOCK,
	A_READ,
	A_IDEN,
	A_EXPR,
	A_RETURN,
	A_WRITE,
	A_ASSIGN,
	A_NUM,
	A_IF,
	A_THEN,
	A_WHILE,
	A_BOOL,
	A_CALL,
	A_ARGLIST,
	A_EXPRSTMT
};
//main stat structure of AST
enum AST_Declared_Type{
	A_INT_TYPE, A_BOOLEAN_TYPE,A_VOID_TYPE
};
enum A_Operator{
	A_PLUS, A_MINUS, A_TIMES, A_DIVIDEDBY, A_AND, A_OR, A_LEQ, A_LESS, A_GREATER, A_GEQ, A_EQ, A_NEQ, A_NOT
};

#include "symtable.h"
struct ASTnode {
	enum AST_Node_Type MyType;
	enum AST_Declared_Type declared_type;
	enum A_Operator operator;
	enum AST_Node_Type isType;
	char * name;
	char * label;
	int size;
    struct SymbTab *symbol;
	struct ASTnode *s1, *s2, *next, *formals, *actual;


};
struct ASTnode * AST_Create_Node(enum AST_Node_Type my_type);
void PT(int N);
void AST_Print (struct ASTnode *p, int level);


#endif
