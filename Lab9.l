/*
    Edited by: Christian Cyr
        //4/29/2022
        //T_Quoted_String is a new addition to tokenize what's 
        //   inside the paranthesis and send it to YACC
        // All rules to make tokens out of keywords 
        //Also recognize operators id's and nums
                    Small LEX routine which returns two formal tokens (INTEGER and VARIABLE)
                   along with single string elements like '+'.

                    This LEX definition is the companion to the docalc.y YACC routine which
                   is a simple calculator

                    Shaun Cooper
                    January 2015

*/

%{
//including header files
int mydebug=0;
int lineno=1;
#include "AST.h"
#include "y.tab.h"
#include "symtable.h"

%}


%% 
int		    {return(T_INT);}       
void		{return(T_VOID);}
boolean	    {return(T_BOOLEAN);}  
begin		{return(T_BEGIN);}  
end		    {return(T_END);}
of		    {return(T_OF);}
read		{return(T_READ);}
write       {return(T_WRITE);}
if          {return(T_IF);}
then        {return(T_THEN);}
else        {return(T_ELSE);}
while       {return(T_WHILE);}
do          {return(T_DO);}
true        {return(T_TRUE);}
false       {return(T_FALSE);}
and         {return(T_AND);}
or          {return(T_OR);}
not         {return(T_NOT);}
return      {return(T_RETURN);}
"<="        {return(T_LEQ);}
">="        {return(T_GEQ);}
"=="        {return(T_EQ);}
"!="        {return(T_NEQ);}

      
[a-zA-Z_][a-zA-Z0-9_]*	{if (mydebug) fprintf(stderr,"ID found\n");
                       yylval.string=strdup(yytext); return(T_ID);}
[0-9][0-9]*	            {if (mydebug) fprintf(stderr,"Digit found\n");
                       yylval.value=atoi((const char *)yytext); return(T_NUM);}
[ \t]		            {}
[,;[\]()+\-*/=<>!] 	    {return (*yytext);}
\".*\"                  {if(mydebug) fprintf(stderr,"String\n");
                        yylval.string=strdup(yytext); return (T_QUOTED_STRING);}
\n 		                { lineno++;}
\/\/.*                  {/*do nothing*/}

%%

int yywrap(void)
{ return 1;}


