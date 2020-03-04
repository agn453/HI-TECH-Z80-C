#define	BUFSIZ		512
#define	_NFILE		6
# ifndef FILE
#define	uchar	unsigned char

extern	struct	_iobuf {
	char *		_ptr;
	int		_cnt;
	char *		_base;
	uchar		_flag;
	char		_file;
} _iob[_NFILE];

extern uchar _setup;
# endif

#define	_IOREAD		01
#define	_IOWRT		02
#define	_IORW		03
#define	_IONBF		04
#define	_IOMYBUF	010
#define	_IOEOF		020
#define	_IOERR		040
#define	_IOSTRG		0100
#define	_IOBINARY	0200

#define	NULL		0
#define	FILE		struct _iobuf
#define	EOF		(-1)
#define	STDSETUP()

#define	stdin		(&_iob[0])
#define	stdout		(&_iob[1])
#define	stderr		(&_iob[2])
#define	getc(p)		fgetc(p)
#define	getchar()	getc(stdin)
#define	putc(x,p)	fputc(x,p)
#define	putchar(x)	putc(x,stdout)
#define	feof(p)		(((p)->_flag&_IOEOF)!=0)
#define	ferror(p)	(((p)->_flag&_IOERR)!=0)
#define	fileno(p)	((uchar)p->_file)

FILE *		fopen();
FILE *		freopen();
FILE *		fdopen();
long		ftell();
char *		fgets();
char *		_bufallo();
