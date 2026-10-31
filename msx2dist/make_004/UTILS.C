#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <conio.h>
#include "make.h"
int fmake(char *);
int examine(FILENODE *, DATE);
int recomp(FILENODE *);
int determ();

extern int laterdt(DATE,DATE);
extern void defmac(char *, char *);
extern void catmac(char *, char *);
extern void diffmac(char *, char *);
extern int undefmac(char *);
extern int spawnblock(char*);
extern int strcasecmp(char*, char*);
void finish(FILENODE *f);

#define PADLEN	64			/* maximum length of filename */ 

MACRO *mroot = NULL;		/* root of macro-list */
FILENODE *froot = NULL; 	/* root of filenode-list */
FILENODE *firstf = NULL;	/* the very first filenode */
FILE *mkfp = NULL;		/* script file */
extern int modcount;
extern int debug;
extern int obsolete;
extern int keepgoing;
extern int verbose;
extern int noexec;
extern char *modnames[MAXMODS]; /* module-names mentioned in commandline */

#define endoftime ULONG_MAX	/* a date, the very last possible */

void fparse(FILE *fp);

/*
 * Construct dependency tree from the makefile 'fn'.
 * Figure out what has to be recompiled, and write a script file to do that.
 */

int fmake(char * fn)
{
  int res;
  FILE *fp;

  if (0 == strcmp("-",fn))
    fp = stdin;
  else  
    if ((fp = fopen(fn, "r")) == NULL) return -1;
  fparse(fp);
  res=determ();
  
  fclose(fp);
  return res;
}

char * mexpand(char *orig, char macchar);

/*
 * Parse the input file, defining macros and building the dependency tree.
 */
void fparse(FILE *fp)
{
  char *strp, *tok1, *tok2, *s;
  FILENODE *lastf = NULL;
  char *ebuf, ibuf[STRSIZ];

  for (;; free(ebuf)) {
    if (fgets(ibuf, STRSIZ, fp) == NULL) break;
    /* changed prototype of mexpand() to handle long expansions - AM */
    ebuf = mexpand(ibuf, MACCHAR);
    escape(ebuf, COMCHAR);

    /* clobber last newline in string */
    s = ebuf + strlen(ebuf) - 1;
    if (s >= ebuf && *s == '\n') *s = '\0';

    if (*ebuf == '\t') {
      addmeth(lastf, ebuf+1);
      continue;
    }

    strp = ebuf;
    if ((tok1 = token(&strp)) == NULL) continue;
    if ((tok2 = token(&strp)) != NULL) {
      if (!strcmp(tok2, DEFMAC)) {
         if (*strp) defmac(tok1, strp);
	 else 
	   if (undefmac(tok1) < 0)
	     fprintf(stderr,"Can't undefine macro '%s'\n", tok1);
	 continue;
      }
      else if (!strcmp(tok2, CATMAC)) {
         if (*strp) catmac(tok1, strp);
         else fprintf(stderr,"'%s %s' ignored.\n", tok1, CATMAC);
	 continue;
      } 
      else if (!strcmp(tok2, DIFFMAC)) {
         if (*strp) diffmac(tok1, strp);
         else fprintf(stderr,"'%s %s' ignored.\n", tok1, DIFFMAC);
	 continue;
      } 
      else if (!strcmp(tok2, DEPEND)) {
	 finish(lastf);

	 lastf = filenode(tok1);
	 if (firstf == NULL) firstf = lastf;
	 lastf->fmake = NULL;

	 /* ${~BEFORE} is prepended by addmeth(), 
          * if the file contains a method line for this root. */

	 lastf->fflag |= ROOTP;
	 while ((tok1 = token(&strp)) != NULL)
            addfile(lastf, tok1);
	 continue;
      }
      else addfile(lastf, tok2);
    }
    do {
      addfile(lastf, tok1);
    } while((tok1 = token(&strp)) != NULL);
  }
  finish(lastf);
}

int match(char *buf, char *pat, char *name);
char *subs(char *x, char *s); 

/*
 * Called to finish the parsing of a dependency rule.
 * The value of AFTER is added to the method.
 * If the rule is a %-rule then a new rule is generated for each
 * target in the expansion
 */
