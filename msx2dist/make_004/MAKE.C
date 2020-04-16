#include <stdio.h>
#include <ctype.h>
#define MAXMODS 50
#define DATE unsigned long

int fmake(char *);

/*
 *  MAKE - Maintain seperate source files
 *
 *  SYNOPSIS
 *	MAKE [-f file] [-a] [-n] [-d] [-k] [-v] [name] ...
 *	   f: use 'file' instead of default makefile
 *	   a: assume all modules are obsolete (recompile everything)
 *	   n: don't recompile, just list steps to recompile
 *	   d: debugging (print tree, file info)
 *	   k: keep going after error
 *	   v: verbose (display steps before executing)
 *	   name: module name to recompile
 *
 *  AUTHORS
 *	Landon M. Dyer, Atari Inc.
 *	Arnold Metselaar
 *
 */

char *mfiles[] = {			/* default makefiles */
  "MAKEFILE",
  "MAKEFILE.2",                   /* e.g. DOS2 makefile */
  "MAKEFILE.1",
  ""
};

/* MACRO *mroot = NULL;		*//* root of macro-list */
/* FILENODE *froot = NULL;	*//* root of filenode-list */
/* FILENODE *firstf = NULL;	*//* the very first filenode */
/* FILE *mkfp = NULL;		*//* script file */
char *modnames[MAXMODS];	/* module-names mentioned in commandline */
int modcount = 0;		/* #of module-names */

int obsolete = 0;		/*nonzero: every file should be recompiled */
int debug = 0;			/*nonzero: turn on debugging */
int noexec = 0; 		/*nonzero: no execution of commands */
int keepgoing = 0; 		/*nonzero: don't give up after first error */
int verbose = 0; 		/*nonzero: print commands before executing */

#define clower(a) (isupper(a) ? tolower(a) : a)

/*
 * process a simple option 
 */
static void opt_char(char c)
{
  switch (clower(c)) {
    case 'a': obsolete=1; break;
    case 'd': debug=1; break;
    case 'n': noexec=1; break;
    case 'k': keepgoing=1; break;
    case 'v': verbose=1; break;
    default:
      fprintf(stderr, "Unknown switch: %c\n", c);
    break;
  }
} 

int main(int argc, char** argv)
{
  int arg, i;
  char *mfile = NULL;

  for (arg = 1; arg < argc; ++arg)
  if (*argv[arg] == '-') {
    if ('f'==clower(argv[arg][1])) {
      if (++arg >= argc) {
  	fprintf(stderr, "-f needs filename argument.\n");
  	return 1;
      }
      mfile = argv[arg];
    }
    else 
      opt_char(argv[arg][1]);
  }
  else 
    if (modcount < MAXMODS) {
      if (argv[arg][0]) /* ignore trailing whitespace */
        modnames[modcount++] = argv[arg];
    }
    else {
      fprintf(stderr, "Too many module names.\n");
      return 1;
    }
  if (mfile != NULL) {
    arg = fmake(mfile);
    if (arg == -1)
      fprintf(stderr, "\nCannot open makefile\n");
  } 
  else {
    for (i = 0; *mfiles[i]; ++i)
       if ((arg=fmake(mfiles[i])) != -1) break;
    if (!*mfiles[i])
      fprintf(stderr, "\nCannot open makefile\n");
  }
  if (debug) prtree();
  return arg;
}
