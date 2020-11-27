#ifndef	_LAVENDER_HD_H_
#define	_LAVENDER_HD_H_

#define	REG_DEV_CTRL		0x3F6
#define	REG_DATA		0x1F0
#define	REG_FEATURES		0x1F1
#define	REG_NSECTOR		0x1F2
#define	REG_LBA_LOW		0x1F3
#define	REG_LBA_MID		0x1F4
#define	REG_LBA_HIGH		0x1F5
#define	REG_DEVICE		0x1F6
#define	REG_CMD			0x1F7

#define	REG_STATUS		REG_CMD	

#define	STATUS_BUSY		0x80

#define	HD_TIMEOUT		10	// 10ms
#define	ATA_IDENTIFY		0xEC

#define	SECTOR_SIZE		512

#define	MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) | ((drv) << 4) | (lba_highest & 0xF) | 0xA0)

typedef	struct	s_hd_cmd
{
	u8	features;
	u8	count;
	u8	lba_low;
	u8	lba_mid;
	u8	lba_high;
	u8	device;
	u8	command;
} HD_CMD;		 

PUBLIC	void	task_hd();

#endif
