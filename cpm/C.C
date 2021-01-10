/*
 *  Copyright (C) 1984-1897 HI-TECH SOFTWARE
 *
 *  This software remains the property of HI-TECH SOFTWARE and is
 *  supplied under licence only. The use of this software is
 *  permitted under the terms of that licence only. Copying of
 *  this software except for the purpose of making backup or
 *  working copies for the use of the licensee on a single
 *  processor is prohibited.
 */

/*----------------------------------------------------------------------*\
 | Note by Jon Saxton, 3 May 2014.					|
 |									|
 | It appears that the source file C.C distributed with Hi-Tech C 3.09	|
 | for CP/M-80 does not quite match the C.COM in the same distribution.	|
 |									|
 | This is a slightly modified version of C.C which yields a C.COM that	|
 | seems to work and so could probably replace the standard compiler	|
 | driver.  I use this version but just in case there is a bug which I	|
 | have not yet discovered, I am leaving the original C.COM unchanged.	|
\*----------------------------------------------------------------------*/

#include    <stdio.h>
#include    <ctype.h>
#include    <cpm.h>
#include    <exec.h>
#include    <stat.h>

/*
 *  C command
 *  CP/M-80 version
 *
 *  C [-C] [-O] [-I] [-F] [-U] [-D] [-S] [-X] [-P] [-W] [-M] [-V] files {-Llib}
 */

#define DFCB    ((struct fcb *)0x5C)

#define MAXLIST 60      /* max arg list */
#define BIGLIST 120     /* room for much more */

#define HITECH  "HITECH"
#define PROMPT  "c"
#define TEMP    "TMP"
#define DEFPATH "0:A:"
#define DEFTMP  ""

#define LIBSUFF ".LIB"      /* library suffix */

#define LFLAGS      "-Z"
#define STDLIB      "C"
#define GETARGS     "-U__getargs"

static char
    keep,       /* retain .obj files, don't link */
    keepas,     /* retain .as files, don't assemble */
    verbose,    /* verbose - echo all commands */
    optimize,   /* invoke optimizer */
    speed,      /* optimize for speed */
    reloc,      /* auto-relocate program at run time */
    xref,       /* generate cross reference listing */
    nolocal;    /* strip local symbols */

static char
    *iuds[MAXLIST],  /* -[IUD] args to preprocessor */
    *objs[MAXLIST],  /* .obj files and others for linker */
    *flgs[BIGLIST],  /* flags etc for linker */
    *libs[MAXLIST],  /* .lib files for linker */
    *c_as[MAXLIST];  /* .c files to compile or .as to assemble */

static uchar
    iud_idx,    /* index into uids[] */
    obj_idx,    /*   "     "  objs[] */
    flg_idx,    /*   "     "  flgs[] */
    lib_idx,    /*   "     "  libs[] */
    c_as_idx;   /*   "     "  c_as[] */

static char *paths[] =
{
    "LINQ",	/* Renamed from LINK to avoid conflict with DRI's LINK-80 */
    "OBJTOHEX",
    "CGEN",
    "OPTIM",
    "CPP",
    "ZAS",
    "LIB",
    "P1",
    "CRTCPM.OBJ",
    "$EXEC",
    "CREF",
};

#define linker  paths[0]
#define objto   paths[1]
#define cgen    paths[2]
#define optim   paths[3]
#define cpp     paths[4]
#define assem   paths[5]
#define libpath paths[6]
#define pass1   paths[7]
#define strtoff paths[8]
#define execprg paths[9]
#define cref    paths[10]

#define RELSTRT strtoff[plen]

static char *   temps[] =
{
    "$CTMP1.$$$",
    "$CTMP2.$$$",
    "$CTMP3.$$$",
    "$CTMP4.$$$",
    "L.OBJ",
    "$$EXEC.$$$",
    "CREF.TMP",
};

#define tmpf1       temps[0]
#define tmpf2       temps[1]
#define tmpf3       temps[2]
#define redname     temps[3]
#define l_dot_obj   temps[4]
#define execmd      temps[5]
#define crtmp       temps[6]

static char    *cppdef[] = { "-DCPM", "-DHI_TECH_C", "-Dz80" };
static char    *cpppath = "-I";

static char     tmpbuf[128];    /* gen. purpose buffer */
static char     single[40];     /* single object file to be deleted */
static short    nfiles;         /* number of source or object files seen */
static char    *outfile;        /* output file name for objtohex */
static FILE    *cmdfile;        /* command file */
static short    plen;           /* length of path */
static char     ebuf[22];       /* error listing file */
static char    *xrname;
static struct stat  statbuf;

