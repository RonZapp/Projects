# Ron Zappardino
# February 23rd, 2017
# Calculates nth fibonacci number with console input for n and output for solution
# Intended for use with SPIM

 	.text
	.globl	main

main:	addi	$sp, $sp, -4	# Allocate stack space.
	sw	$ra, 0($sp)	# Save the return address

	la	$a0, i_msg	# Load input message
	jal	rd_int		# Jump to input function

	addi	$a0, $v0, 0	# Moves input into argument 0 for fibonacci function
	li	$s7, 1		# Save 1 to s7 for comparisons
	jal	fib		# Jump to fibonacci function

	la	$a0, o_msg	# Load output message
	addi	$a1, $v0, 0	# Load solution from fibonacci function
	jal	pr_int		# Jump to output function

		# Restore the values from the stack, and release the stack space.
	lw	$ra, 0($sp)	# Retrieve return address
	addi	$sp, $sp, 0	# Free stack space.

		# Exit system call:  SPIM
	li	$v0, 10
	syscall



	# Recursive function to calc fib numbers
	# Pushes state onto stack, then pops state back off if n==0
	# If n==1 jumps to eq1 and has that function pop state off stack
	# If n>1 jumps to sm fucntion
fib:	addi 	$sp, $sp, -12 	# Allocate stack space
	sw	$ra, 8($sp)	# Save ra
	sw	$s0, 4($sp)	# Save fib
	sw	$a0, 0($sp)	# Save n

	# Jump to eq1 if n == 1, jump to sm if n > 1
	beq	$a0, $s7, eq1	# If n == 1 jump to eq1
	slti	$t0, $a0, 1	# Set to 1 if a0 < 1
	beq	$t0, $zero, sm	# If n != 0, go to sm

	addi	$v0, $zero, 0	# Return 0
	lw	$a0, 0($sp)	# Pop n
	lw	$s0, 4($sp)	# Pop fib
	lw 	$ra, 8($sp)	# Pop ra
	addi	$sp, $sp, 12	# Free stack space
	jr	$ra		# Return to function call


	# Returns 1, pops state from stack
eq1:	addi	$v0, $zero, 1	# Return 1
	lw	$a0, 0($sp)	# Pop n
	lw	$s0, 4($sp)	# Pop fib
	lw	$ra, 8($sp)	# Pop ra
	addi	$sp, $sp, 12	# Remove stack space
	jr	$ra		# Return to function call 


		# Calculates fib(n-1) and fib(n-2) and adds them together
sm:	addi	$a0, $a0, -1 	# Decrement n
	jal	fib 			# Call fib with (n-1)

	addi	$s0, $v0, 0		# Save result of fib(n-1)
	addi	$a0, $a0, -1	# Call fib with (n-2)
	jal	fib

	add	$v0, $v0, $s0	# Fib = fib(n-1) + fib(n-2)
	lw	$ra, 8($sp)	# Load pointer of function caller
	lw	$s0, 4($sp)	# Load s0 (previous fib calculatipns)
	lw	$a0, 0($sp)	# Load a0 (previous n)
	addi 	$sp, $sp, 12	# Clear stack
	jr	$ra		# Complete algorithm




		# Function to print a message and read an int
		# Message should be in $a0
rd_int:	addi	$sp, $sp, -4	# Allocate space on stack
	sw	$ra, 0($sp)	# Store return address

	li	$v0, 4		# Code to print string message
	syscall			# Prints message

	li	$v0, 5		# Code to read int from console
	syscall			# Reads int, stored in $v0

	lw	$ra, 0($sp)	# Retrieve return address
	addi	$sp, $sp, 4	# Free stack space
	jr	$ra		# Return. Int is in $v0



	# Function to print a message and an int
	# Message should be in $a0, int should be in $a1
pr_int: addi	$sp, $sp, -4	# Allocate space on stack
	sw	$ra, 0($sp)	# Store return address

	li	$v0, 4		# Code to print string message
	syscall			# Prints message

	li	$v0, 1		# Code to print int
	addi	$a0, $a1, 0	# Makes int available to syscall
	syscall			# Prints int 

	li	$v0, 4		# Code to print string
	la	$a0, newln	# Make newln avail to syscall
	syscall			# Prints newln

	# Restore the values from the stack, and release the stack space.
	lw	$ra, 0($sp)	# Retrieve return address
	addu	$sp, $sp, 4	# Free stack space.
	jr	$ra		# Return 



	.data
i_msg: .asciiz "Enter fibonacci number to be computed: "
o_msg: .asciiz "Fibonacci value is "         
newln: .asciiz "\n"
