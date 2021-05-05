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


/*

PORT->Group[ 0 ].DIRSET.reg = PORT_PA27; //pin 27 TX LED declared as data output

while (1) {
	if( ( PORT->Group[ 0 ].IN.reg & PORT_IN_IN( PORT_PA16 ) ) == _U_( 0x00000000 ) )
	PORT->Group[ 0 ].OUTCLR.reg = PORT_PA27;
	else
	PORT->Group[ 0 ].OUTSET.reg = PORT_PA27;
}
//Al estar presionado se recibe un cero, por la resistencia pull up
*/

/* Priorities at which the tasks are created. */
#define myTASK_TASK_PRIORITY         	( tskIDLE_PRIORITY + 1 )
#define noOption 0x00
#define idUp 0x01
#define idDown 0x02
#define idLeft 0x03
#define idRight 0x04
#define delay 2000
#define idReturn 0x05



xQueueHandle Global_Queue_Handle = 0;

void sender_task(void *p) {
	while(1) {
		static uint32_t i = noOption;
		if((PORT->Group[0].IN.reg & PORT_IN_IN(PORT_PA14)) == _U_(0x00000000)){
			i = idUp;
		}else if((PORT->Group[0].IN.reg & PORT_IN_IN(PORT_PA09)) == _U_(0x00000000)){
			i = idDown;
		}else if((PORT->Group[0].IN.reg & PORT_IN_IN(PORT_PA08)) == _U_(0x00000000)){
			i = idLeft;
		}else if((PORT->Group[0].IN.reg & PORT_IN_IN(PORT_PA15)) == _U_(0x00000000)){
			i = idRight;
		}else{
			myprintf("No input detected \n");
			i = noOption;
		}
		//function to send data
		if(i != noOption){
			//send data
			if (!xQueueSend(Global_Queue_Handle, &i, delay)){
				myprintf("Failed to send to queue\n");
			}else{
				myprintf("Data Sent to the user\n");
				while(xQueueReceive(Global_Queue_Handle, &i, delay)){
					myprintf("Waiting for ACK\n");
				}
				if(i == idReturn){
					myprintf("Message received");
				}
				i = noOption;
			}
		}
		vTaskDelay(delay);
		/*
		static uint32_t i = 0;
		myprintf("\n send %d to receiver task \n", i++);
		if (!xQueueSend(Global_Queue_Handle, &i, 1000)){
			myprintf("Failed to send to queue\n");
		}
		vTaskDelay(2000);
		*/
		
	}
}

void receiver_task(void *p) {
		static uint32_t rx_int = 0;
		while (1){
			if (xQueueReceive(Global_Queue_Handle, &rx_int, delay)){
				if(rx_int == idUp){
					myprintf("UP \n");
				}else if(rx_int == idDown){
					myprintf("DOWN \n");
				}else if(rx_int == idLeft){
					myprintf("LEFT \n");
				}else if(rx_int == idRight){
					myprintf("RIGHT \n");
				}
				rx_int = idReturn;
				if (!xQueueReceive(Global_Queue_Handle, &rx_int, delay)){
					myprintf("Failed to send data \n");
				}
			}
			vTaskDelay(delay);
		}
}

int main(){
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	initUART();

	Global_Queue_Handle = xQueueCreate(3, sizeof(int));
	
	//setting the ports as input ports
	PORT->Group[ 0 ].PINCFG[ PIN_PA14 ].reg = 0x2; //bit INEN must be set for input pins
	PORT->Group[ 0 ].PINCFG[ PIN_PA09 ].reg = 0x2; //bit INEN must be set for input pins
	PORT->Group[ 0 ].PINCFG[ PIN_PA08 ].reg = 0x2; //bit INEN must be set for input pins
	PORT->Group[ 0 ].PINCFG[ PIN_PA15 ].reg = 0x2; //bit INEN must be set for input pins

	PORT->Group[ 0 ].PINCFG[ PIN_PA27 ].reg = 0x0; //bit PMUXEN must be clear for GPIOs

	//linking the ports as input ports
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA14; //pin D2 declared as data input
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA09; //pin D3 declared as data input
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA08; //pin D4 declared as data input
	PORT->Group[ 0 ].DIRCLR.reg = PORT_PA15; //pin D5 declared as data input
	
	
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