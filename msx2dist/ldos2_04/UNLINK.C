/* remove file of given  name */
extern int _act_file(char * name, int mode, int bc);
extern int errno;

unlink(name)
char *	name;
{
  return _act_file(name, 0, 0x4D);
}

