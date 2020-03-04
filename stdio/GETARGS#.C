/*
 *	_getargs(str, name) - process string into argument buffer.
 *
 *	If str is null, read lines from standard input instead, using name as a
 *	prompt.
 *	Continuation lines are recognized by a \ as the last character on the
 *	line.
 *
 *	The redirections recognized are:
 *
 *		>	>>	<
 *
 *	Redirections of the form >& and >>& are not yet supported.
 *	Note that the >> redirections depend on fseek working correctly
 *	on text files, when seeking relative to the end of the file.
 *
 *	Wildcards (? and *) are expanded in the usual way.
 *
 *	The quotes " and ' may be used to enclose arguments which contain
 *	white space the wild characters and > or <.
 *
 *	The argument buffer is sbrk'ed and a pointer to it is returned.
 *	The count of arguments is left in the global _argc_. The count
 *	may also be found by counting the arguments; the last one is a null
 *	pointer.
 *	The zero'th argument is set to the name parameter.
 */

#include	<cpm.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>


#define	MAXARGS	200		/* max number of arguments */
#define	MAXLEN	100		/* max length of an argument */
#define	QUOTE	0x80		/* quoted bit in args */

#define	isterminator(c)	((c) == 0)
#define	look()		(*str)

extern int	_argc_;
extern char *	sbrk();
extern int	isatty(int);
static char *	name, * str, * bp;
static char	interactive;
static char	redone[3];
static void	sputs(), error();
static void	redirect();
static char *	alloc();
static char	nxtch();
static char	iswild(), isspecial(), isseparator();

char **
_getargs(_str, _name)
char *	_str, * _name;
{
	char **		argv;
	struct fcb	fc, * fx;
	register char *	ap;
	char *		cp;
	short		argc;
	char		c, quote;
	unsigned short	i, j;
	char *		argbuf[MAXARGS];
	char		dmabuf[4][32];
	char		buf[MAXLEN];

	bp = (char *)0;
	quote = redone[0] = redone[1] = redone[2] = 0;
	name = _name;
	str = _str;
	if(interactive = str == (char *)0)
		str = "\\";
	argbuf[0] = name;
	argc = 1;

	/* first step - process arguments and do globbing */

	while(look()) {

		if(argc == MAXARGS)
			error("too many arguments", 0);
		while(isseparator(c = nxtch()))
			continue;
		if(isterminator(c))
			break;
		ap = buf;
		if(isspecial(c)) {
			*ap++ = c;
			if(c == '>' && look() == '>')
				*ap++ = nxtch();
		} else {
			while(!isterminator(c) && (quote || !isspecial(c) && !isseparator(c))) {
				if(ap == &buf[MAXLEN])
					error("argument too long", 0);
				if(c == quote)		/* end of quoted string */
					quote = 0;
				else if(!quote && (c == '\'' || c == '"'))
					quote = c;	/* start of quoted string */
				else {
					if(quote)
						c |= QUOTE;
					*ap++ = c;
				}
				if(!quote && isspecial(look()))
					break;
				c = nxtch();
			}
		}
		*ap = 0;
		if(iswild(buf)) {
			ap = buf;
			while(isdigit(*ap))
				ap++;
			quote = *ap == ':' && ap != buf;
			setfcb(&fc, buf);
			bdos(CPMSDMA, dmabuf);
			c = getuid();
			setuid(fc.uid);
			if((j = bdos(CPMFFST, & fc)) != 255)
			do {
				ap = buf;
				if(quote) {
					sprintf(ap, "%u:", fc.uid);
					ap += strlen(ap);
				}
				if(fc.dr) {
					*ap++ = fc.dr + 'A' - 1;
					*ap++ = ':';
				}
				fx = (struct fcb *)dmabuf[j];
				cp = fx->name;
				while(*cp != ' ' && cp < &fx->name[8])
					*ap++ = *cp++ & ~0x80;
				*ap++ = '.';
				cp = fx->ft;
				while(*cp != ' ' && cp < &fx->ft[3])
					*ap++ = *cp++ & ~0x80;
				*ap++ = 0;
				strcpy(argbuf[argc++] = alloc(ap-buf+1), buf);
			} while((j = bdos(CPMFNXT)) != 255);
			else {
				setuid(c);
				error(buf, ": no match", 0);
			}
			setuid(c);
		} else {
			argbuf[argc++] = ap = alloc(ap-buf+1);
			cp = buf;
			do
				*ap++ = *cp & ~QUOTE;
			while(*cp++);
		}
	}

	/* now do redirection */

	for(i = j = 0 ; j < argc ; j++)
		if(isspecial(c = argbuf[j][0])) {
			if(j == argc-1)
				error("no name after ", argbuf[j], 0);
			if(c == '<')
				redirect("input", argbuf[j+1], "r", stdin);
			else {
				ap = argbuf[j][1] == '>' ? "a" : "w";
				redirect("output", argbuf[j+1], ap, stdout);
			}
			j++;
		} else
			argbuf[i++] = argbuf[j];
	_argc_ = i;
	argbuf[i++] = (char *)0;
	argv = (char **)alloc(i * sizeof *argv);
	bmove(argbuf, argv, i * sizeof *argv);
	return argv;
}

static char
nxtch()
{
	if(interactive && *str == '\\' && str[1] == 0) {
		if(!bp)
			bp = alloc(256);
		if(isatty(fileno(stdin)))
			fprintf(stderr, "%s> ", name);
		gets(bp);
		str = bp;
	}
	if(*str)
		return *str++;
	return 0;
}

static void
error(s)
char *	s;
{
	register char **	sp;

	sp = &s;
	while(*sp)
		sputs(*sp++);
	sputs("\n");
	exit(-1);
}

static void
sputs(s)
register char *	s;
{
	while(*s) {
		if(*s == '\n')
			bdos(CPMWCON, '\r');
		bdos(CPMWCON, *s++);
	}
}

static char *
alloc(n)
short	n;
{
	char *	bp;

	if((bp = sbrk(n)) == (char *)-1)
		error("no room for arguments", 0);
	return bp;
}

static void
redirect(str_name, file_name, mode, stream)
char *	str_name, * file_name, * mode;
FILE *	stream;
{
	if(redone[stream-_iob]++)
		error("Ambiguous ", str_name, " redirection", 0);
	if(freopen(file_name, mode, stream) != stream)
		error("Can't open ", file_name, " for ", str_name, 0);
}

static char
iswild(buf)
char *	buf;
{
	return strchr(buf, '*') || strchr(buf, '?');
}

static char
isspecial(c)
char	c;
{
	return c == '<' || c == '>';
}

static char
isseparator(c)
char	c;
{
	return c == ' ' || c == '\t' || c == '\n';
}
