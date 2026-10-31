/*
 *   Copyright (C) 1984-1987 HI-TECH SOFTWARE
 *
 *   MSXDOS modifications by Pierre Gielen 1993:
 *
 *   - Replaced EXECL routines (C produces a batch file now)
 *   - Added -Q switch to write batch file without executing
 *   - Changed object file types to .O and temporary file
 *     types to .T to allow more commands per line (note:
 *     crt.o = crtcpm.obj). Changed library types back to .LIB.
 *   - Commented out calls to GETENV (for now) because it locks up
 *     the computer (still to be changed in the standard library)
 *
 *   Modifications by Arnold Metselaar:
 *
 *   - Append ".C" _only_ if no filetype at all
 *   - Fixed bug with "-R" switch
 *   - Put object and program files in the same place as their sources 
 *   - Switched back from producing $exec.bat to _spawn
 *   - removed -Q switch
 *   - Use APPEND environment item to find include files (and libs)
 *   - Added -N switch to link without crt.o , the user should provide
 *
 *   HITECH C is almost ANSI C compatible, but it does not recognize
 *   the '#pragma nonrec' directive.
 *
 *
 *   HITECH SOFTWARE has given kind permission to copy this
 *   software for personal use.
 *
 */

#include    <stdio.h>
#include    <string.h>
#include    <ctype.h>

void stack_trace();

/*
 * CC command  MSX-DOS2 version
 *
 * CC [-C] [-O] [-I] [-F] [-U] [-D] [-S] [-X] [-P] [-W] [-M] [-N] files {-Llib}
 */

#define   MAXLIST   60      /* max arg list */
#define   BIGLIST   120      /* room for much more */

#define   HITECH   "HITECH"
#define PROMPT   "c"
#define   TEMP   "TMP"
#define DEFPATH ""
#define   DEFTMP   ""

#define LIBSUFF ".LIB"          /* library suffix */

#define   LFLAGS      "-Z"
#define   STDLIB      "C"
/* originally "-U__getargs", but the -u option to link is broken, so I made
 * a small object file with the same effect */
#define REDIR_O    "redir.obj"


static char   
      keep,      /* retain .obj files, don't link */
      keepas,    /* retain .as files, don't assemble */
      verbose,   /* verbose - echo all commands */
      optimize,  /* invoke optimizer */
      speed,     /* optimize for speed */
      reloc,     /* auto-relocate program at run time */
      xref,      /* generate cross reference listing */
      nolocal,   /* strip local symbols */
      redir,	 /* AM - link code for redirectioning and globbing */
      nocrto;    /* AM - do not link standard crt.o */

static char *   iuds[MAXLIST],   /* -[IUD] args to preprocessor */
       *   objs[MAXLIST],   /* .obj files and others for linker */
       *   flgs[BIGLIST],   /* flags etc for linker */
       *   libs[MAXLIST],   /* .lib files for linker */
       *   c_as[MAXLIST];   /* .c files to compile or .as to assemble */

static uchar   iud_idx,   /* index into iuds[] */
      obj_idx,   /*   "     "  objs[] */
      flg_idx,   /*   "     "  flgs[] */
      lib_idx,   /*   "     "  libs[] */
      c_as_idx;   /*   "     "  c_as[] */
static char *   paths[] =
{
   "~LINK",
   "OBJTOHEX",
   "~CGEN",
   "~OPTIM",
   "~CPP",
   "~ZAS",
   "LIB",
   "~P1",
   "~CREF",
   "CRT.O",
};

#define   linker   paths[0]
#define   objto   paths[1]
#define   cgen   paths[2]
#define   optim   paths[3]
#define   cpp   paths[4]
#define   assem   paths[5]
#define   libpath   paths[6]
#define   pass1   paths[7]
#define   cref   paths[8]
#define   strtoff   paths[9]

#define   RELSTRT   strtoff[plen]

static char *   temps[] =
{
   "$C1.T",
   "$C2.T",
   "$C3.T",
   "$C4.T",
   "L.O",
   "CREF.T"
};

