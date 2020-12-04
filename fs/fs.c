#include	"global.h"
#include	"ipc.h"
#include	"hd.h"
#include	"io.h"
#include	"fs.h"
#include	"string.h"
#include	"asm_lib.h"
#include	"process.h"
#include	"page.h"
#include	"message.h"

PRIVATE	void	init_fs();
PRIVATE	void	mkfs();
PRIVATE	int	do_open();
PRIVATE	int	do_close();
PRIVATE	int	do_rdwt();
PRIVATE	INODE*	create_file(char* path, int flags);
PRIVATE	void	read_super_block();
PRIVATE	int	min(int a, int b);

PUBLIC	MESSAGE	fs_msg;
PRIVATE	PROCESS* pcaller;

PUBLIC	void	task_fs()
{
	
	printf("Task FS starts.\n");
	
	init_fs();
	
	while(1) {
		send_recv(RECEIVE, ANY, &fs_msg);
		int src = fs_msg.source;
		pcaller = &proc_table[src];

		switch (fs_msg.type) {
		case OPEN:
			fs_msg.FD = do_open();
			break;
		case CLOSE:
			fs_msg.RETVAL = do_close();
			break;
		case READ:
		case WRITE:
			fs_msg.CNT = do_rdwt();
			break;
		}


		fs_msg.type = SYSCALL_RET;
		send_recv(SEND, src, &fs_msg);
	}
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
	int i;
	for (i = 0; i < NR_FILE_DESC; i++)
		kmemset(&f_desc_table[i], 0, sizeof(FILE_DESC));

	for (i = 0; i < NR_INODE; i++)
		kmemset(&inode_table[i], 0, sizeof(INODE));

	SUPER_BLOCK* sb = super_block;
	for (; sb < &super_block[NR_SUPER_BLOCK]; sb++)
		sb->sb_dev = NO_DEV;

	MESSAGE driver_msg;
	driver_msg.type = DEV_OPEN;
	driver_msg.DEVICE = MINOR(ROOT_DEV);
	assert(dev_drv_map[MAJOR(ROOT_DEV)].driver_handler != INVALID_DRIVER);
	send_recv(BOTH, dev_drv_map[MAJOR(ROOT_DEV)].driver_handler, &driver_msg);
	
	mkfs();
	read_super_block(ROOT_DEV);

	sb = get_super_block(ROOT_DEV);
	assert(sb->magic == MAGIC_V1);

	root_inode = get_inode(ROOT_DEV, ROOT_INODE);
}

PRIVATE	int	do_open()
{
	int fd = -1;

	char pathname[MAX_PATH];

	int flags = fs_msg.FLAGS;
	int name_len = fs_msg.NAME_LEN;
	int src = fs_msg.source;
	assert(name_len < MAX_PATH);

	kmemcpy((void*) vir2linear(ldt_proc_id2base(src), fs_msg.PATHNAME),
		(void*) vir2linear(ldt_proc_id2base(TASK_FS), pathname),
		name_len);	

	pathname[name_len] = 0;

	int i;
	for (i = 0; i < NR_FILES; i++) {
		if (pcaller->filp[i] == 0) {
			fd = i;
			break;
		}
	}

	if ((fd < 0) || (fd >= NR_FILES))
		panic("filp[] is full (PID: %d)", proc2pid(pcaller));

	for (i = 0; i < NR_FILE_DESC; i++)
		if (f_desc_table[i].fd_inode == 0)
			break;
	if (i >= NR_FILE_DESC)
		panic("f_desc_table[] is full (PID: %d)", proc2pid(pcaller));

	int inode_nr = search_file(pathname);

	INODE* pin = 0;
	if (flags & O_CREAT) {
		if (inode_nr) {
			printf("file exists.\n");
			return -1;
		}
		else {
			pin = create_file(pathname, flags);
		}
	}
	else {
		assert(flags & O_RDWR);

		char filename[MAX_PATH];
		INODE* dir_inode;
		if (strip_path(filename, pathname, &dir_inode) != 0)
			return -1;
		pin = get_inode(dir_inode->i_dev, inode_nr);
	}

	if (pin) {
		pcaller->filp[fd] = &f_desc_table[i];
		f_desc_table[i].fd_inode = pin;
		f_desc_table[i].fd_mode = flags;
		f_desc_table[i].fd_pos = 0;

		int imode = pin->i_mode & I_TYPE_MASK;

		if (imode == I_CHAR_SPECIAL) {
			MESSAGE driver_msg;
			driver_msg.type = DEV_OPEN;
			int dev = pin->i_start_sector;
			driver_msg.DEVICE = MINOR(dev);
			assert(MAJOR(dev) == 4);
			assert(dev_drv_map[MAJOR(dev)].driver_handler != INVALID_DRIVER);

			send_recv(BOTH, dev_drv_map[MAJOR(dev)].driver_handler, &driver_msg);
		}
		else if (imode == I_DIRECTORY) {
			assert(pin->i_num == ROOT_INODE);
		}
		else {
			assert(pin->i_mode == I_REGULAR);
		}
	}
	else {
		return -1;
	}

	return fd;
}

