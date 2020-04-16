/* 
 * creat(name, mode)
 *
 * creates a file with the given name, and opens it for reading and writing
 * mode: ignored
 *
 */
extern int _act_file(char*, int, int);

creat(name, mode)
char *	name;
{ 
  return _act_file(name, 2, 0x0044);
}
