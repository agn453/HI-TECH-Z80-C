/* remove directory of given  name */
extern int _act_file(char * name, int mode, int bc);
extern int errno;

rmdir(name)
char *	name;
{
  return _act_file(name, 8, 0x4D);
}


