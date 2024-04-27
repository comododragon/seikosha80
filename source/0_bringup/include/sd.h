/**
 * SD-related functions
 */


#ifndef SD_H
#define SD_H


#include <stdbool.h>


// Number of read attempts before giving up
#define SD_READ_TIMEOUT 1000


// SD version constants
#define SD_VER_MMC_3_0       0
#define SD_VER_1_X           1
#define SD_VER_2_0_PLUS_SC   2
#define SD_VER_2_0_PLUS_HCXC 3


// SD voltage range constants, used in SD_CMD58
#define SD_VOLTAGE_2V7_3V6 0x00FF8000UL
#define SD_VOLTAGE_3V5_3V6 0x00800000UL
#define SD_VOLTAGE_3V4_3V5 0x00400000UL
#define SD_VOLTAGE_3V3_3V4 0x00200000UL
#define SD_VOLTAGE_3V2_3V3 0x00100000UL
#define SD_VOLTAGE_3V1_3V2 0x00080000UL
#define SD_VOLTAGE_3V0_3V1 0x00040000UL
#define SD_VOLTAGE_2V9_3V0 0x00020000UL
#define SD_VOLTAGE_2V8_2V9 0x00010000UL
#define SD_VOLTAGE_2V7_2V8 0x00008000UL


// CCS bit, related to high-capacity sdcards
#define SD_OCR_CCS         0x40000000UL


// No error return value
#define SD_NO_ERROR                        0x00


// Errors related to sd_transact()
#define SD_ERR_TRANSACT_COMMAND            0xDE
#define SD_ERR_TRANSACT_ARG                0xDD
#define SD_ERR_TRANSACT_CRC                0xDC
#define SD_ERR_TRANSACT_INCOMPLETE         0xDB
#define SD_ERR_TRANSACT_NOT_PREDEFINED     0xDA


// Errors related to sd_transact_app()
#define SD_ERR_TRANSACT_APP_INIT           0xCF
#define SD_ERR_TRANSACT_APP_COMMAND        0xCE
#define SD_ERR_TRANSACT_APP_ARG            0xCD
#define SD_ERR_TRANSACT_APP_CRC            0xCC
#define SD_ERR_TRANSACT_APP_INCOMPLETE     0xCB
#define SD_ERR_TRANSACT_APP_NOT_PREDEFINED 0xCA


// Errors related to sd_init()
#define SD_ERR_INIT_REJECT                 0x1F
#define SD_ERR_INIT_SPI_MISCONFIG          0x1E


// Errors related to sd_read()
#define SD_ERR_READ_TIMEOUT                0x2F
#define SD_ERR_READ                        0x2E
#define SD_ERR_READ_UNEXPECTED             0x2D


// SD commands. Refer to specification
#define SD_CMD0  0x00
#define SD_CMD1  0x01
#define SD_CMD8  0x08
#define SD_CMD12 0x0C
#define SD_CMD16 0x10
#define SD_CMD17 0x11
#define SD_CMD18 0x12
#define SD_CMD55 0x37
#define SD_CMD58 0x3A


// SD app commands
#define SD_ACMD41 0x29


// Cycle a byte, used when waiting for something to happen
#define SD_CYCLE() do {\
	SPDR = 0xFF;\
	while(!(SPSR & (1 << SPIF)));\
} while(0)


// Send a byte
#define SD_SEND(data) do {\
	/* Start transmission of a byte */\
	SPDR = (data);\
	/* Wait for transmission to complete */\
	while(!(SPSR & (1 << SPIF)));\
} while(0)


// Receive a byte
#define SD_RECV(ret) do {\
	SPDR = 0xFF;\
	while(!(SPSR & (1 << SPIF)));\
	ret = SPDR;\
} while(0)


// Send a byte, while receiving one as well
#define SD_SENDRECV(data, ret) do {\
	/* Start transmission of a byte */\
	SPDR = (data);\
	/* Wait for transmission to complete */\
	while(!(SPSR & (1 << SPIF)));\
	/* Read back received value */\
	ret = SPDR;\
} while(0)


// Global variable that stores which SD version was initialised
extern int _sdVer;
// A breathing time used to give some intervals during commands. Might be needed on SPI mode
extern unsigned int _breathTime;

// Cycle a byte. That is used to send/receive nothing, but to stimulate the sdcard with 8 clock cycles
void sd_cycle();

// Send a normal command
int sd_transact_command(unsigned char command, unsigned char *ret);

// Send an app command
int sd_transact_app(unsigned char command, unsigned long arg, unsigned char *ret);

// Initialise sdcard in SPI mode
int sd_init(unsigned long voltageRange, unsigned int breathTime);

// Read a block of data from sdcard
int sd_read(unsigned long byteAddr, unsigned char *buff, unsigned int *crc);

// Read a block of data, save in a ring buffer
//
// byteAddr: start of block read
// buff: buffer to write data in a ring fashion (size must be buffBlockSz * 512 bytes)
// crc: buffer to write crcs, or NULL if don't care (size must be totalClockSz if non-NULL)
// recvCnt: external place to write the current received total count. If NULL, an internal counter is used
// extHalt: external halt signal. If asserted, transmit is paused until it is deasserted. If NULL, it is ignored
// buffBlocksSz: see buff
// totalBlocksSz: total amount of blocks to be read (512-byte block)
int sd_readring(unsigned long byteAddr, unsigned char *buff, unsigned int *crc, unsigned long *recvCnt, volatile bool *extHalt, unsigned long buffSz, unsigned long totalSz);


#endif
