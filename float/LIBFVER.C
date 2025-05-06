#define VERSION "3.09-18"

char *_libfver =
#ifdef Z280
 "LIB280F " VERSION;
#else
 "LIBF " VERSION;
#endif
