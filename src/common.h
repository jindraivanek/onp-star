#ifndef _COMMON_H
#define _COMMON_H

typedef unsigned int uint;

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include <malloc.h>

#define fwriteone(x,f) if(fwrite(&x, sizeof(x), 1, f)!=1) {fprintf(stderr, "Error: %s, ", strerror(errno)); assert(0); }
#define fwritearray(x,size,f) if(fwrite(x, sizeof(*x), size, f)!=size) {fprintf(stderr, "Error: %s, ", strerror(errno)); assert(0); }
#define freadone(x,f) if(fread(&x, sizeof(x), 1, f)!=1 && !feof(f)) {fprintf(stderr, "Error: %s, ", strerror(ferror(f))); assert(0); }
#define freadarray(x,size,f) if(fread(x, sizeof(*x), size, f)!=size && !feof(f)) {fprintf(stderr, "Error: %s, ", strerror(errno)); assert(0); }
#define bufwriteone(x,buf) { memcpy(buf, &x, sizeof(x)); buf+=sizeof(x); }
#define bufwritearray(x,size,buf) { memcpy(buf, x, sizeof(*x)*size); buf+=sizeof(*x)*size; }
#define bufreadone(x,buf) { memcpy(&x, buf, sizeof(x)); buf+=sizeof(x); }
#define bufreadarray(x,size,buf) { memcpy(x, buf, sizeof(*x)*size); buf+=sizeof(*x)*size; }

#define assertstr(x,str) { if(!(x)) { fprintf(stderr, "Error: %s\n",str); assert(x); } }
#define assertcmd(x,cmd) { if(!(x)) { cmd; assert(x); } }
#define exitstr(x,str) { if(!(x)) { fprintf(stderr, "Error: %s\n",str); exit(EXIT_FAILURE); } }

#define EPSILON 1E-9
#define doublelt(a,b) (((a)-(b)) <= -EPSILON)
#define doublelteq(a,b) (((a)-(b)) < EPSILON)
#define doublegt(a,b) (((a)-(b)) >= EPSILON)
#define doublegteq(a,b) (((a)-(b)) > -EPSILON)
#define doubleeq(a,b) (doublelteq(a,b) && doublegteq(a,b))

#define randRange(a, b) (((double) rand() / RAND_MAX) * (b-a) + a)
#define max(a, b) (doublegteq(a,b) ? a : b) 

#endif
