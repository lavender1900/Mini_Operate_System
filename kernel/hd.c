#include	"type.h"
#include	"const.h"
#include	"process.h"
#include	"ipc.h"
#include	"message.h"
#include	"asm_lib.h"
#include	"interrupts.h"
#include	"clock.h"
#include	"hd.h"
#include	"io.h"
#include	"proto.h"

PRIVATE	void	init_hd();
PRIVATE	void	hd_identify(int drive);
PRIVATE	void	hd_open(int device);
PRIVATE	void	print_identify_info(u16* hdinfo);
PRIVATE	void	hd_cmd_out(HD_CMD* cmd);
PRIVATE	void	interrupt_wait();
PRIVATE	int	waitfor(int mask, int val, int timeout);
PRIVATE	void	partition(int dev, int style);
PRIVATE	void	print_hdinfo(HD_INFO* p);

PRIVATE	u8	hdbuf[256*2];
PRIVATE	u8	hd_status;
PRIVATE	HD_INFO	hd_info[1];

PUBLIC	void	hd_handler(int irq)
{
	hd_status = in_byte(REG_STATUS);
	inform_int(TASK_HD);
}

PUBLIC	void	task_hd()
{
	MESSAGE msg;
	init_hd();
	
	while (1) {
		send_recv(RECEIVE, ANY, &msg);
		int src = msg.source;
		switch (msg.type) {
		case DEV_OPEN:
			hd_open(msg.DEVICE);	
			break;
		default:
			panic("HD driver unknown message type : %d ", msg.type);
			break;
		}
		send_recv(SEND, src, &msg);
	}
}

PRIVATE	void	init_hd()
{
	u8* pNrDrives = (u8*)(0x475);
	printf("NrDrives: %d.\n", *pNrDrives);
	assert(*pNrDrives);

	add_irq_handler(AT_WINI_IRQ, hd_handler);
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);

	for (int i = 0; i < (sizeof(hd_info)/sizeof(HD_INFO)); i++)
		kmemset(&hd_info[i], 0, sizeof(HD_INFO));
}

PRIVATE	void	hd_open(int device)
{
	int drive = DRIVE_OF_DEV(device);
	assert(drive == 0);
	hd_identify(drive);
	if (hd_info[drive].open_cnt++ == 0) {
		partition(drive * (NR_PART_PER_DRIVE + 1), P_PRIMARY);
		print_hdinfo(&hd_info[drive]);
	}
}

PRIVATE	void	get_part_table(int drive, int sect_nr, PART_ENTRY* entry)
{
	HD_CMD	cmd;
	cmd.features = 0;
	cmd.count = 1;
	cmd.lba_low = sect_nr & 0xFF;
	cmd.lba_mid = (sect_nr >> 8) & 0xFF;
	cmd.lba_high = (sect_nr >> 16) & 0xFF;
	cmd.device = MAKE_DEVICE_REG(1, drive, (sect_nr >> 24) & 0xF);
	cmd.command = ATA_READ;

	hd_cmd_out(&cmd);
	interrupt_wait();

	port_read(REG_DATA, hdbuf, SECTOR_SIZE);
	kmemcpy(hdbuf + PARTITION_TABLE_OFFSET, entry, sizeof(PART_ENTRY) * NR_PART_PER_DRIVE); 
}

