#include<stdio.h>
#include<stdlib.h>
#include<string.h>

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

char * gera (FILE * myfp) {
  int line = 1;
  int  c;
  int count = 0;
  char* codigo = (char*)malloc(sizeof(char)*1000);

  while ((c = fgetc(myfp)) != EOF) {
    switch (c) {
      case 'r': { /* retorno */
        char var0;
        int idx0;
        if (fscanf(myfp, "et %c%d", &var0, &idx0) != 2)
          error("comando invalido", line);
	switch(var0){
		case '$':
			codigo[count++] = 0xb8;
			for(int i = 0;i<4;i++){
				codigo[count++] = (idx0>>(i*8)) & 0xff;  			
			}
			codigo[count++] = 0xc3;
	}
        break;
	
      }
      case 'v': { /* atribuiÃ§Ã£o e op. aritmetica */
        int idx0, idx1;
        char var0 = c, c0, var1;
        if (fscanf(myfp, "%d %c", &idx0, &c0) != 2)
          error("comando invalido", line);

        if (c0 == '<') { /* atribuiÃ§Ã£o */
          if (fscanf(myfp, " %c%d", &var1, &idx1) != 2)
            error("comando invalido", line);
          printf("%d %c%d < %c%d\n", line, var0, idx0, var1, idx1);
        }
        else { /* operaÃ§Ã£o aritmÃ©tica */
          char var2, op;
          int idx2;
          if (c0 != '=')
            error("comando invalido", line);
          if (fscanf(myfp, " %c%d %c %c%d", &var1, &idx1, &op, &var2, &idx2) != 5)
            error("comando invalido", line);
          printf("%d %c%d = %c%d %c %c%d\n", 
                 line, var0, idx0, var1, idx1, op, var2, idx2);
        }
        break;
      }
      case 'i': { /* desvio condicional */
        char var0;
        int idx0, n;
        if (fscanf(myfp, "flez %c%d %d", &var0, &idx0, &n) != 3)
            error("comando invalido", line);
          printf("%d iflez %c%d %d\n", line, var0, idx0, n);
        break;
      }
      default: error("comando desconhecido", line);
    }
    line ++;
    fscanf(myfp, " ");
  }
  return codigo;
}

