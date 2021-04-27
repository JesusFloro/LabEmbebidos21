#include "sam.h"
#include "myprintf.h"
#include "stdio.h"
//#include "uart.h"

#define RXBUFSIZE 0x400
#define LENGTH_R1 0x03
#define LENGTH_R7 0x07

void initSPIsd(void);
void initUART(void);
void initCycles(void);

#define SIZE_SD_CMD 0x06
#define kCMD00 0x40
#define kCMD08 0x48
#define kCMD17 0X51
#define kCMD55 0x77
#define kCMD41 0x69

#define V 1
#define Zero 0
#define Block_byte_size 512

// Control variables for data transfer and reception

const uint8_t CMD00[SIZE_SD_CMD]  ={0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
const uint8_t CMD08[SIZE_SD_CMD]  ={0x48, 0x00, 0x00, 0x01, 0xAA, 0x87};
uint8_t CMD17[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t CMD172[SIZE_SD_CMD]  ={0x51, 0x00, 0x00, 0x08, 0x00, 0x01};
const uint8_t CMD18[SIZE_SD_CMD]  ={0x52, 0x00, 0x00, 0x00, 0x00, 0x01};
const uint8_t CMD55[SIZE_SD_CMD]  ={0x77, 0x00, 0x00, 0x00, 0x00, 0x65};
const uint8_t CMD41[SIZE_SD_CMD] = {0x69, 0x40, 0x00, 0x00, 0x00, 0x77};

uint8_t RxBuffer[RXBUFSIZE];

void spiInit(void) {
	// Switch to 8MHz clock (disable prescaler) 
	SYSCTRL->OSC8M.bit.PRESC = 0;
	PM->APBCMASK.bit.SERCOM1_ |= 1; //enable the clock for SERCOM1 peripheral
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID_SERCOM1_CORE;
	while(GCLK->STATUS.bit.SYNCBUSY);
	const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0, // Set as MSB first
		.bit.CPHA = 0, // Set as Mode 0
		.bit.CPOL = 0,  // Keep this as 0
		.bit.FORM = 0X0, // Set as SPI frame
		.bit.DIPO = 0X3, // Set as MISO on PAD[3]
		.bit.DOPO = 0x0, // Set as MOSI on PAD[0], SCK on PAD[1], SS_ on PAD[2]
		.bit.MODE = 0x3  // Set as Master
	};
	SERCOM1->SPI.CTRLA.reg = ctrla.reg;
	const SERCOM_SPI_CTRLB_Type ctrlb = {
		.bit.RXEN = 1,   // Set as RX enabled
		.bit.MSSEN = 0,  // Set as HW SS
		.bit.CHSIZE = 0x0  // Set as 8-bit
	};
	SERCOM1->SPI.CTRLB.reg = ctrlb.reg;
	
	
	SERCOM1->SPI.BAUD.reg = 0; // Rate is clock / 2
	// Mux for SERCOM1 PA16,PA17,PA18,PA19
	const PORT_WRCONFIG_Type wrconfig = {
		.bit.WRPINCFG = 1,
		.bit.WRPMUX = 1,
		.bit.PMUX = MUX_PA16C_SERCOM1_PAD0,
		.bit.PMUXEN = 1,
		.bit.HWSEL = 1,
		.bit.PINMASK =(uint16_t)((PORT_PA16|PORT_PA17|PORT_PA19) >> 16)
	};
	PORT->Group[0].WRCONFIG.reg = wrconfig.reg;
	REG_PORT_DIRSET0 = PORT_PA18;
	REG_PORT_OUTSET0 = PORT_PA18;
	SERCOM1->SPI.CTRLA.bit.ENABLE |= 1;
	while(SERCOM1->SPI.SYNCBUSY.bit.ENABLE);
}

uint8_t spiSend(uint8_t data) {
	uint8_t temp;
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
	SERCOM1->SPI.DATA.reg = data;
	while(SERCOM1->SPI.INTFLAG.bit.TXC == 0);
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
	temp = SERCOM1->SPI.DATA.reg;
	myprintf(" %x", temp);
	return temp;
}

void rcvr_datablock(const uint8_t * send_buff, uint32_t lba, uint8_t * receive_buff, uint32_t bs ) {
	uint8_t temp = 0xFF;
	uint32_t i;
	
	/* Ready state for reading process */
	REG_PORT_OUTCLR0 = PORT_PA18;
	myprintf("\n\n");

	/* Send first and last byte from commnad 17. The other four bytes correspond to the direction block you wish to access */
	temp = send_buff[0];
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = ((uint8_t*)&lba)[3];
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = ((uint8_t*)&lba)[2];
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = ((uint8_t*)&lba)[1];
	temp = spiSend(temp);
	myprintf(" %x", temp);
	
	temp = ((uint8_t*)&lba)[0];
	temp = spiSend(temp);
	myprintf(" %x", temp);

	temp = send_buff[5];
	temp = spiSend(temp);
	myprintf(" %x", temp);

	// Reading to find the beginning of the sector (until response is 0xFE)

	temp = spiSend(0xFF);
	while(temp != 0xFE){
		temp = spiSend(0xFF);
		myprintf(" %x", temp);
	}
	
	// Receiving the memory sector/block
	
	myprintf("\n\n");
	for(i=0; i< bs; i++) {
		while(SERCOM1->SPI.INTFLAG.bit.DRE == 0);
		SERCOM1->SPI.DATA.reg = 0xFF;
		while(SERCOM1->SPI.INTFLAG.bit.TXC == 0);
		while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);
		temp = SERCOM1->SPI.DATA.reg;
		*(receive_buff++) = temp;
		myprintf(" %x", temp);
	}
	REG_PORT_OUTSET0 = PORT_PA18;
	myprintf("\n\n");
}


