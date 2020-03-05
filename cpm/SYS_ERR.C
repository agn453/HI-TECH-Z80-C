short   errno;

static char uerr[]="Unknown error";

char *  sys_err[] =
{
    uerr,                           /* 0 */
    "Reading empty space",          /* 1 */
    "Disc is full",                 /* 2 */
    "Can't close extent",           /* 3 */
    "Unwritten extent",             /* 4 */
    "Directory full",               /* 5 */
    "File pointer out of range",    /* 6 */
    "File not found",               /* 7 */
    uerr,                           /* 8 */
    "Invalid FCB",                  /* 9 */
    "Disc has been changed",        /* 10 */
    uerr,uerr,uerr,uerr,uerr,uerr,  /* 11-16 - Below are CP/M-3 errors */
    "Disc I/O error",               /* 17 */
    "Disc is read-only",            /* 18 */
    "File is read-only",            /* 19 */
    "Invalid drive",                /* 20 */
    "File already open",            /* 21 */
    uerr,                           /* 22 */
    "Password error",               /* 23 */
    "File exists",                  /* 24 */
    "Ambiguous filename",           /* 25 */
    "File is wheel protected",      /* 26 */ 
    uerr    
};
short   sys_ner = sizeof sys_err / sizeof sys_err[0];
