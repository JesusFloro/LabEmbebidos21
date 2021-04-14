


int main(void)
{
       SystemInit();
       spiInit();
 
       volatile uint8_t rData;
       volatile uint8_t sData = 85;
       while (1) {
            REG_PORT_OUTCLR0 = PORT_PA18; //initiate transaction by SS_low
              rData = spiSend( sData );
            REG_PORT_OUTSET0 = PORT_PA18; //finish transaction by SS_high
       }
}
