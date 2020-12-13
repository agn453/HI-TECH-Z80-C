#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#if	! unix
/* this is a kludge for a bug in the Venix cpp */
#endif

void			error(char *fmt, ...);
void			put2(unsigned short i);
void			put4(unsigned long i);
unsigned long	get4(void);
unsigned short	get2(void);

#if	! unix
#include <ctype.h>

int casecmp(register char *, char *);

int casecmp(register char * s1, char * s2) {
	char a, b;

	while((a = *s1++)) {
		b = *s2++;
		if(isupper(a))
			a = tolower(a);
		if(isupper(b))
			b = tolower(b);
		if(a != b)
			return 1;
	}
	return 0;
}
#endif	/* unix */

/*========================================
void error(char *s, char* a1, char* a2) {

	fprintf(stderr, "huff: fatal error: ");
	fprintf(stderr, s, a1, a2);
	fputc('\n', stderr);
	exit(1);
}
 ========================================*/
void error(char *fmt, ...) {
	va_list ap;			/* points to the next unnamed argument */
	char *p, *sval;

	fprintf(stderr, "huff: fatal error: ");
	va_start(ap, fmt);	/* set ap to 1st unnamed argument */

	for(p=fmt; *p; p++) {
		if(*p !='%') {
			putchar(*p);
			continue;
		}
		switch(*++p) {
		  case 's':
			for(sval = va_arg(ap, char *); *sval; sval++) fputc(*sval, stderr);
	    	break;
		  default:
			fputc(*p, stderr);
			break;
		}
	}
 	va_end(ap); 		/* cleaning when done */
 	fputc('\n', stderr);
 	exit(1);
}

/*
 *
 */
void put2(unsigned short i) {

	putchar(i & 0xFF);
	putchar(i >> 8);
}

/*
 *
 */
void put4(unsigned long i) {
	put2(i & 0xFFFFL);
	put2(i >> 16);
}

/*
 *
 */
unsigned long get4(void) {
	unsigned long i;

	i =  (unsigned long)getchar();
	i += (unsigned long)getchar() << 8;
	i += (unsigned long)getchar() << 16;
	i += (unsigned long)getchar() << 24;
	if(feof(stdin))
		error("EOF on input file");
	return i;
}

/*
 *
 */
unsigned short get2(void) {
	unsigned short i;

	i =  (unsigned short)getchar();
	i += (unsigned short)getchar() << 8;
	if(feof(stdin))
		error("EOF on input file");
	return i;
}
