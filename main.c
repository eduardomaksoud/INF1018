#include <stdio.h>
typedef int (*funcp) ();
funcp gera(FILE *f);


int main (void) {
  int line = 1;
  int  c;
  funcp func;
  FILE *myfp;
  if ((myfp = fopen ("simples", "r")) == NULL) {
    perror ("nao conseguiu abrir arquivo!");
    exit(1);
  }
  func = gera(myfp);
  printf("%d \n",func());
}
