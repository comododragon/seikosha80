/**
 * Some SPI-related utilities.
 */


#ifndef SPIUTILS_H
#define SPIUTILS_H


#include <stdbool.h>


// Initialise SPI
void spi_setup(void);

// Disable SPI
void spi_disable(void);

// True if SPI was enabled, false otherwise
bool spi_isEnabled(void);


#endif
