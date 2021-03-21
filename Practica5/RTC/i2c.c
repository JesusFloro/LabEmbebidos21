/*******************************************************************************
*
*   i2c.c
*
*   Copyright (c) 2013 Shahrooz Shahparnia (sshahrooz@gmail.com)
*
*   Description:
*   i2c is a command-line utility for executing i2c commands with the 
*   Broadcom bcm2835.  It was developed and tested on a Raspberry Pi single-board
*   computer model B.  The utility is based on the bcm2835 C library developed
*   by Mike McCauley of Open System Consultants, http://www.open.com.au/mikem/bcm2835/.
*
*   Invoking spincl results in a read or write I2C transfer.  Options include the
*   the I2C clock frequency, read/write, address, and port initialization/closing
*   procedures.  The command usage and command-line parameters are described below
*   in the showusage function, which prints the usage if no command-line parameters
*   are included or if there are any command-line parameter errors.  Invoking i2c 
*   requires root privilege.
*
*   This file contains the main function as well as functions for displaying
*   usage and for parsing the command line.
*
*   Open Source Licensing GNU GPLv3
*
*   Building:
* After installing bcm2835, you can build this 
* with something like:
* gcc -o i2c i2c.c -l bcm2835
* sudo ./i2c
*
* Or you can test it before installing with:
* gcc -o i2c -I ../../src ../../src/bcm2835.c i2c.c
* sudo ./i2c
*
*   History:
*   11/05    VERSION 1.0.0: Original
*
*      User input parsing (comparse) and showusage\
*      have been adapted from: http://ipsolutionscorp.com/raspberry-pi-spi-utility/
*      mostly to keep consistence with the spincl tool usage.
*
*      Compile with: gcc -o i2c i2c.c bcm2835.c
*
*      Examples:
*
*           Set up ADC (Arduino: ADC1015)
*           sudo ./i2c -s72 -dw -ib 3 0x01 0x44 0x00 (select config register, setup mux, etc.)
*           sudo ./i2c -s72 -dw -ib 1 0x00 (select ADC data register)
*
*           Bias DAC (Arduino: MCP4725) at some voltage
*           sudo ./i2c -s99 -dw -ib 3 0x60 0x7F 0xF0 (FS output is with 0xFF 0xF0)
*           Read ADC convergence result
*           sudo ./i2c -s72 -dr -ib 2 (FS output is 0x7FF0 with PGA1 = 1)
*  
*      In a DAC to ADC loop back typical results are:
*
*      DAC    VOUT   ADC
*      7FFh   1.6V   677h                    Note ratio is FS_ADC*PGA_GAIN/FS_DAC = 4.096/3.3 = 1.23
*      5FFh   1.2V   4DCh
*      8F0h   1.8V   745h
*      9D0h   2V     7EAh
*      000h   10mV   004h
*
********************************************************************************/

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <string.h>


#define MODE_READ 0
#define MODE_WRITE 1

#define MAX_LEN 32

char wbuf[MAX_LEN];

char* decimalToBCD(int decimal){

  char* fullBcd = malloc(16);

  int i,r[5],j,c=0;
	i=0;
	while(decimal!=0)
	{ r[i]=decimal%10;
	  decimal=decimal/10;
	  i++;
	  c++;
	}

	for(j=c-1;j>=0;j--)
	{ 
    printf(" %d ", r[j]);
    /*if(r[j]==9)
      strncat(fullBcd, "1001", 4);
      //fullBcd = "1001";
	    //printf("1001");
	  else if(r[j]==8)
	    strncat(fullBcd, "1000", 4);
      //printf("1000");
	  else if(r[j]==7)
      strncat(fullBcd, "0111", 4);
	    //printf("0111");
	  else if(r[j]==6)
      strncat(fullBcd, "0110", 4);
	    //printf("0110");
	  else if(r[j]==5)
      strncat(fullBcd, "0101", 4);
	    //printf("0101");
	  else if(r[j]==4)
      strncat(fullBcd, "0100", 4);
	    //printf("0100");
	  else if(r[j]==3)
      strncat(fullBcd, "0011", 4);
	    //printf("0011");
	  else if(r[j]==2)
      strncat(fullBcd, "0010", 4);
	    //printf("0010");
	  else if(r[j]==1)
      strncat(fullBcd, "0001", 4);
	    //printf("0001");
	  else
      strncat(fullBcd, "0000", 4);
	    //printf("000");*/
  }
  //printf("Converted number is: %s", fullBcd);

  return fullBcd;
}

