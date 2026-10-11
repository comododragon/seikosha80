/**
 * SD-related functions
 */


#include "sd.h"

#include <avr/io.h>
#include <util/delay.h>

#include "spiutils.h"


int _sdVer = SD_VER_1_X;
unsigned int _breathTime = 10;


void sd_cycle() {
	int i;
	for(i = 0; i < _breathTime; i++)
		SD_CYCLE();
}


int sd_transact_crc(unsigned char command, unsigned long arg, unsigned char crc, unsigned char *ret) {
	int i, errVal = SD_NO_ERROR;
	unsigned char retVal;

	// Start transmission (pull SS low)
	PORTB &= ~(1 << PORTB2);

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Send command
	SD_SENDRECV(0x40 | (command & 0x3f), retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_COMMAND;
		goto _err;
	}

	// Send 4-byte argument
	for(i = 3; i >= 0; i--) {
		SD_SENDRECV((arg >> (8 * i)) & 0xFF, retVal);
		// Nothing expected here
		if(retVal != 0xFF) {
			errVal = SD_ERR_TRANSACT_ARG;
			goto _err;
		}
	}

	// Send CRC value
	SD_SENDRECV(crc, retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_CRC;
		goto _err;
	}

	// Keep transmitting clock until something different from 0xFF is returned
	for(i = 0; i < 10; i++) {
		SD_RECV(retVal);
		// If response is different from 0xFF, this is a valid return value
		// We are finished
		if(retVal != 0xFF)
			break;
	}

	// If response was 0xFF all the way, raise error
	if(0xFF == retVal) {
		errVal = SD_ERR_TRANSACT_INCOMPLETE;
		goto _err;
	}

	// Get R1
	ret[0] = retVal;

	// Depending on command, we have to get four more bytes (R3/R7...)
	if(SD_CMD8 == command || SD_CMD58 == command) {
		for(i = 4; i; i--) {
			SD_RECV(retVal);
			ret[i] = retVal;
		}
	}

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

_err:

	// Finish transmission
	PORTB |= (1 << PORTB2);

	return errVal;
}


int sd_transact_command(unsigned char command, unsigned char *ret) {
	unsigned long arg;
	unsigned char crc;

	// Send correct CRC for precalculated commands, to avoid headaches
	// (on SPI mode, only some commands require CRC, like CMD0...
	// but not all SD cards respect this)
	switch(command) {
		case SD_CMD0:
			arg = 0x00UL;
			crc = 0x95;
			break;
		case SD_CMD1:
			arg = 0x00UL;
			crc = 0xF9;
			break;
		case SD_CMD8:
			// This lib only supports 2V7-3V6. So there is only one option possible here
			// And as an echo pattern, we send 0xAA which is the recommended pattern on spec
			// Putting the two values together, gives: 0x1AA
			arg = 0x1AAUL;
			crc = 0x87;
			break;
		case SD_CMD58:
			arg = 0x00UL;
			crc = 0xFD;
			break;
		default:
			// Error: unexpected predefined command
			return SD_ERR_TRANSACT_NOT_PREDEFINED;
	}

	return sd_transact_crc(command, arg, crc, ret);
}