#define   tmpf1     temps[0]
#define   tmpf2     temps[1]
#define   tmpf3     temps[2]
#define   redname   temps[3]
#define l_dot_obj   temps[4]
#define   crtmp     temps[5]

static char   * cppdef[] = { "-DCPM", "-DHI_TECH_C", "-Dz80" };
static char   * cpppath = "-I";
static char   * old_append, * new_append;


static char    tmpbuf[128]; /* gen. purpose buffer */
static char    single[40];  /* single object file to be deleted */
static short   nfiles;      /* number of source or object files seen */
static char  * outfile;     /* output file name for objtohex */
static short   plen;        /* length of path */
static char    ebuf[22];    /* error listing file */
static char  * xrname;

/*static struct stat   statbuf;*/

extern char *   malloc(),
       *   getenv(),
       *   rindex();
       *   realloc(char *, int);
extern char **   _getargs();
extern int   dup(int);

static char *   xalloc(short);

void setup();
int doit();

int
main(argc, argv)
char **   argv;
{
   register char *   cp, * xp;
   short      i;

   fprintf(stderr, "HI-TECH C COMPILER (MSXDOS) V3.09\n");
   fprintf(stderr, "Copyright (C) 1993 HI-TECH SOFTWARE\n");
   if (argc == 1)
      argv = _getargs((char *)0, PROMPT);
   setup();
   while(*++argv) {
      if((argv)[0][0] == '-') {
         if(islower(i = argv[0][1]))
            argv[0][1] = i = toupper(i);
         switch(i) {

         case 'A':
            reloc = 1;
            RELSTRT = 'R';
            flgs[flg_idx++] = "-L";
            break;

	 case 'N':       /* AM */
	    nocrto = 1;
	    break;

         case 'R':
            redir=1;
            break;

         case 'V':
            verbose = 1;
            break;

         case 'S':
            keepas = 1;
	    keep = 1;
	    break;

         case 'C':
            if(argv[0][2] == 'r' || argv[0][2] == 'R') {
               xref = 1;
               if(argv[0][3]) {
                  xrname = &argv[0][1];
                  xrname[0] = '-';
                  xrname[1] = 'o';
               } else
                  xrname = (char *)0;
            } else
               keep = 1;
            break;

         case 'O':
	    if (strlen(argv[0])>3)
	      outfile=argv[0]+2;
	    else {
              optimize = 1;
              if(argv[0][2] == 'F' || argv[0][2] == 'f')
                 speed = 1;
	    }
            break;

         case 'I':  
	 /* APPEND does not work as desired
          * if (strlen(new_append)+strlen(argv[0]+1)<256) {
          *    cp=(char*)realloc(new_append,
	  *			 strlen(new_append)+strlen(argv[0]+1));
	  *    if (cp) {
          *       strcat(strcat(cp,"; "),argv[0]+2);
	  *	  new_append=cp;
	  *    }
          *    else 
          *	  fprintf(stderr, "Excessive -I options\n");
          * }
          * else 
	  *   fprintf(stderr, "Excessive -I options\n");
          * break; */

         case 'U':
         case 'D':
            iuds[iud_idx++] = argv[0];
            break;

         case 'L':
            addlib(&argv[0][2]);
            break;

         case 'F':
            argv[0][1] = 'D';
            flgs[flg_idx++] = argv[0];
            break;

         case 'X':
            nolocal = 1;

         case 'P':
         case 'M':
         case 'W':
            flgs[flg_idx++] = argv[0];
            break;

         default:
            fprintf(stderr, "Unknown flag %s\n", argv[0]);
            exit(1);
         }
         continue;
      }
      nfiles++;
      cp = argv[0];
      while(*cp) {
         if(islower(*cp))
            *cp = toupper(*cp);
         cp++;
      }

/* PG: */
      if (strchr(argv[0],'.')==NULL)   /* if no filetype at all */
         argv[0]=strcat(strcpy(xalloc(strlen(argv[0])+3),argv[0]),".C");
         /* make it .C */
/* */      
                cp = rindex(argv[0], '.');
      if(cp && (strcmp(cp, ".C") == 0 || strcmp(cp, ".AS") == 0)) {
         c_as[c_as_idx++] = argv[0];
/* AM: put object files in the same place as their sources
 *     maybe there should be an option controlling this
 *       if(xp = rindex(argv[0], ':'))
 *          xp++;
 *       else  */
            xp = argv[0];
         *cp = 0;
         strcat(strcpy(tmpbuf, xp), ".O");
         addobj(tmpbuf);
         strcpy(single, tmpbuf);
         *cp = '.';
      } else
         addobj(argv[0]);
   }
   if (outfile && keep && (c_as_idx>1)) {
     fprintf(stderr, 
             "-Oname used while keeping more than one %sfile. (Error)\n",
	     keepas?"assembly language ":"object");
     exit(2);
   }
   if (redir) addobj(REDIR_O);
   setenv("APPEND", new_append);
   i=doit();
   setenv("APPEND", old_append);
   return i;
}

