#include	"cpm.h"
#include	<signal.h>

static signal_t where;

signal_t signal(int sig, signal_t action) 
{
	signal_t prev;

	if(sig != SIGINT)
		return (signal_t)-1;
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
		exit(0);
	((int(*)())where)(0);
}