int sd_transact_app(unsigned char command, unsigned long arg, unsigned char *ret) {
	int i, errVal = SD_NO_ERROR;
	unsigned char retVal;

	// Start transmission (pull SS low)
	PORTB &= ~(1 << PORTB2);

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Send CMD55
	SD_SENDRECV(0x40 | (SD_CMD55 & 0x3f), retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_COMMAND;
		goto _err;
	}

	// Send all-zero argument
	for(i = 0; i < 4; i++) {
		SD_SENDRECV(0x00, retVal);
		// Nothing expected here
		if(retVal != 0xFF) {
			errVal = SD_ERR_TRANSACT_ARG;
			goto _err;
		}
	}

	// Send CRC value
	SD_SENDRECV(0x65, retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_CRC;
		goto _err;
	}

	// Keep transmitting clock until something different from 0xFF is returned
	for(i = 0; i < 10; i++) {
		SD_RECV(retVal);
		// If response is different from 0xFF, this is a valid return value
		// We are finished
		if(retVal != 0xFF)
			break;
	}

	// If response was 0xFF all the way, raise error
	if(0xFF == retVal) {
		errVal = SD_ERR_TRANSACT_INCOMPLETE;
		goto _err;
	}

	// Get R1
	ret[0] = retVal;

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Finish first transmission
	PORTB |= (1 << PORTB2);

	// If R1 was different from 0x01, we consider failure
	if(ret[0] != 0x01) {
		errVal = SD_ERR_TRANSACT_APP_INIT;
		goto _err;
	}

	// Start second transmission (pull SS low)
	PORTB &= ~(1 << PORTB2);

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Send ACMD
	SD_SENDRECV(0x40 | (command & 0x3f), retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_APP_COMMAND;
		goto _err;
	}

	// Send 4-byte argument
	for(i = 3; i >= 0; i--) {
		SD_SENDRECV((arg >> (8 * i)) & 0xFF, retVal);
		// Nothing expected here
		if(retVal != 0xFF) {
			errVal = SD_ERR_TRANSACT_APP_ARG;
			goto _err;
		}
	}

	// Send arbitrary CRC value
	SD_SENDRECV(0x01, retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_APP_CRC;
		goto _err;
	}

	// Keep transmitting clock until something different from 0xFF is returned
	for(i = 0; i < 10; i++) {
		SD_RECV(retVal);
		// If response is different from 0xFF, this is a valid return value
		// We are finished
		if(retVal != 0xFF)
			break;
	}

	// If response was 0xFF all the way, raise error
	if(0xFF == retVal) {
		errVal = SD_ERR_TRANSACT_APP_INCOMPLETE;
		goto _err;
	}

	// Get R1
	ret[0] = retVal;

	// Depending on command, we have to get four more bytes (R3/R7...)
	// No APP_CMDS now supported that require this...
	//if(...) {
	//	for(i = 4; i; i--) {
	//		SD_RECV(retVal);
	//		ret[i] = retVal;
	//	}
	//}

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

_err:

	// Finish transmission
	PORTB |= (1 << PORTB2);

	return errVal;
}


