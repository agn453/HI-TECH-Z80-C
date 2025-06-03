#include <stdio.h>

char *_libfver =
#ifdef Z280
 "LIB280F " _HTC_VERSION;
#else
 "LIBF " _HTC_VERSION;
#endif
