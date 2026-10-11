/**
 * Sound-related functions
 */


#include "sound.h"

#include <avr/interrupt.h>
#include <avr/io.h>


// The so famous audio ring buffer
unsigned char audioRingBuffer[RING_BUFFER_SIZE];
// From interrupt to sdcard read: flag to halt sdcard read
volatile bool halt = false;
// Indicator of how much was played already
volatile unsigned long totalPlayed = 0;
// Indicator of how much was read from sdcard
unsigned long totalReceived = 0; 
// Audio size
unsigned long audioSize;
// Sample count
volatile unsigned long sample;


void _sound_start(void) {
	// Drive clkio with no prescaling
	TCCR0B = (TCCR0B & ~((1 << CS02) | (1 << CS01))) | (1 << CS00);

	// Set D6 as output
	DDRD |= (1 << DDD6);
}


void _sound_stop(void) {
	// Stop clock
	TCCR0B &= ~(1 << CS00);

	// Disable D6
	DDRD &= ~(1 << DDD6);

	// Zero counter
	OCR0A = 0;
}


// Interrupt-related variables
volatile bool stopped = true;


// Interrupt handler for the audio sampler
// That will be at 8kHz
ISR(TIMER1_COMPA_vect) {
	// Do nothing if totalReceived is still zero
	if(totalReceived) {
		if(stopped) {
			stopped = false;
			_sound_start();
		}
		else {
			if(sample >= audioSize) {
				_sound_stop();
				stopped = true;
			}
			OCR0A = audioRingBuffer[sample % RING_BUFFER_SIZE];

			++sample;
			++totalPlayed;

			// If stream is halted, wait a bit to do some hysteresis
			if(halt) {
				if(totalReceived < (totalPlayed + RECV_RESUME_THRESHOLD))
					halt = false;
			}
			// If stream is becoming too distant from played position, halt receive
			else {
				if(totalReceived >= (totalPlayed + RECV_PAUSE_THRESHOLD))
					halt = true;
			}
		}
	}
}


void sound_setup(void) {
	// Enable output
	DDRD |= (1 << DDD6);

	// Setup counters

	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B &= ~(1 << WGM02);

	TCCR0A = (TCCR0A | (1 << COM0A1)) & ~(1 << COM0A0);
	TCCR0A &= ~((1 << COM0B1) | (1 << COM0B0));
	OCR0A = audioRingBuffer[0];

	cli();

	TCCR1B = (TCCR1B & ~(1 << WGM13)) | (1 << WGM12);
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));

	TCCR1B = (TCCR1B & ~((1 << CS12) | (1 << CS11))) | (1 << CS10);

	OCR1A = F_CPU / SAMPLE_RATE;

	TIMSK1 |= (1 << OCIE1A);

	totalPlayed = 0;
	sample = 0;

	sei();
}


void sound_reset(void) {
	// Stop counters
	_sound_stop();
	TCCR1B &= ~(1 << CS10);


	halt = false;
	totalPlayed = 0;
	totalReceived = 0;
	sample = 0;
	stopped = true;
}


void sound_setSize(unsigned long size) {
	audioSize = size;
}


unsigned char *sound_getRingBuffer(void) {
	return audioRingBuffer;
}


unsigned long *sound_getTotalReceivedRef(void) {
	return &totalReceived;
}


volatile bool *sound_getHaltRef(void) {
	return &halt;
}


volatile bool sound_isStopped(void) {
	return stopped;
}
