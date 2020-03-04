/*
 *	freopen.c - stdio freopen 
 */

#include	<stdio.h>

extern int	open(char *, int), creat(char *, int);
extern int errno;

FILE *
freopen(name, mode, iob)
char *	name, * mode;
register FILE *	iob;
{
	uchar		c,pls;

	fclose(iob);
	pls = c = 0;
	iob->_flag &= _IONBF;

	if (mode[1]=='+' || (mode[1] && mode[2]=='+')) pls=2;

	switch(*mode) {

	case 'w':
		c++;
	case 'a':
		c++;
	case 'r':
		if(mode[1]=='b' || (mode[1] && mode[2]=='b'))
			iob->_flag = _IOBINARY;
		break;

	}
	switch(c) {

	case 0:		/* "r" or "r+" */
		iob->_file = open(name, pls); /* pls is 0 if R/O, 2 if R/W */
 		break;

	case 1:
		if((iob->_file = open(name, (1 | pls) )) >= 0) /* "a" or "a+" */
			break;
		/* else fall through */
	case 2:
		iob->_file = creat(name, 0666);		/* "w" */
                if (!pls) break;
                close(iob->_file);
                iob->_file = open(name, 3);             /* "w+" */
                if (errno == 7) errno = 5;
		break;
	}
	if(iob->_file < 0)
		return (FILE *)NULL;
	if(!(iob->_flag & (_IONBF|_IOMYBUF)))
		iob->_base = _bufallo();
	if(iob->_base == (char *)-1) {
		iob->_base = (char *)0;
		close(iob->_file);
		iob->_flag = 0;
		return (FILE *)NULL;
	}
	iob->_ptr = iob->_base;
	iob->_cnt = 0;
	if(c)
		iob->_flag |= _IOWRT;	/* w a  only allow writing */
	else
		iob->_flag |= _IOREAD;	/* r only allows reading */
	if(pls) iob->_flag |= _IOWRT | _IOREAD;	/* r+ a+ w+ all allow R/W */

	if(iob->_base && c)
	{
		iob->_cnt   = BUFSIZ;     /* Buffer empty */
		iob->_flag |= _IODIRN;	  /* Defaults to write */

		if (c==1) iob->_flag |= _IOAPPEND; /* Append mode on? */
	}

	if(c == 1)			/* a or a+ */
		fseek(iob, 0L, 2);
	return iob;
}