PRIVATE	int	do_close()
{
	int fd = fs_msg.FD;
	put_inode(pcaller->filp[fd]->fd_inode);
	pcaller->filp[fd]->fd_inode = 0;
	pcaller->filp[fd] = 0;

	return 0;
}

PRIVATE	int	do_rdwt()
{
	int fd = fs_msg.FD;
	void* buf = fs_msg.BUF;
	int len = fs_msg.CNT;

	int src = fs_msg.source;

	assert((pcaller->filp[fd] >= &f_desc_table[0]) && (pcaller->filp[fd] < &f_desc_table[NR_FILE_DESC]));

	if (!(pcaller->filp[fd]->fd_mode & O_RDWR))
		return -1;

	int pos = pcaller->filp[fd]->fd_pos;
	INODE* pin = pcaller->filp[fd]->fd_inode;
	assert(pin >= &inode_table[0] && pin < &inode_table[NR_INODE]);

	int imode = pin->i_mode & I_TYPE_MASK;
	if (imode == I_CHAR_SPECIAL) {
		int t = fs_msg.type == READ ? DEV_READ : DEV_WRITE;
		fs_msg.type = t;

		int dev = pin->i_start_sector;
		assert(MAJOR(dev) == TASK_TTY);

		fs_msg.DEVICE = MINOR(dev);
		fs_msg.BUF = buf;
		fs_msg.CNT = len;
		fs_msg.PROC_NR = src;
		assert(dev_drv_map[MAJOR(dev)].driver_handler != INVALID_DRIVER);
		send_recv(BOTH, dev_drv_map[MAJOR(dev)].driver_handler, &fs_msg);
		assert(fs_msg.CNT == len);

		return fs_msg.CNT;
	}
	else {
		assert(pin->i_mode == I_REGULAR || pin->i_mode == I_DIRECTORY);
		assert((fs_msg.type == READ) || (fs_msg.type == WRITE));

		int pos_end;
		if (fs_msg.type == READ)
			pos_end = min(pos + len, pin->i_size);
		else
			pos_end = min(pos + len, pin->i_nr_sectors * SECTOR_SIZE);

		int off = pos % SECTOR_SIZE;
		int rw_sect_min = pin->i_start_sector + (pos >> SECTOR_SIZE_SHIFT);
		int rw_sect_max = pin->i_start_sector + (pos_end >> SECTOR_SIZE_SHIFT);

		int chunk = min(rw_sect_max - rw_sect_min + 1, FSBUF_SIZE >> SECTOR_SIZE_SHIFT);

		int bytes_rw = 0;
		int bytes_left = len;
		int i;
		for (i = rw_sect_min; i <= rw_sect_max; i+= chunk) {
			int bytes = min(bytes_left, chunk * SECTOR_SIZE - off);
			rw_sector(DEV_READ, pin->i_dev, i * SECTOR_SIZE, chunk * SECTOR_SIZE, TASK_FS, fsbuf);

			if (fs_msg.type == READ) {
				kmemcpy((void*)vir2linear(ldt_proc_id2base(TASK_FS), fsbuf + off),
					(void*)vir2linear(ldt_proc_id2base(src), buf + bytes_rw),
					bytes);
			}
			else {
				kmemcpy((void*)vir2linear(ldt_proc_id2base(src), buf + bytes_rw),
					(void*)vir2linear(ldt_proc_id2base(TASK_FS), fsbuf + off),
					bytes);

				rw_sector(DEV_WRITE, pin->i_dev, i * SECTOR_SIZE, chunk * SECTOR_SIZE, TASK_FS, fsbuf);
			}
			off = 0;
			bytes_rw += bytes;
			pcaller->filp[fd]->fd_pos += bytes;
			bytes_left -= bytes;
		}

		if (pcaller->filp[fd]->fd_pos > pin->i_size) {
			pin->i_size = pcaller->filp[fd]->fd_pos;
			sync_inode(pin);
		}
		
		return bytes_rw;
	}
}

