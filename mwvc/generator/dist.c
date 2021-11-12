#include <stdio.h>
#include <math.h>

static double pow2to31;
static double sqrt8overe;

float unifdist(length)
float length;
{
    int i;
    float unif1;
    float unif;

    i = pow2to31 - random();
    unif1 = i/pow2to31;
    unif = length*unif1;         /* unif is uniformly distributed */
    return(unif);
}   

initrans(seed)
long seed;
{
  pow2to31 = pow (2.0,31.0);
  sqrt8overe = sqrt ((8.0/exp(1.0))); 
  srandom(seed);
}
