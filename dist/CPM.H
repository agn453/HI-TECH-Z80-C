/* header file for CP/M i/o routines for Z-80 C */

/* get basic definitions */

#ifndef	_STDDEF
typedef	int		ptrdiff_t;	/* result type of pointer difference */
typedef	unsigned	size_t;		/* type yielded by sizeof */
#define	_STDDEF

#ifndef	NULL
#define	NULL	((void *)0)
#endif	NULL

extern int	errno;			/* system error number */
#endif	_STDDEF

/*	hitech.h has definitions for uchar, ushort etc */

#include	<hitech.h>
#if	z80
#define	MAXFILE		8	/* max number of files open */
#else	z80
#define	MAXFILE		15	/* max number of files open */
#endif	z80
#define	SECSIZE		128	/* no. of bytes per sector */

extern struct	fcb {
	uchar	dr;		/* drive code */
	char	name[8];	/* file name */
	char	ft[3];		/* file type */
	uchar	ex;		/* file extent */
	char	fil[2];		/* not used */
	char	rc;		/* number of records in present extent */
	char	dm[16];		/* CP/M disk map */
	char	nr;		/* next record to read or write */
	uchar	ranrec[3];	/* random record number (24 bit no. ) */
	long	rwp;		/* read/write pointer in bytes */
	uchar	use;		/* use flag */
	uchar	uid;		/* user id belonging to this file */
}	_fcb[MAXFILE];

extern char		bdos(int, ...);
extern struct fcb *	getfcb(void);
extern short		bdoshl(int, ...);	/* bdos call returning value in hl */
extern uchar		getuid(void);
extern void		setuid(int);
extern uchar		setfcb(struct fcb *, char *);

/*	 flag values in fcb.use */

#define	U_READ	1		/* file open for reading */
#define	U_WRITE	2		/* file open for writing */
#define	U_RDWR	3		/* open for read and write */
#define	U_CON	4		/* device is console */
#define	U_RDR	5		/* device is reader */
#define	U_PUN	6		/* device is punch */
#define	U_LST	7		/* list device */

/*	 special character values */

#define	CPMETX	032		/* ctrl-Z, CP/M end of file for text */
#define	CPMRBT	003		/* ctrl-C, reboot CPM */


#define	MPM	0x100		/* bit to test for MP/M */
#define	CCPM	0x400		/* bit to test for CCP/M */

#define	ISMPM()	(bdoshl(CPMVERS)&MPM)	/* macro to test for MPM */

/*	 what to do after you hit return */

#define	EXIT	(*(int (*)())0)	/* where to go to reboot CP/M */

/*	 bdos calls etc. */

#define	CPMRCON	1		/* read console */
#define	CPMWCON	2		/* write console */
#define	CPMRRDR	3		/* read reader */
#define	CPMWPUN	4		/* write punch */
#define	CPMWLST	5		/* write list */
#define	CPMDCIO	6		/* direct console I/O */
#define	CPMGIOB	7		/* get I/O byte */
#define	CPMSIOB	8		/* set I/O byte */
#define	CPMRCOB	10		/* read console buffered */
#define	CPMICON	11		/* interrogate console ready */
#define	CPMVERS	12		/* return version number */

/*	BDOS calls */

#define	CPMRDS	13		/* reset disk system */
#define	CPMLGIN	14		/* log in and select disk */
#define	CPMOPN	15		/* open file */
#define	CPMCLS	16		/* close file */
#define	CPMFFST	17		/* find first */
#define	CPMFNXT	18		/* find next */
#define	CPMDEL	19		/* delete file */
#define	CPMREAD	20		/* read next record */
#define	CPMWRIT	21		/* write next record */
#define	CPMMAKE	22		/* create file */
#define	CPMREN	23		/* rename file */
#define	CPMILOG	24		/* get bit map of logged in disks */
#define	CPMIDRV	25		/* interrogate drive number */
#define	CPMSDMA	26		/* set DMA address for i/o */
#define	CPMSUID	32		/* set/get user id */
#define	CPMRRAN	33		/* read random record */
#define	CPMWRAN	34		/* write random record */
#define	CPMCFS	35		/* compute file size */
#define	CPMDSEG	51		/* set DMA segment */
