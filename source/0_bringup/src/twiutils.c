/**
 * Some TWI-related utilities.
 */


#include "twiutils.h"


bool __twi_enabled = false;


void twi_setup(void) {
	// Set up pins
	DDRC &= ~((1 << DDC4) | (1 << DDC5));
	PORTC &= ~((1 << PORTC4) | (1 << PORTC5));

	// Set TWI clock to 50kHz (SCL = (CPUCLK / (16 + 2*TWBR*PRESCALER)) = 100000 = (8000000 / (16 + 2*TWBR*PRESCALER))
	// Therefore TWBR * PRESCALER = 72. We set PRESCALER to 1 and TWBR to 72 (0x48)
	TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
	TWBR = 0x48;
}


unsigned char twi_enable(void) {
	unsigned char errCode = TW_NO_ERROR;

	// Check and enable TWI
	TWI_CHECK_STATUS(TW_NO_INFO, errCode);
	TWCR = (1 << TWEN);
	__twi_enabled = true;

_err:

	return errCode;
}


void twi_disable(void) {
	TWCR &= ~(1 << TWEN);
	__twi_enabled = false;
}


bool twi_isEnabled(void) {
	return __twi_enabled;
}


unsigned char twi_readByte(unsigned char addr, unsigned char comm, unsigned char *data) {
	unsigned char errCode = TW_NO_ERROR;

	TWI_WAIT_PREPARE();

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_START, errCode);

	TWI_SEND_SLAW(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_SLA_ACK, errCode);

	TWI_SEND_DATA(comm);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_REP_START, errCode);

	TWI_SEND_SLAR(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MR_SLA_ACK, errCode);

	TWI_RECV_DATA_LAST();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MR_DATA_NACK, errCode);
	data[0] = TWDR;

	TWI_SEND_STOP();

_err:

	if(TW_BUS_ERROR == errCode || TW_MT_SLA_NACK == errCode || TW_MR_SLA_NACK == errCode || TW_MT_DATA_NACK == errCode)
		TWI_SEND_STOP();
	else if(TW_MT_ARB_LOST == errCode || TW_MR_ARB_LOST == errCode)
		TWI_CLEAR_TWINT();

	return errCode;
}


unsigned char twi_writeByte(unsigned char addr, unsigned char comm, unsigned char data) {
	unsigned char errCode = TW_NO_ERROR;

	TWI_WAIT_PREPARE();

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_START, errCode);

	TWI_SEND_SLAW(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_SLA_ACK, errCode);

	TWI_SEND_DATA(comm);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);

	TWI_SEND_DATA(data);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);

	TWI_SEND_STOP();

_err:

	if(TW_BUS_ERROR == errCode || TW_MT_SLA_NACK == errCode || TW_MR_SLA_NACK == errCode || TW_MT_DATA_NACK == errCode)
		TWI_SEND_STOP();
	else if(TW_MT_ARB_LOST == errCode || TW_MR_ARB_LOST == errCode)
		TWI_CLEAR_TWINT();

	return errCode;
}


unsigned char twi_readBlock(unsigned char addr, unsigned char comm, unsigned char *data, unsigned int nelem) {
	int i;
	unsigned char errCode = TW_NO_ERROR;

	TWI_WAIT_PREPARE();

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_START, errCode);

	TWI_SEND_SLAW(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_SLA_ACK, errCode);

	TWI_SEND_DATA(comm);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_REP_START, errCode);

	TWI_SEND_SLAR(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MR_SLA_ACK, errCode);

	for(i = 0; i < (nelem - 1); i++) {
		TWI_RECV_DATA();
		TWI_WAIT();
		TWI_CHECK_STATUS(TW_MR_DATA_ACK, errCode);
		data[i] = TWDR;
	}

	if(nelem) {
		TWI_RECV_DATA_LAST();
		TWI_WAIT();
		TWI_CHECK_STATUS(TW_MR_DATA_NACK, errCode);
		data[nelem - 1] = TWDR;
	}

	TWI_SEND_STOP();

_err:

	if(TW_BUS_ERROR == errCode || TW_MT_SLA_NACK == errCode || TW_MR_SLA_NACK == errCode || TW_MT_DATA_NACK == errCode)
		TWI_SEND_STOP();
	else if(TW_MT_ARB_LOST == errCode || TW_MR_ARB_LOST == errCode)
		TWI_CLEAR_TWINT();

	return errCode;
}


unsigned char twi_writeBlock(unsigned char addr, unsigned char comm, unsigned char *data, unsigned int nelem) {
	int i;
	unsigned char errCode = TW_NO_ERROR;

	TWI_WAIT_PREPARE();

	TWI_SEND_START();
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_START, errCode);

	TWI_SEND_SLAW(addr);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_SLA_ACK, errCode);

	TWI_SEND_DATA(comm);
	TWI_WAIT();
	TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);

	for(i = 0; i < nelem; i++) {
		TWI_SEND_DATA(data[i]);
		TWI_WAIT();
		TWI_CHECK_STATUS(TW_MT_DATA_ACK, errCode);
	}

	TWI_SEND_STOP();

_err:

	if(TW_BUS_ERROR == errCode || TW_MT_SLA_NACK == errCode || TW_MR_SLA_NACK == errCode || TW_MT_DATA_NACK == errCode)
		TWI_SEND_STOP();
	else if(TW_MT_ARB_LOST == errCode || TW_MR_ARB_LOST == errCode)
		TWI_CLEAR_TWINT();

	return errCode;
}
