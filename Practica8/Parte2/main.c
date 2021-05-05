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
xQueueHandle Global_Queue_Handle = 0;

void sender_task(void *p) {
	while(1) {
		static uint32_t i = 0;
		myprintf("\n send %d to receiver task \n", i++);
		if (!xQueueSend(Global_Queue_Handle, &i, 1000)){
			myprintf("Failed to send to queue\n");
		}
		vTaskDelay(2000);
	}
}

void receiver_task(void *p) {
		static uint32_t rx_int = 0;
		while (1){
			if (xQueueReceive(Global_Queue_Handle, &rx_int, 1000)){
				myprintf("Received %d\n", rx_int);
			}else{
				myprintf("\nFailed to receive data from queue\n");
			}
			
		}
			

}

int main()
{
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	initUART();

	Global_Queue_Handle = xQueueCreate(3, sizeof(int));
	
	xTaskCreate( sender_task,   /* The function that implements the task. */
	"tx",              /* The text name assigned to the task. */
	512,            	    /* The size of the stack to allocate to the task. */
	NULL,           	    /* The parameter passed to the task  */
	myTASK_TASK_PRIORITY,   /* The priority assigned to the task. */
	NULL );         	    /* The task handle is not required, so NULL is passed.                  */


	xTaskCreate( receiver_task,   /* The function that implements the task. */
	"rx",              /* The text name assigned to the task. */
	512,            	    /* The size of the stack to allocate to the task. */
	NULL,           	    /* The parameter passed to the task  */
	myTASK_TASK_PRIORITY,   /* The priority assigned to the task. */
	NULL );         	    /* The task handle is not required, so NULL is passed.                  */


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for( ;; );
	return(0);
}