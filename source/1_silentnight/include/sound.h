/**
 * Sound-related functions
 */


#ifndef SOUND_H
#define SOUND_H


#include <stdbool.h>


// Sample rate of the chimes
#define SAMPLE_RATE           10000UL
// Audio ring buffer size
#define RING_BUFFER_SIZE      1024UL
// Upper limit to hold sdcard read
#define RECV_PAUSE_THRESHOLD   800UL
// Lower limit to resume sdcard read
#define RECV_RESUME_THRESHOLD  300UL


// Initialise the counters
void sound_setup(void);

// Stop counters, reset everything
void sound_reset(void);

// Set audio size
void sound_setSize(unsigned long size);

// Get audio ring buffer reference
unsigned char *sound_getRingBuffer(void);

// Get reference for totalReceived counter
unsigned long *sound_getTotalReceivedRef(void);

// Get reference for halt flag
volatile bool *sound_getHaltRef(void);

// True if sound is stopped
volatile bool sound_isStopped(void);


#endif
