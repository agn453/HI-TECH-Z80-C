#include	"cpm.h"

#define	FILL	0, "        ", "   ", 0, {0}, 0, {0}, 0, {0}, 0

/*

   Default stdin, stdout and stderr to the console.

   If PIPEMGR is detected by the start-up routine,
   then it changes these to RSX:, RSX: and ERR: by
   calling the _initrsx function.

 */

struct fcb	_fcb[MAXFILE] =
{
    { FILL, U_CON },	/* stdin */
    { FILL, U_CON },	/* stdout */
    { FILL, U_CON },	/* stderr */
};

void _cpm_clean()
{
    uchar	i;

    i = 0;
    do
        close(i);
    while (++i < MAXFILE);
}

void _putrno(uchar *where, long rno)
{
    where[0] = rno & 0xFF;
    where[1] = (rno >> 8) & 0xFF;
    where[2] = (rno >> 16) & 0xFF;
}

void _initrsx()
{
    /* Use PIPEMGR for stdin, stdout and stderr redirection */
    _fcb[0].use = U_RSX;	/* stdin */
    _fcb[1].use = U_RSX;	/* stdout */
    _fcb[2].use = U_ERR;	/* stderr */
}
