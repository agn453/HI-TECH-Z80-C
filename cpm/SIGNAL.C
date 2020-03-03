#include	"cpm.h"
#include	<signal.h>

static void (*	where)();

void (*
signal(sig, action))()
int	sig;
void (*	action)(int);
{
	void (*	prev)();

	if(sig != SIGINT)
		return (void (*)())-1;
	prev = where;
	where = action;
	return prev;
}

void
_sigchk()
{
	char	c;

	if(where == SIG_IGN || bdos(CPMICON) == 0)
		return;
	c = bdos(CPMRCON);
	if(c != CPMRBT)
		return;
	if(where == SIG_DFL)
		exit();
	(*where)();
}