PRIVATE	INODE*	create_file(char* path, int flags)
{
	char filename[MAX_PATH];
	INODE* dir_inode;


	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;
	int inode_nr = alloc_imap_bit(dir_inode->i_dev);
	int free_sect_nr = alloc_smap_bit(dir_inode->i_dev, NR_DEFAULT_FILE_SECTS);

	INODE*	newino = new_inode(dir_inode->i_dev, inode_nr, free_sect_nr);
	new_dir_entry(dir_inode, newino->i_num, filename);
	
	return newino;
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

	/******************** Write Super Block to the disk **********************/
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

	/*********************** Write Inode Map to the disk *************************/
	kmemset(fsbuf, 0, SECTOR_SIZE);
	for (int i = 0; i < (NR_CONSOLES + 2); i++)
		fsbuf[0] |= (1 << i);

	assert(fsbuf[0] == 0x1F); // 0001 1111	first inode not used, root dir, tty[0-3]
	
	WR_SECT(ROOT_DEV, 2);


	/********************* Write Sector Map to the disk *************************/
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

	/******************** Write Inode Array to the disk *************************/

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

PUBLIC	SUPER_BLOCK*	get_super_block(int dev)
{
	SUPER_BLOCK* sb = super_block;
	for(; sb < &super_block[NR_SUPER_BLOCK]; sb++)
		if (sb->sb_dev == dev)
			return sb;

	panic("super block of device %d not found.\n", dev);

	return 0;
}

PRIVATE	void	read_super_block(int dev)
{
	int i;
	MESSAGE driver_msg;
	driver_msg.type = DEV_READ;
	driver_msg.DEVICE = MINOR(dev);
	driver_msg.POSITION = SECTOR_SIZE * 1;
	driver_msg.BUF = fsbuf;
	driver_msg.CNT = SECTOR_SIZE;
	driver_msg.PROC_NR = TASK_FS;
	
	assert(dev_drv_map[MAJOR(dev)].driver_handler != INVALID_DRIVER);
	send_recv(BOTH, dev_drv_map[MAJOR(dev)].driver_handler, &driver_msg);

	for (i = 0; i < NR_SUPER_BLOCK; i++)
		if (super_block[i].sb_dev == NO_DEV)
			break;

	if (i == NR_SUPER_BLOCK)
		panic("super block slots used up");

	assert(i == 0);

	SUPER_BLOCK* psb = (SUPER_BLOCK*) fsbuf;
	super_block[i] = *psb;
	super_block[i].sb_dev = dev;
}

PRIVATE	int	min(int a, int b)
{
	return a < b ? a : b;
}
