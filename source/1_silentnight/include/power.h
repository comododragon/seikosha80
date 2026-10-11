/**
 * Power-related functions (i.e. for low-power mode).
 */


#ifndef POWER_H
#define POWER_H


// Prepare for ultra low-power mode
void pwr_setup(void);

// Final preparations before sleep
void pwr_sleepPrepare(void);

// Enable sdcard/sound power rail
void pwr_enableRail(void);

// Disable sdcard/sound power rail
void pwr_disableRail(void);


#endif
