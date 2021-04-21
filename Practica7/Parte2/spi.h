//#ifndef SPI_H_
//#define SPI_H_

#ifndef SPI_H_
#define SPI_H_

#include "sam.h"
#include "myprintf.h"
#define BAUDRATE 10000
#define SPI_SERCOM SERCOM1



#define RXBUFSIZE 0x400
#define LENGTH_R1 0x03
#define LENGTH_R7 0x07

#define SIZE_SD_CMD 0x06
#define SIZE_BLOCK 0x0200
#define kCMD00 0x40
#define kCMD08 0x48
#define kCMD55 0x77
#define kCMD41 0x69

void spiInit( void );
uint32_t spiXchg(const uint8_t * send_buff, uint32_t bc, uint8_t * receive_buff );
void rcvr_datablock(const uint8_t * send_buff, uint32_t lba, uint8_t * receive_buff, uint32_t bs );
uint8_t spiSend( uint8_t data );

#endif /* SPI_H_ */
