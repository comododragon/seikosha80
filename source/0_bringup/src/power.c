/**
 * Power-related functions (i.e. for low-power mode).
 */


#include "power.h"

#include <avr/sleep.h>
#include <avr/wdt.h>


void pwr_setup(void) {
	// Disable ADC
	ADCSRA &= ~(1 << ADEN);

	// Disable analog comparator
	ACSR |= (1 << ACD);

	// Disable watchdog timer
	wdt_reset();
	MCUSR &= ~(1 << WDRF);
	WDTCSR |= (1 << WDCE) | (1 << WDE);
	WDTCSR = 0x00;

	// Disable digital input buffers on analog input pins
	DIDR1 |= (1 << AIN1D) | (1 << AIN0D);
	DIDR0 |= (1 << ADC5D) | (1 << ADC4D) | (1 << ADC3D) | (1 << ADC2D) | (1 << ADC1D) | (1 << ADC0D);
}


void pwr_sleepPrepare(void) {
	// Now prepare to sleep
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	sleep_enable();
	// Disable BOD
	// If ADC, BOD and analog comparators are disabled, the internal voltage reference should be disabled automatically
	sleep_bod_disable();
}


void pwr_enableRail(void) {
	DDRD |= (1 << DDD2);
}


void pwr_disableRail(void) {
	DDRD &= ~(1 << DDD2);
}
