
/*-
 * All OPTIMH.C source code is 
 * (c) Copyright (1993-96) by Stefan Nitschke and Alexander Schmid
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, (2) it is not used for military purpose in
 * any form, and (3) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by Stefan Nitschke and his contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the author nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
   Z280 optimiser for the HITECH 3.09 c-compiler
   St.Nitschke 11.05.93
   Erweitert und Umgesetzt von Turbo-Pascal auf Hi-Tech-C
   (Converted and enhanced from Turbo-Pascal to Hi-Tech-C)
   Alexander Schmid 30.10.93

   This version is included in the GitHub repository for Hi Tech Z80 C
   at https://github.com/agn453/HI-TECH-Z80-C

   Modification History

   31-Oct-2023	Tony Nicholson
		Translated German to English comments added in brackets;
		Correct additional byte and replaced counter updates;
		Summary output includes "speed" or "size";
		Change default filetype for output from .ASO to .AS2;
		Some "call csv" and "jp cret" speed optimisations were
		 not being detected (add space and tab tests); and
		Statement labels on optimised statements were being
		 omitted from optimised output.

   18-Mar-1996	Stefan Nitschke

   The following instructions are optimised:

      call amul        -> multw hl,de

      call lmul        -> multuw hl,de

      call csv		-> push	iy, push ix, lda ix,(sp+0)  (+6 bytes)

      jp cret		-> ld sp,ix pop ix pop iy ret (+4 bytes)

      ld  (hl),c
      inc hl           -> ldw (hl),bc
      ld  (hl),b       -> inc hl

      ld  (hl),e
      inc hl           -> ldw (hl),de
      ld  (hl),d       -> inc hl

      ld  c,(hl)
      inc hl           -> ldw bc,(hl)
      ld  b,(hl)       -> inc hl

      ld  e,(hl)
      inc hl           -> ldw de,(hl)
      ld  d,(hl)       -> inc hl

      ld  c,(ix+n)     or iy
      ld  b,(ix+n+1)   -> ldw bc,(ix+n)    (2 byte)

      ld  e,(ix+n)     or iy
      ld  d,(ix+n+1)   -> ldw de,(ix+n)    (2 byte)

      ld  l,(ix+n)     or iy
      ld  h,(ix+n+1)   -> ldw hl,(ix+n)    (2 byte)

      ld  (ix+n),c     or iy
      ld  (ix+n+1),b   -> ldw (ix+n),bc    (2 byte)

      ld  (ix+n),e     or iy
      ld  (ix+n+1),d   -> ldw (ix+n),de    (2 byte)

      ld  (ix+n),l     or iy
      ld  (ix+n+1),h   -> ldw (ix+n),hl    (2 byte)

      or  a
      sbc hl,bc        -> subw hl,bc       (1 byte)

      or  a
      sbc hl,de        -> subw hl,de       (1 byte)

      push  ix
      pop   de
      ld    hl,nn
      add   hl,de      -> lda hl,(ix+nn)   (3 byte)

      push  ix         -> ld  e,ixl
      pop   de         -> ld  d,ixh

      push  ix
      pop   hl         -> lda hl,(ix+0)

   Addressing with large offset

      push  ix
      pop   de
      ld    hl,nn
      add   hl,de      -> lda hl,(ix+nn)   (3 byte)

      push	iy
      pop	de
      ld	hl,nn
      add	hl,de	-> lda	hl,(iy+nn)  (3 byte)


   The following didn't work:
      ld    hl,nn
      push  hl         -> push nn

      ld    hl,(nn)
      push  hl         -> push (nn)

      ld    de,(nn)
      ld    hl,mm      -> ld   hl,(nn)
      add   hl,de      -> lda  hl,(hl+mm)  (1 byte)

 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define TRUE 1
#define FALSE 0

extern char *rindex();

FILE	*infile,*outfile;
char	*inptr,inname[20],*outptr,outname[20];
int	i,count,repl,lines,posit;
char	found;
char	speed;
char	text[101];
char	line[6][101];

/* sucht dden String s im String t */
/* (search for the string s in the string t) */
int instr(char *t, char *s) {
    register int i,j,k;
    for (i=0; s[i] != '\0'; ++i) {
        for (j=i, k=0; t[k]!='\0' && s[j]==t[k]; ++j, ++k);
        if (t[k] == '\0') return(i+1);
    }
    return 0;
}

