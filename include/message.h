#ifndef	_LAVENDER_MESSAGE_H_
#define	_LAVENDER_MESSAGE_H_

// api message type
#define	GET_TICKS	0
#define	DEV_OPEN	1
#define	DEV_CLOSE	2
#define	DEV_READ	3
#define	DEV_WRITE	4
#define	DEV_IOCTL	5
#define	SYSCALL_RET	6
#define	OPEN		7
#define	CLOSE		8
#define	READ		9
#define	WRITE		10

// message request type
#define	DIOCTL_GET_GEO		0

// process identifier
#define	TASK_TTY	3
#define	TASK_SYS	4
#define	TASK_HD		5
#define	TASK_FS		6

#endif
