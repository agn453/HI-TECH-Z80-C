#ifdef debug
#define ASSERT(p) if(!(p))botch("p");else
botch(s)
char *s;
{
	printf("assertion botched: %s\n",s);
	abort();
}
#else
#define ASSERT(p)
#endif

/*	C storage allocator for Z80 and other 8 bit machines
 *	circular first-fit strategy
 *	works with noncontiguous, but monotonically linked, arena
 *	each block is preceded by a ptr to the (pointer of) 
 *	the next following block and a busy flag
 *	bit in flag is 1 for busy, 0 for idle
 *	gaps in arena are merely noted as busy blocks
 *	last block of arena (pointed to by alloct) is empty and
 *	has a pointer to first
 *	idle blocks are coalesced during space search
 *
*/
#define	BLOCK	(85*sizeof(struct store))	/* 255 bytes */
#define BUSY 1
#define NULL 0
#define	testbusy(p)	((p).flag & BUSY)
#define	sbusy(p)	(p).flag |= BUSY
#define	cbusy(p)	(p).flag &= ~BUSY

struct store
{
	struct store *	ptr;
	char		flag;
};

static struct store	allocs[2];	/*initial arena*/
static struct store *	allocp;		/*search ptr*/
static struct store *	alloct;		/*arena top*/
static struct store	allocx;		/* for realloc */
char *			sbrk();

char *
malloc(nw)
unsigned nw;
{
	register struct store *p, *q;
	static unsigned temp;	/*coroutines assume no auto*/

	if(allocs[0].ptr==(struct store *)0) {	/*first time*/
		alloct = allocs[0].ptr = &allocs[1];
		allocp = allocs[1].ptr = &allocs[0];
		sbusy(allocs[0]);
		sbusy(allocs[1]);
	}
	nw = ((nw - 1 + sizeof(struct store)*2)/sizeof(struct store)) * sizeof(struct store);
	ASSERT(allocp>=allocs && allocp<=alloct);
	ASSERT(allock());
	for(p=allocp; ; ) {
		for(temp=0; ; ) {
			if(!testbusy(*p)) {
				while(!testbusy(*(q=p->ptr))) {
					ASSERT(q>p&&q<alloct);
					p->ptr = q->ptr;
				}
				if(q>=(struct store *)((char *)p+nw) && (struct store *)((char *)p+nw)>=p)
					goto found;
			}
			q = p;
			p = p->ptr;
			if(p>q)
				ASSERT(p<=alloct);
			else if(q!=alloct || p!=allocs) {
				ASSERT(q==alloct&&p==allocs);
				return(NULL);
			} else if(++temp>1)
				break;
		}
		temp = ((nw+sizeof(struct store)-1+BLOCK)/BLOCK)*BLOCK;
		q = (struct store *)sbrk(0);
		if((struct store *)((char *)q+temp) < q) {
			return(NULL);
		}
		q = (struct store *)sbrk(temp);
		if((int)q == -1) {
			return(NULL);
		}
		ASSERT(q>alloct);
		alloct->ptr = q;
		if(q!=alloct+1)
			sbusy(*alloct);
		else
			cbusy(*alloct);
		alloct = q->ptr = (struct store *)((char *)q+temp-sizeof(struct store));;
		alloct->ptr = allocs;
		sbusy(*alloct);
		cbusy(*q);
	}
found:
	allocp = (struct store *)((char *)p + nw);
	ASSERT(allocp<=alloct);
	if(q>allocp) {
		allocx = *allocp;
		allocp->ptr = p->ptr;
		allocp->flag = 0;
	}
	p->ptr = allocp;
	sbusy(*p);
	return((char *)(p+1));
}

/*	freeing strategy tuned for LIFO allocation
*/
free(ap)
char *ap;
{
	register struct store *p;

	p = ((struct store *)ap)-1;
	ASSERT(p>allocs[1].ptr&&p<=alloct);
	ASSERT(allock());
	allocp = p;
	ASSERT(testbusy(*p));
	cbusy(*p);
	ASSERT(p->ptr > allocp && p->ptr <= alloct);
}

char *
realloc(p, nbytes)
char *		p;
unsigned short	nbytes;
{
	register struct store *	xp, * q;
	unsigned short		ons;
	unsigned short		ns;

	xp = (struct store *)p;
	ns = (nbytes + sizeof(struct store) - 1)/sizeof(struct store);
	ons = xp[-1].ptr - xp;
	if(testbusy(xp[-1]))
		free((char *)xp);
	if(!(q = (struct store *)malloc(nbytes)) || q == xp)
		return (char *)q;
	ns = q[-1].ptr - q;
	if(ons > ns)
		ons = ns;
	bmove((char *)xp, (char *)q, ons * sizeof(struct store));
	if(q < xp && q+ns > xp)
		q[q+ns-xp] = allocx;
	return (char *)q;
}


#ifdef	debug
showall()
{
	struct store *p, *q;
	int i, used = 0, free = 0;

	for(p = &allocs[0] ; p && p!= alloct ; p = q) {
		q = p->ptr;
		printf("%4.4x %5d %s\n", p, i = (unsigned)q - (unsigned)p,
			testbusy(*p) ? "BUSY" : "FREE");
		if(testbusy(*p))
			used += i;
		else
			free += i;
	}
	printf("%d used, %d free, %4.4x end\n", used, free, alloct);
}
#endif