void
setup()
{
   register char *   cp;
   short      i, len;

   if (!(cp = getenv("PROGRAM")))
      cp=DEFPATH;
   else
      if (strrchr(cp,'\\')) strrchr(cp,'\\')[1]='\0';
   old_append = getenv("APPEND"); if (!old_append) old_append="";
   new_append = strcpy(xalloc(strlen(cp)+1), cp);
   plen=strlen(cp);
   for (i = 0 ; i < sizeof paths/sizeof paths[0] ; i++)
      if (paths[i][0]=='~') 
	 paths[i] = strcat(strcat(strcpy(xalloc(plen+strlen(paths[i])+4), 
					 cp), paths[i]+1), ".COM");
   if (cp!=DEFPATH) free(cp);
   if (cp = getenv(TEMP)) {
      len = strlen(cp);
      for(i = 0 ; i < sizeof temps/sizeof temps[0] ; i++)
         temps[i]=strcat(strcpy(xalloc(len+strlen(temps[i])+1),cp),temps[i]);
      free(cp);
   }

   objs[0] = strtoff;
   obj_idx = 1;
   flgs[0] = LFLAGS;
   flg_idx = 1;
   for(i = 0 ; i < sizeof cppdef/sizeof cppdef[0] ; i++)
      iuds[i] = cppdef[i];
   iud_idx = i;
   outfile = NULL;
}

int  compile(char *, char *);
int assemble(char *, char *);
int doexec(char *, char **);
char * set_ext(char*, char*, char*, int);

int
doit()
{
   register char *   cp;
   register uchar   i;
   char *outf, buf[20];
   int res;

   if(xref)
      close(creat(crtmp, 0600));
   iuds[iud_idx++] = cpppath;
   for(i = res = 0 ; (i < c_as_idx) && (res == 0) ; i++) {
      if ( verbose && (c_as_idx > 1) )
         printf("Working on: '%s'.\n",c_as[i]);
      cp = rindex(c_as[i], '.');
      if(strcmp(cp, ".C") == 0) {
	 outf=(keep && outfile) ? outfile
	                        : set_ext(buf, c_as[i], keepas?".AS":".O", 20);	
         res = compile(c_as[i], outf);
      }
      else {
	 outf=(keep && outfile) ? outfile
	                        : set_ext(buf, c_as[i], ".O", 20);	
         res = assemble(c_as[i], outf);
      }
   }
   remove(tmpf1);
   remove(tmpf2);
   remove(tmpf3);
   if (!keep && !res && (obj_idx>1)) {
      if (!outfile)
	 outfile=set_ext(buf,objs[nocrto?2:1],".COM",20);
      flgs[flg_idx++] = "-Ptext=0,data,bss";
      if (reloc) {
         flgs[flg_idx++] = strcat(strcpy(xalloc(strlen(l_dot_obj)+3), 
						"-o"), l_dot_obj);
      } 
      else {
         flgs[flg_idx++] = "-C100H";
         flgs[flg_idx++] = strcat(strcpy(xalloc(strlen(outfile)+3), 
					 "-O"), outfile);
      }
      /* skip standard startoff file if desired - AM */
      for (i = (nocrto?1:0) ; i < obj_idx ; i++) {
         flgs[flg_idx++] = objs[i];
      }
      addlib(STDLIB);
      for (i = 0 ; i < lib_idx ; i++) {
         flgs[flg_idx++] = libs[i];
      }
      flgs[flg_idx] = 0;
      res = doexec(linker, flgs);
      if (reloc && !res) {
         flgs[0] = "-R";
         flgs[1] = "-B100H";
         flgs[2] = l_dot_obj;
         flgs[3] = outfile;
         flgs[4] = (char *)0;
         res = doexec(objto, flgs);
         remove(l_dot_obj);
      }
      if (c_as_idx == 1 && nfiles == 1)
         remove(single);
   }
   if (xref && !res)
      if (xrname) {
         flgs[0] = xrname;
         strcat(strcpy(tmpbuf, "-h"), outfile);
         if(cp = rindex(tmpbuf, '.'))
            strcpy(cp, ".CRF");
         else
            strcat(tmpbuf, ".CRF");
         flgs[1] = tmpbuf;
         flgs[2] = crtmp;
         flgs[3] = 0;
         res = doexec(cref, flgs);
            remove(crtmp);
      } else
         fprintf(stderr, "Cross reference info left in %s:\
 run CREF to produce listing\n", crtmp);

   return res;
}

