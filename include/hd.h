#ifndef	_LAVENDER_HD_H_
#define	_LAVENDER_HD_H_

#include	"const.h"

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
#define	STATUS_DRQ		0x8

#define	HD_TIMEOUT		500	// 500ms

// hard disk controller command
#define	ATA_IDENTIFY		0xEC
#define	ATA_READ		0x20
#define	ATA_WRITE		0x30

#define	SECTOR_SIZE		512
#define	SECTOR_SIZE_SHIFT	9

#define	P_PRIMARY		0
#define	P_EXTENDED		1
#define	EXT_PART		0x05
#define	NO_PART			0	

#define	MINOR_hd1a		0x10
#define	MINOR_hd2a		(MINOR_hd1a + NR_SUB_PER_PART)
#define	ROOT_DEV		MAKE_DEV(DEV_HD, MINOR_hd2a)	

#define	PARTITION_TABLE_OFFSET	0x1BE

#define	MAX_DRIVES		2	// Use one IDE interface which could connect two hard disks at most
#define	NR_PART_PER_DRIVE	4	// hd1, hd2, hd3, hd4
#define	NR_SUB_PER_PART		16	// at most 16 logical partitions per extended partition
#define	NR_SUB_PER_DRIVE	(NR_SUB_PER_PART * NR_PART_PER_DRIVE)
#define	NR_PRIM_PER_DRIVE	(NR_PART_PER_DRIVE + 1)	// hd0 means whole hard disk
#define	MAX_PRIM		(MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)
#define	MAX_SUBPARTITIONS	(NR_SUB_PER_DRIVE * MAX_DRIVES)

#define	MAKE_DEVICE_REG(lba, drv, lba_highest) (((lba) << 6) | ((drv) << 4) | (lba_highest & 0xF) | 0xA0)
#define	DRIVE_OF_DEV(dev)	(dev <= MAX_PRIM ? \
				dev / NR_PRIM_PER_DRIVE :\
				(dev - MINOR_hd1a) / NR_SUB_PER_DRIVE)


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

typedef	struct	s_part_info
{
	u32	base;	// start sector of partition
	u32	size;	// how many sectors
} PART_INFO;

typedef	struct	s_hd_info
{
	int	open_cnt;
	PART_INFO	primary[NR_PRIM_PER_DRIVE];
	PART_INFO	logical[NR_SUB_PER_DRIVE];
} HD_INFO;

typedef	struct	s_part_entry
{
	u8	status;
	u8	head_start;
	u8	sector_start_cylinder_start_high;
	u8	cylinder_start_low;
	u8	sys_id;
	u8	head_end;
	u8	sector_end_cylinder_end_high;	
	u8	cylinder_end_low;
	u32	lba_start_sector;
	u32	sector_num;
} PART_ENTRY;

PUBLIC	void	task_hd();

#endif
