/**
 * Interface with DS3231 RTC
 */


#include "rtc.h"

#include "twiutils.h"


unsigned char __rtc_time[2] = {0xFF, 0xFF};


unsigned char rtc_setup(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVal;

	// If TWI is not enabled, abort
	if(!twi_isEnabled())
		return 0x01;

	// Ensure 32kHz output is disabled. It draws a lot of current
	retVal = twi_readByte(0x68, 0x0F, &setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);
	setVal &= ~(1 << 3);
	retVal = twi_writeByte(0x68, 0x0F, setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

_err:

	return errCode;
}


unsigned char rtc_readTime(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;

	// Get hours and minutes only
	retVal = twi_readBlock(0x68, 0x01, __rtc_time, 2);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

_err:

	return errCode;
}


unsigned char *rtc_getTimePtr(void) {
	return __rtc_time;
}


unsigned char rtc_setTime(unsigned char hours, unsigned char minutes) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVals[3];

	// Update RTC (and zero seconds)
	setVals[0] = 0x00;
	setVals[1] = minutes;
	setVals[2] = hours;
	retVal = twi_writeBlock(0x68, 0x00, setVals, 3);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

	// If all went well, update internal pointers as well
	__rtc_time[0] = minutes;
	__rtc_time[1] = hours;

_err:

	return errCode;
}


unsigned char rtc_setTimeNextQuarter(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVals[2] = {0x00, __rtc_time[0]};

	if(setVals[1] < 0x15)
		setVals[1] = 0x15;
	else if(setVals[1] < 0x30)
		setVals[1] = 0x30;
	else if(setVals[1] < 0x45)
		setVals[1] = 0x45;
	else
		setVals[1] = 0x00;

	// Update RTC (update minutes and zero seconds)
	retVal = twi_writeBlock(0x68, 0x00, setVals, 2);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

	// If all went well, update internal pointer as well
	__rtc_time[0] = setVals[1];

_err:

	return errCode;
}


unsigned char rtc_setTimeNextHour(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVals[2] = {0x00, __rtc_time[1]};

	if(0x23 == setVals[1])
		setVals[1] = 0x00;
	else if(0x09 == setVals[1])
		setVals[1] = 0x10;
	else if(0x19 == setVals[1])
		setVals[1] = 0x20;
	else
		setVals[1]++;

	// Update RTC (update hours and zero seconds)
	retVal = twi_writeByte(0x68, 0x00, setVals[0]);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);
	retVal = twi_writeByte(0x68, 0x02, setVals[1]);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

	// If all went well, update internal pointer as well
	__rtc_time[1] = setVals[1];

_err:

	return errCode;
}


unsigned char rtc_disableAlarm(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVal;

	retVal = twi_readByte(0x68, 0x0E, &setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);
	setVal &= ~(1 << 1);
	retVal = twi_writeByte(0x68, 0x0E, setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

_err:

	return errCode;
}

unsigned char rtc_setAlarmMinutes(unsigned char minutes) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVals[4] = {0x00, 0x80, 0x80, 0x00};

	// Retrieve RTC control register
	retVal = twi_readByte(0x68, 0x0E, &setVals[3]);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

	// Set up alarm and interrupt enable
	setVals[3] |= (1 << 2) | (1 << 1);

	// Commit the alarm to RTC
	setVals[0] = minutes;
	retVal = twi_writeBlock(0x68, 0x0B, setVals, 4);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

_err:

	return errCode;
}

unsigned char rtc_resetAlarm(void) {
	unsigned char errCode = TW_NO_ERROR;
	unsigned char retVal;
	unsigned char setVal;

	retVal = twi_readByte(0x68, 0x0F, &setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);
	setVal &= ~(1 << 1);
	retVal = twi_writeByte(0x68, 0x0F, setVal);
	TWI_CHECK_RETURN_STATUS(retVal, errCode);

_err:

	return errCode;
}
