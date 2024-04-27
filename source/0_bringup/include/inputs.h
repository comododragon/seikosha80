/**
 * Functions related to the inputs
 */


#ifndef INPUTS_H
#define INPUTS_H


#include <stdbool.h>


extern bool __int0_triggered;
extern bool __int1_triggered;


// Initialise inputs
void ins_setup(void);


// Prepare interrupts for inputs
void ins_prepare(void);


// Disable the interrupts configuration
void ins_unprepare(void);


// Return if INT0 was triggered (must clear with ins_clear())
bool ins_0triggered(void);


// Return if INT1 was triggered (must clear with ins_clear())
bool ins_1triggered(void);


// Clear the trigger flags
void ins_clear(void);


#endif
