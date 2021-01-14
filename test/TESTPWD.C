#include <cpm.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct fcb ffcb;

char password[20];

char *getpass(struct fcb *pfcb)
{
    char
       *cp,
        lyne[100];
    printf("Enter password: ");
    fflush(stdout);
    gets(lyne);
    if (strlen(lyne))
    {
        for (cp = lyne; *cp; ++cp)
            if (islower(*cp))
                *cp = toupper(*cp);
        strncpy(password,lyne,8);
        strcat(password, "        ");
        return password;
    }
    return NULL;
}


int main(int argc, char *argv[])
{
    FILE
        *f;
    int
        c;

    if (argc < 2)
        return 0;

    bdos(CPMERRM, 0xFF);
    setfcb(&ffcb, argv[2]);
/*  _passwd = getpass;  */

    f = fopen(argv[1],"r");

    if (f)
    {
        printf("Processing %s:\n", argv[1]);
        while (!feof(f))
        {
             c = fgetc(f);
             if (!feof(f))
                 putchar(c);
        }
        fclose(f);
        return 0;
    }
    else
    {
        printf("Failed to open %s", argv[1]);
        perror(" - ");
    }
    return 1;
}
