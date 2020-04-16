#include <stdio.h>
#include <unixio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stat.h>

#ifndef O_TRUNC
#define O_TRUNC 0
#endif
#ifndef O_WRONLY
#define O_WRONLY 0
#endif

extern int _spawn(char*, char*, char *);
extern char *token(char **);

int spawnblock(char *);
char * pathfind(char *);
int spawnline(char *, char *);
extern int verbose;
extern int noexec;

static char * tmpnam = "$M.T";

int mkfile(char *s, char *fds, char **q)
{
  int klen, val;
  FILE * fp; 
  register char * res;
  
  for (klen=0, s+=2 ; isalnum(*s) ; ++s) ++klen;
  s-=klen;
  if (noexec) printf("<<%.*s\n", klen, s);
  for (res=*q ; *res && strncmp(res,s,klen) ; ++res) {
    if ( !(res=strchr(res,'\n')) ) 
      break;
  } 
  if (!res)
    fputs("make: runaway << text\n", stderr);
  else {
    fp=noexec ? stdout : fopen(tmpnam,"w");
    if (!fp) {
      val=errno;
      perror(tmpnam);
      fputs(" while creating temp. file in make:mkfile()\n",stderr);
      return val;
    }
    *res=0;
    if ((fputs(*q,fp)==EOF)||(fputs("\32\n",fp)==EOF)) {
      val=errno;
      perror(tmpnam);
      fputs(" while writing temp. file in make:mkfile()\n",stderr);
      if (!noexec) 
	fclose (fp); 
      return val;
    }
    *res=*s;
    if (!noexec && fclose(fp)) {
      val=errno;
      perror(tmpnam);
      fputs(" while closing temp. file in make:mkfile()\n",stderr);
      return val;
    }
    if (noexec) 
      printf("%.*s\n", klen, s);
    while ( *res && ('\n'!=*res++) ) /* nix */ ;
    memmove(s-2, s+klen, strlen(s+klen)+1);
  }
  if (noexec)
    val=0;
  else {
    val=open(tmpnam, O_RDONLY|O_INHER);
    if (val<0) {
      val=errno;
      perror(tmpnam);
      fputs(" while opening for redirectioning.\n", stderr); 
    }
    else {
      fds[0]=val;
      val=0;
    }
  }
  *q=res;
  return val;
}


struct env_sav {
  char * name;
  char * val;
  struct env_sav * next;
} ;
struct env_sav * env_stack;

/*
 * undo changes made by do_set
 */
void pop_env()
{
  struct env_sav *cur, *nxt;

  for ( cur=env_stack ; cur ; cur=nxt) {
    setenv(cur->name, cur->val);
    free(cur->val);
    nxt = cur->next;
    free(cur);
  }
} 

/* process redirectioning */
int redir(char *p, char *fds)
{
  int mode, append, n, fd;
  char *s, *t;

  n=append=0;
  s=p;
  switch (*s) {
  case '2':
    ++n;
    ++s;
  case '>':
    ++n;
    ++s;
    mode=O_WRONLY|O_INHER;
    if ('>'==*s) {
      append=1;
      ++s;
    }
    break;
  case '<':
    ++s;
    mode=O_RDONLY|O_INHER;
  }
  while (*s && isspace(*s)) 
    ++s;
  for (t=s ; *t && !isspace(*t) ; ++t)  
    ;
  if (noexec||verbose) putchar(*p);
  *p=*t;
  *t=0;
  if (noexec||verbose) {
    printf("%s ",p+1);
    fd=0;
  }
  if (!noexec) {
    if (n && !append) {
      fd=creat(s,0666); /* Ensure the file exists.  */
      close(fd);
    }
    fd=open(s,mode);
    if (fd<0) {
      perror(s);
      fputs(" while opening for redirectioning.\n", stderr);
    }
    else {
      fds[n]=fd;
      if (append) lseek(fd, 0, SEEK_END);
    }
  }
  if (*p) memmove(p+1, t+1, strlen(t+1)+1);
  return (fd<0)?errno:0;
}