/* kopiert ab Position p maximal i Zeichen von String a in String b */
/* (from position p, copy up to i characters from string a to string b) */
void copyn(char *a, char *b, int p, int i) {
    register int x;
    char ch;
    for(x=0; x<100; ++x) b[x]='\0';
    for(x=p; x<p+i; ++x){
	ch = a[x];
	b[x-p] = (ch!=0x0D) && (ch!=0x0A) && (ch!=0x00) ? ch : '\0';
    }
}

void csv(char *str1,char *bytes) {
    if(instr(str1,line[0])) {
        fprintf(outfile,";->call csv\n");
        fprintf(outfile,"push\tiy\npush\tix\n;-> lda ix,(sp+0)\ndefb\t%s\n",bytes);
        lines = 0;
        count -= 6;	/* 6 bytes mehr aber schneller */
        repl += 3;	/*	   more (but faster) */
        found = TRUE;
    }
}

void cret(char *str1) {
    if(instr(str1,line[0])) {
        fprintf(outfile,";->jp cret\n");
        fprintf(outfile,"ld\tsp,ix\npop\tix\npop\tiy\nret\n");
        lines = 0;
        count -= 4;	/* 4 bytes mehr aber schneller */
        repl += 3;	/*	   more (but faster) */
        found = TRUE;
    }
}

void mult(char *str1, char *str2,char *bytes) {
    if(instr(str1,line[0])) {
        fprintf(outfile,";->%s  hl,de\n",str2);
        fprintf(outfile,"defb\t%s\n",bytes);
        lines = 0;
        ++count;	/* 1 byte less */
        repl += 3;	/* replaces call amul/lmul */
        found = TRUE;
    }
}

void ldw1(char *str1, char *byte1) {  /* ldw  (hl),xx */
    char *strr1="ld\t(hl),b";
    strr1[8]=str1[1];
    if(instr(strr1,line[0])) {
        if(lines<1) fgets(line[1],100,infile);
        lines = 1;
        if(instr("inc\thl",line[1])) {
            if(lines<2) fgets(line[2],100,infile);
            lines = 2;
            strr1[8]=str1[0];
            if(instr(strr1,line[2])) {
                fprintf(outfile,";->ldw (hl),%s\n",str1);
                fprintf(outfile,"defb\t%s\n",byte1);
                fputs("inc\thl\n",outfile);
                lines = 0;
                found = TRUE;
                repl += 2;
            }
        }
    }
}

void ldw2(char *str1, char *byte1) {  /* ldw  xx,(hl) */
    char *strr1="ld\tb,(hl)";
    strr1[3]=str1[1];
    if(instr(strr1,line[0])) {
        if(lines<1) fgets(line[1],100,infile);
        lines = 1;
        if(instr("inc\thl",line[1])) {
            if(lines<2) fgets(line[2],100,infile);
            lines = 2;
            strr1[3]=str1[0];
            if(instr(strr1,line[2])) {
                fprintf(outfile,";->ldw %s,(hl)\n",str1);
                fprintf(outfile,"defb\t%s\n",byte1);
                fputs("inc\thl\n",outfile);
                lines = 0;
                found = TRUE;
                repl += 2;
            }
        }
    }
}

void ldw3(char *str1, char *str2, char *byte1) {  /* ldw xx,(yy+ofs) */
    char strr1[20];
    strcpy(strr1,"ld\tc,("); strcat(strr1,str2);
    strr1[3]=str1[1];
    if(instr(strr1,line[0])) {
        if(lines<1) fgets(line[1],100,infile);
        lines = 1;
        strr1[3]=str1[0];
        if(instr(strr1,line[1])) {
            lines = 0;
            found = TRUE;
            count += 2;
            repl += 4;
            posit = instr(str2,line[0])+1;
            copyn(line[0],text,posit,instr(")",line[0])-posit-1);
            fprintf(outfile,";->ldw  %s,(%s%s)\n",str1,str2,text);
            fprintf(outfile,"defb\t%s,%s\n",byte1,text);
        }
    }
}

void ldw4(char *str1, char *str2, char *byte1) {  /* ldw  (xx+ofs),yy */
    char strr1[20],strr2[20];
    strcpy(strr1,"ld\t("); strcat(strr1,str2); 
    strcpy(strr2,"),c"); strr2[2]=str1[1];
    if(instr(strr1,line[0]) && instr(strr2,line[0])) {
        if(lines<1) fgets(line[1],100,infile);
        lines = 1;
        strr2[2]=str1[0];
        if(instr(strr1,line[1]) && instr(strr2,line[1])) {
            lines = 0;
            found = TRUE;
            count += 2;
            repl += 4;
            posit = instr(str2,line[0])+1;
            copyn(line[0],text,posit,instr(")",line[0])-posit-1);
            fprintf(outfile,";->ldw  (%s%s),%s\n",str2,text,str1);
            fprintf(outfile,"defb\t%s,%s\n",byte1,text);
        }
    }
 }
 