uint32_t xchg_spi(const uint8_t * send_buff, uint32_t bc, uint8_t * receive_buff ) {
	uint8_t temp = 0xFF;
	uint32_t i;
	uint8_t temp_cmd = send_buff[0];
	
	/* Initialziation for sending bytes, the CMD is send */
	REG_PORT_OUTCLR0 = PORT_PA18;
	for(i=0; i< bc; i++) {
		temp = spiSend(*(send_buff++));
		//myprintf(" %x", temp);
	}
	/* The reading proccess starts depending on the response each command must return */
	switch(temp_cmd) {
		case kCMD00 :
		for(i=0; i<LENGTH_R1; i++) {
			temp = spiSend(0xFF);
			//myprintf(" %x", temp);
		}
		break;
		case kCMD08 :
		for(i=0; i<LENGTH_R7; i++) {
			temp = spiSend(0xFF);
			//myprintf(" %x", temp);
		}
		break;
		case kCMD17 :
		temp = spiSend(0xFF);
		while(temp != 0xFE){
			temp = spiSend(0xFF);
			//myprintf(" %x", temp);
		}
		break;
		case kCMD41 :
		for(i=0; i<LENGTH_R1-1; i++) {
			temp = spiSend(0xFF);
			//myprintf(" %x", temp);
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

void initCycles(void){
	uint32_t i;
	REG_PORT_OUTSET0 = PORT_PA18;
	for(i=0;i<76;i++)
	spiSend(0xFF);
}

/*
int main(void)
{	
	uint32_t temp = 0xFF;
	//Initialize the SAM system 
	SystemInit();
	//Switch to 8MHz clock (disable prescaler) 	
	SYSCTRL->OSC8M.bit.PRESC = 0;
	// Initialize the UART and SPI system 
	initUART();
	spiInit();
	// Initialize the SD card 
	initCycles();
	// Initialization complete 
	// Start and send CMD 00 
	myprintf("\n START\n");
	myprintf("CMD00\n");
	xchg_spi(CMD00, SIZE_SD_CMD, RxBuffer);  // reset instruction 
	myprintf("\n");
	// Send CMD08 
	myprintf("CMD08\n");
	xchg_spi(CMD08, SIZE_SD_CMD, RxBuffer);
	myprintf("\n");
	// Cycle until response of command CMD55 and CMD41 is 0 
	uint32_t response= (uint32_t) V;
	while (response != 0x00){
		myprintf("CMD55\n");
		xchg_spi(CMD55, SIZE_SD_CMD, RxBuffer);
		myprintf("\n");	
		myprintf("CMD41\n");
		response = xchg_spi(CMD41, SIZE_SD_CMD, RxBuffer);
		myprintf("\n");
	}
	// Initialization of SD card complete, send command 17 to start the reading of the card, read the first 512 bytes and print them  
	myprintf("CMD17\n");
	rcvr_datablock(CMD17,(uint32_t)Zero,RxBuffer,Block_byte_size);
	myprintf("\n");
	// Read done, the program has finished 
	
	myprintf("Done\n");
}
*/