/* 
 * Executes a subprocess for each line in blok.
 * Stops when a subprocess returns a nonzero exit-status
 * If a line starts with ':', the ':' is skipped and the exitstatus is ignored.
 * return value is exit-status of last subprocess executed, 
 * or zero if the last line processed starts with ':'
 *
 * if a line contains "<<" followed by a word, the following lines
 * until a line starting with the word are copied to '$M.T'.
 * "<<" and the word are removed from the line and '$M.T' is open for input.
 */

int spawnblock(char *blok)
{
  int res, i;
  char *linbuf, *p, *q, *s;
  char quote, heredoc, fds[3];

  env_stack = NULL;
  linbuf=malloc(strlen(blok)+1); 
  if (!linbuf) {
    fputs("*** Out of memory\n",stderr);
    return 0xDE;
  }
  heredoc=0;
  for (p=strcpy(linbuf,blok),res=0 ; (res==0) && (*p) ; p=q) {
    fds[0]=0; fds[1]=1; fds[2]=2;
    for (q=p ; (*q)&&(*q!='\n') ; ++q) /* nix */;
    if (*q=='\n') *q++='\0';
    /* messy; obey "" and '' */
    for (s=p ; *s && (res==0); ) {
      switch (*s) {
      case '"':
      case '\'':
	quote=*s++;
	while (*s && (*s++ != quote)) 
	  ;
	break;
      case '>':
	res=redir(((s>p) && ('2'==s[-1])) ? s-1 : s, fds);
	break;
      case '<':
	if ('<'==s[1]) {
	  res=mkfile(s, fds, &q);
	  heredoc=!noexec;
        }
        else
	  res=redir(s, fds);
	break;
      default:
	s++;
      } 
    }
    if (res) break;  
    res = spawnline(p,fds);
    for ( i=0 ; i<3 ; ++i) 
      if (fds[i]>2) 
	close(fds[i]);
    if (heredoc) {
      unlink(tmpnam);
      heredoc=0;
    }
  }
  pop_env();
  free(linbuf);
  return res;  
}

/*
 * change the environment like set would do, but store the old value
 */
int do_set(char * line)                                             	
{
  char * q;
  struct env_sav * penv;

  q=token(&line);
  penv=malloc(sizeof(struct env_sav));
  if (penv) {
    penv->val=getenv(q);
    penv->next=env_stack;
    setenv(q,line);
    return 0;
  }
  else {
    fputs("*** Out of memory\n",stderr);
    return 0xDE;
  }
}


char *shell = NULL;

/*
 * executes one subprocess.
 * if the first token in line can be found in the command search path,
 * the file is loaded and executed directly, 
 * otherwise line is interpreted by the program pointed to by the 
 * environment variable SHELL.
 */ 
int spawnline(char *line, char *fds)
{
  char *command, *progfile, ignore;
  static char set[]="set";
  int res;

  if (':' == *line) {
    ignore=1;
    ++line;
  }
  else
    ignore=0;

  command=token(&line);
  if (0 == strcasecmp(&set, command))
    progfile=&set;
  else 
    progfile=pathfind(command);

  if (progfile==NULL) {
    if (!shell) shell=getenv("SHELL");
    if (!shell) {
      fprintf(stderr,
              "make: '%s' not found in path, and 'SHELL' not in environment\n",
	      command);
      return 0x8E; /* unrecognised command */
    }
    else {
      progfile=shell;
      if (*line) *strchr(command, '\0')=' ';
      line=command;
    }
  }

  if (progfile!=&set && strlen(line)>126) {
     fprintf(stderr,"%s %.20s... : Linebuffer overflow\n", progfile, line);
     res=0x8D; /* BUFUL */
  }  
  else {    
    if (verbose||noexec) {
      printf(noexec ? "%s %s\n": "[MAKE] %s %s\n", progfile, line);
    }
    if (progfile==&set)
      res=do_set(line);
    else
      res=noexec ? 0 : _spawn(progfile, line, fds);
    if (progfile == shell) {
      if (res==0x8C) 
	res=0; /* internal 'error' code meaning no error */
      else
	command=token(&line);
    }
  }
  if (res && (!ignore || verbose))
     fprintf(stderr, "'%s' failed with error code %d%s.\n", 
		     command, res, ignore ? " (ignored)" : "");
  return (ignore ? 0 : res);
}

