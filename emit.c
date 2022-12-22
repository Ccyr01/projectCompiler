/*
Edited by Christian Cyr
4/29/2022
**
The purpose of this program is to take syntax written in Algol-C and make it print out in an assembly code called mips.
There is different factors that are taken into consideration when we are making the transformations.
The appropiate steps include writing out the header with appropiate labels and globals.
Depending on the syntax from our algol we make funtions, loops, access of variables in memory locations,
and simple math expressions possible.


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "AST.h"
#include "emit.h"


//CreateLabel to keep track of global strings
int GTEMP2=0;
char * CreateLabel()
{    char hold[100];
     char *s;
     sprintf(hold,"_L%d",GTEMP2++);
     s=strdup(hold);
     return (s);

}




//Pre: ptr for label,command,comment and File...fprintf(fp,)is used to print to file
//Post: print string with appropiate content from pointers
// The point of this method is to structure the mips code in appropiate places
//Labels are far left of screen, commands are next to comments and fp is to get it to the file.
void emit(char *label, char *command, char * comment, FILE *fp){
    //comparing if label is empty
    if(strcmp(label,"")==0){
        //str compare comment to empty
        if(strcmp(comment,"")==0){
            //if both label and comment are empty print ONLY the command
            fprintf(fp,"\t\t%s\n",command);
        }
        //We print both command and comment to the file
        else{
            fprintf(fp,"\t\t%s\t#%s\n",command,comment);
        }
    }
       
    else{
        //label has content
        if(strcmp(comment,"")==0){
            fprintf(fp,"\t\t%s\n",command);
        }
        else{
            //label and comment has content
            fprintf(fp,"%s:\t\t%s\t#%s\n",label,command, comment);
        }
    }
}






//pre:ptr p and fp
//post: print top strings for mips code in file
//This method's objective is to get the top few lines of code that 
//get the program started. The global variables as well as strings are here.
// *This function is called in YACC "EMIT_header(Program, fp)" the param Program is a struct ASTnode
void EMIT_header(struct ASTnode *p, FILE *fp){
    fprintf(fp,".data start of DATA\n");
    //call method to get global strings declared in mips
    Emit_Global_Strings(p,fp);
    fprintf(fp,"_NL:\t.asciiz \"\\n\"\t#New line \n");
    //start global vars aligned
    fprintf(fp,".align 2 #start all of global variable aligned \n\n");
    //call method to get global variables declared
    Emit_Global_Variables(p, fp);
    fprintf(fp,"\n .text \n ");
    fprintf(fp, "\n .globl main \n \n");
    //start main part of program
    EMITAST(p,fp);
} 





//pre: ptr to AST file
//post: print variables
//global variables are at level 0 & sent to top of mips page in header
void Emit_Global_Variables(struct ASTnode *p, FILE *fp){
    //check if p == NULL else search for the vardecs
    if(p==NULL)return;
    //looking for variable declarations at level 0
    if(p->MyType==(A_VARDEC)&&p->symbol->level==0){
        fprintf(fp,"%s:\t.space\t%d\t# define a global variable\n",p->name,p->symbol->mysize*WSIZE);
        
    }
    //recursive calls to  check s1 and next for more global variables
    Emit_Global_Variables(p->s1,fp);
    Emit_Global_Variables(p->next,fp);
}



//pre:two ptrs one for ASTnode and one for file to print to
//post:print global strings in file with mips code
//global strings are sent to the top with a label 
//They are declared inside compound stmnts
void Emit_Global_Strings(struct ASTnode *p, FILE *fp){
    if(p==NULL)return ;
   //looking for A_WRITE node containing a name which is the string
    if(p->MyType==(A_WRITE)&&(p->name != NULL)){
        fprintf(fp,"%s:\t.asciiz %s\t#global string\n",p->label, p->name);
        
    }
    //recursive calls looking for more global strings
    Emit_Global_Strings(p->s1,fp);
    Emit_Global_Strings(p->s2,fp);
    Emit_Global_Strings(p->next,fp);
   
}


//pre: ptr to fundec and FILE
//post: mips code in fp to handle activation record, explores block and inside of function
//Method is used for FUN_DEC 
//sets up the stack for activation record
//does statements inside block and closes or returns properly
void emit_function_head(struct ASTnode *p, FILE *fp){
    char s[100];
    //function name plus four lines of code for setting 
    //sp,ra,old sp, and set sp to new value
    emit(p->name,"","Start of Function",fp);//p-name is T_ID in fun_dec

    sprintf(s,"subu $t0 $sp %d",p->symbol->mysize *WSIZE);//mysize is maxoffset
    emit("",s,"set up $t0 to be new SP",fp);
    emit("","sw $ra  ($t0)","Store return address", fp);
    emit("","sw $sp 4($t0)","Store old stack pointer",fp);
    emit("","move $sp $t0","set the stack pointer to the new value ",fp);
    fprintf(fp,"\n");
    //checking block
    EMITAST(p->s2, fp);
    fprintf(fp, "\n");
    //end of Function
    //select return
    emit("","lw $ra ($sp)"," reset RA",fp);
    emit("","lw $sp 4($sp)","reset the SP to old SP",fp);
    fprintf(fp, "\n");
    //if its the main method it will end it after or else go back to where you were called
    if(strcmp(p->name,"main")==0){
        //need to syscall 10 to quit
        emit("","li $v0 10","",fp);
        emit("","syscall","Return out of mars",fp);
        
    }
    else{//not main
        emit("","jr $ra","Not main jump back to call function",fp);
        
    }
}


//pre:two pointers one struct one FILE
//post: print statements for the specific node
//Nodes get sent here to match MyType
//Once the right type is found send it to the correct function
//or recursively call with right parameters to keep the program moving along
void EMITAST(struct ASTnode *p,FILE *fp){
    if(p==NULL)return;
    switch(p->MyType){
        case A_VARDEC:
            EMITAST(p->s1,fp);
            EMITAST(p->next,fp);
            break;
        case A_FUNDEC:
            emit_function_head(p,fp);
            break;
        case A_BLOCK:
            //local decs are taken care of so no s1
            
            EMITAST(p->s2,fp);
            EMITAST(p->next,fp);

            break;
        case A_WRITE:
            emit_write(p,fp);
            EMITAST(p->next,fp);

            break;
        case A_READ:
            emit_read(p,fp);
            EMITAST(p->next,fp);
            EMITAST(p->s1,fp);
            
            break;
        case A_ASSIGN:
            //printf("HERE\n");
            emit_assign(p,fp);
            EMITAST(p->next,fp);
            break;
        case A_IF:
            emit_if(p,fp);
            EMITAST(p->next,fp);
            break;
        case A_WHILE:
            emit_while(p,fp);
            EMITAST(p->next,fp);
            break;
        //default:printf("EMITAST default no bueno..\n");
            exit(1);

    }
}
    
//emit_write
//pre:two pointers one A_WRITE one to FILE
//post:print out mips commands for write statement
//this fuction is either handling strings if it has a name
//or it is handling an expression
//The goal is to print the string or the number
void emit_write(struct ASTnode *p, FILE *fp){
    
    char s[100];
    if(p->name!=NULL){
       //the if is used to print strings
       //p->label comes from yacc file where we store string 
        sprintf(s,"la $a0 %s\n",p->label);
        //print strings load 4
        emit("","li $v0 4", "set up 4 for print strings",fp);
        //print sprintf contents 
        emit("",s, "print fetch string location. ",fp);
        //syscall to actually print
        emit("","syscall","print ",fp);  
        fprintf(fp,"\n");
     	
    }else
    {   
        //since this is not a string
        //it's an expression and we are sending it to emit_expr
        emit_expr(p->s1,fp);
        //you end up with a number from emit expr so load 1 to print
        emit("","li $v0 1","load immed with 1 to print num",fp);
        //syscall prints the number stored above
        emit("","syscall","print num ",fp);  
        fprintf(fp,"\n");


    }
    
}



//emit_read
//pre two pointers one A_READ one to file
//post print mips commands to read
//reads num associated with var
void emit_read(struct ASTnode *p, FILE *fp){
        //s1 of my A_READ is a var which is a T_ID 
        //so send it to Emit_Ident
        Emit_Ident(p->s1,fp);
        //read int load 5 
        //$v0 used to store operation number
        emit("","li $v0 5","read an int",fp);
        emit("","syscall","syscall for read",fp);
        //store results in $v0
        emit("","sw $v0 ($a0)","store for emit read",fp);
        fprintf(fp,"\n");
}

    
//All EXPRESSIONS including operators
//pre:two pointers one expression one to file
//post:find right case and store value in a0
//The cases functioning are getting the value out of the expression
//operators handled in seperate switch solve simple math equations
//We end up with a value
void emit_expr(struct ASTnode *p, FILE *fp){
    char s[100];
        switch(p->MyType){
            case A_NUM: sprintf(s,"li $a0 %d",p->size);
                            emit("",s,"load a num expr",fp);
                            return;
                break;
            case A_IDEN:
                 Emit_Ident(p,fp);
                 emit("","lw $a0 ($a0)","load a0 with whats inside a0",fp);
                 return;
                break;
           case A_CALL:
                break;
      
                

        }// switch
        //calling emit_expr again to check s1
        emit_expr(p->s1,fp);
        sprintf(s,"sw $a0, %d($sp)",p->symbol->offset*WSIZE);
        emit("",s,"store  in memory sp + offset", fp);
        //calling emit_expr again to check s2
        emit_expr(p->s2,fp);
        sprintf(s,"move $a1, $a0");
        emit("",s,"a0 sotred in a1",fp);
        sprintf(s,"lw $a0 %d($sp)",p->symbol->offset*WSIZE);
        emit("",s,"load word from sp",fp);
        
        //new switch for operators
        //$a0 stores operation parameters
        switch(p->operator){
            case A_PLUS:
                sprintf(s,"add $a0, $a0, $a1");
                emit("",s,"addition",fp);
                break;
            case A_MINUS:
                sprintf(s,"sub $a0, $a0, $a1");
                emit("",s,"subtraction",fp);
                break;
            case A_TIMES:
                sprintf(s,"mul $a0, $a0, $a1");
                emit("",s,"multiplication",fp);
                break;
            case A_DIVIDEDBY://hi gets answer we're looking for lo gets remainder
                sprintf(s,"div $a0, $a0, $a1");
                emit("",s,"division",fp);
                break;
            case A_AND:
                sprintf(s,"and $a0, $a0, $a1");
                emit("",s,"AND",fp);
                break;
            case A_OR:
                sprintf(s,"or $a0, $a0, $a1");
                emit("",s,"or",fp);
                break;
            case A_LEQ:
                sprintf(s,"sle $a0, $a0, $a1");
                emit("",s,"<=",fp);
                break;
            case A_LESS:
                sprintf(s,"slt $a0, $a0, $a1");
                emit("",s,"<",fp);
                break;
            case A_GREATER:
                sprintf(s,"sgt $a0, $a0, $a1");
                emit("",s,">",fp);
                break;
            case A_GEQ:
                sprintf(s,"sge $a0, $a0, $a1");
                emit("",s,">=",fp);
                break;
            case A_EQ:
                sprintf(s,"seq $a0, $a0, $a1");
                emit("",s,"==",fp);
                break;
            case A_NEQ:
                sprintf(s,"sne $a0, $a0, $a1");
                emit("",s,"!=",fp);
                break;
        }
}


//pre:two pointers one for A_ASSIGN other for file
//post: print proper MIPS assembly code into file for assignments
//The purpose for this function is to do the expression on the right
// and store it into the var on the left 
void emit_assign(struct ASTnode *p, FILE *fp){
    char s[100];
        emit_expr(p->s2,fp);//handle expr & get value
        //save value in the p->sym->offset
        sprintf(s,"sw $a0 %d($sp)",p->symbol->offset*WSIZE);
        emit("",s,"getting stored val from temp---in em_assign",fp);
        //emit identifier to handle s1
        //get address to use correct var
        Emit_Ident(p->s1,fp);
        sprintf(s,"lw $a1 %d($sp)",p->symbol->offset*WSIZE);
        emit("",s,"getting the value from the temporary spot that I stored",fp);
        //store extracted value into location from identifier
        emit("","sw $a1, ($a0)","stored val",fp);
        
}

//pre two pointers one A_IDENT one for file
//post: mips code to store globals and locals
//   $a0 will be the address of A_IDENT
//Goal of this method is to have address of var in a0
//for later access in program
void Emit_Ident(struct ASTnode *p, FILE *fp){
    char s[100];    
    if(p->symbol->level==0){
                //global var because level==0
                sprintf(s,"la $a0 %s",p->name);
                emit("",s,"load global var to a0",fp);
                return;
        }
        else{
            //dealing with local level != 0
            //store what is in the stack ptr to a0
            emit("","move $a0 $sp","stack ptr to a0",fp);
            //add offset with what's in a0 which is sp 
            //store in a0 new value sp + offset
            sprintf(s,"add $a0 $a0 %d",p->symbol->offset*WSIZE);
            emit("",s,"store in a0 new value sp + offset",fp);

            
        }
    
    
}

//pre: ptr for A_if 
//post: get reult of expression
//The if statement is to see if the expression is true
// if it is false we jump out of the loop
// if true we do the statements in the loop then go back to check expr until false
void emit_if(struct ASTnode *p,FILE *fp){
        char s[100];    
        char *_L0=CreateLabel();
        char *_L1=CreateLabel();
        //sending the expression from the s1 of if to the emit_expr method
        //leave value in a0
        emit_expr(p->s1,fp);
        //branch if equal then go to else
        //if not just fall through and do then statement underneath
        sprintf(s,"beq $a0, 0 %s",_L0);
        emit("",s,"branch if false which is 0 go to _L0",fp);
        fprintf(fp,"\n");

        //then STATEMENT  since beq is false needs EMITAST for THEN statement
        EMITAST(p->s2->s1,fp);
        fprintf(fp,"\n");
         //jump to skip _L0 steps
        emit("","j _L1","j command to L1",fp);
        fprintf(fp,"\n");
        //first label
        emit(_L0,"","L0 ",fp);
        //if equal jump to else
        EMITAST(p->s2->s2,fp);
        fprintf(fp,"\n");


       
        emit(_L1,"","END Of IF",fp);

}


//pre:ptr from A_WHile
//post:print result of expression
//The emit_while loop will check the expression if it's true 
//then we do the statements in loop if it is true.
// If it is not true we jump out of the loop
void emit_while(struct ASTnode *p, FILE *fp){
    char s[100];
    char *L1=CreateLabel();
    char *L2=CreateLabel();
    //checking 
    emit(L1,"","start of while",fp);
    emit_expr(p->s1,fp);
    //0 is false 1 is true
    //branch if false to get out of loop
    sprintf(s,"beq $a0, 0 %s",L2);
    emit("",s,"branch if expression is not true. Go to end, or else fall through",fp);
    fprintf(fp,"\n");
    //do the statement because expression is true
    EMITAST(p->s2,fp);
    //jumping back to expression to see if its still true
    emit("","j _L0","jump to check expr",fp);
    fprintf(fp,"\n");
    
    emit(L2,"","End of While",fp);

}
