#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <bcm2835.h>
//declarar el dia en especifico
#define year    0x01
#define month   0x01
#define day     0x01
#define weekday 0x01
#define hour    0x12
#define minute  0x00
#define second  0x00
//bits que no estan en uso
#define unusedbits_1    0x7F
#define unusedbits_2    0X3F
#define unusedbits_3    0X1F
#define unusedbits_5    0X07
//Datos estaticos para el RTC
#define	sec	 0
#define	MAX	 30
#define	min	 1
#define	hr 2
#define	dayw 3
#define	daym 4
#define	mon  5
#define	yea  6
#define	len	 8
#define	Temp 77
#define	Address_Byte	1
#define	Time_Bytes		7
#define	wait		5000
#define	Baud_Rate		10000
#define	RTC	0x68

typedef struct
{
	char Record1[75];
    char Record2[75];
    char Record3[75];
    unsigned int times;
}Records;

int main(int argc, char **argv)
{
    //Diccionario con los tiempos definidos previamente
	char buf[]={second, second, minute, hour, weekday, day, month, year};
	//diccionario con los dias de la semana
	char *str[]  ={"mon", "tues", "wed", "thur", "fri", "sat","sun"};
	//Mensajes de error
	Records rec = {"No information","No information","No information",1};
	short cycles=sec;
	//I2C validation of slaves
    if (!bcm2835_init())
        return 1;
    //Initialize slave and baud_Rate
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(RTC);
    bcm2835_i2c_set_baudrate(Baud_Rate);
    printf("START \n");
    //Put initial value of RTC
    bcm2835_i2c_write(buf, len);
    while(1)
    {
        //Lectura de la temperatura
		buf[sec] = second;
		bcm2835_i2c_setSlaveAddress(Temp);
		char data= bcm2835_i2c_read(buf, min);
		printf("Temperature: %d \n", buf[sec]);
		int dcon = (int) buf[sec];
        //RTC, aplicando mascaras de los bits que no se estan usando
		bcm2835_i2c_setSlaveAddress(RTC);
		buf[sec] = second;
		bcm2835_i2c_write_read_rs(buf, Address_Byte, buf, Time_Bytes);
		buf[sec] = buf[sec] & unusedbits_1; //sec
		buf[min] = buf[min] & unusedbits_1; //min
		buf[hr] = buf[hr] & unusedbits_2; //hour
		buf[dayw] = buf[dayw] & unusedbits_5; //day of the week
		buf[daym] = buf[daym] & unusedbits_2; //day of the month
		buf[mon] = buf[mon] & unusedbits_3; //month
        //...Condicionales de temperatura para almacenaje
		if (dcon>=MAX || buf[sec] == 0x00 || buf[sec] == 0x10 || buf[sec] == 0x20 || buf[sec] == 0x30 || buf[sec] == 0x40 || buf[sec] == 0x50){
			//Store the last 3 records
			strcpy(rec.Record3,rec.Record2);
			strcpy(rec.Record2,rec.Record1);
			snprintf(rec.Record1,75,"20%02x/%02x/%02x %02x:%02x:%02x - Record %i of day %s", buf[yea], buf[mon], buf[daym], buf[hr], buf[min], buf[sec], rec.times, str[(unsigned char)buf[dayw]-1]);
			rec.times = rec.times + min;
            //Almacenaje de la temperatura en el archivo... buscarlo en la carpeta destino
			FILE *f = fopen("Recordings.txt", "w");
			printf("%s \n",rec.Record1);
			printf("%s \n",rec.Record2);
			printf("%s \n",rec.Record3);
			fprintf(f,"%s \n",rec.Record1);
			fprintf(f,"%s \n",rec.Record2);
			fprintf(f,"%s \n",rec.Record3);
			//year/month/day
			fclose(f);
		}
/*}*/		bcm2835_delay(wait);
    }
    //end and close i2c connection
    bcm2835_i2c_end();
    bcm2835_close();
    return 0;
}
