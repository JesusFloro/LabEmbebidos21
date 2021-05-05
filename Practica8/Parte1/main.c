/*
 * GccApplication1.c
 *
 * Created: 4/28/2021 11:09:16 AM
 * Author : A00822256
 */ 
/* Kernel includes. */
#include "sam.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "uart.h"
#include "myprintf.h"

/* Priorities at which the tasks are created. */
#define myTASK_TASK_PRIORITY         	( tskIDLE_PRIORITY + 1 )

void my_task(void *p) {
	while(1) {
		static uint32_t i = 0;
		myprintf("\nHello World task 1 %d", i++);
		vTaskDelay(1000);
	}
}

int main()
{
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	initUART();

	
	xTaskCreate( my_task,        	    /* The function that implements the task. */
	"my_task",              /* The text name assigned to the task. */
	512,            	    /* The size of the stack to allocate to the task. */
	NULL,           	    /* The parameter passed to the task  */
	myTASK_TASK_PRIORITY,   /* The priority assigned to the task. */
	NULL );         	    /* The task handle is not required, so NULL is passed.                  */

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
	return(0);
}