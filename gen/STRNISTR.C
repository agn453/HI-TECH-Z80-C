/*
 * char * strncasestr (char *t, char *s, unsigned int n) 
 * 
 * Find string s in string t, disregarding the case of letters.
 * At most n characters from t are used. 
 *
 * Returns 
 *    -	A smallest pointer p between t and t+strlen(t) inclusive, such that
 *	the characters from s upto but not including the first NUL character 
 *	can be found starting from p and strictly before t+n, or
 *    -	NULL if there is no such pointer.
 */
#include <ctype.h>

char * strncasestr (char *t, char *s, unsigned int n) 
{
   char *t1;
   char *s1;
   unsigned int n1;

   if (!*s) return t;
   while (*t && n) {
       t1 = t;
       n1 = n;
       s1 = s;
       while (*s1) {
           if (toupper(*s1) != toupper(*(t1++))) break;
           else {
               ++s1;
               if (!--n1) break;
           }
       }
       if (!*s1) return t;
       --n;
       ++t;
     }
   return (char *) 0;
}
