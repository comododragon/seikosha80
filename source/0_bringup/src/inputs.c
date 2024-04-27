/**
 * Functions related to the inputs
 */


#include "inputs.h"

#include <avr/interrupt.h>


// For INT0 and INT1, we do not need interrupt handlers. We only use interrupt
// to get out of sleep mode in these cases. Nonetheless, an empty ISR must be
// defined. Search for "Empty interrupt service routines" on
// https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html
//EMPTY_INTERRUPT(INT0_vect);
//EMPTY_INTERRUPT(INT1_vect);


bool __int0_triggered = false;
bool __int1_triggered = false;


ISR(INT0_vect) {
	__int0_triggered = true;
}


ISR(INT1_vect) {
	__int1_triggered = true;
}


void ins_setup(void) {
	DDRD &= ~((1 << DDD3) | (1 << DDD2));
	PORTD &= ~((1 << PORTD3) | (1 << PORTD2));
}


void ins_prepare(void) {
	EICRA &= ~((1 << ISC11) | (1 << ISC10) | (1 << ISC01) | (1 << ISC00));
	EIMSK |= (1 << INT1) | (1 << INT0);
}


void ins_unprepare(void) {
	EIMSK &= ~((1 << INT1) | (1 << INT0));
}


bool ins_0triggered(void) {
	return __int0_triggered;
}


bool ins_1triggered(void) {
	return __int1_triggered;
}


void ins_clear(void) {
	__int0_triggered = false;
	__int1_triggered = false;
}
