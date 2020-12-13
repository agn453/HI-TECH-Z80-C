/*
 *	Definitions for the Huffman encoded file structure
 */

#include	<stdio.h>

#ifndef	uchar
#define	uchar	unsigned char
#endif

#ifndef	CHAR_BIT
#define	CHAR_BIT	8
#endif	/* CHAR_BIT */

#define	ALFSIZ		(1<<CHAR_BIT)	/* max no. of distinct characters */
#define	MAXFILENT	500		/* maximum number of files */
#define	MAGIC		0x01BD
#define	HSIZE		10		/* length of header in file */
#define	FSIZE		13		/* length of file entry */

#define	SET(ar, bit)	(ar[(bit)/CHAR_BIT] |= (1<<((bit)%CHAR_BIT)))
#define	CLR(ar, bit)	(ar[(bit)/CHAR_BIT] &= ~(1<<((bit)%CHAR_BIT)))
#define	TST(ar, bit)	((ar[(bit)/CHAR_BIT] & (1<<((bit)%CHAR_BIT)))!=0)

#if	unix
#define	casecmp	strcmp
#endif

typedef struct node {
	uchar		n_c;
	long		n_f;
	struct node *	n_left, * n_right;
}	node;

typedef	struct {
	uchar		h_nbits;
	uchar		h_cbits[ALFSIZ/CHAR_BIT];
}	h_char;

typedef struct {
	uchar		c_chr;
	long		c_freq;
	h_char		c_bits;
}	chent;

typedef	struct {
	char *		f_name;
	long		f_npos;
	long		f_nchrs;
	long		f_pos;
	char		f_asc;
}	filent;

typedef struct {
	short		hd_magic;	/* magic number */
	short		hd_nfiles;	/* no of files */
	short		hd_alfsiz;	/* size of alphabet */
	long		hd_hpos;	/* start pos of file hdrs */
}	hdr;


extern node *		root;
extern chent		clist[ALFSIZ];
extern filent		flist[MAXFILENT];
extern short		alfused;
extern uchar		ascii;
extern unsigned long get4(void);
extern unsigned short get2(void);
extern chent *		cptrs[ALFSIZ];
extern short		factor;
