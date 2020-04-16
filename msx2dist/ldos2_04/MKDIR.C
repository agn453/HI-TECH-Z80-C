/*
 * mkdir (char *)
 *
 * create a directory with the given name.
 */

extern int _act_file(char*, int, int);

mkdir(name)
char *	name;
{ 
  int res;  
  res = _act_file(name, 8, 0x1044);
  return (res < 0) ? -1 : 0;
}

