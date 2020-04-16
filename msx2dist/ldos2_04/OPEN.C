/*
 * opens the file with the given name
 * slashes in the name are converted to backslashes
 *
 * mode: 0 - read
 *       1 - write
 *       2 - read/write
 */

_act_file (char * name, int mode, int bc);

open(name, mode)
char *	name;
{
  return _act_file(name, mode, 0x43);
}

