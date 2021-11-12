/**************************************************************************
  This file contains functions for producing and manipulating partitions.
 **************************************************************************/

#include <stdio.h>

/* #include "defs.h" */

#define MAXN 3400
#define MAXNAME 32

/* stores a partition */
struct partlist {
  int size;             /* number of parts in partition */
  int list[MAXN];       /* the parts */
};

/*********************** produces a simple partition **********************
 of n having k parts, and puts it in *partptr *****************************/
simplepart(n,k,partptr)
int n,k;
struct partlist *partptr;
{
  int l,q,r;

  q = n / k;
  r = n % k;
  for (l=0;l<r;l++) 
    partptr->list[l] = q+1;
  for (l=r;l<k;l++)
    partptr->list[l] = q;
  partptr->size = k;
}

/******************* prints partition part ****************************/
printpart(part)
struct partlist part;
{
  int k;

  for (k=0;k<part.size;k++)
    printf("%4d",part.list[k]);
  printf("%6d", sumsquares(part));
  printf("\n");
}

/******************* prints partition part ****************************/
printpartfile(part,file)
struct partlist part;
char *file;
{
  int k;
  FILE *fp,*fopen();

  fp = fopen(file,"w");
  for (k=0;k<part.size;k++)
    fprintf(fp,"%4d",part.list[k]);
  fprintf(fp,"%6d", sumsquares(part));
  fprintf(fp,"\n");
  fclose(fp);
}

/***************** returns the sum of the squares of partition part **********/
sumsquares(part)
struct partlist part;
{
  int sum, i;

  sum = 0;
  for (i=0; i<part.size; i++)
    sum += part.list[i]*part.list[i];
  return(sum);
}
