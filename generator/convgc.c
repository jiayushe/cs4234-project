/*  Translates graph file in generator format to challenge ascii format.
    To compile:  cc -o convgc convgc.c graphtt.c
    To run: convgc <file1> <file2>

    where file1 holds the input in generator format. The output is
    written to file2 in challenge format.
*/

#include <stdio.h>

/* #include "defs.h" */

#define MAXN 3400
#define MAXNAME 28

/* stores a partition */
struct partlist {
  int size;             /* number of parts in partition */
  int list[MAXN];       /* the parts */
};

main(argc,argv)
int argc; char *argv[];
{
   char file1[MAXNAME], file2[MAXNAME];
   char seedfile[MAXNAME];
   FILE *fp, *fopen();
   int mprime,n,m,c,i,j,rr,seed;

   strcpy(file1,argv[1]);
   strcpy(file2,argv[2]);
   
   loadgraph(file1,&n,&m,&c,&rr,&seed);
   fp = fopen(file2,"w");
   fprintf(fp,"c %d vertices %d edges %d max clique\n",n,m,c);
   fprintf(fp,"c r = %d, seed = %d\n",rr,seed);
   fprintf(fp,"p edge %d %d\n",n,m);
   for (i=0;i<n;i++) for (j=i+1;j<n;j++) if (isedge(i,j))
     fprintf(fp,"e %d %d \n",i+1,j+1);
   fclose(fp);
}
