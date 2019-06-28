#include <stdio.h>
#include <stdlib.h>
typedef int (*funcp) (int,int);
funcp gera(FILE *f);

int main () {
  funcp func;
  FILE *myfp;
  if ((myfp = fopen ("simples", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }
  func = gera(myfp);
  printf("%d\n",func(6,3));
}
