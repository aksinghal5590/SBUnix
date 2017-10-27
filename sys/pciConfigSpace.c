#include <sys/defs.h>
#include <sys/kprintf.h>
#include "sys/pciConfigSpace.h"

uint8_t functionFound;

int enumeratePCI(uint8_t* bus, uint8_t* device)
{
   int res = checkAllBuses(bus, device);
   kprintf("Value of Result in enumeratePCI is: %d\n", res);
   return res;
}

int checkAllBuses(uint8_t* targetBus, uint8_t* targetDevice)
{
     uint16_t bus;
     uint16_t device;

     uint8_t deviceFound = 0; 
     for(bus = 0; bus < 256; bus++) {
	 //kprintf("Bus is: %d \n", bus);
         for(device = 0; device < 32; device++) {
             if(!deviceFound)
                  checkDevice(bus, device, &deviceFound);
	     if(deviceFound)
	     {
		  kprintf("Found device at bus: %d and device at: %d\n", bus, device);
                  *targetBus = bus;
		  *targetDevice = device;
		  break;
	     }
         }
         if(deviceFound)
            break;
     }
     if(!deviceFound)
	return 0;
     else
	return 1;
}

void checkDevice(uint8_t bus, uint8_t device, uint8_t *deviceFound) 
{
     uint16_t vendorID = getVendorID(bus, device);
     if(vendorID == 0xFFFF) return;        // Device doesn't exist
     for(uint8_t i = 0; i < 8; i++)
     {
	if(!(*deviceFound))
            checkFunction(bus, device, i, deviceFound);
        if(*deviceFound)
	{
	    functionFound = i; 
	    break;
	}
     }
}
 
void checkFunction(uint8_t bus, uint8_t device, uint8_t function, uint8_t *deviceFound)
{
     uint8_t baseClass;
     uint8_t subClass;
 
     uint16_t classID = pciConfigReadWord(bus, device, function, 0x0A);
     //kprintf("Class ID is: %d \n", classID);
     baseClass = getBaseClass(classID);
     subClass = getSubClass(classID);
     //kprintf("  Class ID is: %d, baseClass is: %d and  subclass id is: %d \n", classID, baseClass, subClass);
     if( (baseClass == 0x01) && (subClass == 0x06) )
     {
	kprintf("Found AHCI device at device number: %d and bus is: %d\n", device, bus);
	kprintf("Found AHCI device is baseclass: %x and subclass: %x\n", baseClass, subClass);
        *deviceFound = 1;
     }
}

uint8_t getBaseClass(uint16_t classID)
{
     uint8_t tmp = 0;
     tmp = (uint8_t)(((classID & 0xFF00) >> 8) & 0xFF);
     return tmp;
}

uint8_t getSubClass(uint16_t classID)
{
     uint8_t tmp = 0;
     tmp = (uint8_t)((classID & 0x00FF) & 0xFF);
     return tmp;
}

uint16_t getVendorID(uint8_t bus, uint8_t device)
{
     return(pciConfigReadWord(bus, device, 0, 0));
}

uint16_t pciConfigReadWord(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)function;
    uint16_t tmp = 0;
 
    address = (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((sysInLong (0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}


uint32_t pciConfigReadWord_32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)function;
    uint32_t tmp = 0;
 
    address = (uint32_t)(1<<31) | (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint32_t)((sysInLong (0xCFC)));
    return (tmp);
}

void pciConfigWriteWord_32(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)function;
     
    address = (uint32_t)(1<<31) | (uint32_t)((lbus << 16) | (ldevice << 11) |
              (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
 
    /* write out the address */
    sysOutLong (0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    sysOutLong (0xCFC, 0xB7000);
}


void sysOutLong(uint16_t port, uint32_t value)
{	
	__asm__ volatile
	(
		"outl %1, %0 \n\t"
		:
		:"Nd"(port) , "a"(value)
	);
}

uint32_t sysInLong(uint16_t port)
{
        uint32_t ret;
        __asm__ volatile
        (
                "inl %1, %0 \n\t"
                :"=a"(ret)
                :"Nd"(port)
        );
	return ret;
}
