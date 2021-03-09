@Jesus Flores A00822256
@Jorge Besnier A01039882


@Variables declaration
	.data
	.balign 4
prompt:	.asciz	"Give me x > "
format: .asciz 	"%d"
num:	.int	0
num2:	.int	0
output: .asciz 	"your answer is: %d\n"
@Main code
	.text
	.global main
	.extern printf
	.extern scanf

main:   push 	{ip, lr}	@ push return address + dummy register
				@ for alignment

	ldr	r0, =prompt	@ print the prompt
	bl	printf

	ldr     r0, =format	@ call scanf, and pass address of format
	ldr	r1, =num	@ string and address of num in r0, and r1,
	bl	scanf		@ respectively.

@Move constant to additional registers
	mov	r6,#0x6
	mov	r7,#0x9
	mov	r8,#0x2
@Move obtained value to r2 and multiply it. Store in r0
	ldr	r2, =num
	ldr	r2,[r2]
	ldr	r1, =num
	ldr	r1,[r1]
	mul	r0,r1,r2
@Move value into r1 and use it to continue the operation
	mov	r1,r0
	mul	r0,r1,r6
	mov 	r3,r2
	mul	r2,r3,r7
	add	r1,r0,r2
	add	r0,r1,r8
@Store the result into num2
	ldr	r1, =num2
	str	r0, [r1]
@Print the result with printf
	ldr	r0, =output
	ldr	r1, =num2	@ print num formatted by output string.
	ldr	r1, [r1]
	bl	printf
    pop 	{ip, pc}	@ pop return address into pc
