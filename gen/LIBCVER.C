#define VERSION "3.09-18"

char *_libcver =
#ifdef Z280
 "LIB280C " VERSION;
#else
 "LIBC " VERSION;
#endif
