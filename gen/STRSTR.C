/*
 * char * strstr (char *t, char *s) 
 * 
 * Find string s in string t.
 *
 * Return 
 *    -	The smallest pointer p between t and t+strlen(t) inclusive, such that
 *	the characters from s upto but not including the first NUL character 
 *	can be found starting from p, or
 *    -	NULL if there is no such pointer.
 */
char * strstr (char *t, char *s) 
{
   char *t1;
   char *s1;

   do 
     {
       t1 = t;
       s1 = s;
       while(*s1) {
         if (*s1 != *(t1++)) 
	    break;
	 else
	    ++s1;
       }
       if (!*s1) return t;
     } 
   while (*(t++));

   return (char *) 0;
}
