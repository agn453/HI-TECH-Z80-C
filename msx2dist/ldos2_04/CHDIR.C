/*
 * file: chdir.c
 * author Arnold Metselaar
 */
int _act_file(char * name, short mode, short bc);

int chdir(char * pad)
{
  return _act_file(pad, 0, 0x0059);
}
