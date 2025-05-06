#include	<stdio.h>
#include	<ctype.h>


/*
 *	doprnt for 8086
 */

/* Routines for formatted output must be able to cope with 
   strings longer than 255 bytes.  -  Arnold M */
static unsigned int ival;
static char *	x;
static FILE *	ffile;
extern int	atoi(char *);
extern int	strlen(char *);

static
pputc(c)
int	c;
{
	putc(c, ffile);
}

static char *
icvt(cp)
register char *	cp;
{
	ival = atoi(cp);
	while(isdigit((unsigned)*cp))
		cp++;
	return cp;
}

_doprnt(file, f, a)
FILE *	file;
register char *		f;
int *		a;
{
	char	c;
	uchar	fill, left;
	int	prec;
	unsigned int i, width; /* Allow for long strings.  - Arnold M */ 
	uchar	base, sign, len;
	uchar	ftype;
	extern	short _pnum(), _fnum();

	ffile = file;
	while(c = *f++)
		if(c != '%')
			pputc(c);
		else {
			base = 10;
			width = 0;
			sign = 0;
			left = 0;
			ftype = 0;
			len = sizeof(int)/sizeof *a;
			if(*f == '-') {
				f++;
				left++;
			}
			fill = *f == '0';
			if(isdigit((unsigned)*f)) {
				f = icvt(f);
				width = ival;
			} else if(*f == '*') {
				width = *a++;
				f++;
			}
			if(*f == '.')
				if(*++f == '*') {
					prec = *a++;
					f++;
				} else {
					f = icvt(f);
					prec = ival;
				}
			else
				prec = fill ? width : -1;
			if(*f == 'l') {
				f++;
				len = sizeof(long)/sizeof *a;
			}
			switch(c = *f++) {

			case 0:
				return;
			case 'o':
				base = 8;
				break;
			case 'd':
				sign = 1;
				break;

			case 'x':
				sign = 1; /* a-f in lower case */
			case 'X':
				base = 16;
				break;

			case 's':
				x = *(char **)a;
				a += sizeof(char *)/sizeof *a;
				if(!x)
					x = "(null)";
				i = strlen(x);
dostring:
				if(prec < 0)
					prec = 0;
				if(prec && prec < i)
					i = prec;
				if(width > i)
					width -= i;
				else
					width = 0;
				if(!left)
					while(width--)
						pputc(' ');
				while(i--)
					pputc(*x++);
				if(left)
					while(width--)
						pputc(' ');
				continue;
			case 'c':
				c = *a++;
			default:
				x = &c;
				i = 1;
				goto dostring;

			case 'u':
				break;

			case 'e':
				sign++;

			case 'g':
				sign++;

			case 'f':
				if(prec < 0)
					prec = 6;
				ftype = 1;
				break;
			}
			if(left) {
				left = width;
				width = 0;
			}
			if(prec < 0)
				prec = 0;
			if(ftype) {
				width = _fnum(*(double *)a, prec, width, sign, pputc);
				a += sizeof(double)/sizeof(*a);
			} else {
				width = _pnum((len == sizeof(int)/sizeof *a ? (sign ? (long)*a : (unsigned long)*a) : *(long *)a), prec, width, sign, base, pputc);
				a += len;
			}
			while(left-- > width)
				pputc(' ');
		}
}
