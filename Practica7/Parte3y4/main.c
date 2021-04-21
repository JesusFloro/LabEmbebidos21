

#include "sam.h"
#include "myprintf.h"
#include "spi.h"
#include "stdint.h"
#define RXBUFSIZE 0x400
#define LENGTH_R1 0x03
#define LENGTH_R7 0x07
//#define  SIZE_BLOCK 512

#define SIZE_SD_CMD 0x06
#define kCMD00 0x40
#define kCMD08 0x48
#define kCMD55 0x77
#define kCMD41 0x69
#define kCMD17 0X51
#define ONE 1

const uint8_t CMD00[SIZE_SD_CMD]  ={0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
const uint8_t CMD08[SIZE_SD_CMD]  ={0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
uint8_t CMD17[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t CMD172[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x08, 0x00, 0x01};
const uint8_t CMD18[SIZE_SD_CMD]  ={0x52, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t CMD55[SIZE_SD_CMD]  ={0x77, 0x00, 0x00, 0x00, 0x00, 0x65};
const uint8_t CMD41[SIZE_SD_CMD] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77};

uint8_t RxBuffer[RXBUFSIZE];

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


void initCycles(void);

/*
int main(void)
{
	uint32_t temp = 0xFF;
	// Initialize the SAM system 
	SystemInit();

	UARTInit();
	spiInit();

	initCycles();
	//myprintf("\n");
	
	spiXchg( CMD00, SIZE_SD_CMD, RxBuffer );  // reset instruction 
	myprintf("\n");

	
	myprintf("\nDone");
}
*/
int main(void){
	uint32_t temp = 0xFF;
	/* Initialize the SAM system */
	SystemInit();
	/* Switch to 8MHz clock (disable prescaler) */
	SYSCTRL->OSC8M.bit.PRESC = 0;
	/* Initialize the UART and SPI system */
	UARTInit();
	spiInit();
	/* Initialize the SD card */
	initCycles();
	/* Initialization complete */
	/* Start and send CMD 00 */
	myprintf("\n START\n");
	myprintf("CMD00\n");
	spiXchg(CMD00, SIZE_SD_CMD, RxBuffer);  // reset instruction
	myprintf("\n");
	/* Send CMD08 */
	myprintf("CMD08\n");
	spiXchg(CMD08, SIZE_SD_CMD, RxBuffer);
	myprintf("\n");
	/* Cycle until response of command CMD55 and CMD41 is 0 */
	uint32_t response= (uint32_t)ONE;
	while (response != 0x00){
		myprintf("CMD55\n");
		spiXchg(CMD55, SIZE_SD_CMD, RxBuffer);
		myprintf("\n");
		myprintf("CMD41\n");
		response = spiXchg(CMD41, SIZE_SD_CMD, RxBuffer);
		myprintf("\n");
	}
	/* Initialization of SD card complete, send command 17 to start the reading of the card, read the first 512 bytes and print them  */
	myprintf("COMANDO en Seccion  CMD17\n");
	rcvr_datablock(CMD17, SIZE_BLOCK, RxBuffer, SIZE_BLOCK );
	//rcvr_datablock(CMD17, (uint32_t)0, RxBuffer, SIZE_BLOCK );

	myprintf("\n");
	/* Read done, the program has finished */
	
	myprintf("Done\n");
	return temp;
}




