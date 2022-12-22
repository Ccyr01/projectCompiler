.data start of DATA
_NL:	.asciiz "\n"	#New line 
.align 2 #start all of global variable aligned 

Y:	.space	4	# define a global variable

 .text 
 
 .globl main 
 
main:			#Start of Function
		subu $t0 $sp 24	#set up $t0 to be new SP
		sw $ra  ($t0)	#Store return address
		sw $sp 4($t0)	#Store old stack pointer
		move $sp $t0	#set the stack pointer to the new value 

		li $a0 0	#load a num expr
		sw $a0 12($sp)	#getting stored val from temp---in em_assign
		move $a0 $sp	#stack ptr to a0
		add $a0 $a0 8	#store in a0 new value sp + offset
		lw $a1 12($sp)	#getting the value from the temporary spot that I stored
		sw $a1, ($a0)	#stored val
		move $a0 $sp	#stack ptr to a0
		add $a0 $a0 8	#store in a0 new value sp + offset
		lw $a0 ($a0)	#load a0 with whats inside a0
		sw $a0, 16($sp)	#store rhs val in memory
		li $a0 5	#load a num expr
		move $a1, $a0	#a0 sotred in a1
		lw $a0 16($sp)	#load word from sp
		slt $a0, $a0, $a1	#<
		beq $a0, 0 _L0	#branch if false which is 0 go to _L0

		move $a0 $sp	#stack ptr to a0
		add $a0 $a0 8	#store in a0 new value sp + offset
		lw $a0 ($a0)	#load a0 with whats inside a0
		sw $a0, 20($sp)	#store rhs val in memory
		li $a0 1	#load a num expr
		move $a1, $a0	#a0 sotred in a1
		lw $a0 20($sp)	#load word from sp
		add $a0, $a0, $a1	#addition
		li $v0 1	#load immed with 1 to print num
		syscall	#print num 


		j _L1	#j command to L1

_L0:			#L0 

_L1:			#END Of IF

		lw $ra ($sp)	# reset RA
		lw $sp 4($sp)	#reset the SP to old SP

		li $v0 10
		syscall	#Return out of mars
