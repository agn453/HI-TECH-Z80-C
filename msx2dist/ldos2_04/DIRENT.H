/* dirent.h */
/* Declarations for directory access in libdos2. */

#define NAME_MAX 12

struct dirent /* A dos2 file information block. */
{
  char d_valid;
  char d_name[NAME_MAX+1];
  char d_attribute;
  unsigned short int d_mtime; 
  unsigned short int d_mdate;
  unsigned short int d_cluster; 
  unsigned long int d_size;
  char d_drive;
  char d_private[64-26]; /* for use by dos2, do not change */ 
} ;

struct _dir {
  struct dirent dir_dir;
  struct dirent dir_file;
} ;

typedef struct _dir DIR;

extern DIR * opendir (char * name);
extern struct dirent * readdir (DIR * dir);
extern int closedir (DIR * dir);
extern void rewinddir (DIR * dir);
extern int mkdir (char *);
extern int rmdir (char *);

