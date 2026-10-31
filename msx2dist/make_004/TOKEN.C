#include <stdio.h>
#include <ctype.h>
#include "make.h"

void stripwh(char **strpp);

/*
 * Get next token from the string.  Return a pointer to it, or NULL.
 * Adjust pointer to point to next part of string.
 * The string is modified.
 * A token consists of any number of non-white characters.
 */
char *token(char **strpp)
{
  char *s, *beg;

  stripwh(strpp);
  if (!**strpp) return NULL;

  beg = s = *strpp;
  while (*s && !isspace(*s)) ++s;
  if (*s) *s++ = '\0';
  *strpp = s;
  return beg;
}


/*
 * Parse character escape-sequences in a line of text.
 *	<EscChar><EscChar> = <EscChar>
 *	<EscChar>n = newline, and so on
 *	<EscChar><char> = <char>
 * The string is truncated at the first non-escaped occurance of 'comchar'.
 */
escape(char *str, char comchar)
{
  char *d, c;

  for (d = str; *str && *str != comchar; ++str)
    if (*str == ESCCHAR && *(str + 1)) 
      switch((c = *++str)) {
	case ESCCHAR:
	  *d++ = ESCCHAR;
	  break;
	case 'n':
	  *d++ = '\n';
	  break;
        case 'r':
	  *d++ = '\r';
	  break;
	case 't':
	  *d++ = '\t';
	  break;
	case 'b':
	  *d++ = '\b';
	  break;
        case 'f':
	  *d++ = '\f';
	  break;

	default:
	  *d++ = c;
	  break;
    } 
    else *d++ = *str;

  *d++ = 0;
}


void stripwh(char **strpp)
{
  char *s;

  s = *strpp;
  while(isspace(*s)) ++s;
  *strpp = s;
}

