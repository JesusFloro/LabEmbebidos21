@Jesus Flores A00822256
@Jorge Besnier A01039882


	.data
y:	.word	10

.text
.global main

main:
	push	{ip,lr} @ push return address + dummy register

	ldr	r1, =y 	@meter la direccion de y a r1
	movw	r1, #0xAAAA @escribir 0xAAAAAAAA a r1
	movt	r1, #0xAAAA
	ldr	r1,=0xBBBBBBBB @escribir 0xBBBBBBBB a r1
	ldr	r2,=0xCCCCCCCC @escribir 0xCCCCCCCC a r2
	mov	r1,r2  @Copiar el valor de  r2 hacia r1

	pop	{ip, pc} @ pop return address into pc
