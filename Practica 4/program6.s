@ ---------------------------------------
@	Variables
@ ---------------------------------------
	
	 .data
	 .balign 4	
Intro: 	 .asciz  "Blink test\n"
ErrMsg:	 .asciz	"Error\n"
pin:	 .int	7
i:	 .int	0
delayMs: .int	250
OUTPUT	 =	1
	
@ ---------------------------------------
@	Global setup
@ ---------------------------------------	
	.text
	.global main
	.extern printf
	.extern wiringPiSetup
	.extern delay
	.extern digitalWrite
	.extern pinMode
	
main:   push 	{ip, lr}	@ push return address + dummy register
				@ for alignment
@Initialization
	ldr	r0, =Intro	
        	bl 	printf		
	bl	wiringPiSetup
	mov	r1,#-1
	cmp	r0, r1
	bne	init
	ldr	r0, =ErrMsg
	bl	printf
	b	done
@pinMode declaration or setup
init:
	ldr	r0, =pin
	ldr	r0, [r0]
	mov	r1, #OUTPUT
	bl	pinMode
	ldr	r4, =i
	ldr	r4, [r4]
	mov	r5, #10
@Loop that repeats the blinking a previously set number of times.
forLoop:
	cmp	r4, r5
	bgt	done
	ldr	r0, =pin
	ldr	r0, [r0]
	mov	r1, #1
	bl 	digitalWrite
	ldr	r0, =delayMs
	ldr	r0, [r0]
	bl	delay
	ldr	r0, =pin
	ldr	r0, [r0]
	mov	r1, #0
	bl 	digitalWrite
	ldr	r0, =delayMs
	ldr	r0, [r0]
	bl	delay
	add	r4, #1
	b	forLoop
done:	
        pop 	{ip, pc}	@ pop return address into pc
