/****************************************************************************
  This file includes functions for manipulating a graph 
 ****************************************************************************/

#include <stdio.h>

/* #include "defs.h" */

#define MAXN 3400
#define MAXNAME 32

/* stores a partition */
struct partlist {
  int size;             /* number of parts in partition */
  int list[MAXN];       /* the parts */
};

typedef int *Row;

static int N;                         /* number of vertices in the graph */
static Row *Adjmat;

/************************ create empty graph ********************************/
initgraph(n)
int n;
{
  int v,w;

  N = n;
  Adjmat = (Row *) malloc(n*sizeof(Row));
  for (v=0;v<n;v++) Adjmat[v] = (int *) malloc(n*sizeof(int));
  for (v=0;v<n;v++) for (w=0;w<n;w++) Adjmat[v][w] = 0;
}


/************************ complements the graph *****************************/
complement() {
  int n,v,w;

  for (v=0;v<N;v++) for (w=0;w<N;w++) if (v!=w) Adjmat[v][w] = !Adjmat[v][w];
}

/************************ prints graph to file  ****************************/
printgraph(filename,n,m,c,rr,seed)
char filename[];
int n,m,c,rr,seed;
{     
  int v,w,count;
  FILE *fp,*fopen();

  fp = fopen(filename,"w");
  fprintf(fp,"%d %d %d %d %d\n",N,m,c,rr,seed);
  for (v=0;v<N;v++) {
    count = 0;
    for (w=0;w<N;w++) if (Adjmat[v][w]) count++;
    fprintf(fp,"%d ",count);
    for (w=0;w<N;w++) if (Adjmat[v][w]) fprintf(fp," %d",w);
    fprintf(fp,"\n");
  }
}

/********************* returns 1 if vertices i and j are adjacent ***********/
isedge(i,j)
int i,j;
{
  return(Adjmat[i][j]);
}

/******************** inserts edge (i,j) ***********************************/
addedge(i,j)
int i,j;
{
  Adjmat[i][j] = 1;
  Adjmat[j][i] = 1;
}     

/************ returns a random permutation of numbers 0-(n-1) in pos ********/
permutepos(pos,n)
int pos[], n;
{
  float unifdist(),r1;
  int i,temp,t1;

  for (i=0;i<n;i++) 
    pos[i] = i;

  for (i=0;i<n;i++) {
    r1 = unifdist((float) n-i);
    t1 = (int) r1;
    if (t1==n-i) t1--;
    t1 = i + t1;
    temp = pos[t1];
    pos[t1] = pos[i];
    pos[i] = temp;
  }
}
/************************** permute graph *******************************/
permuteg()
{
  int perms[MAXN];
  int v,w,v1,w1,*row;
  Row *Adjmat1;

  permutepos(perms,N);   /* get permutation of vertices */
  Adjmat1 = (Row *) malloc(N*sizeof(Row));
  row = (int *) malloc(N*sizeof(int));
  for (v=0;v<N;v++) Adjmat1[v] = Adjmat[v];
  for (v=0;v<N;v++) {
    v1 = perms[v];
    Adjmat[v1] = Adjmat1[v];
    for (w=0;w<N;w++) row[w] = Adjmat1[v][w];
    for (w=0;w<N;w++) {
      w1 = perms[w]; Adjmat[v1][w1] = row[w];
    }
  }
  free(Adjmat1); free(row);
}
/************************** reads in graph from file filename ***************/
loadgraph(filename,npt,mpt,cpt,rrpt,seedpt)
char filename[];
int *npt,*mpt,*cpt,*rrpt,*seedpt;
{
  FILE *fp,*fopen();
  int v,w,ver,ct;

  fp = fopen(filename,"r"); 
  fscanf(fp,"%d %d %d %d %d",npt,mpt,cpt,rrpt,seedpt);
  N = *npt;
  Adjmat = (Row *) malloc(N*sizeof(Row));
  for (v=0;v<N;v++) Adjmat[v] = (int *) malloc(N*sizeof(int));
    
  for (v=0;v<N;v++) {
    for (w=0;w<N;w++) Adjmat[v][w] = 0;
    fscanf(fp, "%d",&ct);
    for (w=0;w<ct;w++) {
      fscanf(fp, "%d",&ver);
      Adjmat[v][ver] = 1;
    }
  }
  fclose(fp);
}
