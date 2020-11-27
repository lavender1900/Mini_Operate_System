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
PRIVATE	void	print_identify_info(u16* hdinfo);
PRIVATE	void	hd_cmd_out(HD_CMD* cmd);
PRIVATE	void	interrupt_wait();
PRIVATE	int	waitfor(int mask, int val, int timeout);

PRIVATE	u8	hdbuf[256*2];
PRIVATE	u8	hd_status;

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
			hd_identify(0);
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
	printf("sssdsfs");
	printf("NrDrives: %d.\n", *pNrDrives);
	assert(*pNrDrives);

	add_irq_handler(AT_WINI_IRQ, hd_handler);
	enable_irq(CASCADE_IRQ);
	enable_irq(AT_WINI_IRQ);
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
		for (int i = 0; i < iinfo[k].len/2; i++) {
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
