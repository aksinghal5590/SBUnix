#include <sys/defs.h>
#include <sys/ahci.h>
#include "sys/kprintf.h"
#include "sys/pciConfigSpace.h"

static int check_type(hba_port_t *port);

extern uint8_t functionFound;

void performAHCITask()
{
	uint8_t bus, device;
	int res = findAHCIDevice(&bus, &device);
	if(res)
	    findDeviceAttached(bus, device);
}

int findAHCIDevice(uint8_t *bus, uint8_t *device)
{
	return(enumeratePCI(bus, device));
}

void findDeviceAttached(uint8_t bus, uint8_t device)
{	
	hba_mem_t* abar;
	//uint16_t bar5LW, bar5HW;
	//uint16_t bar5LW = pciConfigReadWord(bus, device, 0, 0x24);
        //uint16_t bar5HW = pciConfigReadWord(bus, device, 0, 0x26);
      	//uint32_t bar5 = (uint32_t)((((bar5HW & ((uint32_t)0x00000FFF)) << 16) & ((uint32_t) 0xFFFF0000)) | (((uint32_t) 0x0000FFFF) & bar5LW));
	uint32_t bar5 = pciConfigReadWord_32(bus, device, functionFound, 0x24);
//	kprintf("BAR5 address is: %x  and BAR5LW is: %x and BAR5HW is:%x \n", bar5, bar5LW, bar5HW);
	kprintf("BAR5 is: %x\n", bar5);
	kprintf("BAR5 address is: %x and bus is: %d and device is: %d\n", bar5, bus, device);
	pciConfigWriteWord_32(bus, device, functionFound, 0x24);
	//sysOutLong(0xCFC, 0xB7000);
	bar5 = pciConfigReadWord_32(bus, device, functionFound, 0x24);
	//bar5 =  bar5 & 0xB7000;
	kprintf("After BAR5 is: %x\n", bar5);
	kprintf("After BAR5 address is: %x and bus is: %d and device is: %d\n", bar5, bus, device);
	sysOutLong(0xCFC, bar5);
	abar = (hba_mem_t*)((uint64_t)bar5);
	int32_t port_num = probe_port(abar);
	kprintf("SATA device found at port: %d\n", port_num);

	abar->ghc=(uint32_t)(1<<31);
        abar->ghc=(uint32_t)(1<<0);
	abar->ghc=(uint32_t)(1<<31);
        abar->ghc=(uint32_t)(1<<1);

	uint32_t *write_buf = (uint32_t*)0x5000000;
	uint32_t *read_buf = (uint32_t*)0x6000000;

	hba_port_t *port = &(abar->ports[port_num]);

	port_rebase(port, port_num);
	for(int i = 0; i < 100; i++) {

		writeBlock((char*)write_buf, (char)i);
        	write(port, i, 0, 8, write_buf);
		write_buf += 512;

		read(port, i, 0, 8, read_buf);
		kprintf("===== Data at Block %d:\n", i);
		readBlock((char*)read_buf);
		kprintf("\n");
		read_buf += 512;
	}
}

void writeBlock(char *writeBuf, char c) {
	for(int i = 0; i < 4*1024; i++) {
		*(writeBuf+i) = c;
	}
}

void readBlock(char *readBuf) {
	for(int i = 0; i < 4*1024; i++) {
		kprintf("%d ", *(readBuf + i));
	}
}

int read(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf)
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
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 4K bytes (8 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 4*1024;	// 4K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 512;	// 4K bytes/8
		count -= 8;	// 8 sectors
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
 
	cmdfis->count = count;
 
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

int write(hba_port_t *port, uint32_t startl, uint32_t starth, uint32_t count, uint32_t *buf) {

	port->is_rwc = (uint32_t)-1;		// Clear pending interrupt bits
	int spin = 0; // Spin lock timeout counter
	int slot = find_cmdslot(port);
	if (slot == -1)
		return -1;
 
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);	// Command FIS size
	cmdheader->w = 1;		// Write to device
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (uint32_t)((count-1)>>4) + 1;	// PRDT entries count
 
	hba_cmd_tbl_t *cmdtbl = (hba_cmd_tbl_t*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(hba_cmd_tbl_t) +
 		(cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
 
	// 4K bytes (8 sectors) per PRDT
	int i;
	for (i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
		cmdtbl->prdt_entry[i].dbc = 4*1024;	// 4K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 512;	// 4k bytes/8
		count -= 8;	// 8 sectors
	}
	// Last entry
	cmdtbl->prdt_entry[i].dba = (uint64_t)buf;
	cmdtbl->prdt_entry[i].dbc = count<<9;	// 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;
 
	// Setup command
	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*)(&cmdtbl->cfis);
 
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	// Command
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;
 
	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode
 
	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);
 
	cmdfis->count = count;
 
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
			kprintf("Write disk error1\n");
			return -1;
		}
	}
 
	// Check again
	if (port->is_rwc & HBA_PxIS_TFES)
	{
		kprintf("Write disk error2\n");
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
			int dt = check_type(&abar->ports[i]);
			if (dt == AHCI_DEV_SATA)
			{
				kprintf("SATA drive found at port %d\n", i);
				sataPort = i;
				break;	
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
 	/*uint32_t ssts = port->ssts;
	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;
 
	if (det != HBA_PORT_DET_PRESENT)	// Check drive status
		return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
 */
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
 
void port_rebase(hba_port_t *port, int portno)
{
	stop_cmd(port);	// Stop command engine

	port->cmd=port->cmd & 0xffff7fff; //Bit 15
        port->cmd=port->cmd & 0xffffbfff; //Bit 14
        port->cmd=port->cmd & 0xfffffffe; //Bit 0
	port->cmd=port->cmd & 0xfffffff7; //Bit 4
 
	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno<<10);
	//port->clb = 0;
	memset((void*)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32<<10) + (portno<<8);
	//port->fb = 0;
	memset((void*)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*)(port->clb);
	for (int i=0; i<32; i++)
	{
		cmdheader[i].prdtl = 8;	// 8 prdt entries per command table
					// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba = AHCI_BASE + (40<<10) + (portno<<13) + (i<<8);
		//cmdheader[i].ctbau = 0;
		memset((void*)cmdheader[i].ctba, 0, 256);
	}
 
	start_cmd(port);	// Start command engine
	port->is_rwc = 0;
        port->ie = 0xffffffff;

	port->sctl = 0x301;
	int timer = 10000000;
	while(timer--);
	port->sctl = 0x300;
	port->cmd |= 0x10000006;
	port->serr_rwc = 0xffffffff;
	port->is_rwc = 0xffffffff;
}
 
// Start command engine
void start_cmd(hba_port_t *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);
 
	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}
 
// Stop command engine
void stop_cmd(hba_port_t *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;
	// Clear FRE (bit4)
        port->cmd &= ~HBA_PxCMD_FRE;
 
	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}
