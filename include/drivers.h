#ifndef	_LAVENDER_DRIVERS_H_
#define	_LAVENDER_DRIVERS_H_

#include	"type.h"
#include	"message.h"

typedef	struct	s_dev_drv {
	u32	driver_handler;
} DEV_DRIVER;

#define	INVALID_DRIVER		0
#define	TASK_HD_DRIVER		TASK_HD		// hard disk driver
#define	TASK_TTY_DRIVER		TASK_TTY	// console driver

#endif
