#include	"type.h"
#include	"const.h"
#include	"fs.h"
#include	"hd.h"
#include	"message.h"
#include	"ipc.h"
#include	"global.h"
#include	"asm_lib.h"
#include	"string.h"

PUBLIC	int	alloc_imap_bit(int dev)
{
	int inode_nr = 0;
	int i, j, k;

	int imap_blk0_nr = 1 + 1; // 1 boot sector + 1 super block
	SUPER_BLOCK* sb = get_super_block(dev);

	for (i = 0; i < sb->nr_imap_sectors; i++) {
		RD_SECT(dev, imap_blk0_nr + i);
		for (j = 0; j < SECTOR_SIZE; j++) {
			if (fsbuf[j] == 0xFF)
				continue;

			for (k = 0; ((fsbuf[j] >> k) & 1) != 0; k++) {}

			inode_nr = (i * SECTOR_SIZE + j) * 8 + k;
			fsbuf[j] |= (1 << k);

			WR_SECT(dev, imap_blk0_nr + i);
			break;
		}
		return inode_nr;
	}

	panic("inode map is probably full.\n");

	return 0;
}

PUBLIC	int	alloc_smap_bit(int dev, int nr_sects_to_alloc)
{
	int i, j, k;

	SUPER_BLOCK* sb = get_super_block(dev);
	int smap_blk0_nr = 1 + 1 + sb->nr_imap_sectors;
	int free_sect_nr = 0;

	for (i = 0; i < sb->nr_smap_sectors; i++) {
		RD_SECT(dev, smap_blk0_nr + i);

		for (j = 0; j < SECTOR_SIZE && nr_sects_to_alloc > 0; j++) {
			k = 0;
			if (!free_sect_nr) {
				if (fsbuf[j] == 0xFF)
					continue;
				for (; ((fsbuf[j] >> k) & 1) != 0; k++) {}
				free_sect_nr = (i * SECTOR_SIZE + j) * 8 + k - 1 + sb->n_1st_sector;
			}

			for (; k < 8; k++) {
				assert(((fsbuf[j] >> k) & 1) == 0);
				fsbuf[j] |= (1 << k);
				if (--nr_sects_to_alloc == 0)
					break;
			}
		}

		if (free_sect_nr)
			WR_SECT(dev, smap_blk0_nr + i);

		if (nr_sects_to_alloc == 0)
			break;
	}

	assert(nr_sects_to_alloc == 0);
	return free_sect_nr;
}

PUBLIC	INODE*	new_inode(int dev, int inode_nr, int start_sect)
{
	INODE* new_inode = get_inode(dev, inode_nr);

	new_inode->i_mode = I_REGULAR;
	new_inode->i_size = 0;
	new_inode->i_start_sector = start_sect;
	new_inode->i_nr_sectors = NR_DEFAULT_FILE_SECTS;
	new_inode->i_dev = dev;
	new_inode->i_cnt = 1;
	new_inode->i_num = inode_nr;

	sync_inode(new_inode);
	return new_inode;
}

PUBLIC	void	new_dir_entry(INODE* dir_inode, int inode_nr, char* filename)
{
	int dir_blk0_nr = dir_inode->i_start_sector;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE;

	int m = 0;
	DIR_ENTRY* pde;
	DIR_ENTRY* new_de = 0;

	int i, j;
	for (i = 0; i < nr_dir_blks; i++) {
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
		pde = (DIR_ENTRY*) fsbuf;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++,pde++) {
			if (++m > nr_dir_entries)
				break;
			if (pde->inode_nr == 0) {
				new_de = pde;
				break;
			}
		}
		if (m > nr_dir_entries || new_de)
			break;
	}

	if (!new_de) {
		new_de = pde;
		dir_inode->i_size += DIR_ENTRY_SIZE;
	}
	new_de->inode_nr = inode_nr;
	_strcpy(new_de->name, filename);

	WR_SECT(dir_inode->i_dev, dir_blk0_nr + i);
	sync_inode(dir_inode);
}

