#include <string.h>
extern char *malloc();

char * strdup(char * str)
{
  char *dup;

  if ( (dup = malloc(strlen(str)+1)) ) /* add one for the closing \0 */
    return strcpy(dup, str);
  return (dup);
}