void pushix(void) {
    fprintf(outfile,";->ld  d,ixh\n");
    fprintf(outfile,"defb\t0ddh,54h\n");
    fprintf(outfile,";->ld  e,ixl\n");
    fprintf(outfile,"defb\t0ddh,5dh\n");
    found = TRUE;
    --count;  /* eine Byte MEHR, aber schneller ! */
    repl += 4; /* one more byte (but faster) */
}
 
void pushiy(void) {
    fprintf(outfile,";->ld  d,iyh\n");
    fprintf(outfile,"defb\t0fdh,54h\n");
    fprintf(outfile,";->ld  e,iyl\n");
    fprintf(outfile,"defb\t0fdh,5dh\n");
    found = TRUE;
    --count;  /* eine Byte MEHR, aber schneller ! */
    repl += 4; /* one more byte (but faster) */
}

void subwhl(char *str1, char *byte1) {
    if(instr("or\ta",line[0])) {
        char strr1[20];
        strcpy(strr1,"sbc\thl,"); strcat(strr1,str1); 
        if(lines<1) fgets(line[1],100,infile);   /* subw  hl,xx */
        lines = 1;
        if(instr(strr1,line[1])) {
            lines = 0;
            found = TRUE;
            ++count;
            repl += 2;
            fprintf(outfile,";->subw  hl,%s\n",str1);
            fprintf(outfile,"defb\t%s\n",byte1);
        }
    }
}

void label(void) {
/* check for statement label in line[0].  if detected, output the label
   on a line by itself and strip the label from line[0] */

    register int p, x;
    char lab[101], islabel, c;

    if(p = instr(":",line[0])) {
	copyn(line[0],lab,0,p); /* copy label */
	islabel = TRUE;
	for (x=0; x<(p-1); x++) {
	    c = lab[x]; /* valid chars are 0..9, a..z, $ and _ */
	    if( (!isalnum(c)) && (c!='$') && (c!='_') ) {
		islabel = FALSE;
	    }
	}
	if (islabel && (line[0][p+1]!='\0')) { /* check if bare label too */
            fprintf(outfile,"%s\n",lab);
	    for (x=0; x<=p; x++) line[0][x] = ' '; /* blank out label */
	}
    }
}

