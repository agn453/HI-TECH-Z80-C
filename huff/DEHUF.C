#include <stdlib.h>
#include "huff.h"

	hdr	hd;
static	short	indent;
static	uchar	listflg;
static	uchar	xtractflg;
static	uchar	debug;
static	char **	av;

extern void align(void);
extern void bld_tree(void);
extern void error(char *fmt, ...);
extern int casecmp(register char *, char *);
extern int gethch(void);
int	   isarg(char * s);
char * getname(short i);
void   list(short i);
void   extract(short i);
void   prtree(register node * np);

#if	!unix
extern	char **	_getargs();
extern	int	_argc_;
#endif	/* unix */
/*
 *
 */
int main(int argc, char **	argv) {
	short i;

#if	!unix
	if(argc == 1) {
		argv = _getargs(0, "dehuff");
		argc = _argc_;
	}
#endif	/* unix */

	argc--;
	argv++;
	while(argc && **argv == '-') {
		switch(argv[0][1]) {

		case 'l':
		case 'L':
			listflg++;
			break;

		case 'x':
		case 'X':
			xtractflg++;
			break;

		case 'd':
		case 'D':
			debug++;
			break;

		default:
			fprintf(stderr, "Unrecognized flag %s ignored\n", argv[0]);
			break;
		}
		argc--;
		argv++;
	}
	if(argc < 1) {
		printf("Usage of dehuff:\n\nTo list the contents of a .HUF file:\n");
		printf("\n	dehuff file.huf\n\nTo extract files from a .HUF file:\n");
		printf("\n	dehuff -x file.huf\n\nIf no extra arguments are supplied, then the entire contents\n");
		printf("of the .HUF file will be listed or extracted, otherwise only files\n");
		printf("matching an argument will be listed or extracted, e.g.\n\n");
		printf("	dehuff -x xyz.huf afile.c\n\nwould extract only the file afile.c.\n");
		printf("The number reported for each file is its length in bytes\n");
		exit(1);
	}
	av = argv+1;
	if(!freopen(*argv, "rb", stdin))
		error("Can't open file %s", *argv);
	hd.hd_magic = get2();
	if(hd.hd_magic != MAGIC)
		error("%s is not a huf file", *argv);
	hd.hd_nfiles = get2();
	alfused = hd.hd_alfsiz = get2();
	hd.hd_hpos = get4();
	bld_tree();
	if(debug)
		prtree(root);
	for(i = 0 ; i < hd.hd_nfiles ; i++)
		if(isarg(getname(i)))
			if(xtractflg)
				extract(i);
			else
				list(i);
}

/*
 *
 */
int isarg(char * s) {
	register char ** p;

	if(*av == 0)		/* default is all members */
		return 1;
	for(p = av ; *p ; p++)
		if(casecmp(*p, s) == 0)
			return 1;
	return 0;
}

/*
 *
 */
char * getname(short i) {
	static   char	fbuf[100];
	register char *	cp;
	filent			hf;

	fseek(stdin, hd.hd_hpos+i*FSIZE, 0);
	hf.f_npos  = get4();
	hf.f_nchrs = get4();
	hf.f_pos = get4();
	hf.f_asc = getchar();
	fseek(stdin, hf.f_npos, 0);
	align();
	for(cp = fbuf ; (*cp++ = gethch()) ; )
		continue;
	return fbuf;
}

/*
 *
 */
void list(short i) {
	register char *	cp;
	filent			hf;
	char			fbuf[100];

	fseek(stdin, hd.hd_hpos+i*FSIZE, 0);
	hf.f_npos  = get4();
	hf.f_nchrs = get4();
	hf.f_pos = get4();
	hf.f_asc = getchar();
	fseek(stdin, hf.f_npos, 0);
	align();
	for(cp = fbuf ; (*cp++ = gethch()) ; )
		continue;
	fprintf(stderr, "%-20.20s    %ld\n", fbuf, hf.f_nchrs);
}

/*
 *
 */
void extract(short i) {
	register char *	cp;
	filent			hf;
	FILE *			fp;
	char			fbuf[100];

	fseek(stdin, hd.hd_hpos+i*FSIZE, 0);
	hf.f_npos  = get4();
	hf.f_nchrs = get4();
	hf.f_pos = get4();
	hf.f_asc = getchar();
	fseek(stdin, hf.f_npos, 0);
	align();
	for(cp = fbuf ; (*cp++ = gethch()) ; )
		continue;
	if(!(fp = fopen(fbuf, hf.f_asc ? "w" : "wb"))) {
		fprintf(stderr, "Can't create %s\n", fbuf);
		return;
	}
	fprintf(stderr, "%-20.20s    %ld\n", fbuf, hf.f_nchrs);
	fseek(stdin, hf.f_pos, 0);
	align();
	while(hf.f_nchrs--)
		putc(gethch(), fp);
	fclose(fp);
}

/*
 *
 */
void prtree(register node * np) {
	short i;

	for(i = indent ; i-- ; )
		fputc(' ', stderr);
	if(np->n_left) {
		fprintf(stderr, "X\n");
		indent += 4;
		prtree(np->n_left);
		prtree(np->n_right);
		indent -= 4;
	} else
		if(np->n_c >= ' ' && np->n_c <= 0176)
			fprintf(stderr, "'%c'\n", np->n_c);
		else
			fprintf(stderr, "%03o\n", np->n_c);
}
