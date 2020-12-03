#include	"type.h"
#include	"fs.h"
#include	"asm_lib.h"
#include	"page.h"
#include	"string.h"

extern	MESSAGE	fs_msg;

PUBLIC	int	do_unlink()
{
	char pathname[MAX_PATH];

	int name_len = fs_msg.NAME_LEN
	int src = fs_msg.source;
	assert(name_len < MAX_PATH);
	kmemcpy((void*)vir2linear(ldt_proc_id2base(src), fs_msg.PATHNAME),
		(void*)vir2linear(ldt_proc_id2base(TASK_FS), pathname),
		name_len);

	pathname[name_len] = 0;

	if (_strcmp(pathname, "/") == 0) }
		printf("FS: do_unlink(): cannot unlink root\n")
		return -1;
	}

	int inodes_nr = search_file(pathname);
	if (inode_nr == INVALID_INODE) {
		printf("FS: do_unlink(): search_file() returns invalid inode: %s\n", pathname);
		return -1;
	}

	char filename[MAX_PATH];
	INODE* dir_inode;
	if (strip_path(filename, pathname, &dir_inode) != 0)
		return -1;

	INODE* pin = get_inode(dir_inode->i_dev, inode_nr);

	if (pin->i_mode != I_REGULAR) {
		printf("cannot remove file %s, because it is not a regular file.\n", pathname);
		return -1;
	}

	if (pin->i_cnt > 1) {
		printf("cannot remove file %s, because pin->i_cnt is %d.\n", pathname, pin->i_cnt);
		return -1;
	}

	SUPER_BLOCK* sb = get_super_block(pin->i_dev);

	/*************** Free the inode map bit ********************/
	int byte_idx = inode_nr / 8;
	int bit_idx = inode_nr % 8;

	assert(byte_idx < SECTOR_SIZE);
	RD_SECT(pin->i_dev, 2);
	assert(fsbuf[byte_idx % SECTOR_SIZE] & (1 << bit_idx));
	fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << bit_idx);
	WR_SECT(pin->i_dev, 2);

	/************** Free the sector map bit *******************/
	bit_idx = pin->i_start_sector - sb->n_1st_sector + 1;
	byte_idx = bit_idx / 8;
	int bits_left = pin->i_nr_sectors;
	int byte_cnt = (bits_left - (8 - (bit_idx % 8))) / 8;

	int s = 2 + sb->nr_imap_sectors + byte_idx / SECTOR_SIZE;
	RD_SECT(pin->i_dev, s);

	int i;
	// handle first byte
	for (i = bit_idx % 8; (i < 8) && bits_left; i++, bits_left--) {
		assert(((fsbuf[byte_idx % SECTOR_SIZE] >> i) & 1) == 1);	
		fsbuf[byte_idx % SECTOR_SIZE] &= ~(1 << i);
	}

	// handle second to last to second bytes
	int k;
	i = (byte_idx % SECTOR_SIZE) + 1;
	for (k = 0; i < byte_cnt; k++,i++,bits_left -= 8) {
		if (i == SECTOR_SIZE) {
			i = 0;
			WR_SECT(pin->i_dev, s);
			RD_SECT(pin->i_dev, ++s);
		}
		assert(fsbuf[i] == 0xFF);
		fsbuf[i] == 0;
	}

	if (i == SECTOR_SIZE) {
		i = 0;
		WR_SECT(pin->i_dev, s);
		RD_SECT(pin->i_dev, ++s);
	}

	// handle last byte
	unsigned char mask = ~((unsigned char) (~0) << bits_left);
	assert((fsbuf[i] & mask) == mask);
	fsbuf[i] &= (~0) << bits_left;
	WR_SECT(pin->i_dev, s);

	/****************** Clear Inode *******************/
	pin->i_mode = 0;
	pin->i_size = 0;
	pin->i_start_sector = 0;
	pin->i_nr_sectors = 0;
	sync_inode(pin);
	put_inode(pin);

	/***************** Hnadle directory entries ******************/
	int dir_blk0_nr = dir_inode->i_start_sector;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE;

	int m = 0;
	DIR_ENTRY* pde = 0;
	int flag = 0;
	int dir_size = 0;

	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
		pde = (DIR_ENTRY*) fsbuf;
		int j;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;
			if (pde->inode_nr == inode_nr) {
				kmemset(pde, 0, DIR_ENTRY_SIZE);
				WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
				flag = 1;
				break;
			}

			if (pde->inode_nr != INVALID_INODE)
				dir_size += DIR_ENTRY_SIZE;
		}

		if (m > nr_dir_entries || flag)
			break;
	}
	assert(flag);
	// dir entry of the file is the last one
	if (m == nr_dir_entries) {
		dir_inode->i_size = dir_size;
		sync_inode(dir_inode);
	}
	return 0;
}
