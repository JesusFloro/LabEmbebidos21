#include "spi.h"
#include "stdio.h"
#include "myprintf.h"
#include "sam.h"
#define BUF_SIZE 4

// Control variables for data transfer and reception
volatile uint32_t i;
uint8_t tx_buf[BUF_SIZE] = {0x03u, 0x03u, 0x22u, 0x04u};
uint8_t rx_buf[BUF_SIZE];
uint32_t br;


void spiInit( void ) {
    /* Switch to 8MHz clock (disable prescaler) */
    SYSCTRL->OSC8M.bit.PRESC = 0;

    PM->APBCMASK.bit.SERCOM1_ = 1; //enable the clock for SERCOM1 peripheral
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID_SERCOM1_CORE;
    while( GCLK->STATUS.bit.SYNCBUSY ) { }
    
    //structures to configure the SERCOM1 peripheral
	//CTRLA.bit.DORD = 0x0;
	//SERCOM1.SPI.CTRLA.bit.DORD 0x0;
    const SERCOM_SPI_CTRLA_Type ctrla = {
		.bit.DORD = 0x0, // MSB first
		.bit.CPHA = 0x0, // Mode 0
		.bit.CPOL = 0x0, 
		.bit.FORM = 0x0, // SPI frame
		.bit.DIPO = 0x3, // MISO on PAD[3]
		.bit.DOPO = 0x0, // MOSI on PAD[0], SCK on PAD[1], SS_ on PAD[2]
		.bit.MODE = 0x3  // Master Mode
    };
    SERCOM1->SPI.CTRLA.reg = ctrla.reg;    
    const SERCOM_SPI_CTRLB_Type ctrlb = {
       .bit.RXEN = 0x1,   // RX enabled
       .bit.MSSEN = 0x1,  // Manual SC -> posible cambio a 0x0, revisar con el profesor
       .bit.CHSIZE = 0x0  // 8-bit
    };
    SERCOM1->SPI.CTRLB.reg = ctrlb.reg;

    //Formula to configure the desired baudrate
    uint32_t br = ( double )( 8000000 / ( 2 * BAUDRATE ) ) - 1 ;
    SERCOM1->SPI.BAUD.reg = SERCOM_SPI_BAUD_BAUD( ( uint8_t )br );

    //structure to configure multiple PINs
    const PORT_WRCONFIG_Type wrconfig = {
        .bit.WRPINCFG = 1,
        .bit.WRPMUX = 1,
        .bit.PMUX = MUX_PA16C_SERCOM1_PAD0 | MUX_PA17C_SERCOM1_PAD1 | MUX_PA19C_SERCOM1_PAD3,
        .bit.PMUXEN = 1,
        .bit.HWSEL = 1,
        .bit.PINMASK = ( uint16_t )( ( PORT_PA16 | PORT_PA17 | PORT_PA19 ) >> 16 )
    };
    PORT->Group[0].WRCONFIG.reg = wrconfig.reg;

    //SS/CS (Slave Select/Chip Select) PIN 18 configuration
    REG_PORT_DIRSET0 = PORT_PA18;
    REG_PORT_OUTSET0 = PORT_PA18;
    
    //enable the SPI
    SERCOM1->SPI.CTRLA.bit.ENABLE = 1;
    while( SERCOM1->SPI.SYNCBUSY.bit.ENABLE ) { }
}

uint8_t spiSend(uint8_t data) {
	uint8_t temp;
	while(SERCOM1->SPI.INTFLAG.bit.DRE == 0); //wait until buffer is empty
	SERCOM1->SPI.DATA.reg = data;//transmit data
	while(SERCOM1->SPI.INTFLAG.bit.TXC == 0);//wait until there is no data to transmit
	while(SERCOM1->SPI.INTFLAG.bit.RXC == 0);//wait until a data is received
	temp = SERCOM1->SPI.DATA.reg;//read data
	myprintf(" %x", temp); //printf the value in putty
	return temp;
}

