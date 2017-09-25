#include <sys/defs.h>
#include <sys/ahci.h>
#include "sys/kprintf.h"
#include "sys/pciConfigSpace.h"

static int check_type(hba_port_t *port);

void performAHCITask()
{
	uint8_t bus, device;
	findAHCIDevice(&bus, &device);
	findDeviceAttached(bus, device);
}

void findAHCIDevice(uint8_t *bus, uint8_t *device)
{
	enumeratePCI(bus, device);
}

void findDeviceAttached(uint8_t bus, uint8_t device)
{	
	hba_mem_t* abar;
	uint16_t bar5LW = pciConfigReadWord(bus, device, 0, 0x24);
        uint16_t bar5HW = pciConfigReadWord(bus, device, 0, 0x26);
      	uint32_t bar5 = (uint32_t)((((bar5HW & ((uint32_t)0x00000FFF)) << 16) & ((uint32_t) 0xFFFF0000)) | (((uint32_t) 0x0000FFFF) & bar5LW));
	kprintf("BAR5 address is: %x  and BAR5LW is: %x and BAR5HW is:%x \n", bar5, bar5LW, bar5HW);
	kprintf("BAR5 address is: %x and bus is: %d and device is: %d\n", bar5, bus, device);
	
	sysOutLong(0xCFC, bar5);
	uint64_t ptr = (uint64_t)bar5;
	uint32_t* ptr2;
	ptr2 = (uint32_t*)ptr;
	abar = (hba_mem_t*)ptr2;
	//populateHBA_MEM(bar5);
	int32_t port = probe_port(abar);
	kprintf("SATA device found at port: %d\n", port);
}

void populateHBA_MEM(uint32_t bar5)
{
	uint64_t ptr = (uint64_t)(bar5 + 0x0C);
	uint32_t* ptr2;
	ptr2 = (uint32_t*)ptr;
	uint32_t val = *ptr2;
	kprintf("Val of pi is: %x\n", val);
	//abar.pi = val;
}

int32_t probe_port(hba_mem_t *abar)
{
	// Search disk in impelemented ports
	uint32_t pi = abar->pi;
	int32_t sataPort = -1;
	kprintf("Value of pi in probe_prt is: %x\n", pi);
	int i = 0;
	while (i<32)
	{
		if (pi & 1)
		{
			//abar->port[i] = abar + 0x100 + (i*0x7F);
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
				sataPort = i;		
			}
			else if (dt == AHCI_DEV_SATAPI)
			{
				kprintf("SATAPI drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				kprintf("SEMB drive found at port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				kprintf("PM drive found at port %d\n", i);
			}
			else
			{
				kprintf("No drive found at port %d\n", i);
			}
		}
 
		pi >>= 1;
		i ++;
	}
	return sataPort;
}
 
// Check device type
static int check_type(hba_port_t *port)
{
	//uint32_t ssts = (port+0x28);
 	uint32_t ssts = port->ssts;
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 
	//uint32_t sig = port+0x24;
	//port->sig = sig;
	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		return AHCI_DEV_SATAPI;
	case SATA_SIG_SEMB:
		return AHCI_DEV_SEMB;
	case SATA_SIG_PM:
		return AHCI_DEV_PM;
	default:
		return AHCI_DEV_SATA;
	}
}
