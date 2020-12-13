#include	<stdlib.h>
#include	"huff.h"

char	cl[ALFSIZ];
short	alfused;
static	short	clidx;
static	short	g_bit;
static	short	g_char;
node *	root;

void   bld_tree(void);
node * get_tree(void);
void   align(void);
int    get_bit(void);
int    gethch(void);
extern void error(char *fmt, ...);

void bld_tree(void) {
	short i;

	for(i = 0 ; i < alfused ; i++)
		cl[i] = getchar();
	align();
	clidx = 0;
	root = get_tree();
}

node * get_tree(void) {
	register node *	tp;

	if(!(tp = (node *)calloc(sizeof(node), 1)))
		error("Out of memory");
	if(get_bit()) {
		tp->n_left  = get_tree();
		tp->n_right = get_tree();
	} else
		tp->n_c = cl[clidx++];
	return tp;
}

void align(void) {
	g_char = 0;
	g_bit = CHAR_BIT;
}

int get_bit(void) {
	if(g_bit == CHAR_BIT)
		if((g_char = getchar()) == EOF)
			error("Read error or EOF on huf file");
		else
			g_bit = 0;
	return g_char & (1 << g_bit++);
}

int gethch(void) {
	register node *	tp;

	tp = root;
	while(tp->n_left)
		tp = get_bit() ? tp->n_left : tp->n_right;
	return tp->n_c & 0xFF;
}
