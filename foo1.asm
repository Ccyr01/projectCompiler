.data # start of the DATA section, strings first

_L0:	.asciiz "enter x"		#global string
_L1:	.asciiz "you entered "		#global string
_NL: .asciiz "\n "# NEw line
.align 2 # start all of global variable aligned

x:	.space 4		# define a global variable

.text
 
.globl main

 			
	.text		
			
main:			#Start of Function 
	subu $t0  $sp 8		#set up $t0 to be the new spot for SP
	sw $ra ($t0)		 #Store the return address 
	sw $sp 4($t0)		 #Store the old stack pointer 
	move $sp $t0 		# set the stack pointer to the new value
			
	li $v0, 4		#print a string
	la $a0, _L0		#print fetch string location
	syscall		
			
	la $a0, x		#load gobal variable from data segment
	li $v0 5		#read in a number 
	syscall		#now read  
	sw $v0 ($a0)		# end of read statement 
			
	li $v0, 4		#print a string
	la $a0, _L1		#print fetch string location
	syscall		
			
	la $a0, x		#load gobal variable from data segment
	lw $a0 ($a0)		#expression is identifier
			
	li $v0 1		#set up write number call
	syscall		
			
	li $v0 0    		#  return  NULL zero (0) 
	lw $ra ($sp)		# reset return address 
	lw $sp 4($sp)		# reset stack pointer 
			
	li $v0, 10		#Main function ends 
	syscall		#MAIN FUNCTION EXITS
			
