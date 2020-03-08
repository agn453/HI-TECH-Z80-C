
#include <stdio.h>      /* for printf */
#include <time.h>       /* for strftime */

char test[80];

int main(int argc, char *argv[])
{
      int len;
      char *fmt;
      time_t now;

      time(&now);

      fmt = (argc == 1) ? "%I:%M %p\n%c\n" : argv[1];
      len = strftime(test,sizeof test, fmt, localtime(&now));
      printf("%d: %s\n", len, test);
      return !len;
}
