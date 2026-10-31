/*	String functions */

#ifndef	_STDDEF
typedef	int		ptrdiff_t;	/* result type of pointer difference */
typedef	unsigned	size_t;		/* type yielded by sizeof */
#define	_STDDEF
#define	offsetof(ty, mem)	((int)&(((ty *)0)->mem))
#endif	_STDDEF

#ifndef	NULL
#define	NULL	((void *)0)
#endif	NULL

extern int	errno;			/* system error number */

extern void *	memcpy(void *, void *, size_t);
extern void *	memmove(void *, void *, size_t);
extern void *	memset(void *, int, size_t);
extern char *	strcat(char *, char *);
extern char *	strncat(char *, char *, size_t);
extern char *	strcpy(char *, char *);
extern char *	strncpy(char *, char *, size_t);
extern char *	strdup(char *);
extern int	memcmp(void *, void *, size_t);
extern int	strcmp(char *, char *);
extern int	strncmp(char *, char *, size_t);
extern int	strcasecmp(char *, char *);
extern int	strncasecmp(char *, char *, size_t);
extern void *	memchr(void *, int, size_t);
extern size_t	strlen(char *);
extern char *	strchr(char *, int);
extern char *	index(char *, int);
extern char *	strrchr(char *, int);
extern char *	rindex(char *, int);
extern char *	strstr(char*, char*);
extern char *	strcasestr(char*, char*);
extern char *	strnstr(char*, char*, size_t);
extern char *	strncasestr(char*, char*, size_t);
