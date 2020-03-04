#include    "cpm.h"

int isatty(uchar f)
{
    switch(_fcb[f].use)
    {
    case U_CON:
    case U_RDR:
    case U_PUN:
    case U_LST:
    case U_RSX:
    case U_ERR:
        return 1;
    }
    return 0;
}
