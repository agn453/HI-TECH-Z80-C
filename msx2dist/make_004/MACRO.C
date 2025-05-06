#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "make.h"
extern char * strcasestr(char *, char *);

extern int debug;

/*
 * Macro processing
 */

static int emacro(char *name, char * dest);

/*
 * Perform macro substitution from 'orig' to newly allocated string.
 * Return pointer to the string.
 * A macro reference is in one of two forms:
 *		<MACCHAR>(macro-name)
 *  	or	<MACCHAR><single-character>
 *
 * "<MACCHAR><MACCHAR>" expands to a single '<MACCHAR>'
 */
char * mexpand(char *orig, char macchar)
{
  int ddi, destsiz;
  char *s, *d, *dest, mname[STRSIZ];

  destsiz=strlen(orig);
  for (s=orig; *s;)
    if (*s == macchar) {
      if (*++s == macchar) {
	destsiz--;
	s++;
	continue;
      }
      if (!*s) {
	destsiz--;
	break;
      }
      d = mname;
      if (*s != '(') {
	*d++ = *s++;
	destsiz-=2;
      }
      else {
	for (++s; *s && *s!=')';) *d++ = *s++;
	destsiz-=d-mname+3;
	if (*s != ')') 
	  destsiz++;
	else ++s;
      }
      *d = 0;
      destsiz += emacro(mname, NULL);
    } 
    else
      s++;
  dest=xalloc(destsiz+1);
  ddi = 0;
  for (s=orig; *s;)
    if (*s == macchar) {
      if (*++s == macchar) {
	dest[ddi++] = *s++;
	continue;
      }
      if (!*s) break;
      d = mname;
      if (*s != '(') *d++ = *s++;
      else {
	for (++s; *s && *s!=')';) *d++ = *s++;
	if (*s != ')') puts("Missed matching ')'");
	else ++s;
      }
      *d = 0;
      ddi += emacro(mname, dest+ddi);
    } 
    else 
      dest[ddi++] = *s++;

  dest[ddi]=0;
  if (strlen(dest)!=destsiz) {
    fprintf(stderr, "strlen(dest)=%d, destsiz=%d.\n",strlen(dest),destsiz);
    exit(2);
  } 
  return dest;
}


/*
 * Lookup a macro, possible performing 
 * a substitution.
 * Return number of chars in the expansion.
 */
static int emacro(char *name, char * dest)
{
  char *end, *pat, *rep, *def;
  int res, pl, rl;

  end=strchr(name, ':');
  if (end) {
    *end = 0;
    def=gmacro(name);
    if (!def) {
      if (dest) 
	fprintf(stderr, "Undefined macro: %s\n", name);
      return 0;
    }
    *end = ':';
    pat = end+1;
    rep = strchr(pat, '=');
    res = 0;
    if (!rep) {
      if (dest) fprintf(stderr, 
			"$(%s): missing '=' in macro substitution\n", name);
    }
    else {
      pl = rep-pat;
      ++rep;
      rl = strlen(rep);
      end = def+strlen(def)-pl;
      while (def <= end) {
	if ((0 == def[pl] || isspace(def[pl])) 
	    && 0 == strncmp(def, pat, pl)) {
	  res += rl;
	  def += pl;
	  if (dest) {
	    strcpy(dest, rep);
	    dest += rl;
	  }
	}
	else {
	  if (dest) *(dest++) = *def;
	  res++;
	  def++;
	}
      }
      res += strlen(def);
      if (dest) strcpy(dest, def);
    }
  }
  else {
    def = gmacro(name);
    if (!def) {
      if (dest) 
	fprintf(stderr, "Undefined macro: %s\n", name);
      return 0;
    }
    res = strlen(def);
    if (dest) strcpy(dest, def);
  }
  return res;
}


/*
 * Define a macro.
 * Give the macro called 'name' the string expansion 'def'.
 * Old macro-names are superseded, NOT replaced.
 * exit with error-message if can't define the macro.
 */
void defmac(char *name, char *def)
{
  MACRO *m;

  /* we need not check for out of mem. here; xalloc does that for us */
  m = (MACRO *)xalloc(sizeof(MACRO));
  m->mname = (char *)xalloc(strlen(name)+1);
  m->mvalue = (char *)xalloc(strlen(def)+1);

  strcpy(m->mname, name);
  strcpy(m->mvalue, def);
  m->mnext = mroot;
  mroot = m;
}

/*
 * Searches tok in lst, 
 * tok must appear as a separate token,
 * comparison is case insensitive.
 */
char * tok_find(char * lst, char *tok)
{
   char * srch, c;
   int toklen;

   toklen=strlen(tok);
   for (srch = lst ; ; ) {
      srch = strcasestr(srch, tok);
      if (!srch)
	 break;
      c=srch[toklen];
      if (isspace(c)) c=0;
      if ( !c && (srch == lst || isspace(srch[-1])) ) 
	 break;
      else
	 srch += toklen;
   }
   return srch;
}

/*
 * diffmac - remove tokens from macro.
 *
 * All tokens in the old value the macro that do not appear as token
 * in del are copied to the new value.
 * The old value is superseded, NOT replaced.
 */
void diffmac(char *name, char *del)
{
   MACRO *m;
   char *cp, * newval;
   char *next, *last;

   for ( m=mroot ; (m!=NULL) && (strcmp(name, m->mname)!=0) ; m=m->mnext )
      ;

   if ( m!=NULL ) {
      cp = m->mvalue;
      newval = xalloc(strlen(cp)+1);
      for (next=newval ; *cp ; ) {
	 while (isspace(*cp)) cp++;
	 if (*cp) {
	    last = next;
	    while (*cp && !isspace(*cp)) *next++ = *cp++;
	    *next = 0;
	    if (tok_find(del, last))
	       next = last;
	    else
	       *next++ = ' ';
	 }
      }
      *next = 0;
      defmac(name, newval);
      free(newval);
   }
   else
      fprintf(stderr, "%s -= ... ignored because the macro is undefined.\n", 
	      name);
}

/*
 * Replaces the macro called name with 
 * the concatenation of its value and suf.
 * If there is no macro called name, a new
 * macro called name with value suf is created
 */ 
void catmac(char * name, char * suf)
{
   MACRO *m;
   char *cp;

   for ( m=mroot ; (m!=NULL) && (strcmp(name, m->mname)!=0) ; m=m->mnext )
      ;

   if ( m!=NULL ) {
      if ( (cp=realloc(m->mvalue, strlen(m->mvalue)+strlen(suf)+2)) == NULL ) 
	 allerr();
      m->mvalue=strcat(strcat(cp," "), suf);
   }
   else {
      fprintf(stderr, "Macro %s not yet defined.\n", name);
      defmac(name, suf);
   }
} 

/*
 * undefmac - undefine a macro.
 * Return 0 if macro was succesfully undefined, -1 if not found.
 */
int undefmac(char * name)
{
  MACRO *m = mroot;
  MACRO *prev = NULL;

  while (m != NULL && strcmp(name, m->mname)) {
    prev = m;
    m = m->mnext;
  }

  if (m == NULL) return -1;
  if (prev == NULL) mroot = m->mnext;
  else prev->mnext = m->mnext;

  free(m->mname);
  free(m->mvalue);
  free(m);
  return 0;
}


/*
 * Lookup a macro called 'name'.
 * Return a pointer to its definition,
 * or NULL if it does not exist.
 */
char *gmacro(char *name)
{
  MACRO *m;
        
  for (m=mroot; m != NULL; m=m->mnext)
    if (!strcmp(name, m->mname)) return m->mvalue;
  return NULL;
}