void finish(FILENODE * fnd)
{
   NODE *dep, *rp, *dp;
   FILENODE *rfp; 
   char *cp, *xp, *fn, *rest;	
   char stem[PADLEN];

   if (fnd->fmake)
     addmeth(fnd, gmacro(AFTER));
   if (fnd->fname[0]=='%') {
     fnd->fflag |= DUMMYP ;
     for ( rp=fnd->fnode ; rp&&strcmp(rp->nfile->fname,DEPEND) ; rp=rp->nnext)
	if (strchr(rp->nfile->fname, '%')) rp->nfile->fflag |= DUMMYP ;
     if ( (dep=rp) )
	dep->nfile->fflag |= DUMMYP ;
     else {
	fprintf(stderr, "Can't find second '%s' in %%-rule.\n", DEPEND);
	exit(2);
     } 
     /* the 'dependencies' in the linked list are in reverse order */
     if (dep->nnext == NULL) {
	fprintf(stderr, "In %%-rule: No macro name given.\n");
	exit(2);
     }
     fn = dep->nnext->nfile->fname;
     if ( (cp=gmacro(fn)) == NULL ) {
	fprintf(stderr, "In %%-rule: macro '%s' not defined.\n", fn);
	exit(2);
     }
     if (debug) {
       puts("%-rule:");
       printf("pattern ='%s'\n",fnd->fname);
       printf("$(%s)='%s'\n", fn, cp);
     }
     dep->nnext->nfile->fflag |= DUMMYP ; 
     xp=xalloc(strlen(cp)+1); /* xalloc checks for out of mem. */
     rest=strcpy(xp,cp);
     while ( (fn=token(&rest)) ) {
	if (! match(stem, fnd->fname, fn))
	   fprintf(stderr, "'%s' does not match '%s'.\n", 
		   fn, fnd->fname);
	else {
	   if (debug) printf("\t%s\n",fn);
	   rfp=filenode(fn);
	   rfp->fflag |= ROOTP;
	   for ( dp=fnd->fnode ; dp!=dep ; dp=dp->nnext ) {
	      cp=subs(dp->nfile->fname, stem);
	      addfile(rfp, cp);
	      free(cp);
	   }
	   if (fnd->fmake) 
	     rfp->fmake = subs(fnd->fmake, stem);
	}
     }
     free(xp);
     for ( rp=fnd->fnode ; rp ; rp=dp) {
	dp=rp->nnext;
	/* if (rp->nfile->fflag & DUMMYP) frnode(rp->nfile); */
	free(rp);
     }
     fnd->fnode=NULL;
     if (fnd->fmake) {
	free(fnd->fmake);
	fnd->fmake=NULL;
     }
   } /* endif (fnd-->fname[0]=='%') */
}

/*
 * 'matches' name against the pattern;
 * pat must begin with a '%', and the end of name must
 * be equal to the second to last characters in pat.
 * if this is the case then the beginning of name is stored in buf and
 * returns 1 on success, 0 otherwise
 */
int match(char *buf, char *pat, char *name)
{
   int len;

   len=strlen(name)-strlen(pat+1);
   if (len>=PADLEN) {
	fprintf(stderr, "'%s' : too long!\n");
	return 0;
   } 
   if (strcasecmp(name+len,pat+1)!=0)
      return 0;
   strncpy(buf, name, len);
   buf[len]='\0';
   return 1;
}

/*
 * returns a newly malloc()ed string, formed from x by 
 * replacing each occurence of '%' with s.
 */
char *subs(char *x, char *s)
{
   char *p, *res;
   int len, slen;

   slen=strlen(s);
   for (p=x,len=0 ; (*p) ; ++p)
      len+=(*p=='%') ? slen : 1; 
   res=xalloc(len+1); /* xalloc() checks for out of mem. */
   for (p=res ; (*x) ; ++x) {
      if (*x=='%') {
	strcpy(p,s);
	p+=slen;
      }
      else
	*p++ = *x;
   }
   *p='\0';
   return res;
}
/*
 * Determine sequence of recompiles from the creation dates.
 * If there is anything to recompile, 
 * then do it by calling recomp().
 */
