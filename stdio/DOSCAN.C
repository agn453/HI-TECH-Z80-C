/*
 *	_doscan - implement scanf, fscanf, sscanf
 */

#include	<stdio.h>
#include 	<ctype.h>

extern int	atoi();

static FILE *	fp;

static
range(c, base)
int	c;
uchar	base;
{
	if(isdigit(c))
		c -= '0';
	else
		{
		if (isupper(c))
			c = tolower(c) ;
		if (isalpha(c))
			c = c - 'a' + 10 ;
		else
			return -1 ;
		}
	if (c >= base)
		return -1 ;
	return c ;
}

static
wspace()
{
	int	c;

	while(isspace(c = getc(fp)))
		continue;
	if(c != EOF)
		ungetc(c, fp);
}

_doscan(file, fmt, args)
FILE *		file;
register char *	fmt;
int **		args;
{
	uchar	c, sign, base, n, noass,len;
	char	width ;
	char *	sptr;
	int	ch;
	long	val;

	fp = file;
	n = 0;
	while(c = *fmt++) {

		len = 0 ;
		if(isspace(c)) {
			wspace();
			continue;
		}
		if(c == '%') {
			noass = 0;
			width = 0;
loop:
			switch(c = *fmt++) {

			case '\0':
				return n ? n : feof(fp) ? EOF : 0;

			case '*':
				noass++;
				goto loop;

			case 'l':
				len++;
			case 'h':
				goto loop;

			case 'D':
				len++;
			case 'd':
				base = 10;
				break;

			case 'O':
				len++;
			case 'o':
				base = 8;
				break;

			case 'X':
				len++;
			case 'x':
				base = 16;
				break ;

			case 's':
				wspace();
				if ( !noass )
					sptr = (char *)*args++;
				if ((ch = getc(fp)) == EOF )
					return n ? n : EOF;
				while(ch && ch != EOF && !isspace(ch)) {
					if(ch == *fmt) {
						fmt++;
						break;
					}
					if ( !noass ) 
						*sptr++ = ch;
					if(--width == 0)
						break;
					ch = getc(fp);
				}
				n++;
				if ( !noass ) 
					*sptr = 0;
				continue;

			case 'c':
				if ( !noass )
					sptr = (char *)*args++;
				do {
					if ((ch = getc(fp)) == EOF) 
						return n ? n : EOF;
					if ( !noass )
						*sptr++ = ch;
				} while(--width > 0);
				n++;
				continue;
			default:
				if(isdigit(c)) {
					width = atoi(fmt-1);
					while(isdigit(*fmt))
						fmt++;
					goto loop;
				}
				if(c != (ch = getc(fp)))
					if(ch == EOF)
						return n ? n : EOF;
					else {
						ungetc(ch, fp);
						return n;
					}
				continue;
			}
			wspace();
			val = 0;
			sign = 0;
			ch = getc(fp);
			if(ch == '-') {
				sign++;
				ch = getc(fp);
			}
			if(range(ch, base) == -1)
				return n ? n : feof(fp) ? EOF : 0;
			do {
				val = val * base + range(ch, base);
			} while (( --width != 0 ) && ( range(ch = getc(fp),base) != -1 )) ;
			n++;
			if (range(ch,base) == -1)
				ungetc(ch, fp);
			if(sign)
				val = -val;
			if ( !noass )
				if(len)
					*(long *)*args++ = val;
				else
					**args++ = val;
			continue;
		} else if(c != (ch = getc(fp))) {
			if(ch != EOF) {
				ungetc(ch, fp);
				return n;
			} else
				return n ? n : EOF;
		}
	}
	return n;
}