PRIVATE	void	partition(int device, int style)
{
	int drive = DRIVE_OF_DEV(device);
	HD_INFO* p_hd = &hd_info[drive];

	PART_ENTRY part_table[NR_SUB_PER_DRIVE];
	if (style == P_PRIMARY) {
		get_part_table(drive, drive, part_table);
		int nr_prim_parts = 0;
		for (int i = 0; i < NR_PART_PER_DRIVE; i++) {
			if (part_table[i].sys_id == NO_PART)
				continue;
			nr_prim_parts++;
			int dev_nr = i + 1;
			p_hd->primary[dev_nr].base = part_table[i].lba_start_sector;
			p_hd->primary[dev_nr].size = part_table[i].sector_num;

			if (part_table[i].sys_id == EXT_PART)
				partition(device + dev_nr, P_EXTENDED);
		}
		assert(nr_prim_parts != 0);
	}
	else if (style == P_EXTENDED) {
		int j = device % NR_PRIM_PER_DRIVE;
		int ext_start_sect = p_hd->primary[j].base;
		int s = ext_start_sect;
		int nr_last_sub = (j - 1) * NR_SUB_PER_PART;
		
		for (int i = 0; i < NR_SUB_PER_PART; i++) {
			int dev_nr = nr_last_sub + i;
			get_part_table(drive, s, part_table);
			p_hd->logical[dev_nr].base = s + part_table[0].lba_start_sector;
			p_hd->logical[dev_nr].size = part_table[0].sector_num;

			s = ext_start_sect + part_table[1].lba_start_sector;

			if (part_table[1].sys_id == NO_PART)
				break;
		}
	}
	else {
		assert(0);
	}
}

PRIVATE	void	print_hdinfo(HD_INFO* p_hd)
{
	for (int i = 0; i < NR_PART_PER_DRIVE + 1; i++) {
		printf("%sPART_%d: base %d(0x%x), size %d(0x%x) (in sector)\n",
		i == 0? " " : "     ",
		i,
		p_hd->primary[i].base,
		p_hd->primary[i].base,
		p_hd->primary[i].size,
		p_hd->primary[i].size);
	}

	for (int i = 0; i < NR_SUB_PER_DRIVE; i++) {
		if (p_hd->logical[i].size == 0)
			continue;
		printf("         %d: base %d(%x), size %d(%x) (in sector)\n",
		i,
		p_hd->logical[i].base,
		p_hd->logical[i].base,
		p_hd->logical[i].size,
		p_hd->logical[i].size);	
	}
}

PRIVATE	void	hd_identify(int drive)
{	
	HD_CMD	cmd;
	cmd.device = MAKE_DEVICE_REG(0, drive, 0);
	cmd.command = ATA_IDENTIFY;
	hd_cmd_out(&cmd);
	interrupt_wait();
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);
	print_identify_info((u16*)hdbuf);

	u16* hdinfo = (u16*) hdbuf;
	hd_info[drive].primary[0].base = 0;
	hd_info[drive].primary[0].size = ((int) hdinfo[61] << 16) + hdinfo[60];
}

PRIVATE	void	print_identify_info(u16* hdinfo)
{
	int i, k;
	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char* desc;
	} iinfo[] = {{10, 20, "HD SN"}, {27, 40, "HD Model"}};
	
	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char* p = (char*) &hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2 + 1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printf("%s: %s\n", iinfo[k].desc, s);
	}


	int capabilities = hdinfo[49];
	printf("LBA supported: %s\n", (capabilities & 0x200) ? "Yes" : "No");

	int cmd_set_supported = hdinfo[83];
	printf("LBA48 supported: %s\n", (cmd_set_supported & 0x400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printf("HD size: %dMB\n", sectors * 512 / (1024*1024));
}

PRIVATE	void	hd_cmd_out(HD_CMD* cmd)
{
	if (!waitfor(STATUS_BUSY, 0, HD_TIMEOUT))
		panic("hd error.");

	out_byte(REG_DEV_CTRL, 0);
	out_byte(REG_FEATURES, cmd->features);
	out_byte(REG_NSECTOR, cmd->count);
	out_byte(REG_LBA_LOW, cmd->lba_low);
	out_byte(REG_LBA_MID, cmd->lba_mid);
	out_byte(REG_LBA_HIGH, cmd->lba_high);
	out_byte(REG_DEVICE, cmd->device);
	out_byte(REG_CMD, cmd->command);
}

PRIVATE	void	interrupt_wait()
{
	MESSAGE msg;
	send_recv(RECEIVE, INTERRUPT, &msg);
}

PRIVATE	int	waitfor(int mask, int val, int timeout)
{
	int t = get_ticks();
	while (((get_ticks() - t) * 1000 / TIME_INT_FREQ) < timeout)
		if ((in_byte(REG_STATUS) & mask) == val)
			return 1;
	return 0;
}