addobj(s)
char *   s;
{
   char *   cp;

   cp = xalloc(strlen(s)+1);
   strcpy(cp, s);
   objs[obj_idx++] = cp;
}

addlib(s)
char *   s;
{
   char *   cp;

   strcpy(tmpbuf, libpath);
   strcat(strcat(tmpbuf, s), LIBSUFF);
   cp = xalloc(strlen(tmpbuf)+1);
   strcpy(cp, tmpbuf);
   libs[lib_idx++] = cp;
}

void 
error(s, a)
char *   s;
{
   fprintf(stderr, s, a);
   stack_trace();
   exit(1);
}


static char *
xalloc(s)
short   s;
{
   register char *   cp;

   if(!(cp = malloc(s)))
      error("Out of memory");
   return cp;
}

upcase(s)
register char *   s;
{
   while(*s) {
      if(*s >= 'a' && *s <= 'z')
         *s -= 'a'-'A';
      s++;
   }
}

int _spawn(char *, char *, char fds[3]);

int
doexec(name, vec)
char *   name;
char **   vec;
{
   uchar   len;
   int res;
   char **   pvec;
   char *   redir[2];
   FILE *   cfile;
   char   redbuf[20];
   char   xbuf[130];
   static char fds[3] = { 0, 1, 2 };

   pvec = vec;

   len = 0;
   redbuf[0] = 0;
   while (*pvec)
      len += strlen(*pvec++)+1;

   if (len > 124) {
      if(!(cfile = fopen(redname, "w"))) {
	 perror(redname);
         fputs(", can't create file to pass options.\n",stderr);
         return 1;
      }
      len = 0;
      while(*vec) {
         len += strlen(*vec)+1;
         if (len > 126) {
            len = strlen(*vec)+1;
            fprintf(cfile, "\\\n");
         }
	 if (verbose) printf("%s ",*vec);
         fprintf(cfile, "%s ", *vec++);
      }
      fputc('\n', cfile);
      putchar('\n');
      fclose(cfile);
      redir[1] = (char *)0;
      sprintf(redbuf, "<%s", redname);
      redir[0] = redbuf;
      vec = redir;
   }

   xbuf[0] = 0;
   while(*vec)
      strcat(strcat(xbuf, " "), *vec++);

   if (verbose) printf("[CC] %s %s\n", name, xbuf);
   res=_spawn(name, xbuf, fds);

   if (*redbuf)
      remove(redbuf+1);
   return res;
}

