/*****************************************************************************

  Main file for generator of graphs with known maximum clique size. 
  
 To run:
        % gen <n> <m> <c> <rr> <seed> <file>

 where 
       n = number of vertices
       m = number of edges
       c = maximum clique size
       rr = construction parameter, should be at least 0 and smaller than
            n/c. rr = n/(3c) seems to be a good value. See related paper
            for more details.
       seed = seed for random number generator.
       file = output file.

 References: Approximately solving maximum clique using neural network
             and related heuristics, A. Jagota, L. Sanchis, R. Ganesan,
             Second Dimacs Challenge Conference, October 1993.

             Some experimental and theoretical results on test case
             generators for the maximum clique problem, L. Sanchis and
             A. Jagota, Dimacs technical report 93-69, October 1993.

 *****************************************************************************/

#include <stdio.h>

/* #include "defs.h" */

#define MAXN 3400
#define MAXNAME 32

/* stores a partition */
struct partlist {
  int size;             /* number of parts in partition */
  int list[MAXN];       /* the parts */
};

static int *chosen;                 /* used in selecting edges */
/**********************************************************************/

/***** vertices are partitioned into cliques - partition is given in part -
       patt will hold the first and last vertex in each clique -
       in each clique, all vertices except first will be in cover **********/
fillin(part,patt)
struct partlist part;
int patt[2][MAXN];
{
  int ct, i, sum;

  ct = part.size;
  for (sum = 0, i = 0; i<ct; i++) {
    patt[0][i] = sum;
    patt[1][i] = (sum = sum + part.list[i]) - 1;
  }
}

/********* put in all clique edges *****************************/
addcliqedges(patt,ct)
int patt[2][MAXN], ct;
{
  int i,j,k;

  for (i=0;i<ct;i++)
    for (j=patt[0][i];j<patt[1][i];j++)
      for (k=j+1;k<=patt[1][i];k++)
        addedge(j,k);
}

/****************** selects choose bits out of ct bits, uniformly at random.
  result is put in array setbits ********************************************/
subset(setbits, ct, choose)
int setbits[], ct, choose;
{
  int totalct,i;
  float r, unifdist();

  totalct = ct;
  for (i=0;i<totalct;i++) {
    r = unifdist((float) ct--);
    if (setbits[i] = (r<choose)) choose--;
  }
}

/***** chooses edges going between pairs of cover vertices in different 
       cliques. choosem edges are chosen out of leftm available edges *****/
addedges1(patt,ct,choosem,leftm,bp,rr)
int patt[2][MAXN], ct, choosem, leftm,bp,rr;
{
  int i,j,k,l,counter;

/*  printf("choose %d out of %d\n",choosem,leftm); */
  subset(chosen, leftm,choosem);
  counter = 0;
  for (i=0;i<bp;i++) for (j=patt[0][i]+1;j<=patt[1][i];j++)
  for (k=bp;k<ct;k++) for (l=patt[0][k]+1;l<=patt[1][k];l++) 
     if (chosen[counter++]) addedge(j,l);
  if (rr>0) {
     for (i=0;i<bp-1;i++) for (j=patt[0][i]+1;j<=patt[0][i]+rr;j++)
     for (k=i+1;k<bp;k++) for (l=patt[0][k]+1;l<=patt[0][k]+rr;l++)
        if (chosen[counter++]) addedge(j,l);
     for (i=bp;i<ct-1;i++) for (j=patt[0][i]+1;j<=patt[0][i]+rr;j++)
     for (k=i+1;k<ct;k++) for (l=patt[0][k]+1;l<=patt[0][k]+rr;l++) 
        if (chosen[counter++]) addedge(j,l);
  }
}

/***** chooses edges going between the noncover vertex of each clique and
       the cover vertices in the other cliques.
       choosem edges are chosen out of leftm available edges *****/
addedges2(patt,ct,choosem,leftm,bp,rr)
int patt[2][MAXN], ct, choosem, leftm, bp, rr;
{
  int i,j,k,l,counter;

/*  printf("choose %d out of %d\n",choosem,leftm); */
  subset(chosen,leftm,choosem);
  counter = 0;
  for (i=0;i<ct;i++) for (j=0;j<ct;j++) 
    if ((i<bp && j>=bp) || (i>=bp && j<bp)) {
     for (k=patt[0][j]+1;k<=patt[1][j];k++)
        if (chosen[counter++]) addedge(patt[0][i],k);
   } else if (i!=j && rr>0)
     for (k=patt[0][j]+1;k<=patt[0][j]+rr;k++)
        if (chosen[counter++]) addedge(patt[0][i],k);
}

