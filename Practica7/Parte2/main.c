

#include "sam.h"
#include "myprintf.h"
#include "spi.h"
#include "stdint.h"

void UARTInit(void) {
	/* Initialize the SAM system */
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	
	/* port mux configuration*/
	PORT->Group[0].DIR.reg |= (1 << 10);              	/* Pin 10 configured as output */
	PORT->Group[0].PINCFG[PIN_PA11].bit.PMUXEN = 1;   	/* Enabling peripheral functions */
	PORT->Group[0].PINCFG[PIN_PA10].bit.PMUXEN = 1;   	/* Enabling peripheral functions */
	
	/*PMUX: even = n/2, odd: (n-1)/2 */
	PORT->Group[0].PMUX[5].reg |= 0x02;               	/* Selecting peripheral function C */
	PORT->Group[0].PMUX[5].reg |= 0x20;               	/* Selecting peripheral function C */
	
	/* APBCMASK */
	//PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;   		   /* SERCOM 0 enable*/
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;

	/*GCLK configuration for sercom0 module: using generic clock generator 0, ID for sercom0, enable GCLK*/

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |
	GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);

	
	/* configure SERCOM0 module for UART as Standard Frame, 8 Bit size, No parity, BAUDRATE:9600*/

	SERCOM0->USART.CTRLA.reg =
	SERCOM_USART_CTRLA_DORD | SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
	SERCOM_USART_CTRLA_RXPO(3/*PAD3*/) | SERCOM_USART_CTRLA_TXPO(1/*PAD2*/);
	
	uint64_t br = (uint64_t)65536 * (8000000 - 16 * 9600) / 8000000;
	
	SERCOM0->USART.CTRLB.reg = SERCOM_USART_CTRLB_RXEN | SERCOM_USART_CTRLB_TXEN | SERCOM_USART_CTRLB_CHSIZE(0/*8 bits*/);

	SERCOM0->USART.BAUD.reg = (uint16_t)br;

	SERCOM0->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}

/*


int main(void)
{
	uint32_t temp = 0xFF;
    SystemInit();
	UARTInit();
    spiInit();
    
	initCycles();
	  
	 // spiXchg( CMD00, SIZE_SD_CMD, RxBuffer );  // reset instruction 
	 // while(1){
		  	  temp = spiXchg( CMD00, SIZE_SD_CMD, RxBuffer );  // reset instruction 
			 // myprintf("\n R: %x", temp);
			  myprintf("\n");
			  temp = spiXchg( CMD08, SIZE_SD_CMD, RxBuffer );
			  //myprintf("\n R: %x",temp);
			  myprintf("\n");
			  do{	
				    temp = spiXchg( CMD55, SIZE_SD_CMD, RxBuffer );
				    myprintf("\n");
					temp = spiXchg( CMD41, SIZE_SD_CMD, RxBuffer );
					myprintf("\n");
			  }while (temp == 1);
			  
			   rcvr_datablock(CMD17, SIZE_BLOCK, RxBuffer, SIZE_BLOCK );
			  
	 // }
	  
}
*/

#define RXBUFSIZE 0x400
#define LENGTH_R1 0x03
#define LENGTH_R7 0x07

void initCycles(void);

#define SIZE_SD_CMD 0x06
#define kCMD00 0x40
#define kCMD08 0x48
#define kCMD55 0x77
#define kCMD41 0x69

const uint8_t CMD00[SIZE_SD_CMD]  ={0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
const uint8_t CMD08[SIZE_SD_CMD]  ={0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
uint8_t CMD17[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t CMD172[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x08, 0x00, 0x01};
const uint8_t CMD18[SIZE_SD_CMD]  ={0x52, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t CMD55[SIZE_SD_CMD]  ={0x77, 0x00, 0x00, 0x00, 0x00, 0x65};
const uint8_t CMD41[SIZE_SD_CMD] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77};

uint8_t RxBuffer[RXBUFSIZE];

int main(void)
{
	uint32_t temp = 0xFF;
	/* Initialize the SAM system */
	SystemInit();

	UARTInit();
	spiInit();

	initCycles();
	//myprintf("\n");
	
	spiXchg( CMD00, SIZE_SD_CMD, RxBuffer );  /* reset instruction */
	myprintf("\n");

	//Write your code here

	myprintf("\nDone");
}

void initCycles(void){
	uint32_t i;
	REG_PORT_OUTSET0 = PORT_PA18;
	for(i=0;i<76;i++)
	spiSend(0xFF);
}

//ADD spiXchg FUNCTION TO THE “spi.c” FILE

uint32_t spiXchg(const uint8_t * send_buff, uint32_t bc, uint8_t * receive_buff ) {
	uint8_t temp = 0xFF;
	uint32_t i;
	uint8_t temp_cmd = send_buff[0];
	
	REG_PORT_OUTCLR0 = PORT_PA18;
	for(i=0; i< bc; i++) {
		temp = spiSend(*(send_buff++));
		myprintf(" %x", temp);
	}
	switch(temp_cmd) {
		case kCMD00 :
		for(i=0; i<LENGTH_R1; i++) {
			temp = spiSend(0xFF);
			myprintf(" %x", temp);
		}
		break;
		case kCMD08 :
		for(i=0; i<LENGTH_R7; i++) {
			temp = spiSend(0xFF);
			myprintf(" %x", temp);
		}
		break;
		case kCMD41 :
		for(i=0; i<LENGTH_R1-1; i++) {
			temp = spiSend(0xFF);
			myprintf(" %x", temp);
		}
		spiSend(0xFF);
		break;
		case kCMD55 :
		for(i=0; i<LENGTH_R1; i++) {
			temp = spiSend(0xFF);
			//myprintf(" %x", temp);
		}
		break;
		default :
		myprintf("\n Error in CMD");
	}
	REG_PORT_OUTSET0 = PORT_PA18;
	return(temp);
}