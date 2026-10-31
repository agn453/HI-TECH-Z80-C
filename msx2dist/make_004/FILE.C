#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
 * The header file string.h should declare
 * extern int strcasecmp(char * s1, char * s2);
 * a function that is provided as part of the 'fixes for Hitech libraries'.
 */

#include "make.h"
extern int debug;

/*
 * Return file-node for 'fname'.
 * If it doesn't exist or fname starts with %, then create one.
 */
FILENODE *filenode(char *fname)
{
  FILENODE *f, *afnode(), *gfile();

  if ( (f = gfile(fname)) == NULL )
    f = afnode(fname);

  return f;
}

/*
 * Add a dependency to the node 'fnd'.
 * 'fnd' will depend on 'fname'.
 */
addfile(FILENODE *fnd, char *fname)
{
  NODE *n;
  FILENODE *f;

  if (fnd == NULL) {			/* punt if no root file */
    fprintf(stderr, "No current root, can't add dependency '%s'\n", fname);
    return;
  }
  f = filenode(fname);
  n = (NODE *)xalloc(sizeof(NODE)); /* xalloc checks for out of mem. */
  n->nnext = fnd->fnode;
  fnd->fnode = n;
  n->nfile = f;
}


/*
 * Add a line of method-text to the node 'fnode'.
 */
addmeth(FILENODE *fnode, char *methtext)
{
  int len;
  char * p;

  if (fnode == NULL || methtext == NULL) return;

  len = strlen(methtext) + 2;
  if (fnode->fmake == NULL) {
    p = gmacro(BEFORE);
    /* xalloc checks for out of mem.  */
    if (!p) p="";
    fnode->fmake = (char *)xalloc((strlen(p)+1)); 
    strcpy(fnode->fmake, p);
  }
  len += strlen(fnode->fmake);
  /* realloc doesn't.  */
  if ( (fnode->fmake=realloc(fnode->fmake, len)) == NULL) allerr();

  strcat(fnode->fmake, methtext);
  len = strlen(fnode->fmake);
  if (len && fnode->fmake[len - 1] != '\n')
    strcat(fnode->fmake, "\n");
}


/*
 * Get a filenode for the file called 'fn'.
 * Returns NULL if the node doesn't exist.
 */
FILENODE *gfile(char *fn)
{
  FILENODE *f;

  for (f = froot ; f != NULL ; f = f->fnext)
    if (!strcasecmp(fn, f->fname)) return f;
  return NULL;
}


/*
 * Alloc space for a new file node.
 * To faciiltate %-rules, do not check for 
 * an existing FILENODE with the same name. -AM
 */
FILENODE *afnode(char *name)
{
  FILENODE *f;

  /* we need not check for NULL here; xalloc does that for us */
  f = (FILENODE *)xalloc(sizeof(FILENODE));
  f->fname = (char *)xalloc(strlen(name)+1);
  strcpy(f->fname, name);
  f->fmake = NULL;
  f->fnode = NULL;
  f->fdate = 0L;
  f->fflag = 0;

  f->fnext = froot;
  froot = f;
  return f;
}


/*
 * Print dependency tree.
 */
prtree()
{
  FILENODE *f;
  NODE *n;

  puts("\ndependency tree");
  for (f = froot ; f != NULL ; f = f->fnext) {
    if ( !(f->fflag&DUMMYP) ) {
      printf("%s%s%s", f->fname,
	     (f->fflag & ROOTP) ? " (root)" : "",
	     (f->fflag & REBUILT) ? " (rebuilt)" : "",
	     (f->fflag & FAILED) ? " (failed)" : "");
      if (f->fdate != 0L)
	printf("(%lu)\n", f->fdate);
      else 
	putchar('\n');	
      if (f->fmake != NULL)
        printf("%s", f->fmake);
      for (n = f->fnode ; n != NULL ; n = n->nnext)
        printf("\t%s\n", (n->nfile)->fname);
      putchar('\n');
    }
  }
}

