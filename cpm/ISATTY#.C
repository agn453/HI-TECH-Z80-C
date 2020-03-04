#include	"cpm.h"

isatty(f)
uchar	f;
{
	switch(_fcb[f].use) {

	case U_CON:
	case U_RDR:
	case U_PUN:
	case U_LST:
		return 1;
	}
	return 0;
}
