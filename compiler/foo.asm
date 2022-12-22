.data start of DATA
_L0:	.asciiz "enter x"	#global string
_L1:	.asciiz "you entered "	#global string
_NL:	.asciiz "\n"	#New line 
.align 2 #start all of global variable aligned 

x:	.space	4	# define a global variable

 .text 
 
 .globl main 
 
main:			#Start of Function
		subu $t0 $sp 8	#set up $t0 to be new SP
		sw $ra  ($t0)	#Store return address
		sw $sp 4($t0)	#Store old stack pointer
		move $sp $t0	#set the stack pointer to the new value 

		li $v0 4	#set up 4 for print strings
		la $a0 _L0
	#print fetch string location. 
		syscall	#print 

		la $a0 x	#load global var to a0
		li $v0 5	#read an int
		syscall	#syscall for read
		sw $v0 ($a0)	#store for emit read

		li $v0 4	#set up 4 for print strings
		la $a0 _L1
	#print fetch string location. 
		syscall	#print 

		la $a0 x	#load global var to a0
		lw $a0 ($a0)	#load a0 with whats inside a0
		li $v0 1	#load immed with 1 to print num
		syscall	#print num 


		lw $ra ($sp)	# reset RA
		lw $sp 4($sp)	#reset the SP to old SP

		li $v0 10
		syscall	#Return out of mars
