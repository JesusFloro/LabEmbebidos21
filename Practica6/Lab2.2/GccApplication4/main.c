/*
 * main.c
 *
 * Created: 01/11/2017 08:47:35 p. m.
 *  Author: alexfloro
 */ 

#include "sam.h"
#include "RTCControl.h"
#include "stdint.h"
#include "myprintf.h"

#define SLAVE_ADDR 0x68u

RTControl RTCi;
RTControl *pRTC= &RTCi;

int prev_sec = 0, sec = 0;

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

void setDate(void){
	pRTC->year.reg=0x21;
	pRTC->month.reg=0x04;
	pRTC->date.reg=0x13;
	pRTC->days.reg=0x07;
	pRTC->hours.reg=0x10;
	pRTC->minutes.reg=0x07;
	pRTC->seconds.reg=0x00;
}

int main(){
	UARTInit(); //Initialize UART
	initI2C(); //Initialize I2C
	setDate(); //Set date
	sendI2CDataArray(SLAVE_ADDR, pRTC);
	
	ReciveI2CDataArray(SLAVE_ADDR, pRTC);
	prev_sec = (pRTC->seconds.field.dec_seg) * 10 + (pRTC->seconds.field.un_seg);
	while (1){
		ReciveI2CDataArray(SLAVE_ADDR, pRTC);
		sec = (pRTC->seconds.field.dec_seg) * 10 + (pRTC->seconds.field.un_seg);
		if (prev_sec != sec){
			myprintf("%d%d/%d%d/20%d%d - ", pRTC->date.field.dec_date, pRTC->date.field.un_date, pRTC->month.field.dec_month, pRTC->month.field.un_month, pRTC->year.field.dec_year, pRTC->year.field.un_year);
			myprintf("%d%d:%d%d:%d%d\n", pRTC->hours.field.dec_hr, pRTC->hours.field.un_hr, pRTC->minutes.field.dec_min, pRTC->minutes.field.un_min, pRTC->seconds.field.dec_seg, pRTC->seconds.field.un_seg);
			prev_sec = sec;
		}
	}
}
