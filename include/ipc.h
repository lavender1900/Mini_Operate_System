#ifndef	_LAVENDER_IPC_H_
#define	_LAVENDER_IPC_H_

#define	ASSERT
#ifdef	ASSERT
	void	assertion_failure(char* exp, char* file, char* base_file, int line);
	#define	assert(exp) if (exp) {} \
			else assertion_failure(#exp, __FILE__, __BASE_FILE__, __LINE__)
#else
	#define	assert(exp)
#endif

PUBLIC	int	panic(char* format, ...);
PUBLIC	int	msg_send(PROCESS* current, int dest, MESSAGE* m);	
PUBLIC	int	msg_receive(PROCESS* current, int src, MESSAGE* m);

// Message Source
#define	NO_TASK		-1
#define	ANY		-2
#define	INTERRUPT	-3

// Message Type
#define	HARD_INT	1

// Function type
#define	SEND		1
#define	RECEIVE		2

// Process flag
#define	SENDING		0x2
#define	RECEIVING	0x4

#endif
