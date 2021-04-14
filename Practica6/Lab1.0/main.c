#include "sam.h"
//#include "component\pm.h"
#include "SAMD21G18A_H.h"

#define PINCFG_CONFIG_VALUE 0b00000000
#define GENCTRL_CONFIG_VALUE 0x10600
#define CLKCTRL_CONFIG_VALUE 0x401B



int main(void) {
/*
    main:  //PORT configuration for general-purpose PIN
                     LDR  R0, =0x41004400 //PORT Base Address
                     LDR  R1, =0b00000000 //config value
                     LDR  R2, =0x4E
                     STRB R1, [R0, R2]    //PINCFG14
                     LDR  R2, =0x00004000 //PA14 bit set position
                     STR  R2, [R0, #0x08] //DIRSET -- data direction as output
    */
        PORT ->Group[0].PINCFG[14].reg = PINCFG_CONFIG_VALUE; //
        PORT ->Group[0].DIRSET.reg = PORT_PA14;
    /*
        //TIMER configuration
              //Configure the POWER MANAGER to enable the TC3 module
                     LDR  R3, =0x40000400 //PM Base Address
                     LDR  R1, [R3, #0x20] //read register APBCMASK
                     LDR  R4, =0x00000800 //mask value
                     ORRS R1, R1, R4      //to set desired bit(s)
                     STR  R1, [R3, #0x20] //APBCMASK
    */
        PM -> APBCMASK.reg |= 0x00000800;
    /*
              //Configure the GENERIC CLOCK CONTROL used by the TC3 module
                     LDR  R3, =0x40000C00 //GCLK Base Address
                     LDR  R1, =0          //config value
                     STR  R1, [R3, #0x08] //GENDIV
                     LDR  R1, =0x10600    //config value
                     STR  R1, [R3, #0x04] //GENCTRL
                     LDR  R1, =0x401B     //config value
                     STRH R1, [R3, #0x02] //CLKCTRL
    */
        GCLK ->GENDIV.reg = 0x00000000;
        GCLK ->GENCTRL.reg = 0x10600;
        GCLK ->CLKCTRL.reg = 0x401B;
    /*
              //Determine the initial count
              //Time = Toscillator * (2^TIMERMODE - COUNT) * Prescaler
                     LDR  R3, =0x42002C00 //TC3 Base Address
                     LDR  R6, =65307      //initial count of the timer + 27 cycles
                     STRH R6, [R3, #0x10] //COUNT  
    */
        TC3 -> COUNT16.COUNT.reg = 65307;
    /*            
              //Configure the TC3 peripheral
                     LDR  R1, =0          //config value
                     STRH R1, [R3]        //CTRLA
                     STRB R1, [R3, #0x05] //CTRLBSET
                     LDR  R5, =0x1        //to clear OVF flag
                     STRB R5, [R3, #0x0E] //INTFLAG
    */
        TC3 -> COUNT16.CTRLA.reg = 0;
        TC3 -> COUNT16.CTRLBSET.reg = 0;
        TC3 -> COUNT16.INTFLAG.reg = 0x1;
    
    /*
              //Enabling the TC3 peripheral
                     LDR  R1, [R3]        //reads CTRLA
                     LDR  R4, =0x2        //mask value
                     ORRS R1, R1, R4      //to set desired bit(s)
                     STRH R1, [R3]        //CTRLA
    */
        TC3 ->COUNT16.CTRLA.reg |= 0x2;
    /*
              //Loop to sync the TC clock with the processor/main clock
                     LDR  R4, =0x80       //mask value
read:                LDRB R1, [R3, #0x0F] //reads STATUS
                     ANDS R1, R1, R4      //to test SYNCBUSY bit
                     BNE  read            //jump if it not synchronize
    */
        while(!TC3 -> COUNT16.STATUS.bit.SYNCBUSY);
    /*
              //Loop to generate the square-wave signal
            //EXTRA cycles to ADD to the initial count---it will be still a
            //polling-error of +/- 7 us in the period of the signal
            //LDRB(4) + ANDS(1) + BEQ(2) + LDRB(4) + ANDS(1) + BEQ(1) + STR(4) +
            //STRB(4) + STRH(4) + B(2) = 27 cycles
again:                   LDRB R1, [R3, #0x0E] //reads the INTFLAG
                     ANDS R1, R1, R5      //to test OVF flag
                     BEQ  again           //jump if is not overflowed
                     STR  R2, [R0, #0x1C] //toggle the output pin
                     STRB R5, [R3, #0x0E] //clear OVF flag
                     STRH R6, [R3, #0x10] //recharge the initial COUNT
                     B    again
    */
    while(1){
        while(!TC3->COUNT16.INTFLAG.bit.OVF);
        PORT -> Group[0].OUTTGL.reg = 0x01;
        TC3  -> COUNT16.INTFLAG.reg = 0x01;
        TC3  -> COUNT16.COUNT.reg = 65307;
    }
    
}
