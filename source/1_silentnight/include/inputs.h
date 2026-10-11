/**
 * Functions related to the inputs
 */


#ifndef INPUTS_H
#define INPUTS_H


#include <stdbool.h>


// Initialise inputs
void ins_setup(void);


// Prepare interrupts for inputs
void ins_prepare(void);


// Disable the interrupts configuration
void ins_unprepare(void);


// Return if INT0 was triggered (must clear with ins_clear())
bool ins_triggered(void);


// Clear the trigger flags
void ins_clear(void);


#endif
