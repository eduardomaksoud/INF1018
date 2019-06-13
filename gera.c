#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define uchar unsigned char

static void error (const char *msg, int line) {
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}


void printar_codigo(uchar* codigo, int n) {
	int i;
	for (i = 0; i < n; i++)
		printf("[%p] %02d: %02x\n", &(codigo[i]), i, codigo[i]);
	printf("\n");
}


char* gera (FILE * myfp){
	int line = 1;
	int  c;
	int count = 0;
	char* codigo = (char*)malloc(sizeof(char)*1000);

/* CÓDIGO DE MÁQUINA PARA ABRIR ESPAÇO NA PILHA */  
 	codigo[count++] = 0x55;
	codigo[count++] = 0x48;   
	codigo[count++] = 0x89;
	codigo[count++] = 0xe5;
	codigo[count++] = 0x48;   
	codigo[count++] = 0x83;
	codigo[count++] = 0xec;
	codigo[count++] = 0x20;                          	
/*	
	push %rbp
	movq %rsp, %rbp
	subq $32, %rsp
*/
/* CÓDIGO DE MÁQUINA PARA ABRIR ESPAÇO NA PILHA */ 

while ((c = fgetc(myfp)) != EOF) {
	switch (c){
		case 'r': { /*tratamento de retorno */
        		char var0;
        		int idx0;
        		if (fscanf(myfp, "et %c%d", &var0, &idx0) != 2)		
          			error("comando invalido", line);
			switch(var0){
				case '$':/* retorno de constante ($) */
					codigo[count++] = 0xb8;
					for(int i = 0;i<4;i++){
						codigo[count++] = (idx0>>(i*8)) & 0xff;  			
					}
					codigo[count++] = 0xc9;
					codigo[count++] = 0xc3;
					break;
				case 'v':/* retorno de variável (v) */
					codigo[count++] = 0x8b;
					codigo[count++] = 0x45;
					codigo[count++] = 0xff - 4*idx0;
					codigo[count++] = 0xc9;
					codigo[count++] = 0xc3;
					break;
			}
			break;	
		}
		case 'v': { /* tratamento de atribuição e op. aritmetica */
			int idx0, idx1;
        		char var0 = c, c0, var1;
        		if (fscanf(myfp, "%d %c", &idx0, &c0) != 2)
				error("comando invalido", line);		
        		switch(c0){ /* atribuição */
				case '<':{
          				if (fscanf(myfp, " %c%d", &var1, &idx1) != 2)
            					error("comando invalido", line);
	  				switch(var1){
	    					case '$':/* atribuição de variável para constante */
							codigo[count++] = 0xc7;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0;
							for(int i = 0;i<4;i++){
								codigo[count++] = (idx1>>(i*8)) & 0xff;  			
							}
							break;
	    					case 'v':/* atribuição de variável para variável */
							codigo[count++] = 0x44;
							codigo[count++] = 0x8b;
							codigo[count++] = 0x55;
							codigo[count++] = 0xff - idx1*4;
							codigo[count++] = 0x44;
							codigo[count++] = 0x89;
							codigo[count++] = 0x55;
							codigo[count++] = 0xff - idx0*4;
							break;					
					}
					break; 
				}		
        			case '=':{ /*tratamento de operação aritmética */
          				char var2, op;
          				int idx2;
          				if (c0 != '=')
            					error("comando invalido", line);
          				if (fscanf(myfp, " %c%d %c %c%d", &var1, &idx1, &op, &var2, &idx2) != 5)						
            					error("comando invalido", line);    
					break;				
        			}
      			}
			break;
		}
		case 'i': { /*tratamento de desvio condicional */
        		char var0;
        		int idx0, n;
        		if (fscanf(myfp, "flez %c%d %d", &var0, &idx0, &n) != 3){
				printf("%d",count);
        	    		error("comando invalido", line);
			}
       			break;
		}
      	default:
		error("comando desconhecido", line);
	}
    line ++;
    fscanf(myfp, " ");
}
  //printar_codigo(codigo,count);
  return codigo;
}

