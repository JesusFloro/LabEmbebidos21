@Program to ask the user for two numbers, 
@add the numbers and display the sum result at the end. 
@The program should prompt for:
@a.      Give me the first operand:
@b.     Give me the second operand:
@c.      The result of # + # is: #


@ ---------------------------------------
@     Data Section
@ ---------------------------------------
	
        .data
        .balign 4	
prompt1: .asciz  "Give me the first Operand: "
prompt2: .asciz  "Give me the second Operand: "
format: .asciz  "%d"
num:    .int    0
num2:    .int    0
output: .asciz  "your input: %d\n"
string: .asciz "\na + b = %d\n"
b:      .word 44
c:      .word 0       @ will contain a+b 

	
@ ---------------------------------------
@     Code Section
@ ---------------------------------------
	
.text
.global main
.extern printf
.extern scanf

main:   
		@For Fisrt Operand
		push    {ip, lr}         @ push return address + dummy register for alignment
        ldr     r0, =prompt1      @ print the prompt
        bl      printf

        ldr     r0, =format      @ call scanf, and pass address of format
        ldr     r1, =num         @ string and address of num in r0, and r1,
        bl      scanf            @ respectively.

        Ldr     r1, =num         @ print num formatted by output string.
        Ldr     r1, [r1]
        ldr     r0, =output
        bl      printf
        @second Operand
        ldr     r0, =prompt2      @ print the prompt
        bl      printf

        ldr     r0, =format      @ call scanf, and pass address of format
        ldr     r1, =num2         @ string and address of num in r0, and r1,
        bl      scanf            @ respectively.

        Ldr     r1, =num2         @ print num formatted by output string.
        Ldr     r1, [r1]
        ldr     r0, =output
        bl      printf
        
        ldr    r1, =num		@ get address of a into r1
        ldr    r1, [r1]          @ get a into r1
        ldr    r2, =num2            @ get address of b into r2
        ldr    r2, [r2]          @ get b into r2
        add    r1, r1, r2        @ add r1 to r2 and store into r1
        ldr    r2, =c            @ get address of c into r2
        str    r1, [r2]          @ store r1 into c

        ldr    r0, =string       @ get address of string into r0
        ldr    r1, [r2]          @ pass c=a+b into r1
        bl     printf		@ print string and r1 as param
        
        
        pop     {ip, pc}         @ pop return address into pc
