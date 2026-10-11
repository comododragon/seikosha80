/**
 * Functions related to the inputs
 */


#include "inputs.h"

#include <avr/interrupt.h>


// For INT0, we do not need interrupt handlers. We only use interrupt
// to get out of sleep mode in these cases. Nonetheless, an empty ISR must be
// defined. Search for "Empty interrupt service routines" on
// https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
EMPTY_INTERRUPT(INT0_vect);


void ins_setup(void) {
	DDRD &= ~((1 << DDD3) | (1 << DDD2));
	PORTD &= ~((1 << PORTD3) | (1 << PORTD2));
}


void ins_prepare(void) {
	// Reset interrupt configuration
	EICRA &= ~((1 << ISC01) | (1 << ISC00));
	// Set up for edge detection
	EICRA |= (1 << ISC01);
	// Clear EIFR flags
	EIFR |= (1 << INTF0);
	// Enable interrupt
	EIMSK |= (1 << INT0);
}


void ins_unprepare(void) {
	EIMSK &= ~(1 << INT0);
}


bool ins_triggered(void) {
	return (EIFR & (1 << INTF0));
}


void ins_clear(void) {
	EIFR |= (1 << INTF0);
}