int determ()
{
  FILENODE *f;
  int i,res;
  char *m;

  if (firstf == NULL) {			/* empty tree */
    printf("No changes\n");
    return 0;
  }
  res=0;
  if (modcount == 0) 
    res=(examine(firstf, endoftime)==FAILED)?1:0;
  else 
    for (i = 0; (i < modcount) && (!res || keepgoing) ; ++i) {
      if ((f = gfile(modnames[i])) == NULL) {
	fprintf(stderr, "Can't find root '%s'.\n", modnames[i]);
	continue;
      }
      if ((f->fflag & ROOTP)== 0) {
        fprintf(stderr, "'%s' is not a root!\n", f->fname);
        continue;
      }
      res=(examine(f,endoftime)==FAILED)||res?1:0;
    }

  if (mkfp != NULL) {
    m=gmacro(DEINIT);
    if (m)
      res=(spawnblock(m) || res)?1:0;
  }
  else
    printf("No changes\n");

  return res;
}


/*
 * Examine filenode 'fnd' and see if it has to be recompiled.
 * 'date' is the last-touched date of the node's father
 * (or 'endoftime' if its a root file.)
 * Returns REBUILT if recompilation was successful,
 *         FAILED if recompilation failed and 
 *         0 otherwise
 *
 * Root files with NO dependencies are assumed not to be up to date.
 */
int examine(FILENODE *fnd, DATE date)
{
  int rebuildp = 0;
  NODE *n;

  if (debug) {
    printf("\n examine(%s, ", fnd->fname);
    if (endoftime == date) 
      printf("endoftime) {");
    else
      printf("%lu) {", date);
  }

  getdate(fnd);

  if ((fnd->fnode == NULL) && (fnd->fflag & ROOTP)) {
    if (debug) puts("root w/o dependencies");
    rebuildp = REBUILT;
  }
  else 
    for ( n=fnd->fnode ;
          (n!=NULL) && (keepgoing || ((rebuildp&FAILED)==0)) ;
          n=n->nnext) {
      if (n->nfile->fflag & FAILED) {
        rebuildp = FAILED;
        fnd->fflag |= FAILED;
      }
      else 
        rebuildp |= examine(n->nfile, fnd->fdate);
  }
  if (rebuildp & FAILED)
    fprintf(stderr, "'%s' not remade because of errors\n", fnd->fname);
    
  if ( REBUILT==(rebuildp&(REBUILT|FAILED)) ) {
    if (debug) putchar('\n');
    if (recomp(fnd)) rebuildp=FAILED;
  }
  if (laterdt(fnd->fdate, date) > 0) {
     if (debug) printf ("newer; %lu ", fnd->fdate);
     rebuildp |= REBUILT;
  }
  if (obsolete || (date==endoftime) )
    rebuildp |= REBUILT;

  if (debug) printf(" %s }", rebuildp 
			   ? ( (rebuildp&FAILED) ? "failed" : "rebuilt" )
			   : "uptodate" );
  return (rebuildp&FAILED)?FAILED:rebuildp;
}


/*
 * Make sure a filenode gets recompiled.
 * returns non-zero on faillure
 */
int recomp(FILENODE *f)
{
/* FILENODE *sf; */
  char *m;
  int res;

  res = 0;
  if (mkfp == NULL) {
    mkfp = stdout;
    if ((m = gmacro(INIT)) != NULL)
      res=spawnblock(m);
  }
  if (f->fflag & REBUILT) return res;
  if (verbose||debug) printf("** (re)compiling %s:\n",f->fname);
  if (f->fmake != NULL)
    res=spawnblock(f->fmake);
  f->fflag |= (res ? FAILED : REBUILT);

  return res;
}

/*
 * Try to allocate memory, and check for NULL	
 */
void *xalloc(unsigned len)
{
  void * res;
  res=malloc(len);
  if (!res) allerr(); /* does not return */
  return res;
}

/*
 * Complain about being out of memory, and then die.
 */
allerr() {
  fprintf(stderr, "Can't alloc -- no space left (I give up!)\n");
  noexec=1;
  exit(1);
}

