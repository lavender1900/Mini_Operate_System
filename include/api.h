#ifndef	_LAVENDER_API_H_
#define	_LAVENDER_API_H_

PUBLIC	int	get_ticks();
PUBLIC	int	get_hd_info();
PUBLIC	int	open(const char* pathname, int flags);	
PUBLIC	void	close(int fd);
PUBLIC	int	file_read(int fd, void* buf, int count);
PUBLIC	int	file_write(int fd, void* buf, int count);

#endif
