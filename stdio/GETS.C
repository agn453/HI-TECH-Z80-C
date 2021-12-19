/*
 *  gets and fgets for HI-TECH C stdio
 */

#include    <stdio.h>
#include    <string.h>

char *fgets(char *s, int n, register FILE *f)
{
    char *s1 = s;
    int c;

    while (--n && (c = fgetc(f)) != EOF && (*s++ = c) != '\n')
        /* VOID */;
    *s = 0;
    if (s == s1)
        return (char *)NULL;
    return s1;
}

char *gets(char *s)
{
    if ((s = fgets(s, -1, stdin)) == (char *)NULL)
        return (char *)NULL;
    s[strlen(s)-1] = 0;
    return s;
}