extern char
    *malloc(),
    *getenv(),
    *fcbname(),
    *rindex(),
    *strcat(),
    *strcpy(),
   **_getargs();
extern int
    strlen(),
    strcmp(),
    dup();

static char
   *xalloc(short);

void
    doexec(char *, char **),
    addobj(char *),
    addlib(char *),
    setup(),
    error(char *, char *),
    doit(),
    assemble(char *),
    compile(char *),
    print(char *),
    put_cmd(int),
    rm(int, char *);

int main(int argc, char **argv)
{
    register char *cp, *xp;
    short       i;

    fprintf(stderr, "Hi-Tech C Compiler (CP/M-80) V3.09-7\n");
    fprintf(stderr, "Copyright (C) 1984-87 HI-TECH SOFTWARE\n");
#if EDUC
    fprintf(stderr, "Licensed for Educational purposes only\n");
#endif  EDUC
    if (argc == 1)
        argv = _getargs((char *)0, PROMPT);
    setup();
    while (*++argv)
    {
        if ((argv)[0][0] == '-')
        {
            if (islower(i = argv[0][1]))
                argv[0][1] = i = toupper(i);
            switch(i)
            {
            case 'A':
                reloc = 1;
                RELSTRT = 'R';
                flgs[flg_idx++] = "-L";
                break;

            case 'R':
                flgs[flg_idx++] = GETARGS;
                break;

            case 'V':
                verbose = 1;
                break;

            case 'S':
                keepas = 1;

            case 'C':
                if (argv[0][2] == 'r' || argv[0][2] == 'R')
                {
                    xref = 1;
                    if (argv[0][3])
                    {
                        xrname = &argv[0][1];
                        xrname[0] = '-';
                        xrname[1] = 'o';
                    }
                    else
                        xrname = (char *)0;
                }
                else
                    keep = 1;
                break;

            case 'O':
                optimize = 1;
                if(argv[0][2] == 'F' || argv[0][2] == 'f')
                    speed = 1;
                break;

            case 'I':
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
        ++nfiles;
        cp = argv[0];
        while (*cp)
        {
            if (islower(*cp))
                *cp = toupper(*cp);
            ++cp;
        }
        cp = rindex(argv[0], '.');
        if (cp && (strcmp(cp, ".C") == 0 || strcmp(cp, ".AS") == 0))
        {
            c_as[c_as_idx++] = argv[0];
            if (xp = rindex(argv[0], ':'))
                ++xp;
            else
                xp = argv[0];
            *cp = 0;
            strcat(strcpy(tmpbuf, xp), ".OBJ");
            addobj(tmpbuf);
            strcpy(single, tmpbuf);
            *cp = '.';
        }
        else
            addobj(argv[0]);
    }
    doit();
}

void setup()
{
    register char * cp;
    short       i, len;

    if (!(cp = getenv(HITECH)))
        if (stat("P1.COM", &statbuf) >= 0)
            cp = "";
        else
            cp = DEFPATH;
    plen = strlen(cp);
    cpppath = strcat(strcpy(xalloc(plen+strlen(cpppath)+1), cpppath), cp);
    for (i = 0 ; i < sizeof paths/sizeof paths[0] ; i++)
        paths[i] = strcat(strcpy(xalloc(plen+strlen(paths[i])+1), cp),
                          paths[i]);
    if (cp = getenv(TEMP))
    {
        len = strlen(cp);
        for (i = 0; i < sizeof temps/sizeof temps[0]; ++i)
            temps[i] = strcat(strcpy(xalloc(len+strlen(temps[i])+1), cp),
                              temps[i]);
    }
    if (strcmp(fcbname(fileno(stdout)), "CON:"))   /* redirect errors */
    {
        strcat(strcpy(ebuf, "-E"), fcbname(fileno(stdout)));
        close(fileno(stdout));
        stdout->_file = dup(fileno(stderr));
    }
    objs[0] = strtoff;
    obj_idx = 1;
    flgs[0] = LFLAGS;
    flg_idx = 1;
    for (i = 0; i < sizeof cppdef/sizeof cppdef[0]; ++i)
        iuds[i] = cppdef[i];
    iud_idx = i;
}

void doit()
{
    register char * cp;
    register uchar  i;

    if (xref)
        close(creat(crtmp, 0600));
    iuds[iud_idx++] = cpppath;
    if (!(cmdfile = fopen(execmd, "wb")))
        error("Can't create temporary file %s", execmd);
    put_cmd(SKP_ERR);
    if (verbose)
        put_cmd(ECHO);
    for (i = 0 ; i < c_as_idx ; i++)
    {
        cp = rindex(c_as[i], '.');
        if (strcmp(cp, ".C") == 0)
            compile(c_as[i]);
        else
            assemble(c_as[i]);
        put_cmd(TRAP);
    }
    rm(RM_FILE, tmpf1);
    rm(RM_FILE, tmpf2);
    rm(RM_FILE, tmpf3);
    if (!keep)
    {
        flgs[flg_idx++] = "-Ptext=0,data,bss";
        if (reloc)
            flgs[flg_idx++] = strcat(strcpy(xalloc(strlen(l_dot_obj)+3), "-o"),
                                     l_dot_obj);
        else
        {
            flgs[flg_idx++] = "-C100H";
            flgs[flg_idx++] = strcat(strcpy(xalloc(strlen(outfile)+3), "-O"),
                                     outfile);
        }
        for (i = 0 ; i < obj_idx ; i++)
            flgs[flg_idx++] = objs[i];
        addlib(STDLIB);
        for(i = 0 ; i < lib_idx ; i++)
            flgs[flg_idx++] = libs[i];
        flgs[flg_idx] = 0;
        put_cmd(IF_NERR);
        doexec(linker, flgs);
        if (reloc)
        {
            flgs[0] = "-R";
            flgs[1] = "-B100H";
            flgs[2] = l_dot_obj;
            flgs[3] = outfile;
            flgs[4] = (char *)0;
            doexec(objto, flgs);
            rm(RM_FILE, l_dot_obj);
        }
        if (c_as_idx == 1 && nfiles == 1)
            rm(RM_FILE, single);
    }
    if (xref)
        if (xrname)
        {
            flgs[0] = xrname;
            strcat(strcpy(tmpbuf, "-h"), outfile);
            if(cp = rindex(tmpbuf, '.'))
                strcpy(cp, ".CRF");
            else
                strcat(tmpbuf, ".CRF");
            flgs[1] = tmpbuf;
            flgs[2] = crtmp;
            flgs[3] = 0;
            put_cmd(IF_NERR);
            doexec(cref, flgs);
            rm(RM_FILE, crtmp);
        }
        else
        {
            sprintf(tmpbuf, "Cross reference info left in %s: "
                            "; run CREF to produce listing\n", crtmp);
            print(tmpbuf);
        }
    put_cmd(TRAP);
    rm(RM_EXIT, execmd);
    fclose(cmdfile);
    fclose(stdout);
    fclose(stdin);
    setfcb(DFCB, execmd);
    execl(execprg, execprg, execmd, (char *)0);
    error("Can't execute %s", execprg);
}

void rm(int type, char *file)
{
    char    buf[40];

    if (verbose)
    {
        strcat(strcpy(buf, "ERA "), file);
        print(buf);
    }
    setfcb(DFCB, file);
    putc(type, cmdfile);
    putc(16, cmdfile);
    fwrite(DFCB, 1, 16, cmdfile);
}

void print(char *s)
{
    putc(PRINT, cmdfile);
    putc(strlen(s), cmdfile);
    fputs(s, cmdfile);
}

void put_cmd(int i)
{
    putc(i, cmdfile);
    putc(0, cmdfile);
}

void addobj(char *s)
{
    char *  cp;
    uchar   len;
    static char oname;

    if (oname == 0)
    {
        oname = 1;
        if(cp = rindex(s, '.'))
            len = cp - s;
        else
            len = strlen(s);
        cp = xalloc(len + strlen("-O.COM") + 1);
        strncat(strcpy(cp, "-O"), s, len);
        strcpy(cp+len+2, ".COM");
        outfile = cp+2;
    }
    cp = xalloc(strlen(s)+1);
    strcpy(cp, s);
    objs[obj_idx++] = cp;
}

void addlib(char *s)
{
    char *  cp;

    strcpy(tmpbuf, libpath);
    strcat(strcat(tmpbuf, s), LIBSUFF);
    cp = xalloc(strlen(tmpbuf)+1);
    strcpy(cp, tmpbuf);
    libs[lib_idx++] = cp;
}

void error(char *s, char *a)
{
    fprintf(stderr, s, a);
    exit(1);
}

static char *xalloc(short s)
{
    register char * cp;

    if (!(cp = malloc(s)))
        error("Out of memory", NULL);
    return cp;
}

void upcase(char *s)
{
    while (*s)
    {
        if (*s >= 'a' && *s <= 'z')
            *s -= 'a'-'A';
        ++s;
    }
}

void doexec(char *name, char **vec)
{
    uchar           len;
    char          **pvec;
    char           *redir[2];
    char            redbuf[20];
    FILE           *cfile;
    static short    redno;
    char            xbuf[130];

    pvec = vec;
    len = 0;
    redbuf[0] = 0;
    while (*pvec)
        len += strlen(*pvec++)+1;
    if (len > 124)
    {
        sprintf(xbuf, redname, ++redno);
        if (!(cfile = fopen(xbuf, "w")))
            error("Can't create %s", xbuf);
        len = 0;
        while (*vec)
        {
            len += strlen(*vec);
            fprintf(cfile, "%s ", *vec++);
            if (len > 126)
            {
                len = 0;
                fprintf(cfile, "\\\n");
            }
        }
        fputc('\n', cfile);
        fclose(cfile);
        redir[1] = (char *)0;
        sprintf(redbuf, "<%s", xbuf);
        redir[0] = redbuf;
        vec = redir;
    }
    xbuf[0] = 0;
    while(*vec)
        strcat(strcat(xbuf, " "), *vec++);
    len = strlen(xbuf);
    putc(EXEC, cmdfile);
    putc(len+50, cmdfile);
    setfcb(DFCB, name);
    strcpy(DFCB->ft, "COM");
    DFCB->nr = 0;
    putc(DFCB->uid, cmdfile);
    fwrite(DFCB, 1, 16, cmdfile);
    setfcb(DFCB, &xbuf[1]);
    DFCB->nr = 0;
    fwrite(DFCB, 1, 32, cmdfile);
    putc(len, cmdfile);
    fwrite(xbuf, 1, len, cmdfile);
    if (redbuf[0])
        rm(RM_FILE, &redbuf[1]);
}

void assemble(char *s)
{
    char  *vec[5];
    char   buf[80];
    char  *cp;
    uchar  i;

    if (c_as_idx > 1)
        print(s);
    i = 0;
    if (optimize && !speed)
        vec[i++] = "-J";
    if (nolocal)
        vec[i++] = "-X";
    if (cp = rindex(s, ':'))
        ++cp;
    else
        cp = s;
    strcat(strcpy(buf, "-O"), cp);
    if (rindex(buf, '.'))
        *rindex(buf, '.') = 0;
    strcat(buf, ".OBJ");
    vec[i++] = buf;
    vec[i++] = s;
    vec[i] = (char *)0;
    doexec(assem, vec);
}

void compile(char *s)
{
    register char *cp;
    short          i, j;
    char          *vec[MAXLIST];
    char           cbuf[50];

    if (c_as_idx > 1)
        print(s);
    for (j = 0; j < iud_idx ; ++j)
        vec[j] = iuds[j];
    vec[j++] = s;
    vec[j++] = tmpf1;
    vec[j] = (char *)0;
    doexec(cpp, vec);
    if (cp = rindex(s, ':'))
        s = cp+1;
    *rindex(s, '.') = 0;
    i = 0;
    if (keepas && !optimize)
        vec[i++] = "-S";
    if (xref)
        vec[i++] = strcat(strcpy(cbuf, "-c"), crtmp);
    if (ebuf[0])     /* error redirection */
        vec[i++] = ebuf;
    vec[i++] = tmpf1;
    vec[i++] = tmpf2;
    vec[i++] = tmpf3;
    vec[i++] = (char *)0;

    /* vec[0] is junk.  Move everything down one slot.  */
    for (i=0; vec[i]; ++i)
	vec[i] = vec[i+1];
    doexec(pass1, vec);
    vec[0] = tmpf2;
    vec[1] = keepas && !optimize ? strcat(strcpy(tmpbuf, s), ".AS") : tmpf1;
    vec[2] = (char *)0;
    doexec(cgen, vec);
    if (keepas && !optimize)
        return;
    cp = tmpf1;
    if (optimize)
    {
        i = 0;
        if (speed)
            vec[i++] = "-F";
        vec[i++] = tmpf1;
        if (keepas)
            vec[i++] = strcat(strcpy(tmpbuf, s), ".AS");
        else
            vec[i++] = tmpf2;
        vec[i] = (char *)0;
        doexec(optim, vec);
        if (keepas)
            return;
        cp = tmpf2;
    }
    i = 0;
    if (nolocal)
        vec[i++] = "-X";
    if (optimize && !speed)
        vec[i++] = "-J";
    vec[i++] = "-N";
    vec[i++] = strcat(strcat(strcpy(tmpbuf, "-o"), s), ".OBJ");
    vec[i++] = cp;
    vec[i] = (char *)0;
    doexec(assem, vec);
}
