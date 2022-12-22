/*
Edited by Christian Cyr
4/29/2022
**
This emit.h contains all the prototypes of the emit.c 
*/
#ifndef EMIT_H
#define EMIT_H




#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AST.h"
#include "symtable.h"
#define WSIZE 4
#define LOGWSIZE 2


FILE *fp;
char * CreateLabel();

void EMITAST(struct ASTnode *p,FILE *fp);

void EMIT_header(struct ASTnode *p, FILE *fp);
void Emit_Global_Strings(struct ASTnode *p, FILE *fp);
void Emit_Global_Variables(struct ASTnode *p,FILE *fp);
void emit(char *label, char *command, char * comment, FILE *fp);
void emit_data_segment(FILE *fp);
void emit_function_head(struct ASTnode *p, FILE *fp);
void emit_write(struct ASTnode *p, FILE *fp);
void Emit_Ident(struct ASTnode *p, FILE *fp);
void emit_expr(struct ASTnode *p, FILE *fp);
void emit_read(struct ASTnode *p, FILE *fp);
void emit_assign(struct ASTnode *p, FILE *fp);
void emit_if(struct ASTnode *p,FILE *fp);
void emit_while(struct ASTnode *p, FILE *fp);
#endif
