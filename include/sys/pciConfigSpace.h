#ifndef _PCICONFIGSPACE_H
#define _PCICONFIGSPACE_H

#include <sys/defs.h>

int enumeratePCI(uint8_t* bus, uint8_t* device);
int checkAllBuses(uint8_t *targetBus, uint8_t *targetDevice);
void checkDevice(uint8_t bus, uint8_t device, uint8_t *deviceFound);
void checkFunction(uint8_t bus, uint8_t device, uint8_t function, uint8_t *deviceFound);
uint8_t getBaseClass(uint16_t classID);
uint8_t getSubClass(uint16_t classID);
uint16_t getVendorID(uint8_t bus, uint8_t device);
uint16_t pciConfigReadWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
uint32_t pciConfigReadWord_32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pciConfigWriteWord_32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void sysOutLong(uint16_t port, uint32_t value);	
uint32_t sysInLong(uint16_t port);

#endif
