#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stat.h>

extern _flip(char *, char *, int); /* substitute '\\' for '/' */ 

#define MAXLEN 66
static char **path_elt;
static char padbuf[MAXLEN+1];
static char *nopath[]={"", NULL};

/*
 * Tries to find command s in the search path.
 * Returns the filename if it is found and
 * NULL otherwise.
 */
char * pathfind(char* s)
{
  char *pad,*p;
  int n,i;
  struct stat dummy;

  if (path_elt==NULL) {
    pad=getenv("PATH");
    if ((!pad)||strlen(pad)==0) path_elt=nopath;
    else {
      for (p=pad, n=1 ; *p ; ) if (*p++ == ';') ++n;
      path_elt=malloc((n+1)*sizeof(char*)); 
      if (path_elt) {
	path_elt[0]=pad;
	for (i=0,p=pad ; (*p) ; )
	  switch (*p) {
	    case ';':
	      *p='\0';
              path_elt[++i]=++p;
              break;
            case ' ':
              ++path_elt[i];
              /* fall through */
            default:
              ++p;
          }
        path_elt[++i]=NULL;
      }
      else {
	fputs("Not enough memory, ignoring $PATH\n",stderr);
	path_elt=nopath;
      }
    }
  }
  
  for (i=0,pad=NULL ; path_elt[i] && !pad ; ++i)
    if (strlen(path_elt[i])+strlen(s)+5<=MAXLEN) {
      strcpy(padbuf,path_elt[i]);
      if (*padbuf)
        switch (padbuf[strlen(padbuf)-1]) {
        case ':': 
        case '/': 
        case '\\':
          break;
        default: 
          strcat(padbuf,"\\");
        }
      strcat(strcat(padbuf,s),".COM");
      if (stat(padbuf, &dummy)==0) { 
	_flip(padbuf, padbuf, MAXLEN+1);
	pad=padbuf;
      } 
    }
  return pad;
}


