/*
 *	Declarations for Unix style low-level I/O functions.
 */

#ifndef	_STDDEF
typedef	int		ptrdiff_t;	/* result type of pointer difference */
typedef	unsigned	size_t;		/* type yielded by sizeof */
#define	_STDDEF
#define	offsetof(ty, mem)	((int)&(((ty *)0)->mem))
#endif	_STDDEF

#ifndef	NULL
#define	NULL	((void *)0)
#endif	NULL

#ifndef SEEK_SET
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#endif
#ifndef O_RDONLY
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_INHER 4
#endif
 
extern int	errno;			/* system error number */

extern int	open(char *, int);
extern int	close(int);
extern int	creat(char *, int);
extern int	dup(int);
extern long	lseek(int, long, int);
extern int	read(int, void *, int);
extern int	unlink(char *);
extern int	write(int, void *, int);
extern int	isatty(int);
extern int	chmod(char *, int);
