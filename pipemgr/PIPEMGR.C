/*********************************************************************
* PIPEMGR helper subroutines. One tests if stdin is coming from a    *
* pipe, and the other shuffles the arguments so that blank arguments *
* are dropped.                                                       *
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys.h>
#include <cpm.h>
#include "pipemgr.h"

extern int _piped;

struct
{
      char func;
      char npars;
      char *txt;
} pcb = {0x76, 1, "PIPEMGR "};


int stdin_piped(void)
{
      if (!_piped) return 0;

      return (bdoshl(0x3C, &pcb) & 0x100);
}


void init_args(int *argc, char ***argv)
{
    int i,j;

    for (i = 1; i < (*argc); i++)
    {
        if ((*argv)[i][0]) continue;
        for (j = i; j < ((*argc) - 1); j++) (*argv)[j] = (*argv)[j+1];
        --i; 
        --(*argc);
    }
}


char safe_toupper(char c)
{
      if (!islower(c)) return c;
      return toupper(c);
}

