#include	"global.h"
#include	"ipc.h"
#include	"hd.h"
#include	"io.h"
#include	"fs.h"
#include	"string.h"
#include	"asm_lib.h"

#define	RD_SECT(dev, sect_nr) rw_sector(DEV_READ, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, TASK_FS, fsbuf)

#define	WR_SECT(dev, sect_nr) rw_sector(DEV_WRITE, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, TASK_FS, fsbuf)

PRIVATE	void	init_fs();
PRIVATE	void	mkfs();

PUBLIC	void	task_fs()
{
	printf("Task FS starts.\n");
	
	init_fs();
	
	while(1) {}
}

PUBLIC	int	rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr, void* buf)
{
	MESSAGE	driver_msg;
	driver_msg.type = io_type;
	driver_msg.DEVICE = MINOR(dev);
	driver_msg.POSITION = pos;
	driver_msg.BUF = buf;
	driver_msg.CNT = bytes;
	driver_msg.PROC_NR = proc_nr;
	assert(dev_drv_map[MAJOR(dev)].driver_handler != INVALID_DRIVER);

	send_recv(BOTH, dev_drv_map[MAJOR(dev)].driver_handler, &driver_msg);

	return 0;
}

PRIVATE void	init_fs()
{
	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	assert(dev_drv_map[MAJOR(ROOT_DEV)].driver_handler != INVALID_DRIVER);
	send_recv(BOTH, dev_drv_map[MAJOR(ROOT_DEV)].driver_handler, &driver_msg);
	
	mkfs();
} 

PRIVATE	void	mkfs()
{
	MESSAGE driver_msg;
	int i, j;

	int bits_per_sector = SECTOR_SIZE * 8;

	PART_INFO geo;
	driver_msg.type = DEV_IOCTL;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	driver_msg.REQUEST = DIOCTL_GET_GEO;
	driver_msg.BUF = &geo;
	driver_msg.PROC_NR = TASK_FS;
	
	assert(dev_drv_map[MAJOR(ROOT_DEV)].driver_handler != INVALID_DRIVER);
	send_recv(BOTH, dev_drv_map[MAJOR(ROOT_DEV)].driver_handler, &driver_msg);

	printf("dev size: %x sectors\n", geo.size);

	SUPER_BLOCK sb;
	sb.magic = MAGIC_V1;
	sb.nr_inodes = bits_per_sector;
	sb.nr_inode_sectors = sb.nr_inodes * INODE_SIZE / SECTOR_SIZE;
	sb.nr_sectors = geo.size;
	sb.nr_imap_sectors = 1;
	sb.nr_smap_sectors = sb.nr_sectors / bits_per_sector + 1;
	sb.n_1st_sector = 1 + 1 + sb.nr_imap_sectors + sb.nr_smap_sectors + sb.nr_inode_sectors;
	sb.root_inode = ROOT_INODE;
	sb.inode_size = INODE_SIZE;

	sb.inode_isize_off = (int) &((INODE*) 0)->i_size;
	sb.inode_start_off = (int) &((INODE*) 0)->i_start_sector; 
	sb.dir_ent_size = DIR_ENTRY_SIZE;
	
	sb.dir_ent_inode_off = (int) &((DIR_ENTRY*) 0)->inode_nr;
	sb.dir_ent_fname_off = (int) &((DIR_ENTRY*) 0)->name;

	kmemset(fsbuf, 0x90, SECTOR_SIZE);
	kmemcpy(&sb, fsbuf, SUPER_BLOCK_SIZE);

	WR_SECT(ROOT_DEV, 1);

	printf("devbase: %x, superblock: %x, inode map: %x, sector map: %x, inodes: %x, 1st_sector: %x\n",
		geo.base * SECTOR_SIZE,
		(geo.base + 1) * SECTOR_SIZE, 
		(geo.base + 1 + 1) * SECTOR_SIZE,
		(geo.base + 1 + 1 + sb.nr_imap_sectors) * SECTOR_SIZE,
		(geo.base + 1 + 1 + sb.nr_imap_sectors + sb.nr_smap_sectors) * SECTOR_SIZE,
		(geo.base + sb.n_1st_sector) * SECTOR_SIZE);

	kmemset(fsbuf, 0, SECTOR_SIZE);
	for (int i = 0; i < (NR_CONSOLES + 2); i++)
		fsbuf[0] |= (1 << i);

	assert(fsbuf[0] == 0x1F); // 0001 1111
	
	WR_SECT(ROOT_DEV, 2);

	kmemset(fsbuf, 0, SECTOR_SIZE);
	int nr_sectors = NR_DEFAULT_FILE_SECTS + 1;

	for (i = 0; i < nr_sectors / 8; i++)
		fsbuf[i] = 0xFF;

	for (j = 0; j < nr_sectors % 8; j++)
		fsbuf[i] |= (1 << j);

	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sectors);

	kmemset(fsbuf, 0, SECTOR_SIZE);
	for (i = 1; i < sb.nr_smap_sectors; i++)
		WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sectors + i);

	kmemset(fsbuf, 0, SECTOR_SIZE);
	INODE* pi = (INODE*) fsbuf;
	pi->i_mode = I_DIRECTORY;
	pi->i_size = DIR_ENTRY_SIZE * 4;
	pi->i_start_sector = sb.n_1st_sector;
	pi->i_nr_sectors = NR_DEFAULT_FILE_SECTS;

	for (i = 0; i < NR_CONSOLES; i++) {
		pi = (INODE*) (fsbuf + (INODE_SIZE * (i + 1)));
		pi->i_mode = I_CHAR_SPECIAL;
		pi->i_size = 0;
		pi->i_start_sector = MAKE_DEV(DEV_CHAR_TTY, i);
		pi->i_nr_sectors = 0;
	}
	WR_SECT(ROOT_DEV, 2 + sb.nr_imap_sectors + sb.nr_smap_sectors);

	kmemset(fsbuf, 0, SECTOR_SIZE);
	DIR_ENTRY* pde = (DIR_ENTRY*) fsbuf;

	pde->inode_nr = 1;
	_strcpy(pde->name, ".");

	for (i = 0; i < NR_CONSOLES; i++) {
		pde++;
		pde->inode_nr = i + 2;
		sprintf(pde->name, "dev_tty%d", i);
	}
	WR_SECT(ROOT_DEV, sb.n_1st_sector);
}