int
assemble(s,d)
char * s;
char * d;	
{
   char *   vec[5];
   char   buf[80];
   uchar   i;

   i = 0;
   if (optimize && !speed)
      vec[i++] = "-J";
   if (nolocal)
      vec[i++] = "-X";
/* AM: put object files in the same place as their sources
 *     maybe there should be an option controlling this
 * if(cp = rindex(s, ':'))
 *    cp++;
 * else
 *    cp = d; */
   strcat(strcpy(buf, "-O"), d);

   vec[i++] = buf;
   vec[i++] = s;
   vec[i] = (char *)0;
   return doexec(assem, vec);
}

int
compile(s,d)
char * s;
char * d;
{
   register char *   cp;
   uchar   i, j;
   int res;
   char *   vec[MAXLIST];
   char   cbuf[50];

   for(j = 0; j < iud_idx ; j++)
      vec[j] = iuds[j];
   vec[j++] = s;
   vec[j++] = tmpf1;
   vec[j] = (char *)0;
   res=doexec(cpp, vec);
   if (res) {
      fprintf(stderr,"CPP failed on '%s'.\n",s);
      return res;
   }
/* AM: put object files in the same place as their sources
 *     maybe there should be an option controlling this
 * if(cp = rindex(s, ':'))
 *    s = cp+1; */
   *rindex(s, '.') = 0;
   i = 0;
   if(keepas && !optimize)
      vec[i++] = "-S";
   if(xref)
      vec[i++] = strcat(strcpy(cbuf, "-c"), crtmp);
   if(ebuf[0])      /* error redirection */
      vec[i++] = ebuf;
   vec[i++] = tmpf1;
   vec[i++] = tmpf2;
   vec[i++] = tmpf3;
   vec[i++] = (char *)0;
   res=doexec(pass1, vec);
   if (res) {
      fprintf(stderr,"Pass1 failed while processing '%s'.\n",s);
      return res;
   }
   vec[0] = tmpf2;
   vec[1] = keepas && !optimize ? d : tmpf1;
   vec[2] = (char *)0;
   res=doexec(cgen, vec);
   if (res) {
      fprintf(stderr,"Code generator failed while processing '%s'.\n",s);
      return res;
   }
   if(keepas && !optimize)
      return res;
   cp = tmpf1;
   if(optimize) {
      i = 0;
      if (speed)
         vec[i++] = "-F";
      vec[i++] = tmpf1;
      if (keepas)
         vec[i++] = d;
      else
         vec[i++] = tmpf2;
      vec[i] = (char *)0;
      res=doexec(optim, vec);
      if (res) {
         fprintf(stderr,"Optimiser failed while processing '%s'.\n",s);
         return res;
      }
      if (keepas)
         return res;
      cp = tmpf2;
   }
   i = 0;
   if (nolocal)
      vec[i++] = "-X";
   if (optimize && !speed)
      vec[i++] = "-J";
   vec[i++] = "-N";
   vec[i++] = strcat(strcpy(tmpbuf, "-o"), d);
   vec[i++] = cp;
   vec[i] = (char *)0;
   res=doexec(assem, vec);
   if (res)
      fprintf(stderr,"Assembler failed while processing '%s'.\n",s);
   return res;
}

char * set_ext(char *buf, char *nm, char *ex, int len)
{
  char *p;
  
  strncpy(buf, nm, len-strlen(ex)-2);
  buf[len-strlen(ex)-1]=0;
  p=strrchr(buf, '.');
  if (p) *p=0;
  strcat(buf, ex);
  return buf;
}

/* printf debugging: */

typedef struct sf {
  struct sf * up;
  int 	iy;
  char	* pc;
} frame ;

void stack_trace()
{
  int i;
  frame *fp;

  fp=(frame *)((&i)+1);
  while ( (unsigned int) (fp->up)>(unsigned int) fp ) {
    fprintf(stderr, "Called from %04x  (fp=%04x),\n", fp->pc-3, fp);
    fp = fp->up;
  }
  fprintf(stderr, "Called from %04x.\n", fp->pc-3);
}
/* end of source *****************************************************/