int sd_init(unsigned long voltageRange, unsigned int breathTime) {
	int i, errVal = SD_NO_ERROR;
	unsigned char retVal[5];

	// SPI must be enabled beforehand
	if(!(
		(DDRB & (1 << DDB5)) &&
		!(DDRB & (1 << DDB4)) &&
		(DDRB & (1 << DDB3)) &&
		(DDRB & (1 << DDB2)) &&
		(PORTB & (1 << PORTB4)) &&
		(PORTB & (1 << PORTB2)) &&
		(SPCR & (1 << SPE)) &&
		(SPCR & (1 << MSTR))
	)) {
		errVal = SD_ERR_INIT_SPI_MISCONFIG;
		goto _err;
	}
	// Also another check
	if(!spi_isEnabled()) {
		errVal = SD_ERR_INIT_SPI_MISCONFIG;
		goto _err;
	}

	// Reset also SD_VER
	_sdVer = SD_VER_1_X;
	// Set breath time, used between commands to blank cycles
	_breathTime = breathTime;

	// Wait for >= 1ms before starting to communicate with microSD
	_delay_ms(2000);

	// Send >= 74 dummy clocks. Lets round to 80 and that'll be 10 bytes
	for(i = 0; i < 10; i++)
		SD_CYCLE();

	// Send CMD0 to reset SD
	errVal = sd_transact_command(SD_CMD0, retVal);
	if(errVal != SD_NO_ERROR)
		goto _err;
	// Reject SD if R1 was not 0x01
	if(retVal[0] != 0x01) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}

	// Send CMD8 to send voltage range and echo pattern
	errVal = sd_transact_command(SD_CMD8, retVal);
	if(errVal != SD_NO_ERROR)
		goto _err;
	// Only 2.0+ will respond to this. If invalid command is returned,
	// we only assume that this is 1.X/MMC and move on
	if(0x01 == retVal[0]) {
		_sdVer = SD_VER_2_0_PLUS_SC;

		// The transmitted value must have echoed back (i.e. 0x1AA)
		if((retVal[4] != 0x00) || (retVal[3] != 0x00) || (retVal[2] != 0x01) || (retVal[1] != 0xAA)) {
			errVal = SD_ERR_INIT_REJECT;
			goto _err;
		}
	}
	else if(retVal[0] != 0x05) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}

	// Send CMD58 to get OCR registers and check operating voltage range
	errVal = sd_transact_command(SD_CMD58, retVal);
	if(errVal != SD_NO_ERROR)
		goto _err;
	// All cards should respond to this. Otherwise reject
	if(retVal[0] != 0x01) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// For every voltage bit, it CANNOT be set by this call and unset on SD
	// That would mean that the host expects a voltage range that the SD does not
	// SD_VOLTAGE_3V5_3V6
	if((voltageRange & SD_VOLTAGE_3V5_3V6) && !(retVal[3] & (SD_VOLTAGE_3V5_3V6 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_3V4_3V5
	if((voltageRange & SD_VOLTAGE_3V4_3V5) && !(retVal[3] & (SD_VOLTAGE_3V4_3V5 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_3V3_3V4
	if((voltageRange & SD_VOLTAGE_3V3_3V4) && !(retVal[3] & (SD_VOLTAGE_3V3_3V4 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_3V2_3V3
	if((voltageRange & SD_VOLTAGE_3V2_3V3) && !(retVal[3] & (SD_VOLTAGE_3V2_3V3 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_3V1_3V2
	if((voltageRange & SD_VOLTAGE_3V1_3V2) && !(retVal[3] & (SD_VOLTAGE_3V1_3V2 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_3V0_3V1
	if((voltageRange & SD_VOLTAGE_3V0_3V1) && !(retVal[3] & (SD_VOLTAGE_3V0_3V1 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_2V9_3V0
	if((voltageRange & SD_VOLTAGE_2V9_3V0) && !(retVal[3] & (SD_VOLTAGE_2V9_3V0 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_2V8_2V9
	if((voltageRange & SD_VOLTAGE_2V8_2V9) && !(retVal[3] & (SD_VOLTAGE_2V8_2V9 >> 16))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}
	// SD_VOLTAGE_2V7_2V8
	if((voltageRange & SD_VOLTAGE_2V7_2V8) && !(retVal[2] & (SD_VOLTAGE_2V7_2V8 >> 8))) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}

	for(i = 0; i < 10; i++) {
		// Send ACMD41 command
		sd_transact_app(SD_ACMD41, (SD_VER_2_0_PLUS_SC == _sdVer)? 0x40000000L : 0x00L, retVal);
		if(errVal != SD_NO_ERROR)
			goto _err;
		// If R1 is 0x00, initialisation is done
		if(0x00 == retVal[0]) {
			break;
		}
		// If R1 is not 0x01 nor 0x00, this might be an MMC
		else if(retVal[0] != 0x01) {
			_sdVer = SD_VER_MMC_3_0;
			break;
		}

		// If code reached here, R1 was 0x01. We must keep trying
		_delay_ms(1000);
	}

	// If R1 was 0x00, initialisation is done
	// We just need some final steps
	if(0x00 == retVal[0]) {
		// Send CMD58 again to get the CCS bit
		errVal = sd_transact_command(SD_CMD58, retVal);
		if(errVal != SD_NO_ERROR)
			goto _err;
		// All cards should respond to this (now as 0x00). Otherwise reject
		if(retVal[0]) {
			errVal = SD_ERR_INIT_REJECT;
			goto _err;
		}

		// Check CCS bit. If 1, this is SDHC/SDXC, if not, it is SDSC
		if(retVal[4] & (SD_OCR_CCS >> 24))
			_sdVer = SD_VER_2_0_PLUS_HCXC;
	}
	// If R1 was not 0x00 nor 0x01, this might still be an MMC
	else if(SD_VER_MMC_3_0 == _sdVer) {
		// We repeat initialisation, but with CMD1 instead of ACMD41
		for(i = 0; i < 10; i++) {
			// Send ACMD41 command
			sd_transact_command(SD_CMD1, retVal);
			if(errVal != SD_NO_ERROR)
				goto _err;
			// If R1 is 0x00, initialisation is done
			if(0x00 == retVal[0]) {
				break;
			}
			// If R1 is not 0x01 nor 0x00, we are out of options, reject
			else if(retVal[0] != 0x01) {
				errVal = SD_ERR_INIT_REJECT;
				goto _err;
			}

			// If code reached here, R1 was 0x01. We must keep trying
			_delay_ms(1000);
		}

		// If answer was 0x00 or 0x01 but we still timeout'd, reject
		if(0x00 == retVal[0] || 0x01 == retVal[1]) {
			errVal = SD_ERR_INIT_REJECT;
			goto _err;
		}
	}
	// If answer was 0x00 or 0x01 but we still timeout'd, reject
	else {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}

	// At last, send CMD16 to set block size
	// This will have no effect on SDHC/SDXC, but we are setting to the same value
	// for these standards anyways (512 bytes)
	sd_transact_crc(SD_CMD16, 0x200UL, 0x01, retVal);
	if(errVal != SD_NO_ERROR)
		goto _err;
	// Response should be 0x00
	if(retVal[0]) {
		errVal = SD_ERR_INIT_REJECT;
		goto _err;
	}

	// Aaaaand we done! If no problems happened, we return the SD version
	errVal = _sdVer;

_err:

	return errVal;
}


int sd_read(unsigned long byteAddr, unsigned char *buff, unsigned int *crc) {
	int i, errVal = SD_NO_ERROR;
	unsigned char retVal;

	// SDHC/SDXC use block address, all rest use byte address
	unsigned long byteBlockAddr = (SD_VER_2_0_PLUS_HCXC == _sdVer)? (byteAddr >> 9) : byteAddr;

	// Start transmission (pull SS low)
	PORTB &= ~(1 << PORTB2);

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Send command
	SD_SENDRECV(0x40 | (SD_CMD17 & 0x3f), retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_COMMAND;
		goto _err;
	}

	// Send 4-byte argument
	for(i = 3; i >= 0; i--) {
		SD_SENDRECV((byteBlockAddr >> (8 * i)) & 0xFF, retVal);
		// Nothing expected here
		if(retVal != 0xFF) {
			errVal = SD_ERR_TRANSACT_ARG;
			goto _err;
		}
	}

	// Send CRC value
	SD_SENDRECV(0x01, retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_CRC;
		goto _err;
	}

	// Keep transmitting clock until something different than 0xFF is returned
	// An R1 == 0x00 may arrive midpoint as well, we acknowledge and pass by
	// Timeout of SD_READ_TIMEOUT bytes
	for(i = 0; i < SD_READ_TIMEOUT; i++) {
		SD_RECV(retVal);
		if(retVal != 0x00 && retVal != 0xFF)
			break;
	}

	// If response was 0xFF all the way, timeout
	if(0xFF == retVal) {
		errVal = SD_ERR_READ_TIMEOUT;
		goto _err;
	}
	// If response was 0x0X, an error was returned
	else if(0x00 == (retVal & 0xF0)) {
		buff[0] = retVal;
		errVal = SD_ERR_READ;
		goto _err;
	}
	// If response was not 0x0X nor 0xFE (START_BLOCK), this is unexpected
	else if(retVal != 0xFE) {
		errVal = SD_ERR_READ_UNEXPECTED;
		goto _err;
	}

	// Otherwise, read block
	for(i = 0; i < 512; i++) {
		SD_RECV(retVal);
		buff[i] = retVal;
	}

	// Read 16-bit CRC
	*crc = 0;
	SD_RECV(retVal);
	*crc |= (retVal << 8) & 0xFF00;
	SD_RECV(retVal);
	*crc |= retVal & 0x00FF;

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

_err:

	// Finish transmission
	PORTB |= (1 << PORTB2);

	return errVal;
}


int sd_readring(unsigned long byteAddr, unsigned char *buff, unsigned int *crc, unsigned long *recvCnt, volatile bool *extHalt, unsigned long buffBlocksSz, unsigned long totalBlocksSz) {
	int i, errVal = SD_NO_ERROR;
	unsigned char retVal;
	unsigned long totalBlocksCnt;
	unsigned long buffIdx = 0;
	unsigned long internalRecvCnt;
	volatile bool internalHalt = false;

	// SDHC/SDXC use block address, all rest use byte address
	unsigned long byteBlockAddr = (SD_VER_2_0_PLUS_HCXC == _sdVer)? (byteAddr >> 9) : byteAddr;

	// If recvCnt is NULL, we use internalRecvCnt instead
	unsigned long *refRecvCnt = recvCnt? recvCnt : &internalRecvCnt;
	// Reset counter
	*refRecvCnt = 0;
	// If extHalt is NULL, we disable it
	volatile bool *refHalt = extHalt? extHalt : &internalHalt;

	// Start transmission (pull SS low)
	PORTB &= ~(1 << PORTB2);

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

	// Send command
	SD_SENDRECV(0x40 | (SD_CMD18 & 0x3f), retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_COMMAND;
		goto _err;
	}

	// Send 4-byte argument
	for(i = 3; i >= 0; i--) {
		SD_SENDRECV((byteBlockAddr >> (8 * i)) & 0xFF, retVal);
		// Nothing expected here
		if(retVal != 0xFF) {
			errVal = SD_ERR_TRANSACT_ARG;
			goto _err;
		}
	}

	// Send CRC value
	SD_SENDRECV(0x01, retVal);
	// Nothing expected here
	if(retVal != 0xFF) {
		errVal = SD_ERR_TRANSACT_CRC;
		goto _err;
	}

	// Main loop to get all blocks
	for(totalBlocksCnt = 0; totalBlocksCnt < totalBlocksSz; totalBlocksCnt++) {
		// Pause if extHalt is asserted
		//while(*refHalt);

		// Keep transmitting clock until something different than 0xFF is returned
		// An R1 == 0x00 may arrive midpoint as well, we acknowledge and pass by
		// Timeout of SD_READ_TIMEOUT bytes
		for(i = 0; i < SD_READ_TIMEOUT; i++) {
			SD_RECV(retVal);
			if(retVal != 0x00 && retVal != 0xFF)
				break;
		}

		// If response was 0xFF all the way, timeout
		if(0xFF == retVal) {
			errVal = SD_ERR_READ_TIMEOUT;
			goto _err;
		}
		// If response was 0x0X, an error was returned
		else if(0x00 == (retVal & 0xF0)) {
			buff[0] = retVal;
			errVal = SD_ERR_READ;
			goto _err;
		}
		// If response was not 0x0X nor 0xFE (START_BLOCK), this is unexpected
		else if(retVal != 0xFE) {
			errVal = SD_ERR_READ_UNEXPECTED;
			goto _err;
		}

		// The last block has a different treament because SD_CMD12 is sent in parallel
		if(totalBlocksSz - 1 == totalBlocksCnt) {
			// Read block, with SD_CMD12 in parallel on last elements
			for(i = 0; i < 512; i++) {
				// Pause if extHalt is asserted
				while(*refHalt);

				// Send SD_CMD12
				if(508 == i)
					SD_SENDRECV(0x40 | (SD_CMD12 & 0x3f), retVal);
				// Send all zero argument
				else if(i >= 509 && i <= 511)
					SD_SENDRECV(0x00, retVal);
				// Normal part before SD_CMD12
				else
					SD_RECV(retVal);

				buff[buffIdx] = retVal;
				if((buffBlocksSz * 512) - 1 == buffIdx)
					buffIdx = 0;
				else
					buffIdx += 1;

				*refRecvCnt += 1;
			}

			// Only store CRC16 if crc argument is non-NULL, also sending last part of SD_CMD12!
			if(crc) {
				// Read 16-bit CRC
				crc[totalBlocksCnt] = 0;
				// Sending last byte of all-zero argument
				SD_SENDRECV(0x00, retVal);
				crc[totalBlocksCnt] |= (retVal << 8) & 0xFF00;
				// Sending dummy CRC7
				SD_SENDRECV(0x01, retVal);
				crc[totalBlocksCnt] |= retVal & 0x00FF;
			}
			else {
				// Sending last byte of all-zero argument
				SD_SEND(0x00);
				// Sending dummy CRC7
				SD_SEND(0x01);
			}
		}
		// Not last block, act normally
		else {
			// Read block
			for(i = 0; i < 512; i++) {
				// Pause if extHalt is asserted
				while(*refHalt);

				SD_RECV(retVal);
				buff[buffIdx] = retVal;
				if((buffBlocksSz * 512) - 1 == buffIdx)
					buffIdx = 0;
				else
					buffIdx += 1;

				*refRecvCnt += 1;
			}

			// Only store CRC16 if crc argument is non-NULL
			if(crc) {
				// Read 16-bit CRC
				crc[totalBlocksCnt] = 0;
				SD_RECV(retVal);
				crc[totalBlocksCnt] |= (retVal << 8) & 0xFF00;
				SD_RECV(retVal);
				crc[totalBlocksCnt] |= retVal & 0x00FF;
			}
			else {
				SD_CYCLE();
				SD_CYCLE();
			}
		}
	}

	// Now that SD_CMD12 was sent, we wait for an R1 = 0x00 response
	for(i = 0; i < SD_READ_TIMEOUT; i++) {
		SD_RECV(retVal);
		if(0x00 == retVal)
			break;
	}

	// Timeout if timeout (lol)
	if(SD_READ_TIMEOUT - 1 == i) {
		errVal = SD_ERR_READ_TIMEOUT;
		goto _err;
	}

	// And at last, we wait for 0xFF to show up
	for(i = 0; i < SD_READ_TIMEOUT; i++) {
		SD_RECV(retVal);
		if(0xFF == retVal)
			break;
	}

	// Timeout if timeout (lol)
	if(SD_READ_TIMEOUT - 1 == i) {
		errVal = SD_ERR_READ_TIMEOUT;
		goto _err;
	}

	// Cycle some blank bytes. Some slow/old SD cards might need this breath
	// Otherwise, they just get unstable and respond INVALID_COMMAND on
	// unexpected times
	sd_cycle();

_err:

	// Finish transmission
	PORTB |= (1 << PORTB2);

	return errVal;
}