void setCurrentTime(){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    //printf("now: %d-%02d-%02d %0temporal2d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    /*bcm2835_i2c_write(wbuf, len);
    /char wbuf[MAX_LEN];
    #define MAX_LEN 32
    #define sec tm.tm_sec
    //char temporal[5]; //ACA HAY QUE PASAR DECIMAL A BCD y luego escribir
    //bcm2835\_i2c_write(temporal, 1);*/
    printf("\n Segundos: ");
    char* segundos = decimalToBCD(tm.tm_sec);
    printf("\n Minutos: ");
    char* minutos = decimalToBCD(tm.tm_min);
    printf("\n Horas: ");
    char* horas = decimalToBCD(tm.tm_hour);
    printf("\n Dia: ");
    char* dia = decimalToBCD(tm.tm_mday);
    printf("\n Mes: ");
    char* mes = decimalToBCD((tm.tm_mon + 1));
    printf("\n Anio: ");
    char* anio = decimalToBCD((tm.tm_year + 1900));
    printf("\n anio: ");
    //bcm2835_i2c_write(segundos, 1);
    
    }

typedef enum {
    NO_ACTION,
    I2C_BEGIN,
    I2C_END
} i2c_init;

uint8_t  init = NO_ACTION;
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address = 0x00;
uint32_t len = 0;
uint8_t  mode = MODE_READ;

//*******************************************************************************
//  comparse: Parse the command line and return EXIT_SUCCESS or EXIT_FAILURE
//    argc: number of command-line arguments
//    argv: array of command-line argument strings
//*******************************************************************************

int comparse(int argc, char **argv) {
    int argnum, i, xmitnum;
	
    if (argc < 2) {  // must have at least program name and len arguments
                     // or -ie (I2C_END) or -ib (I2C_BEGIN)
        fprintf(stderr, "Insufficient command line arguments\n");
        return EXIT_FAILURE;
    }
    
    argnum = 1;
    while (argnum < argc && argv[argnum][0] == '-') {

        switch (argv[argnum][1]) {

            case 'i':  // I2C init
                switch (argv[argnum][2]) {
                    case 'b': init = I2C_BEGIN; break;
                    case 'e': init = I2C_END; break;
                    default:
                        fprintf(stderr, "%c is not a valid init option\n", argv[argnum][2]);
                        return EXIT_FAILURE;
                }
                break;

            case 'd':  // Read/Write Mode
                switch (argv[argnum][2]) {
                    case 'r': mode = MODE_READ; break;
                    case 'w': mode = MODE_WRITE; break;
                    default:
                        fprintf(stderr, "%c is not a valid init option\n", argv[argnum][2]);
                        return EXIT_FAILURE;
                }
                break;

            case 'c':  // Clock divider
                clk_div = atoi(argv[argnum]+2);
                break;

            case 's':  // Slave address
                slave_address = atoi(argv[argnum]+2);
                break;

            default:
                fprintf(stderr, "%c is not a valid option\n", argv[argnum][1]);
                return EXIT_FAILURE;
        }

        argnum++;   // advance the argument number

    }

    // If command is used for I2C_END or I2C_BEGIN only
    if (argnum == argc && init != NO_ACTION) // no further arguments are needed
        return EXIT_SUCCESS;
	
    // Get len
    if (strspn(argv[argnum], "0123456789") != strlen(argv[argnum])) {
        fprintf(stderr, "Invalid number of bytes specified\n");
        return EXIT_FAILURE;
    }
    
    len = atoi(argv[argnum]);


    if (len > MAX_LEN) {
    	fprintf(stderr, "Invalid number of bytes specified\n");
        return EXIT_FAILURE;
    }
    
    argnum++;   // advance the argument number

    xmitnum = argc - argnum;    // number of xmit bytes

    memset(wbuf, 0, sizeof(wbuf));

    for (i = 0; i < xmitnum; i++) {
        if (strspn(argv[argnum + i], "0123456789abcdefABCDEFxX") != strlen(argv[argnum + i])) {
            fprintf(stderr, "Invalid data: ");
	    fprintf(stderr, "%d \n", xmitnum);
            return EXIT_FAILURE;
        }
        wbuf[i] = (char)strtoul(argv[argnum + i], NULL, 0);
    }

    return EXIT_SUCCESS;
}