PUBLIC	int	strip_path(char* filename, const char* pathname, INODE** ppinode)
{
	const char* s = pathname;
	char* t = filename;
	
	if (s == 0)
		return -1;

	if (*s == '/')
		s++;

	while (*s) {
		if (*s == '/')
			return -1;
		*t++ = *s++;
		if (t - filename >= MAX_FILENAME_LEN)
			break;
	}
	*t = 0;
	*ppinode = root_inode;
	return 0;
}

PUBLIC	int	search_file(char* path)
{
	int i, j;

	char filename[MAX_PATH];
	kmemset(filename, 0, MAX_FILENAME_LEN);
	INODE* dir_inode;
	if (strip_path(filename, path, &dir_inode) != 0)
		return 0;
	
	int dir_blk0_nr = dir_inode->i_start_sector;
	int nr_dir_blks = (dir_inode->i_size + SECTOR_SIZE - 1) / SECTOR_SIZE;
	int nr_dir_entries = dir_inode->i_size / DIR_ENTRY_SIZE;

	int m = 0;
	DIR_ENTRY* pde;
	for (i = 0; i < nr_dir_blks && m <= nr_dir_entries; i++) {
		RD_SECT(dir_inode->i_dev, dir_blk0_nr + i);
		pde = (DIR_ENTRY*) fsbuf;
		for (j = 0; j < SECTOR_SIZE / DIR_ENTRY_SIZE; j++, pde++) {
			if (kmemcmp(filename, pde->name, MAX_FILENAME_LEN) == 0)
				return pde->inode_nr;
			if (++m > nr_dir_entries)
				break;
		}
	}

	return 0;
}

PUBLIC	INODE*	get_inode(int dev, int num)
{
	if (num == 0)
		return 0;

	INODE* p;
	INODE* q = 0;

	for (p = &inode_table[0]; p < &inode_table[NR_INODE]; p++) {
		if (p->i_cnt) {
			if ((p->i_dev == dev) && (p->i_num == num)) {
				p->i_cnt++;
				return p;
			}
		}
		else {
			if (!q)
				q = p;
		}
	}

	if (!q)
		panic("the inode table is full");

	q->i_dev = dev;
	q->i_num = num;
	q->i_cnt = 1;

	SUPER_BLOCK* sb = get_super_block(dev);
	int blk_nr = 1 + 1 + sb->nr_imap_sectors + sb->nr_smap_sectors + ((num - 1) / (SECTOR_SIZE / INODE_SIZE));

	RD_SECT(dev, blk_nr);
	INODE* pinode = (INODE*) ((u8*)fsbuf + ((num - 1) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE);
	q->i_mode = pinode->i_mode;
	q->i_size = pinode->i_size;
	q->i_start_sector = pinode->i_start_sector;
	q->i_nr_sectors = pinode->i_nr_sectors;
	
	return q;
}

PUBLIC	void	put_inode(INODE* pinode)
{
	assert(pinode->i_cnt > 0);
	pinode->i_cnt--;
}

PUBLIC	void	sync_inode(INODE* p)
{
	INODE* pinode;
	SUPER_BLOCK* sb = get_super_block(p->i_dev);
	int blk_nr = 1 + 1 + sb->nr_imap_sectors + sb->nr_smap_sectors + ((p->i_num - 1) / (SECTOR_SIZE / INODE_SIZE));
	RD_SECT(p->i_dev, blk_nr);
	pinode = (INODE*) ((u8*)fsbuf + (((p->i_num - 1) % (SECTOR_SIZE / INODE_SIZE)) * INODE_SIZE));
	
	pinode->i_mode = p->i_mode;
	pinode->i_size = p->i_size;
	pinode->i_start_sector = p->i_start_sector;
	pinode->i_nr_sectors = p->i_nr_sectors;
	WR_SECT(p->i_dev, blk_nr);
}
