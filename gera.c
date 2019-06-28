#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define uchar unsigned char

typedef int (*funcp) ();

typedef struct JmpPoint
{
	int srcPoint;
	int destLine;
	struct JmpPoint* next;
} JmpPoint;

static JmpPoint* AddJmp(JmpPoint* next, int srcPoint, int destLine)
{
	JmpPoint* jmp = (JmpPoint*)malloc(sizeof(JmpPoint));
	if (!jmp)
	{
		fprintf(stderr, "Memória insuficiente para terminar compilação, abortando.\n");
		exit(EXIT_FAILURE);
	}
	jmp->next = next;
	jmp->srcPoint = srcPoint;
	jmp->destLine = destLine;
	return jmp;
}

static uchar ComputeOffset(int dest, int src)
{
	int dist = dest - src;
	uchar ret = dist & 0x7F;
	ret -= 1;
	if (dist < 0)
	{
		ret |= 0x80;
	}
	return ret;
}

static void CompileJmpPoints(
	JmpPoint* jmp,		// ponteiro para o primeiro nó de uma lista enc. de JmpPoints.
	int* codePoints,  	// vetor de posições no código, onde cada índice corresponde à sua respectiva linha no código simples original menos 1.
	uchar* code)		// ponteiro para o buffer do código de máquina sendo gerado.
{
	JmpPoint* next;
	while (jmp)
	{
		next = jmp->next;
		code[jmp->srcPoint] = ComputeOffset(codePoints[jmp->destLine - 1], jmp->srcPoint);
		jmp = next;
	}
}

static void error (const char *msg, int line) 
{
  fprintf(stderr, "erro %s na linha %d\n", msg, line);
  exit(EXIT_FAILURE);
}