void check(void) {
/******************************************************************************/
    if(instr("mul",line[0])) {
       if(!found) mult("call\tamul","multw","0edh,0d2h");  /* multw  hl,de    */
       if(!found) mult("call\tlmul","multuw","0edh,0d3h"); /* multuw  hl,de   */
    }
/******************************************************************************/
    if(speed) {
        /* both these increase speed at the expense of 6 extra bytes */
        if(instr("csv",line[0])) {
            if(!found) csv("call csv","0ddh,0edh,02h,0,0");  /* call csv   */
            if(!found) csv("call\tcsv","0ddh,0edh,02h,0,0");  /* call csv   */
        }
        if(instr("cret",line[0])) {
            if(!found) cret("jp cret");  /* jp cret   */
            if(!found) cret("jp\tcret");  /* jp cret   */
        }
    }
/******************************************************************************/
    if(instr("(hl),",line[0])) {
        if(!found) ldw1("bc","0edh,0eh");                  /* ldw  (hl),bc    */
        if(!found) ldw1("de","0edh,1eh");                  /* ldw  (hl),de    */
    }
/******************************************************************************/
    if(instr(",(hl)",line[0])) {
        if(!found) ldw2("bc","0edh,06h");                  /* ldw  bc,(hl)    */
        if(!found) ldw2("de","0edh,16h");                  /* ldw  de,(hl)    */
    }
/******************************************************************************/
    if(instr("(ix",line[0])) {
        if(!found) ldw3("bc","ix","0ddh,0edh,06h");        /* ldw bc,(ix+ofs) */
        if(!found) ldw3("de","ix","0ddh,0edh,16h");        /* ldw de,(ix+ofs) */
        if(!found) ldw3("hl","ix","0ddh,0edh,26h");        /* ldw hl,(ix+ofs) */

        if(!found) ldw4("bc","ix","0ddh,0edh,0eh");        /* ldw (ix+ofs),bc */
        if(!found) ldw4("de","ix","0ddh,0edh,1eh");        /* ldw (ix+ofs),de */
        if(!found) ldw4("hl","ix","0ddh,0edh,2eh");        /* ldw (ix+ofs),hl */
    }
/******************************************************************************/
    if(instr("(iy",line[0])) {
        if(!found) ldw3("bc","iy","0fdh,0edh,06h");        /* ldw bc,(iy+ofs) */
        if(!found) ldw3("de","iy","0fdh,0edh,16h");        /* ldw de,(iy+ofs) */
        if(!found) ldw3("hl","iy","0fdh,0edh,26h");        /* ldw hl,(iy+ofs) */

        if(!found) ldw4("bc","iy","0fdh,0edh,0eh");        /* ldw (iy+ofs),bc */
        if(!found) ldw4("de","iy","0fdh,0edh,1eh");        /* ldw (iy+ofs),de */
        if(!found) ldw4("hl","iy","0fdh,0edh,2eh");        /* ldw (iy+ofs),hl */
    }
/******************************************************************************/
    if(instr("or\ta",line[0])) {
        if(!found) subwhl("bc","0edh,0ceh");               /* subw  hl,bc     */
        if(!found) subwhl("de","0edh,0deh");               /* subw  hl,de     */
    }
/******************************************************************************/
   if(!found) if(instr("push\tix",line[0])) {
        if(lines<1) fgets(line[1],100,infile);             /* lda hl,(ix+ofs) */
        lines = 1;
        if(instr("pop\tde",line[1])) {
            if(lines<2) fgets(line[2],100,infile);
            lines = 2;
            if(instr("ld\thl,",line[2])) {
                if(lines<3) fgets(line[3],100,infile);
                lines = 3;
                if(instr("add\thl,de",line[3])) {
                    copyn(line[2],text,instr("hl,",line[2])+2,6);
                    if(instr(")",text))
                        copyn(text,text,instr(")",text)-1,strlen(text));
                    fprintf(outfile,";->lda  hl,(ix+%s)\n",text);
                    fputs("defb\t0edh,2ah\n",outfile);
                    fprintf(outfile,"defw\t%s\n",text);
                    lines = 0;
                    found = TRUE;
                    count += 3;
                    repl += 2;
                }
            }
            else {
                pushix();
                strcpy(line[0],line[1]);
                strcpy(line[1],line[2]);
                lines = 1;
            }
        }
        else if(instr("pop\thl",line[1])) {
            fprintf(outfile,";->lda  hl,(ix+0)\n");
            fprintf(outfile,"defb\t0edh,2ah,0,0\n");
            found = TRUE;
            repl += 4;
            --count;
            lines = 0;
        }
    }
/******************************************************************************/
   if(!found) if(instr("push\tiy",line[0])) {
        if(lines<1) fgets(line[1],100,infile);             /* lda hl,(iy+ofs) */
        lines = 1;
        if(instr("pop\tde",line[1])) {
            if(lines<2) fgets(line[2],100,infile);
            lines = 2;
            if(instr("ld\thl,",line[2])) {
                if(lines<3) fgets(line[3],100,infile);
                lines = 3;
                if(instr("add\thl,de",line[3])) {
                    copyn(line[2],text,instr("hl,",line[2])+2,6);
                    if(instr(")",text))
                        copyn(text,text,instr(")",text)-1,strlen(text));
                    fprintf(outfile,";->lda  hl,(iy+%s)\n",text);
                    fputs("defb\t0edh,32h\n",outfile);
                    fprintf(outfile,"defw\t%s\n",text);
                    lines = 0;
                    found = TRUE;
                    count += 3;
                    repl += 2;
                }
            }
            else {
                pushiy();
                strcpy(line[0],line[1]);
                strcpy(line[1],line[2]);
                lines = 1;
            }
        }
        else if(instr("pop\thl",line[1])) {
            fprintf(outfile,";->lda  hl,(iy+0)\n");
            fprintf(outfile,"defb\t0edh,32h,0,0\n");
            found = TRUE;
            repl += 4;
            --count;
            lines = 0;
        }
    }

/******************************************************************************/
/* verursacht Fehler */
/* (causes errors) */
#ifdef xxx
    if(!found) if(instr("ld\thl,",line[0]) && instr("(",line[0])==0) {
        if(lines<1)  fgets(line[1],100,infile);            /* push arg        */
        lines = 1;
        if(instr("push\thl",line[1])) {
            lines = 0;
            found = TRUE;
            repl += 2;
            posit = instr("hl,",line[0])+2;
            copyn(line[0],text,posit,50);
            fprintf(outfile,";->push  %s\n",text);
            fputs("defb\t0fdh,0f5h\n",outfile);
            fprintf(outfile,"defw\t%s\n",text);
        }
    }
#endif
/******************************************************************************/
/* verursacht Fehler */
/* (causes errors) */
#ifdef xxx
    if(!found) if(instr("ld\thl,(",line[0])) {
        if(lines<1)  fgets(line[1],100,infile);            /* push (arg)      */
        lines = 1;
        if(instr("push\thl",line[1])) {
            lines = 0;
            found = TRUE;
            repl += 2;
            posit = instr("hl,",line[0])+2;
            copyn(line[0],text,posit,50);
            fprintf(outfile,";->push  %s\n",text);
            fputs("defb\t0ddh,0d5h\n",outfile);
            ++posit;
            copyn(line[0],text,posit,instr(")",line[0])-posit-1);
            fprintf(outfile,"defw\t%s\n",text);
        }
    }
#endif
/******************************************************************************/
/* verursacht Fehler */
/* (causes errors) */
#ifdef xxx
   if(!found) if(instr("ld\tde,(",line[0])) {
        if(lines<1) fgets(line[1],100,infile);             /* lda hl,(hl+ofs) */
        lines = 1;
        if(instr("ld\thl,",line[1]) && !instr("(",line[1])) {
            if(lines<2) fgets(line[2],100,infile);
            lines = 2;
            if(instr("add\thl,de",line[2])) {
                posit = instr("de",line[0]+1);
                line[0][posit] = 'h'; line[0][posit+1] = 'l';
                copyn(line[1],text,instr("hl,",line[1])+2,50);
                fputs(line[0],outfile);
                fprintf(outfile,";->lda  hl,(hl+%s)\n",text);
                fputs("defb\t0edh,3ah\n",outfile);
                fprintf(outfile,"defw\t%s\n",text);
                lines = 0;
                found = TRUE;
                ++count;
                repl += 5;
            }
        }
    }
#endif 
/******************************************************************************/
}

