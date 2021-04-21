#ifndef SPI_H_
#define SPI_H_

#define BAUDRATE 9600
#include "stdint.h"

void spiInit( void );
uint8_t spiSend( uint8_t data );

#endif /* SPI_H_ */