funcp gera (FILE * myfp){
	int line = 1;
	int c;
	int count = 0;
	int codePos[30];
	uchar codigo[1000];
	char* ret;
	JmpPoint* first = NULL;

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

	while ((c = fgetc(myfp)) != EOF) 
	{
		codePos[line - 1] = count;
		switch (c)
		{
			case 'r': 
			{ /*tratamento de retorno */
				char var0;
				int idx0;
				if (fscanf(myfp, "et %c%d", &var0, &idx0) != 2)		
					error("comando invalido", line);
				switch(var0)
				{
					case '$':/* retorno de constante ($) */
						codigo[count++] = 0xb8;
						for(int i = 0;i<4;i++)
						{
							codigo[count++] = (idx0>>(i*8)) & 0xff;  			
						}
						codigo[count++] = 0xc9;
						codigo[count++] = 0xc3;
						break;
					case 'v':/* retorno de variável (v) */
						codigo[count++] = 0x8b;
						codigo[count++] = 0x45;
						codigo[count++] = 0xff - 4*idx0 + 1;
						codigo[count++] = 0xc9;
						codigo[count++] = 0xc3;
						break;
				}
				break;	
			}
			case 'v': 
			{ /* tratamento de atribuição e op. aritmetica */
				int idx0, idx1;
				char c0, var1;
				if (fscanf(myfp, "%d %c", &idx0, &c0) != 2)
					error("comando invalido", line);	
				switch(c0)
				{ /* atribuição */
					case '<':
					{
						if (fscanf(myfp, " %c%d", &var1, &idx1) != 2)
							error("comando invalido", line);
						switch(var1)
						{
							case '$':/* atribuição de variável para constante */
								codigo[count++] = 0xc7;
								codigo[count++] = 0x45;
								codigo[count++] = 0xff - 4*idx0 + 1;

								for(int i = 0;i<4;i++)
								{
									codigo[count++] = (idx1>>(i*8)) & 0xff;  			
								}
								break;
							case 'v':/* atribuição de variável para variável */
								codigo[count++] = 0x44;
								codigo[count++] = 0x8b;
								codigo[count++] = 0x55;
								codigo[count++] = 0xff - idx1*4 + 1;
								codigo[count++] = 0x44;
								codigo[count++] = 0x89;
								codigo[count++] = 0x55;
								codigo[count++] = 0xff - idx0*4 + 1;
								break;
							case 'p':
								codigo[count++] = 0x89;
								switch (idx1)
								{
									case 1:
										codigo[count++] = 0x7d;
										break;
									case 2:
										codigo[count++] = 0x75;
										break;
									case 3:
										codigo[count++] = 0x55;
										break;
								}
								codigo[count++] = 0xff - idx0*4 + 1;
								break;			
						}
						break; 
					}		
			case '=':
			{ /*tratamento de operação aritmética */
				char var2, op;
				int idx2;

				fscanf(myfp, " %c%d %c %c%d", &var1, &idx1, &op, &var2, &idx2);
				switch(op)
				{
					case '+':
						if (var1 == '$' && var2 == '$')
						{
							codigo[count++] = 0xb8;

							for (int i = 0;i<4;i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}
							codigo[count++] = 0x05;

							for (int i = 0;i<4;i++)
							{
								codigo[count++] = (idx2>>(i*8)) & 0xff;
							}

							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;
						}
						else if (var1 == 'v' && var2 == '$')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x83;
							codigo[count++] = 0xc0;

							for (int i = 0; i<4; i++)
							{
								codigo[count+i] = (idx2>>(i*8)) & 0xff;
							}

							count++;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else if (var1 == '$' && var2 == 'v')
						{
							codigo[count++] = 0xb8;

							for(int i = 0; i<4; i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}

							codigo[count++] = 0x03;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;
						}
						else if (var1 == 'v' && var2 == 'v')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x03;	
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else
						{
							error("operação invalida",line);
						}
						break;
					case '-':
						if(var1 == '$' && var2 == '$')
						{
							codigo[count++] = 0xb8;
							for(int i = 0;i<4;i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}
							codigo[count++] = 0x83;
							codigo[count++] = 0xe8;

							for(int i = 0;i<4;i++)
							{
								codigo[count+i] = (idx2>>(i*8)) & 0xff;
							}
							count++;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else if(var1 == 'v'&& var2 == '$')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x83;
							codigo[count++] = 0xe8;
							for(int i = 0; i<4; i++)
							{
								codigo[count+i] = (idx2>>(i*8)) & 0xff;
							}
							count++;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;
						}
						else if(var1 == '$' && var2 == 'v')
						{
							codigo[count++] = 0xb8;
							for(int i = 0; i<4; i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}
							codigo[count++] = 0x2b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;							
						}
						else if(var1 == 'v' && var2 == 'v')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x2b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else
						{
							error("operação invalida",line);
						}
						break;
					case '*':
						if(var1 == '$' && var2 == '$')
						{
							codigo[count++] = 0xb8;
							for(int i = 0;i<4;i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}
							codigo[count++] = 0x6b;
							codigo[count++] = 0xc0;
							for(int i = 0;i<4;i++)
							{
								codigo[count+i] = (idx2>>(i*8)) & 0xff;
							}
							count++;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;			
						}
						else if(var1 == 'v' && var2 == '$')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x6b;
							codigo[count++] = 0xc0;
							for(int i = 0; i<4; i++)
							{
								codigo[count+i] = (idx2>>(i*8)) & 0xff;
							}
							count++;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else if(var1 == '$' && var2 == 'v')
						{
							codigo[count++] = 0xb8;

							for(int i = 0; i<4; i++)
							{
								codigo[count++] = (idx1>>(i*8)) & 0xff;
							}

							codigo[count++] = 0x0f;
							codigo[count++] = 0xaf;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}
						else if(var1 == 'v' && var2 == 'v')
						{
							codigo[count++] = 0x8b;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx1 + 1;
							codigo[count++] = 0x0f;
							codigo[count++] = 0xaf;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx2 + 1;
							codigo[count++] = 0x89;
							codigo[count++] = 0x45;
							codigo[count++] = 0xff - 4*idx0 + 1;	
						}	
						else 
						{
							error("Operação invalida",line);
						}
						break;
					} 
					break;			
					}
				}
				break;
			}
			case 'i': 
			{ /*tratamento de desvio condicional */
				char var0;
				int idx0, n;
				if (fscanf(myfp, "flez %c%d %d", &var0, &idx0, &n) != 3)
				{
					error("comando invalido", line);
				} 
				else 
				{
					codigo[count++] = 0x8b;
					codigo[count++] = 0x45;
					codigo[count++] = 0xff - 4*idx0 + 1;
					codigo[count++] = 0xbb;
					codigo[count++] = 0x00;
					codigo[count++] = 0x00;
					codigo[count++] = 0x00;
					codigo[count++] = 0x00;
					codigo[count++] = 0x39;
					codigo[count++] = 0xd8;
					codigo[count++] = 0x7e;
					first = AddJmp(first, count, n);
					count++;
				}
				break;
			}
			default:
				error("comando desconhecido", line);
		}
		line++;
		fscanf(myfp, " ");
	}
	CompileJmpPoints(first, codePos, codigo);

	ret = (char*)malloc(count);
	if (!ret)
	{
		fprintf(stderr, "Memória insuficiente para terminar compilação, abortando.\n");
		exit(EXIT_FAILURE);
	}
	memcpy(ret, codigo, count);
	return (funcp)ret;
}