main(int argc,*argv[]) {
    if(argc==1) {
        puts("Z280 optimiser for the HITECH C-Compiler");
        printf("Name of input file :"); scanf("%s",inname);
        printf("Name of output file :"); scanf("%s",outname);
    }
    else if(argc==2) {
        strcpy(inname,(char*)argv[1]);
        if(rindex(inname,'.')==NULL)
        strcat(inname,".AS");
        inptr=inname; outptr=outname;
        while(*inptr != '.') *outptr++ = *inptr++;
        strcat(outname,".AS2");
    }
    else if(argc==3) {
        strcpy(inname,(char*)argv[1]);
        strcpy(outname,(char*)argv[2]);
    }
    else if(argc==4) {
        if(stricmp((char *)argv[1],"-F")==0)
            speed = 1;
	else {
	    printf("First arg can only be -F for speed (Fast) optimization\n");
	    exit(1);
	}
        strcpy(inname,(char*)argv[2]);
        strcpy(outname,(char*)argv[3]);
    }
    if((infile = fopen(inname,"r"))==NULL) {
        printf("Can't open %s\n",inname);
        exit(1);
    }
    if((outfile = fopen(outname,"w"))==NULL) {
        printf("Can't open %s\n",outname);
        exit(1);
    }
    count = 0;
    repl = 0;
    while(fgets(line[0],100,infile) != NULL) {
	label();
        lines = 0;
        found = FALSE;
        check();
        if(!found) fputs(line[0],outfile);
        while(lines>0) {
            found = FALSE;
            strcpy(line[0],line[1]);
            strcpy(line[1],line[2]);
            strcpy(line[2],line[3]);
            --lines;
            check();
            if(!found) fputs(line[0],outfile);
        }
    }
    printf(" %d bytes %s optimised away\n",count,speed?"speed":"size");
    printf(" %d bytes replaced\n",repl);
    fputs("\n; optimiser statistics:\n",outfile);
    fprintf(outfile,"; %d bytes %s optimised away\n",count,speed?"speed":"size");
    fprintf(outfile,"; %d bytes replaced\n\n",repl);
    fputc(0x1a,outfile); /* explizites (explicit) EOF */
    fclose(infile);
    fclose(outfile);
    exit(0);
}  

