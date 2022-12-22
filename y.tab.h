/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_NUM = 258,
    T_ID = 259,
    T_QUOTED_STRING = 260,
    T_INT = 261,
    T_VOID = 262,
    T_END = 263,
    T_BEGIN = 264,
    T_OF = 265,
    T_READ = 266,
    T_BOOLEAN = 267,
    T_IF = 268,
    T_THEN = 269,
    T_ELSE = 270,
    T_WHILE = 271,
    T_DO = 272,
    T_TRUE = 273,
    T_FALSE = 274,
    T_AND = 275,
    T_OR = 276,
    T_NOT = 277,
    T_RETURN = 278,
    T_LEQ = 279,
    T_EQ = 280,
    T_GEQ = 281,
    T_NEQ = 282,
    T_WRITE = 283
  };
#endif
/* Tokens.  */
#define T_NUM 258
#define T_ID 259
#define T_QUOTED_STRING 260
#define T_INT 261
#define T_VOID 262
#define T_END 263
#define T_BEGIN 264
#define T_OF 265
#define T_READ 266
#define T_BOOLEAN 267
#define T_IF 268
#define T_THEN 269
#define T_ELSE 270
#define T_WHILE 271
#define T_DO 272
#define T_TRUE 273
#define T_FALSE 274
#define T_AND 275
#define T_OR 276
#define T_NOT 277
#define T_RETURN 278
#define T_LEQ 279
#define T_EQ 280
#define T_GEQ 281
#define T_NEQ 282
#define T_WRITE 283

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 77 "Lab9.y" /* yacc.c:1909  */

	int value;
	char *string;
	enum AST_Declared_Type declared_type;
	enum A_Operator operator;
	struct ASTnode * astnode;

#line 118 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
