/*
 *	Copyright (C) 1984-1897 HI-TECH SOFTWARE
 *
 *	This software remains the property of HI-TECH SOFTWARE and is
 *	supplied under licence only. The use of this software is
 *	permitted under the terms of that licence only. Copying of
 *	this software except for the purpose of making backup or
 *	working copies for the use of the licensee on a single
 *	processor is prohibited.
 */
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<cpm.h>
#include	<exec.h>
#include	<stat.h>
#include    <sys.h>
#include    <stdlib.h>
#include    <unixio.h>

#define MAXLINE (200)
#define START ("__ovrbgn")
int sym2as(char * fname, char * tmpas);

int main(int argc, char ** argv) {
    int rc=0;
    fprintf(stderr,"SYMTOAS V1.00\n");
    if(argc<2) {
        fprintf(stderr,"Missing sym file\n");
        return -1;
    }
    if(argc==2) {
        fprintf(stderr,"Yop");
        rc=sym2as(argv[1],0);
    } else {
        rc=sym2as(argv[1],argv[2]);
    }
    if(rc==-1) {
        return -1;
    }
    return 0;
}

int sym2as(char * fname, char * tmpas) {
    static char line[MAXLINE+1];
    char *addr=line;
    char *sym=line+5;
    register int i=0;
    register int j=0;
    int c=0;
    char* r=0;
    int base=0x100;
    FILE * in;
    FILE * out;

    in=fopen(fname,"rt");
    if(!in) {
        fprintf(stderr,"Cannot open %s\n",fname);
        return -1;
    }
    if(0==tmpas) {
        out=stdout;
    } else {
        out=fopen(tmpas,"wt");
        fprintf(stderr,"%s %X\n",tmpas,out);
        if(!out) {
            fclose(in);
            return -1;
        }
    }
    fprintf(out,"%31s\n","psect data");
    while(!feof(in)&& !ferror(in)) {
        i=0; 
        while(i<MAXLINE) {
            c=fgetc(in);
            if(isspace(c)&&(i==0)) {
                continue;
            }
            if(c==EOF ||c=='\n'||c=='\r') {
                break;
            }
            line[i++]=c;
        }
        line[i]=0;
        r=index(line,' ');
        if(!r) {
            continue;
        }
        *r=0;
        addr=line;
        sym=r+1;
        while(*sym && isspace(*sym)) {
            sym++;
        }
        fprintf(out,"%27s %s\n","global",sym);
        if(strlen(addr)>4) {
            addr+=strlen(addr);
            addr-=4;
        }
        for(j=strlen(sym)-1;j>=0;j--) {
            if(isspace(sym[j])) {
                sym[j]=0;
            }
        }
        fprintf(out,"%-20s %s 0%sh\n",sym,"defl",addr);
    }
    fprintf(out,"%24s\n","end\n");
    if(ferror(in)) {
        fclose(in);
        if(0!=tmpas) {
            fclose(out);
        }
        return -1;
    }
    fclose(in);
    if(0!=tmpas) {
        fclose(out);
    }
    return base;
}
