/*
 * char * strcasestr (char *t, char *s) 
 * 
 * Find string s in string t, disregarding the case of letters.
 *
 * Return 
 *    -	The smallest pointer p between t and t+strlen(t) inclusive, such that
 *	the characters from s upto but not including the first NUL character 
 *	can be found starting from p, or
 *    -	NULL if there is no such pointer.
 */
#include <ctype.h>

char * strcasestr (char *t, char *s) 
{
   char *t1;
   char *s1;

   do 
     {
       t1 = t;
       s1 = s;
       while(*s1) {
         if (toupper(*s1) != toupper(*(t1++))) 
	    break;
	 else
	    ++s1;
       }
       if (!*s1) return t;
     } 
   while (*(t++));

   return (char *) 0;
}
