/**
 * Some SPI-related utilities.
 */


#include "spiutils.h"

#include "avr/io.h"


bool __spi_enabled = false;


void spi_setup(void) {
	// Set up SPI, with pullup on MISO as suggested by the tutorial
	// (http://elm-chan.org/docs/mmc/mmc_e.html)

	// Set SCK, MOSI and SS outputs
	DDRB = (1 << DDB5) | (1 << DDB3) | (1 << DDB2);
	// Set pullup on MISO and pull SS high
	PORTB = (1 << PORTB4) | (1 << PORTB2);
	// Enable SPI on master mode, clock rate to between 100kHz and 400kHz
	//SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1);

	__spi_enabled = true;
}


void spi_disable(void) {
	// Disable SPI
	SPCR &= ~(1 << SPE);
	// Reset pin states
	DDRB &= ~((1 << DDB5) | (1 << DDB3) | (1 << DDB2));
	PORTB &= ~((1 << PORTB4) | (1 << PORTB2));

	__spi_enabled = false;
}


bool spi_isEnabled(void) {
	return __spi_enabled;
}