//*******************************************************************************
//  showusage: Print the usage statement and return errcode.
//*******************************************************************************
int showusage(int errcode) {
    printf("i2c \n");
    printf("Usage: \n");
    printf("  i2c [options] len [rcv/xmit bytes]\n");
    printf("\n");
    printf("  Invoking i2c results in an I2C transfer of a specified\n");
    printf("    number of bytes.  Additionally, it can be used to set the appropriate\n");
    printf("    GPIO pins to their respective I2C configurations or return them\n");
    printf("    to GPIO input configuration.  Options include the I2C clock frequency,\n");
    printf("    initialization option (i2c_begin and i2c_end).  i2c must be invoked\n");
    printf("    with root privileges.\n");
    printf("\n");
    printf("  The following are the options, which must be a single letter\n");
    printf("    preceded by a '-' and followed by another character.\n");
    printf("    -dx where x is 'w' for write and 'r' is for read.\n");
    printf("    -ix where x is the I2C init option, b[egin] or e[nd]\n");
    printf("      The begin option must be executed before any transfer can happen.\n");
    printf("        It may be included with a transfer.\n");
    printf("      The end option will return the I2C pins to GPIO inputs.\n");
    printf("        It may be included with a transfer.\n");
    printf("    -cx where x is the clock divider from 250MHz. Allowed values\n");
    printf("      are 150 through 2500.\n");
    printf("      Corresponding frequencies are specified in bcm2835.h.\n");
    printf("\n");
    printf("    len: The number of bytes to be transmitted or received.\n");
    printf("    The maximum number of bytes allowed is %d\n", MAX_LEN);
    printf("\n");
    printf("\n");
    printf("\n");
    return errcode;
}

char buf[MAX_LEN];
int i;
uint8_t data;

int main(int argc, char **argv) {

    printf("Running ... \n");
    
    // parse the command line
    if (comparse(argc, argv) == EXIT_FAILURE) return showusage (EXIT_FAILURE);

    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
      
    // I2C begin if specified    
    if (init == I2C_BEGIN)
    {
      if (!bcm2835_i2c_begin())
      {
        printf("bcm2835_i2c_begin failed. Are you running as root??\n");
	    return 1;
      }
      //set the current date and time
      setCurrentTime();
    }
	  

    // If len is 0, no need to continue, but do I2C end if specified
    if (len == 0) {
         if (init == I2C_END) bcm2835_i2c_end();
	 printf("... done!\n");
         return EXIT_SUCCESS;
    }

    bcm2835_i2c_setSlaveAddress(slave_address);
    bcm2835_i2c_setClockDivider(clk_div);
    fprintf(stderr, "Clock divider set to: %d\n", clk_div);
    fprintf(stderr, "len set to: %d\n", len);
    fprintf(stderr, "Slave address set to: %d\n", slave_address);   
    
    if (mode == MODE_READ) {
    	for (i=0; i<MAX_LEN; i++) buf[i] = 'n';
    	data = bcm2835_i2c_read(buf, len);
    	printf("Read Result = %d\n", data);   
    	for (i=0; i<MAX_LEN; i++) {
    		if(buf[i] != 'n') printf("Read Buf[%d] = %x\n", i, buf[i]);
	}    
    }
    //len = 0; //regresar a la posicion inicial para que siempre que leamos salgan los primeros datos

    if (mode == MODE_WRITE) {
    	data = bcm2835_i2c_write(wbuf, len);
    	printf("Write Result = %d\n", data);
    }   

    // This I2C end is done after a transfer if specified
    if (init == I2C_END) bcm2835_i2c_end();   
    bcm2835_close();
    printf("... done!\n");
    return 0;
}

