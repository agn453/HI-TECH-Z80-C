#include	"cpm.h"

#define	FILL	0, "        ", "   ", 0, {0}, 0, {0}, 0, {0}, 0

struct fcb	_fcb[MAXFILE] =
{
	{ FILL, U_CON },	/* stdin */
	{ FILL, U_CON },	/* stdout */
	{ FILL, U_CON },	/* stderr */
};

_cpm_clean()
{
	uchar	i;

	i = 0;
	do
		close(i);
	while(++i < MAXFILE);
}

_putrno(where, rno)
uchar *	where;
long	rno;
{
	where[0] = rno & 0xFF;
	where[1] = (rno >> 8) & 0xFF;
	where[2] = (rno >> 16) & 0xFF;
}
