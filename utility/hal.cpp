
#include <stdint.h>
#include <stdbool.h>  //Required for driverlib compatibility
#include "_EDIT_THESE_SETTINGS.h"
#include "Energia.h"
#include "HAL.h"
#include "../../SPI/SPI.h"
#include "hal_version.h"


#define LEFT_BUTTON             GPIO_PIN_4
#define RIGHT_BUTTON            GPIO_PIN_0
#define ALL_BUTTONS             (LEFT_BUTTON | RIGHT_BUTTON)

uint8_t MRSTpin;
uint8_t MRDYpin;
uint8_t SRDYpin;
uint8_t SPImodule;

int printf(const char *fmt, ... ) {
#ifdef ENABLE_UART_DEBUG
	char tmp[64]; // resulting string limited to 64 chars
	va_list args;
	va_start (args, fmt );
	vsnprintf(tmp, 64, fmt, args);
	va_end (args);
	Serial.print(tmp);
	return strlen(tmp);
#endif
}


/** Initializes Ports/Pins: sets direction, interrupts, pullup/pulldown resistors etc. */
void halInit()
{
	 pinMode(MRDYpin,OUTPUT);	//SS & MRDY
	 pinMode(SRDYpin,INPUT);	// SRDY
	pinMode(MRSTpin,OUTPUT);	//MRST
	

    RADIO_OFF();
    SPI_SS_CLEAR();


}


/** Display information about this driver firmware */
void displayVersion()
{
    int i = 0;
    printf("\r\n");
    for (i=0; i<8; i++)
        printf("-");
    printf(" Module Interface and Examples %s ", MODULE_INTERFACE_STRING);
    for (i=0; i<8; i++)
            printf("-");
    printf("\r\n");
    printf("%s", MODULE_VERSION_STRING);
}

void halSpiInitModule()
{
    // Disable the SSI Port
    //SSIDisable(SSI2_BASE);

    // Reconfigure the SSI Port for Module operation.
    // Clock polarity = inactive is LOW (CPOL=0); Clock Phase = 0; MSB first; Master Mode, 2MHz, data is 8bits wide;
    //SSIConfigSetExpClk(SSI2_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 1000000, 8);

    // Enable the SSI Port
    //SSIEnable(SSI2_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //

#if defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM____) || defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
	SPI.setModule(SPImodule);
#else
	SPI.begin();
#endif
	SPI.setDataMode(SPI_MODE0);
	
#if defined(__TM4C1294NCPDT__) || defined(__TM4C129XNCZAD__) 
	SPI.setClockDivider(SPI_CLOCK_DIV2); //1 MHz SPI Clock  
#else
	SPI.setClockDivider(SPI_CLOCK_DIV16); //1 MHz SPI Clock  
#endif

	

    // Don't select the module
    SPI_SS_CLEAR();
}

/**
Sends a message over SPI to the Module.
SPI uses a "write-to-read" approach to read data out, you must write data in.
This is a private method that gets wrapped by other methods, e.g. spiSreq(), spiPoll(), etc.
To Write, set *bytes, numBytes.
To Read, set *bytes only. Don't need to set numBytes because Module will stop when no more bytes read.
@param bytes the data to be sent or received.
@param numBytes the number of bytes to be sent. This same buffer will be overwritten with the received data.
@note Modify this method for other hardware implementations.
@pre SPI port configured for writing
@pre Module has been initialized
@post bytes contains received data, if any
@see Stellaris Application Note spma002.pdf, "Adding 32kB of Serial SRAM to a Stellaris Microcontroller"
*/
void spiWrite(unsigned char *bytes, unsigned char numBytes)
{

	uint8_t ulReadData;
	while(numBytes--)    // Loop while there are more bytes left to be transferred.
    {
		ulReadData=SPI.transfer(*bytes);
		//printf("spi MISO val=%u, MOSI val=%u, numBytes=%u\r\n",ulReadData,*bytes,numBytes);
        //SSIDataPut(SSI2_BASE, *bytes);              // Write the next byte to the SSI controller with a blocking put.
        //SSIDataGet(SSI2_BASE, &ulReadData);         // Read into a long first
        *bytes++ = (unsigned char)ulReadData;       // ...and then convert it to a char
    }


	/* Note: Stellaris processors have a tx/rx FIFO. If you'd like to wait until the FIFO is empty
	 * before returning from this method, include the following code:
	 */
    //while ( (~HWREG(SSI0_BASE + SSI_O_SR)) & SSI_SR_TFE) ;   //wait while Transmit FIFO is NOT empty
	//while ( (HWREG(SSI0_BASE + SSI_O_SR)) & SSI_SR_BSY) ;   //wait while busy - works without this
}


/**
A fairly accurate Blocking Delay in Milliseconds - delays by at least the specified number of
milliseconds (mSec)
@pre SysCtlClockSet() has been called to set the processor clock rate
@param ms number of milliseconds to delay
@note change if you are using an RTOS.
*/
void delayMs(uint16_t ms)
{
	delay(ms);
}
