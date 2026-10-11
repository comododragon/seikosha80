/**
 * Some TWI-related utilities.
 */


#ifndef TWIUTILS_H
#define TWIUTILS_H


#include <util/twi.h>
#include <stdbool.h>


// The only define "missing" on util/twi.h...
#define TW_NO_ERROR 0xD0


// Check current status of TWI interface
#define TWI_CHECK_STATUS(expectedCode, errCode) do {\
	if((TWSR & TW_NO_INFO) != (expectedCode)) {\
		errCode = TWSR & TW_NO_INFO;\
		goto _err;\
	}\
} while(0)

// Check return status of TWI interface
#define TWI_CHECK_RETURN_STATUS(funcRetVal, errCode) do {\
	if(funcRetVal != TW_NO_ERROR && funcRetVal != TW_NO_INFO) {\
		errCode = funcRetVal;\
		goto _err;\
	}\
} while(0)

// Prepare to start a new transaction
#define TWI_WAIT_PREPARE() while(((TWSR & TW_NO_INFO) != TW_NO_INFO) || (TWCR & (1 << TWSTO)))

// Send START byte
#define TWI_SEND_START() TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN)

// Wait for operation to complete (within a transaction)
#define TWI_WAIT() while(!(TWCR & (1 << TWINT)))

// Send STOP byte
#define TWI_SEND_STOP() TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)

// Send SLA+W
#define TWI_SEND_SLAW(slawAddr) do {\
	TWDR = ((slawAddr) << 1) | TW_WRITE;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
} while(0)

// Clear TWINT flag
#define TWI_CLEAR_TWINT() TWCR = (1 << TWINT) | (1 << TWEN)

// Send data byte
#define TWI_SEND_DATA(mtData) do {\
	TWDR = mtData;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
} while(0)

// Send SLA+R
#define TWI_SEND_SLAR(slarAddr) do {\
	TWDR = ((slarAddr) << 1) | TW_READ;\
	TWCR = (1 << TWINT) | (1 << TWEN);\
} while(0)

// Receive data byte
#define TWI_RECV_DATA() TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA)

// Receive last data byte
#define TWI_RECV_DATA_LAST() TWCR = (1 << TWINT) | (1 << TWEN)


// Initialise TWI
void twi_setup(void);

// Enable TWI
unsigned char twi_enable(void);

// Disable TWI
void twi_disable(void);

// True if TWI was enabled, false otherwise
bool twi_isEnabled(void);

// Read a single byte
unsigned char twi_readByte(unsigned char addr, unsigned char comm, unsigned char *data);

// Write a single byte
unsigned char twi_writeByte(unsigned char addr, unsigned char comm, unsigned char data);

// Read a multi-byte block
unsigned char twi_readBlock(unsigned char addr, unsigned char comm, unsigned char *data, unsigned int nelem);

// Write a multi-byte block
unsigned char twi_writeBlock(unsigned char addr, unsigned char comm, unsigned char *data, unsigned int nelem);


#endif
