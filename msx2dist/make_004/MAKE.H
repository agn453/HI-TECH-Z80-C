#define ESCCHAR '\\'            /* escape char (is one \) */
#define MACCHAR '$'             /* macro-definition char */
#define COMCHAR '#'             /* comment char */
#define	DEFMAC	"="		/* macro-definition token */
#define	CATMAC	"+="		/* macro-concatenation token */
#define DIFFMAC "-="		/* macro-filtering token */
#define	DEPEND	":"		/* dependency-definition token */
#define	INIT	"~INIT"			/* initialization macro */
#define	DEINIT	"~DEINIT"		/* de-init macro */
#define	BEFORE	"~BEFORE"		/* the per-root 'startup' method */
#define	AFTER	"~AFTER"		/* the per-root 'wrapup' method */

#define DEBUG	if(0)
#define	STRSIZ	512
#define	MAXMODS	50

/* file attributes */
#define	REBUILT	0x01		/* file has been reconstructed */
#define	ROOTP	0x02		/* file was named on left side of DEPEND */
#define FAILED	0x04		/* reconstructing the file failed */
#define DUMMYP  0x08		/* 'file' only exists within %-rule */ 

#define DATE unsigned long


struct node {
	struct filenode *nfile;	/* this node's file */
	struct node *nnext;	/* the next node */
};
typedef struct node NODE;


struct filenode {
	char *fname;		/* the filename */
	char *fmake;		/* remake string for file */
	DATE fdate;		/* 32 bit last-modification date */
	NODE *fnode;		/* files this file depends on */
	char fflag;		/* magic flag bits */
	struct filenode *fnext;	/* the next file */
};
typedef struct filenode FILENODE;


struct macro {
	char *mname;		/* the macro's name */
	char *mvalue;		/* the macro's definition */
	struct macro *mnext;	/* the next macro */
};
typedef struct macro MACRO;


extern MACRO *mroot;
extern FILENODE *froot;

char *gmacro(char *name);
FILENODE *filenode(char *fname);
FILENODE *gfile(char *fn);
char *token(char **strpp);
void *xalloc(unsigned len);

