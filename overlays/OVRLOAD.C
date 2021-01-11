#include <overlay.h>
#include <string.h>
#include <unixio.h>

extern unsigned _ovrsize;
extern char *_ovrstart;
extern intptr_t _ovrbgn(intptr_t args);
extern char ovrfilename[15]={0};

/* Overlay loader for Hi-Tech C */
intptr_t ovrload(char *name,intptr_t args)
{
	int fd, size;
    char ovrname[9];
	char filename[15], *p, *index();

    strncpy(ovrname,name,8);
    ovrname[8]=0;
	strcpy(filename,ovrname);	/* Copy the filename */
	strcat(filename,".ovr");	/* add the extent */
	if ((fd = open(filename, 0)) < 0)
	{
		strcpy(filename,"a0:");	/* not there -- see if it's on A0: */
		strcat(filename, (p = index(ovrname,':')) ? p+1 : ovrname);
		strcat(filename,".ovr");
		if ((fd = open(filename,0)) < 0) {
			goto error;		/* Not there either */
        }
	}
    if(!strcmp(filename,ovrfilename) && _ovrbgn) {
        return _ovrbgn(args);
    }
	size = read(fd,_ovrstart,_ovrsize);
	close(fd);
	if (size < 0 || !_ovrbgn) {
		goto error;			/* read error */
    }
    strcpy(ovrfilename,filename);
	return _ovrbgn(args);		/* ok, execute the overlay */

error:
	return -1;
}
