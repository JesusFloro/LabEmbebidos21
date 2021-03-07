	.data
y:	.word	10

.text
.global main

main:
	push	{ip,lr} @ push return address + dummy register

	ldr	r1, =y 	@get address of y into r1
	movw	r1, #0xAAAA @Write the value of 0xAAAAAAAA to r1
	movt	r1, #0xAAAA
	ldr	r1,=0xBBBBBBBB @Write the value of 0xBBBBBBBB to r1
	ldr	r2,=0xCCCCCCCC @Write the value of 0xCCCCCCCC to r2
	mov	r1,r2  @Copies value from r2 to r1

	pop	{ip, pc} @ pop return address into pc
