#include	"huff.h"
#include	<stdio.h>
#include	<stdlib.h>
typedef struct {
	node *	np_node;
	uchar	np_len;
}	np;

node *	root;
chent	clist[ALFSIZ];
filent	flist[MAXFILENT];
short	alfused;
chent *	cptrs[ALFSIZ];
static	np	nptrs[ALFSIZ];
static	uchar	p_bit;
static	uchar	p_char;
static	uchar	level;
static	short	nidx;

/* extern	char *	calloc(); */
extern  void error(char *fmt, ...);

void   make_tree(char **);
node * newnode(void);
node * bld(short, short);
int    cmpr(chent **, chent **);
int    cmpnp(np *, np *);
void   bld_bits(node *, h_char);
void   pinit(void);
void   align(void);
void   put_bit(int);
void   puthch(uchar);
void   put_tchrs(node *);
void   walk_tree(node *);
void   put_tree(void);

/*
 *
 */
void make_tree(char ** namlist) {
	FILE *	fp;
	int	c,  fno;
	h_char	tch;
	register char *	cp;

	fno = 0;
	while(*namlist) {
		if(**namlist == '-')
			switch(namlist[0][1]) {

			case 'a':
			case 'A':
				ascii = 1;
				namlist++;
				continue;

			case 'b':
			case 'B':
				ascii = 0;
				namlist++;
				continue;

			default:
				break;
			}
		if(!(fp = fopen(*namlist, ascii ? "r" : "rb"))) {
			fprintf(stderr, "Can't open %s\n", *namlist++);
			continue;
		}
		flist[fno].f_name = *namlist;
		flist[fno].f_asc = ascii;
		while((c = getc(fp)) != EOF) {
			clist[c & (ALFSIZ-1)].c_freq++;
			flist[fno].f_nchrs++;
		}
		fclose(fp);
		for(cp = *namlist ; *cp ; )
			clist[*cp++ & (ALFSIZ-1)].c_freq++;
		clist[0].c_freq++;
		namlist++;
		fno++;
	}
	for(c =0 ; c < ALFSIZ ; c++) {
		cptrs[c] = clist+c;
		clist[c].c_chr = c;
	}
	qsort(cptrs, ALFSIZ, sizeof cptrs[0], cmpr);
	for(alfused = 0 ; alfused < ALFSIZ && cptrs[alfused]->c_freq ; alfused++)
		continue;
	level = 0;
	nidx = 0;
	root = bld(0, alfused-1);
	qsort(nptrs, alfused, sizeof nptrs[0], cmpnp);
	for(c = 0 ; c < alfused ; c++)
		nptrs[c].np_node->n_c = cptrs[c]->c_chr;
	tch.h_nbits = 0;
	bld_bits(root, tch);
}

/*
 *
 */
node * newnode(void) {
	register node *	np;

	if(!(np = (node *)calloc(1, sizeof(node))))
		error("Out of memory");
	return np;
}

/*
 *
 */
node * bld(short first, short last) {
	int	i;
	long	tot, run;
	register node *	l, * np;

	if(first == last) {
		l = newnode();
		nptrs[nidx].np_node = l;
		nptrs[nidx++].np_len = level+1;
		l->n_left = l->n_right = (node *)0;
		return l;
	}
	level++;
	for(i = first, tot = 0 ; i <= last ; i++)
		tot += cptrs[i]->c_freq;
	tot /= factor;
	run = cptrs[i = first]->c_freq;
	while(run < tot && i != last)
		run += cptrs[++i]->c_freq;
	if(i == last)
		i = (first+last)/2;
	if(i == first) {
		l = newnode();
		nptrs[nidx].np_node = l;
		nptrs[nidx++].np_len = level+1;
		l->n_left = l->n_right = (node *)0;
	} else
		l = bld(first, i);
	np = newnode();
	np->n_left = l;
	np->n_right = bld(i+1, last);
	level--;
	return np;
}

/*
 *
 */
int cmpr(register chent ** p1, chent ** p2) {
	long	i;

	i = (*p2)->c_freq - (*p1)->c_freq;
	if(i > 0)
		return 1;
	if(i < 0)
		return -1;
	return 0;
}

/*
 *
 */
int cmpnp(np * p1, np * p2) {

	return p1->np_len - p2->np_len;
}

/*
 *
 */
void bld_bits(register node * nodep, h_char tch) {

	if(!nodep->n_left) {	/* leaf node */
		clist[nodep->n_c].c_bits = tch;
		return;
	}
	SET(tch.h_cbits, tch.h_nbits);
	tch.h_nbits++;
	bld_bits(nodep->n_left, tch);
	CLR(tch.h_cbits, tch.h_nbits-1);
	bld_bits(nodep->n_right, tch);
}

/*
 *
 */
void pinit(void) {

	p_char = 0;
	p_bit = 0;
}

/*
 *
 */
void align(void) {

	if(p_bit)
		putchar(p_char);
	pinit();
}

/*
 *
 */
void put_bit(int i) {

	if(i)
		p_char |= 1 << p_bit;
	if(++p_bit == CHAR_BIT) {
		putchar(p_char);
		p_char = 0;
		p_bit = 0;
	}
}

/*
 *
 */
void puthch(uchar c) {
	h_char	tch;
	short	i;

	tch = clist[c].c_bits;
	for(i = 0 ; i < tch.h_nbits ; i++) {
		if(TST(tch.h_cbits, i))
			p_char |= 1 << p_bit;
		if(++p_bit == CHAR_BIT) {
			putchar(p_char);
			p_char = 0;
			p_bit = 0;
		}
	}
}

/*
 *
 */
void put_tchrs(register node * tp) {

	if(tp->n_left) {
		put_tchrs(tp->n_left);
		put_tchrs(tp->n_right);
	} else
		putchar(tp->n_c);
}

/*
 *
 */
void walk_tree(register node * tp) {

	if(tp->n_left) {
		put_bit(1);
		walk_tree(tp->n_left);
		put_bit(0);
		walk_tree(tp->n_right);
	}
}

/*
 *
 */
void put_tree(void) {

	put_tchrs(root);
	walk_tree(root);
	put_bit(0);		/* a safeguard */
}