/***********************************************************************/

main(argc,argv)
int argc; char *argv[];
{
  struct partlist part;
  int n,m,vc,bp,rr,c,mprime,k;
  int remedges,edges,cliques;
  int edges1, edges2, totedges1, totedges2,i,q,j,r;
  long seed;
  int firstc, secondc, firstnc, secondnc;
  char file[MAXNAME];
  int cliqpatt[2][MAXN];

   n = atoi(argv[1]);
   m = atoi(argv[2]);
   c = atoi(argv[3]);
   rr = atoi(argv[4]);
   seed = atoi(argv[5]);
   strcpy(file, argv[6]);

   bp = c/2;
   initrans(seed);
   vc = n - c;
   mprime = n*(n-1)/2 - m;
   q = n / c;

  /* input error checking */
  if (n <= 0) {printf("ILLEGAL vertex count \n"); n = 0;}
  else if (c < 1 || c > n) {printf("ILLEGAL max clique value\n"); n=0;}
  else if (rr < 0 || rr > q-1)
            {printf("ILLEGAL r value\n"); n = 0;}
  else {               /* check m */
     r = n % (n-vc);
     k = r*(q+1)*q/2 + (n-vc-r)*q*(q-1)/2;
     if (mprime < k || mprime > vc*(vc-1)/2 + vc*(n-vc)) {
            m = n*(n-1)/2 - k;}
  }

  if (n>0) { 
     initgraph(n);                      /* empty graph */
     cliques = n-vc;                    /* graph will have n-vc cliques */
     simplepart(n,cliques,&part);
/*     printpart(part); */
     edges = (sumsquares(part)-n)/2;    /* edges in the cliques */
     remedges = mprime - edges;              /* other edges */

     fillin(part,cliqpatt,cliques);                 /* fill in cliqpatt */
     addcliqedges(cliqpatt,cliques);                /* add clique edges */

     firstc = cliqpatt[1][bp-1] + 1 - bp;
     secondc = n - cliqpatt[1][bp-1] - 1 - (cliques-bp);
     firstnc = bp;
     secondnc = cliques - bp;
     
/*     printf("firstc = %d, secondc = %d, bp = %d, r = %d\n",firstc,
             secondc,bp,rr); */
     totedges1 = firstc*secondc + bp*(bp-1)*rr*rr/2
                  + (cliques-bp)*(cliques-bp-1)*rr*rr/2; 
                                            /* available edges joining cover 
                                               vertices in separate cliques
                                               (type 1 edges) */
     totedges2 = firstc*secondnc + firstnc*secondc
                  + bp*(bp-1)*rr
                  + (cliques-bp)*(cliques-bp-1)*rr;
                                            /* available edges joining a cover 
                                               and a noncover vertex in 
                                               separate cliques 
                                               (type 2 edges) */
     
     edges2 = 2*(n-vc)*mprime/n - vc;            /* type 2 edges to be chosen 
                                               to make average degrees equal */

/*     printf("Goal for edges: edges1 = %d, edges2 = %d\n", remedges - edges2,
            edges2);  */
     /* adjust if necessary */
     if (remedges <= totedges2) {
        if (edges2 > remedges) edges2 = remedges;
     } else if (edges2 > totedges2) edges2 = totedges2;
     edges1 = remedges - edges2;
     if (edges1 > totedges1) {
        printf("Value of r too small: reducing edges by %d\n",
                            edges1 - totedges1);
        edges1 = totedges1;
     }
/*     printf("For vc edges choosing %d out of %d\n",edges1, totedges1);
     printf("For other edges choosing %d out of %d\n",edges2, totedges2); */

     /* add additional edges */
     chosen = (int *) malloc(n*(n-1)/2*sizeof(int));
     addedges1(cliqpatt,cliques,edges1,totedges1,bp,rr);
     addedges2(cliqpatt,cliques,edges2,totedges2,bp,rr);

     /* check existence of independent set of size c */
     for (i=0;i<cliques-1;i++) {
       for (j=i+1;j<cliques;j++) {
          if (isedge(cliqpatt[0][i],cliqpatt[0][j])) {printf("Error\n");}
       }
     }

     /* randomly permute graph */
     permuteg();

     /* complement graph - get graph with clique of size c */
     complement();

     /* output graph to file */
     printgraph(file,n,m,c,rr,seed);
  }
}




   

   
