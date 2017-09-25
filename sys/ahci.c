#include <sys/defs.h>
#include <sys/ahci.h>
#include "sys/kprintf.h"
#include "sys/pciConfigSpace.h"
#include "sys/memset.h"

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
	int32_t port_num = probe_port(abar);
	kprintf("SATA device found at port: %d\n", port_num);
	
	hba_port_t *port = &(abar->ports[port_num]);

	readWriteAHCI(port);
}

void readWriteAHCI(hba_port_t *port)
{

/*	hba_cmd_list_t *cmdList = port->clb;
	hba_fis_t *fis = port->fb;
	hba_cmd_header_t header0 = cmdList->headers[0];
	hba_cmd_tbl_t cmdTable0 = header0->ctba;
	hba_prdt_entry_t prdt0 = cmdTable0->prdt_entry[0];
*/	
}

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint64_t *buf)
{
	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return -1;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 0;		// Read from device
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 8K bytes (16 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024;	// 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	// 4K words
		count -= 16;	// 16 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_READ_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count = (uint16_t)(count & 0x0000FFFF);
 
	// The below loop waits until the port is no longer busy before issuing a new command
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		kprintf("Port is hung\n");
		return -1;
	}
 
	port->ci = 1<<slot;	// Issue command
 
	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
			break;
		if (port->is_rwc & HBA_PxIS_TFES)	// Task file error
		{
			kprintf("Read disk error\n");
			return -1;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Read disk error\n");
		return -1;
	}
 
	return 1;
}
 
// Find a free command list slot
int find_cmdslot(hba_port_t *port)
{
	// If not set in SACT and CI, the slot is free
	uint32_t slots = (port->sact | port->ci);
	for (int i=0; i<32; i++)
	{
		if ((slots&1) == 0)
			return i;
		slots >>= 1;
	}
	kprintf("Cannot find free command list entry\n");
	return -1;
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
		i++;
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
