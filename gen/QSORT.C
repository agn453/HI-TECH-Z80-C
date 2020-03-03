/*
 *	Quicksort based on the algorithm given in
 *	"Algorithms + Data Structures = Programs" by N. Wirth.
 */

qsort(base, nel, width, compar)
char *base; int (*compar)();
unsigned width,nel;
{
	register char *	x;
	extern char *	malloc();
	register int	i,j,l,r;
	struct
	{
		int	l,r;
	}	stack[20];
	int	s;
	char	xbuf[800];

	if(width < sizeof xbuf)
		x = xbuf;
	else if(!(x = malloc(width+1)))
		return;			/* can't do much */
	x[width] = 0;
	s = 0;
	stack[0].l = 0;
	stack[0].r = nel-1;
	do {
		/* take top request from stack */
		l = stack[s].l;
		r = stack[s--].r;

		do {
			i = l;
			j = r;
			bmove(base+width*((i+j)/2), x, width);
			do {
				while((*compar)(base+i*width, x) < 0)
					i++;
				while((*compar)(x, base+j*width) < 0)
					j--;
				if(i <= j) {
					_swap(width, base+i*width, base+j*width);
					i++;
					j--;
				}
			} while(i <= j);
			if(j-l < r-i) {
				if(i < r) {	/* stack right partition */
					stack[++s].l = i;
					stack[s].r = r;
				}
				r = j;		/* continue with left */
			} else {
				if(l < j) {
					stack[++s].l = l;
					stack[s].r = j;
				}
				l = i;
			}
		} while(l < r);
	} while(s >= 0);
	if(x != xbuf)
		free(x);
}
