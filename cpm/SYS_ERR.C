short	errno;
char *	sys_err[] =
{
	"Unknown error",
	"Reading empty space",
	"Bullshit",
	"Can't close extent",
	"Unwritten extent",
	"Directory full",
	"Seek past end of disk???",
};
short	sys_ner = sizeof sys_err / sizeof sys_err[0];
