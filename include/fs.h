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

#define	NR_FILES		10
#define	NR_FILE_DESC		256
#define	NR_INODE		4096
#define	NR_SUPER_BLOCK		8
#define	MAX_PATH		64

#define	O_CREAT			1
#define	O_READ			2
#define	O_WRITE			4
#define	O_RDWR			(O_READ | O_WRITE)

#define	INVALID_INODE		0

#define	NR_DEFAULT_FILE_SECTS	2048	// 2048 * 512 = 1MB

#define	RD_SECT(dev, sect_nr) rw_sector(DEV_READ, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, TASK_FS, fsbuf)
#define	WR_SECT(dev, sect_nr) rw_sector(DEV_WRITE, dev, (sect_nr) * SECTOR_SIZE, SECTOR_SIZE, TASK_FS, fsbuf)

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
	u32	i_start_sector;	// when inode points to a special file (I_CHAR_SPECIAL..), i_start_sector means composition of Major Device No and Minor Device No
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

typedef	struct	s_file_desc
{
	int	fd_mode;
	int	fd_pos;
	INODE*	fd_inode;
} FILE_DESC;

PUBLIC	int	rw_sector(int io_type, int dev, u64 pos, int bytes, int proc_nr, void* buf);
PUBLIC	int	alloc_imap_bit(int dev);
PUBLIC	int	alloc_smap_bit(int dev, int nr_sects_to_alloc);
PUBLIC	INODE*	new_inode(int dev, int inode_nr, int start_sect);
PUBLIC	void	new_dir_entry(INODE* dir_inode, int inode_nr, char* filename);
PUBLIC	int	strip_path(char* filename, const char* pathname, INODE** ppinode);
PUBLIC	int	search_file(char* path);
PUBLIC	INODE*	get_inode(int dev, int num);
PUBLIC	void	put_inode(INODE* pinode);
PUBLIC	void	sync_inode(INODE* p);
PUBLIC	SUPER_BLOCK*	get_super_block(int dev);
PUBLIC	int	do_unlink();

#endif
