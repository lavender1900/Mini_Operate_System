#ifndef	_LAVENDER_FS_H_
#define	_LAVENDER_FS_H_

#define	MAGIC_V1		0x111
#define	MAX_FILENAME_LEN	12
#define	ROOT_INODE		1
#define	I_TYPE_MASK		0x170000
#define	I_REGULAR		0x100000
#define	I_BLOCK_SPECIAL		0x060000
#define	I_DIRECTORY		0x040000
#define	I_CHAR_SPECIAL		0x020000
#define	I_NAMED_PIPE		0x010000

#define	NR_DEFAULT_FILE_SECTS	2048	// 2048 * 512 = 1MB

typedef	struct	s_super_block
{
	u32	magic;
	u32	nr_inodes;
	u32	nr_sectors;
	u32	nr_imap_sectors;
	u32	nr_smap_sectors;
	u32	n_1st_sector;
	u32	nr_inode_sectors;
	u32	root_inode;
	u32	inode_size;
	u32	inode_isize_off;
	u32	inode_start_off;
	u32	dir_ent_size;
	u32	dir_ent_inode_off;
	u32	dir_ent_fname_off;
	int	sb_dev;
} SUPER_BLOCK;

#define	SUPER_BLOCK_SIZE	((int) &((SUPER_BLOCK*) 0)->sb_dev)

typedef	struct	s_inode
{
	u32	i_mode;
	u32	i_size;
	u32	i_start_sector;
	u32	i_nr_sectors;
	u8	_ununsed[16];
	int	i_dev;
	int	i_cnt;
	int	i_num;
} INODE;

#define	INODE_SIZE		32

typedef	struct s_dir_entry {
	int inode_nr;
	char	name[MAX_FILENAME_LEN];
} DIR_ENTRY; 

#define	DIR_ENTRY_SIZE		sizeof(DIR_ENTRY)

#endif
