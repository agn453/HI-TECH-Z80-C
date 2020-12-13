#include	<stdlib.h>
#include	"huff.h"

uchar	ascii;
short	factor;
static	int		indent;
static	uchar	debug;
static	char *	outfile;
static	short	filecnt;
static	long	totchrs;
static	long	totsize;

static	hdr		hd;
extern	long	ftell();
extern  void	error(char *fmt, ...);
extern  void	make_tree(char **);
void putnames(void);
void putfiles(void);
void puthdr(void);
void prtree(register node *);
void prchars(void);
void putbch(h_char);
extern void put_tree(void);
#if	!unix
extern	char **	_getargs();
extern	int	_argc_;
#endif	/* unix */
extern void align(void);
extern void puthch(uchar);
extern void put2(unsigned short);
extern void put4(unsigned long);
int main(int argc, char ** argv) {
	long pc;
	char buf[10];

#if	!unix
	if(argc == 1) {
		argv = _getargs(0, "enhuff");
		argc = _argc_;
	}
#endif

	argc--;
	argv++;
	while(argc && **argv == '-') {
		switch(argv[0][1]) {

		case 'f':
		case 'F':
			factor = atoi(&argv[0][2]);
			break;

		case 'a':
		case 'A':
			ascii = 1;
			break;

		case 'b':
		case 'B':
			ascii = 0;
			break;

		case 'd':
		case 'D':
			debug++;
			break;

		default:
			fprintf(stderr, "Unrecognized flag %s ignored\n", *argv);
			break;
		}
		argv++;
		argc--;
	}
	if(factor == 0)
		factor = 2;
	if(argc < 2)
		error("USAGE: enhuff [options] outfile file1 file2 ...");
	outfile = *argv++;
	if(freopen(outfile, "r", stdout)) {
		fprintf(stderr, "File %s exists; want to overwrite it? ", outfile);
		fgets(buf, sizeof buf, stdin);
		if(buf[0] != 'y' && buf[0] != 'Y')
			exit(1);
	}
	if(!freopen(outfile, "wb", stdout))
		error("Can't create %s", outfile);
	make_tree(argv);
	if(debug)
		prchars();
	if(debug > 1)
		prtree(root);
	for(filecnt = 0 ; flist[filecnt].f_name ; filecnt++)
		continue;
	fseek(stdout, (long)HSIZE, 0);
	put_tree();
	hd.hd_alfsiz = alfused;
	align();
	hd.hd_hpos = ftell(stdout);
	putnames();
	putfiles();
	puthdr();
	if(fclose(stdout) == EOF)
		error("Error closing output file - out of disk space?");
	pc = (totchrs-totsize)*100;
	pc /= totchrs;
	fprintf(stderr, "Source bytes %ld, enhuff'ed bytes %ld, compression %d%%\n", totchrs, totsize, (int)pc);
}

/*
 *
 */
void putnames(void) {
	register filent *	fp;
	register char *		cp;

		fseek(stdout, (long)FSIZE * filecnt + hd.hd_hpos, 0);
		for(fp = flist ; fp->f_name ; fp++) {
			fp->f_npos = ftell(stdout);
			for(cp = fp->f_name ; *cp ;)
				puthch(*cp++);
			puthch(0);
			align();
		}
	}

/*
 *
 */
void putfiles(void) {
	int			c;
	register filent *	fp;

	for(fp = flist ; fp->f_name ; fp++) {
		fprintf(stderr, "%-20.20s    %ld\n", fp->f_name, fp->f_nchrs);
		fp->f_pos = ftell(stdout);
		totchrs += fp->f_nchrs;
		if(!freopen(fp->f_name, fp->f_asc ? "r" : "rb", stdin))
			error("Can't re-open file %s", fp->f_name);
		while((c = getchar()) != EOF)
			puthch(c);
		align();
	}
	totsize = ftell(stdout);
}

/*
 *
 */
void puthdr(void) {
	register filent * fp;

	hd.hd_magic = MAGIC;
	hd.hd_nfiles = filecnt;
	fseek(stdout, 0L, 0);
	put2(hd.hd_magic);
	put2(hd.hd_nfiles);
	put2(hd.hd_alfsiz);
	put4(hd.hd_hpos);
	fseek(stdout, hd.hd_hpos, 0);
	for(fp = flist ; fp->f_name ; fp++) {
		put4(fp->f_npos);
		put4(fp->f_nchrs);
		put4(fp->f_pos);
		putchar(fp->f_asc);
	}
}

/*
 *
 */
void prtree(register node * np) {
	short	i;

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

/*
 *
 */
void prchars(void) {
	short	i;
	long	nchars, nbits, xx;

	nchars = nbits = 0;
	for(i = 0 ; i < alfused ; i++) {
		nchars += cptrs[i]->c_freq;
		nbits += cptrs[i]->c_freq * cptrs[i]->c_bits.h_nbits;
		if(cptrs[i]->c_chr >= ' ' && cptrs[i]->c_chr <= 0176)
			fprintf(stderr, "'%c'\t", cptrs[i]->c_chr);
		else
			fprintf(stderr, "%03o\t", cptrs[i]->c_chr);
		fprintf(stderr, "%7ld\t", cptrs[i]->c_freq);
		putbch(cptrs[i]->c_bits);
		fputc('\n', stderr);
	}
	nbits /= CHAR_BIT;
	xx = (nchars - nbits)*100;
	i = xx/nchars;
	fprintf(stderr, "%ld chars reduced to %ld - %d%% compression\n", nchars, nbits, i);
}

/*
 *
 */
void putbch(h_char tch) {
	short	i;

	for(i = 0 ; i < tch.h_nbits ; i++)
		if(TST(tch.h_cbits, i))
			putc('1', stderr);
		else
			putc('0', stderr);
}
