#include <cpm.h>

static char	mess[] = "Fatal error - program aborted\r\n$";

abort()
{
	bdos(CPMWCOB, mess);
	exit(-1);
}
