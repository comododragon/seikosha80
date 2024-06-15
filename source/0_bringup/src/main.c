/**
 * Main function
 */


#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <stdlib.h>

#include "index.h"
#include "inputs.h"
#include "power.h"
#include "rtc.h"
#include "sd.h"
#include "sound.h"
#include "twiutils.h"
#include "spiutils.h"


#define ASSERT(cond) do {\
	if(!(cond))\
		goto _err;\
} while(0)


int main(void) {
	int sdRetVal = SD_NO_ERROR;
	bool firstRun = true;
	unsigned char *rtcTime, setTime[2];
	unsigned char hours, hours12, quarter, nextQuarter;
	unsigned long audioSize;
	unsigned long byteAddr;

	twi_setup();
	ins_setup();
	pwr_setup();

	while(1) {
		// Kinda debounce
		_delay_ms(1000);

		// Enable TWI
		ASSERT(TW_NO_ERROR == twi_enable());
		ASSERT(TW_NO_ERROR == rtc_setup());

		// Read hours and minutes
		ASSERT(TW_NO_ERROR == rtc_readTime());
		rtcTime = rtc_getTimePtr();

		// On first run ever, there will be no alarm set and no push
		// button action. So, no need to fast forward (first time only)
		if(firstRun) {
			firstRun = false;
		}
		else {
			// XXX Would be interesting to check what happens if one presses one or more buttons and does not release

			// If PIND2 is 1, it means that alarm has not triggered
			// In this case, we might have woken up due to a button action
			if(PIND & (1 << PIND2)) {
				setTime[0] = 0x00;
				setTime[1] = rtcTime[0];
				setTime[2] = rtcTime[1];

				// If INT0 triggered, advance one hour
				if(ins_0triggered()) {
					ASSERT(TW_NO_ERROR == rtc_setTimeNextHour());
#if 0
					if(0x23 == rtcTime[1])
						setTime[2] = 0x00;
					else if(0x09 == rtcTime[1])
						setTime[2] = 0x10;
					else if(0x19 == rtcTime[1])
						setTime[2] = 0x20;
					else
						setTime[2]++;
#endif
				}
				// If INT1 triggered, advance fifteen minutes
				else if(ins_1triggered()) {
					ASSERT(TW_NO_ERROR == rtc_setTimeNextQuarter());
#if 0
					if(rtcTime[0] < 0x15)
						setTime[1] = 0x15;
					else if(rtcTime[0] < 0x30)
						setTime[1] = 0x30;
					else if(rtcTime[0] < 0x45)
						setTime[1] = 0x45;
					else
						setTime[1] = 0x00;
#endif
				}

#if 0
				// Update RTC (and zero seconds)
				errCode = writeBlock(0x68, 0x00, setVals, 3);
				TWI_CHECK_RETURN_STATUS(errCode);
#endif
			}

			// Disarm alarm in any case
			// It may have triggered normally, or with the modification above
			ASSERT(TW_NO_ERROR == rtc_disarmAlarm());
		}

		// Prepare arguments to select the chimes

		// Convert from BCD to decimal
		hours = (rtcTime[1] & 0xF) + (10 * ((rtcTime[1] >> 4) & 0x3));

		// Convert from 24 to 12 hours (1-12)
		if(!hours) {
			hours12 = 12;
		}
		else if(hours <= 12) {
			hours12 = hours;
		}
		else {
			hours12 = hours - 12;
		}

		// For minute we just need to know which quarter it is. No conversion required
		if(rtcTime[0] < 0x15)
			quarter = 0;
		else if(rtcTime[0] < 0x30)
			quarter = 1;
		else if(rtcTime[0] < 0x45)
			quarter = 2;
		else
			quarter = 3;

		// Prepare sdcard/sound according to the variables above

		// Enable power rail for sdcard/sound
		pwr_enableRail();

		// Give a 1sec break
		_delay_ms(1000);

		// Enable SPI
		spi_setup();

		// Try to initialise SD
		sdRetVal = sd_init(SD_VOLTAGE_2V7_3V6, 10);
		ASSERT(SD_NO_ERROR == sdRetVal || SD_VER_MMC_3_0 == sdRetVal || SD_VER_1_X == sdRetVal || SD_VER_2_0_PLUS_SC == sdRetVal || SD_VER_2_0_PLUS_HCXC == sdRetVal);
#if 0
		int i;
		for(i = 0; i < (hours + 1); i++) {
			DDRD |= (1 << DDD2);
			_delay_ms(1000);
			DDRD &= ~(1 << DDD2);
			_delay_ms(1000);
		}
		_delay_ms(2000);
		for(i = 0; i < quarter + 1; i++) {
			DDRD |= (1 << DDD2);
			_delay_ms(500);
			DDRD &= ~(1 << DDD2);
			_delay_ms(500);
		}
#endif


		// Prepare audio counter, then trigger interrupt just a bit before call to sd_readring
		// Nothing will happen until data start to pour in

		// Get audio index (byte address) and size
		if(!quarter) {
			// Select a full-hour chime
			byteAddr = sdcardIndex[hours12 + 2];
			audioSize = sdcardIndex[hours12 + 3] - byteAddr;
		}
		else {
			// Select a split-hour chime
			byteAddr = sdcardIndex[quarter - 1];
			audioSize = sdcardIndex[quarter] - byteAddr;
		}

		// The full wave files have 5 seconds of silence.
		// Remove approx. 4 seconds of it, rounded to 512 bytes still
		audioSize -= 32256;
 
		sound_setSize(audioSize);

		// Set up counters
		sound_setup();

		// Start stream
		sdRetVal = sd_readring(byteAddr, sound_getRingBuffer(), NULL, sound_getTotalReceivedRef(), sound_getHaltRef(), RING_BUFFER_SIZE / 512, audioSize / 512);
		ASSERT(SD_NO_ERROR == sdRetVal);

		// Wait until sound finishes. The output will be disabled by the interrupt
		while(!sound_isStopped());

		// Reset sound system
		sound_reset();

		// Disable SPI
		spi_disable();

		// Disable sdcard/sound power rail
		pwr_disableRail();

		// Give a 0.5sec break, this allows the gate rail to stabilise at VCC
		// Hopefully avoiding interference with the interrupt/wakeup logic
		_delay_ms(500);

		// Set alarm to next quarter
		switch(quarter) {
			case 0:
				nextQuarter = 0x15;
				break;
			case 1:
				nextQuarter = 0x30;
				break;
			case 2:
				nextQuarter = 0x45;
				break;
			default:
				nextQuarter = 0x00;
				break;
		}

#if 1
		ASSERT(TW_NO_ERROR == rtc_setAlarmMinutes(nextQuarter));
#endif

		cli();

		// Clear the trigger flags
		ins_clear();

		twi_disable();
		ins_prepare();
		pwr_sleepPrepare();

		// Zzzzzzz
		sei();
		sleep_cpu();

		// Welcome back. Disable interrupt for now until we done business
		sleep_disable();
		ins_unprepare();
		cli();
	}

_err:

	// Disable SPI
	spi_disable();
	// Disable sdcard/sound power rail
	pwr_disableRail();
	cli();
	// Clear the trigger flags
	ins_clear();
	twi_disable();

	return 0;
}
