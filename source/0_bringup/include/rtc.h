/**
 * Interface with DS3231 RTC
 */


#ifndef RTC_H
#define RTC_H


// Initialise RTC
unsigned char rtc_setup(void);

// Read time and save to internal pointer
unsigned char rtc_readTime(void);

// Get internal time pointer. Returns a 2-byte array in BCD format ([1] is hour, [0] is minute)
unsigned char *rtc_getTimePtr(void);

// Set time on both internal pointer and DS32321. BCD format
unsigned char rtc_setTime(unsigned char hours, unsigned char minutes);

// Set time to next quarter. Hours are not changed. Both internal pointer and DS3231 are set
unsigned char rtc_setTimeNextQuarter(void);

// Set time to next hour. Minutes are not changed. Both internal pointer and DS3231 are set
unsigned char rtc_setTimeNextHour(void);

// Disable alarm
unsigned char rtc_disableAlarm(void);

// Set alarm for a given minute, BCD format
unsigned char rtc_setAlarmMinutes(unsigned char minutes);

// Disarm alarm
unsigned char rtc_disarmAlarm(void);


#endif
