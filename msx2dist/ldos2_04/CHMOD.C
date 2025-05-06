#include	<stat.h>

extern int errno;
extern int _act_file(char*, short, short);

chmod(name, mode)
register char *	name;
{
  int bc;
  
  bc=0x50;
  if ((mode & S_IWRITE) == 0) bc|=0x100;
  if (mode & S_HIDDEN) bc|=0x200;
  if (mode & S_SYSTEM) bc|=0x400;
  if (mode & S_ARCHIVE) bc|=0x2000;
  
  return _act_file (name, 0, bc);
}
