/* xargs.c - simple xargs program */

/* xargs [-a file] [-t|--no-exec|-b] <command> [initial-args ...]
 * read token from stdin and construct/execute subcommands;
 * each subcommand is of the form 
 * <command> [initial-args ...] <tokens from stdin>
 * each token read from stdin is passed to one subcommand
 */

#include <unixio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * pathfind(char* s);
int _spawn(char*,char*,char*);
char *token(char**);

/* max. total length of arguments (for cp/m and msxdos(2)): */
#define ARGLEN 126
/* require some margin for the extra arguments */
#define MARGIN 12

#define BUFLEN 512
int verbose, do_exec;

void print_help(FILE* fp)
{
  static char *help[]={
  "Usage: xargs [options] [command [initial args...]]",
  "",
  "Execute command repeatedly with arguments read from standard input.", 
  "Options can be",
  "--help\t: print this help and exit",
  "--no-exec\t: do not execute the commands, implies -t",
  "--\t: marks end of options to xargs, the next item is used as the command",
  "-a file\t: read arguments from file ratherthan stdin",
  "-b\t: generate batch file; do not search command in PATH and",
  "\t  do not execute the commands, implies -t",
  "-t\t: type out commands",
  NULL
  };
  int i;
  for (i=0 ; help[i] ; ++i) {
    fputs(help[i],fp);
    fputc('\n', fp);
  }
}

static char *shell;
int parse(int argc, char *argv[], char **name, char **prog, char **iniargs)
{
  int i,batch, rest, res, len;
  char *p;
  FILE * help;

  *name=NULL;
  help=NULL;
  batch=verbose=0; 
  do_exec=1;
  for (i=1, rest=argc ; i<rest ; ++i) {
    if ('-' != argv[i][0]) {
      rest=i;
      break;
    }
    switch (argv[i][1]) {
    case 'a': 
      if (argv[i][2])
	*name=argv[i]+2;
      else {
	if (++i < argc)
	  *name=argv[i];
	else {
	  fprintf(stderr, "-a option needs filename");
	  help=stderr;
	  res=1;
	}
      }
      break;
    case 'b':
      batch=1;
      break;
    case 't':
      verbose=1;
      break;
    case '-':
      if (argv[i][2]) {
	if (0==strcmp(argv[i]+2,"help")) {
	  help=stdout;
	  res=0x8C;
	}
	else if (0==strcmp(argv[i]+2,"no-exec")) {
	  do_exec=0;
	  verbose=1;
	}
	else {
	  fprintf(stderr, "unknown option: %s", argv[i]);
	  help=stderr;
	  res=1;
	}
      }
      else
	rest=i+1;
      break;
    default:
      fprintf(stderr, "unknown option: %s", argv[i]);
      help=stderr;
      res=1;
    }
  }
  if (help) {
    print_help(help);
    return res;
  }
  if (batch || rest==argc) {
    verbose=1;
    do_exec=0;
    *prog="";
  }
  else {
    *prog=pathfind(argv[rest]);
    if  (NULL==*prog) {
      if (!shell) shell=getenv("SHELL");
      if (!shell) {
        fprintf(stderr,
                "xargs: '%s' not found in path, and 'SHELL' not in environment\n",
	        argv[rest]);
        return 0x8E; /* unrecognised command */
      }
      else 
        *prog=shell;
    }
    else
      ++rest;
  }
  if (rest==argc)
    *iniargs=" ";
  else {
    for (i=rest, len=argc-rest ; i<argc ; ++i)
      len+=strlen(argv[i]);
    if (**prog) ++len;
    if (len>ARGLEN-MARGIN) {
      fprintf(stderr,"xargs: the initial arguments take up too much space.\n");
      return 0x8D; /* .BUFUL */
    } 
    *iniargs=p=malloc(len+1);
    if (**prog)
      *p++=' ';
    for (i=rest ; i<argc ; ++i) {
      strcpy(p,argv[i]);
      p+=strlen(argv[i]);
      *p++=' ';
    }
    *p=0;
  }
  return 0;
}

int do_read(char * name, int fd, char* buf, unsigned blen)
{
  int len,res;
  char *p;

  res=0;
  len=read(fd, buf, blen);
  if (len<0) {
    res=errno;
    perror(name);
    fputs(" while reading.\n", stderr);
    buf[0]=0;
    res=1;
  }
  else {
    buf[len]=0;
    p=strchr(buf, 26); /* check for cp/m eof */
    if (p) {
      *p=0;
      res=1;
    }
  }
  return res;
}

int do_once(char * prog, char * argbuf, char * fds)
{
  int res;
  if (verbose) {
    if (do_exec) printf("[XARGS] ");
    printf("%s%s\n", prog, argbuf);
  }
  if (do_exec)
    res=_spawn(prog, argbuf, fds);
  else
    res=0;
  return res;
}

int do_xargs(char * input, int in, char * prog, char * iniargs)
{
  int res, len;
  unsigned char inilen;
  char ateof, *p, *q, *tok, fds[3];
  static char inbuf[BUFLEN+1];
  static char argbuf[BUFLEN+1];
  
  if (0==in) {
    res=open("NUL", O_RDONLY|O_INHER);
    if (res<0) {
      res=errno;
      perror("xargs: NUL");
      fputs(" while opening for input.\n",stderr);
      return res;
    }
    else
      fds[0]=res;
  }
  else
    fds[0]=0;
  fds[1]=1; fds[2]=2;	

  res=0;
  inilen=strlen(iniargs);
  strcpy(argbuf, iniargs);
  q=argbuf+inilen;
  p=inbuf;
  ateof=do_read(input, in, inbuf, BUFLEN);
  while (*p||!ateof) {
    tok=token(&p);
    if (tok) {
      len=strlen(tok);
      if (len+inilen>ARGLEN) {
        fprintf(stderr, "%.20s... too long, ignored\n", tok);
      }
      else {
        if (q+len>argbuf+ARGLEN) {
	  *q--=0;
	  if (' '==*q) *q=0;
	  res=do_once(prog, argbuf, fds);
	  q=argbuf+inilen;
        }
        strcpy(q,tok);
        q+=len;
        if (q<argbuf+ARGLEN)
	  *q++=' ';
      }
    }
    if (!ateof && (p>inbuf+BUFLEN/2)) {
      len=inbuf+BUFLEN-p;
      memcpy(inbuf, p, len);
      ateof=do_read(input,in,inbuf+len,BUFLEN-len);
      p=inbuf;
    }
  }
  *q--=0;
  if (' '==*q) *q=0;
  res=do_once(prog, argbuf, fds);

  if (fds[0]) 
    close(fds[0]);
  return res;
}

int main(int argc, char *argv[])
{
  char * argfile, * prog, *iniargs;
  int fd, res;

  res=parse(argc, argv, &argfile, &prog, &iniargs);
  if (res) 
    return (0x8C==res) ? 0 : res;

  if (argfile) {
    fd=open(argfile, O_RDONLY);
    if (fd<0) {
      res=errno;
      perror(argfile);
      fprintf(stderr, " while opening to read arguments. \n");
      return res;
    }
  }
  else
    fd=0;

  res=do_xargs(fd?argfile:"(stdin)", fd, prog, iniargs);

  if (fd)
    close(fd);
}

