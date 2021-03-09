@Jesus Flores A00822256
@Jorge Besnier A01039882


@ ---------------------------------------
@	Data Section
@ ---------------------------------------
	.data
	.balign	4
format: .asciz	"%d"
char: .asciz	"%s"
num1:	.int	0
num2:	.int	0
num3:	.int	0
num4:	.int	0
oper:	.int	0
req1:	.asciz	"Give me the first operand:"
req2:	.asciz	"Give me the second operand:"
req4:	.asciz	"Give me the third operand:"
req3:	.asciz	"Give me the operation(+,-,*,/):"
string: .asciz	"\n%d %c %d %c %d = %d\n"
@ ---------------------------------------
@	Code Section
@ ---------------------------------------
	.text
	.global main
	.extern printf
	.extern scanf
main:	push 	{ip, lr}	@ push return address + dummy register
				@ for alignment

	ldr	r0, =req1	@ print the prompt
	bl	printf

	ldr     r0, =format	@ call scanf, and pass address of format
	ldr	r1, =num1	@ string and address of num in r0, and r1,
	bl	scanf		@ respectively.

	ldr	r0, =req2	@ print the prompt
	bl	printf

	ldr     r0, =format	@ call scanf, and pass address of format
	ldr	r1, =num2	@ string and address of num in r0, and r1,
	bl	scanf		@

	ldr	r0, =req4	@ print the prompt
	bl	printf

	ldr  r0, =format	@ call scanf, and pass address of format
	ldr	r1, =num4	@ string and address of num in r0, and r1,
	bl	scanf		@

	ldr	r0, =req3	@ print the prompt
	bl	printf

	ldr     r0, =char	@ call scanf, and pass address of format
	ldr	r1, =oper	@ string and address of num in r0, and r1,
	bl	scanf		@

	ldr	r1, =num1		@ r1 <- a
	ldr	r1, [r1]
	ldr	r2, =num2		@ r2 <- b
	ldr	r2, [r2]
	ldr	r4, =oper       @ r3 <- oper
	ldr	r4, [r4]
	ldr	r6, =num4       @ r6 <- c
	ldr	r6, [r6]
    @ Branches for operation assignement
	cmp	r4,#'+'
	beq	sum
	cmp	r4,#'-'
	beq	res
	cmp	r4,#'*'
	beq	mult
	cmp	r4,#'/'
	beq	div
@ three operand addition
sum:	add	r0,r1,r2
	add	r0,r0,r6
	ldr	r3, =num3
	str	r0, [r3]
	b	print
@ three operand substraction
res:
	sub	r0,r1,r2
	sub	r0,r0,r6
	ldr	r3, =num3
	str	r0, [r3]
	b 	print
@ three operand multiplication
mult:
	mul	r0,r1,r2
	mov	r5,r0
	mul	r0,r5,r6
	ldr	r3, =num3
	str	r0, [r3]
	b 	print
@ three operand division
div:
	mov 	r0,#0x00
loop:	add	r0,r0,r2
	add	r5,r5,#1
	cmp	r0,r1
	blt	loop
	mov 	r0,#0x00
	mov	r1,r5
	mov 	r5,#0x00
loop2:	add	r0,r0,r6
	add	r5,r5,#1
	cmp	r0,r1
	blt	loop2
	ldr	r3, =num3
	str	r5, [r3]
	b 	print

@ Operation in order to print the string
print:
    @adresses of each variable to each register
	ldr 	r0, =string 	@ get address of string into r0
	ldr	r1, =num1		@
	ldr	r2, =oper		@
	ldr	r3, =num2		@
	ldr	r4, =oper
	ldr	r5, =num4
	ldr	r6, =num3
    @data load from the variables
	ldr	r1, [r1]
	ldr	r2, [r2]
	ldr	r3, [r3]
	ldr	r4, [r4]
	ldr	r5, [r5]
	ldr	r6, [r6]
    @Action to access the stack
    push    {r4-r6}
    @Call function printf
    bl 	printf		@ print string and pass params
				@ into r1, r2, and r3
    pop     {r4-r6}
    pop 	{ip, pc}	@ pop return address into pc
