/***************************************************************************
 * Manipulando Imagens TIFF (Tag Image File Format)
 * autoras: Lissa Pesci
 *          Miriam Yumi Peixoto
 * compile me: gcc -o primeira primeira.c teste_imagem.c -lm -ltiff
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "imagem.h"

int IMAGE_WIDTH  = 512;
int IMAGE_HEIGHT = 512;

int main () {
	int **x;
	signed char *c;
	int i, j, tam;
	
	x = aloca_i(8,8);
	
	x[0][0] = 62; x[0][1] = -3; x[0][2] = 2;  x[0][3] = -1;x[0][4] = 2;  x[0][5] = 0;x[0][6] = 0;  x[0][7] = 1;
	x[1][0] = 4;  x[1][1] = -4; x[1][2] = 2;  x[1][3] = 0; x[1][4] = 0;  x[1][5] = 0;x[1][6] = 0;  x[1][7] = 0;
	x[2][0] = -1; x[2][1] = -2; x[2][2] = 0;  x[2][3] = 0; x[2][4] = -1; x[2][5] = 0;x[2][6] = -1; x[2][7] = 0;
	x[3][0] = 0;  x[3][1] = 0;  x[3][2] = 1;  x[3][3] = -1;x[3][4] = 0;  x[3][5] = 0;x[3][6] = 0;  x[3][7] = 0;
	x[4][0] = 0;  x[4][1] = 0;  x[4][2] = 0;  x[4][3] = 0; x[4][4] = 0;  x[4][5] = 0;x[4][6] = 0;  x[4][7] = 0;
	x[5][0] = 0;  x[5][1] = 0;  x[5][2] = -1; x[5][3] = 0; x[5][4] = 0;  x[5][5] = 0;x[5][6] = 0;  x[5][7] = 0;
	x[6][0] = 0;  x[6][1] = 0;  x[6][2] = 0;  x[6][3] = 0; x[6][4] = 0;  x[6][5] = 0;x[6][6] = 0;  x[6][7] = 0;
	x[7][0] = 0;  x[7][1] = 0;  x[7][2] = 0;  x[7][3] = 0; x[7][4] = 0;  x[7][5] = 0;x[7][6] = 0;  x[7][7] = 0;
		
	c = rle(x, &tam);
	printf("tam = %d\n", tam);
	for (i = 0; i < tam; i++){
		printf("%hhd ", c[i]);
	}
	printf("\n");
	x = rle_d(c, &tam);
	printf("descompressao:\n");
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			printf("%d\t", x[i][j]);
		}
		printf("\n");
	}
	
	printf("tam = %d\n", tam);
	
	return 0;
}

/*
matriz inicial:
62	-3	2	-1	2	0	0	1
4	-4	2	0	0	0	0	0
-1	-2	0	0	-1	0	-1	0
0	0	1	-1	0	0	0	0
0	0	0	0	0	0	0	0
0	0	-1	0	0	0	0	0
0	0	0	0	0	0	0	0
0	0	0	0	0	0	0	0

zigue-zague nao comprimido:
62 -3 4 -1 -4 2 -1 2 -2 0 0 0 0 0 2 0 0 0 1 0 0 0 0 0 -1 -1 0 0 1 0 0 0 0 -1 0 0 ...

zigue-zague comprmido:
62 -3 4 -1 -4 2 -1 2 -2 0 4 2 0 2 1 0 4 -1 -1 0 1 1 0 3 -1 0 6 -1 0 21

indices do zigue-zague (pra ver se tinha feito certo):
x[0][0] = 62 tam = 0 count = 0
x[0][1] = -3 tam = 1 count = 0
x[1][0] = 4 tam = 2 count = 0
x[2][0] = -1 tam = 3 count = 0
x[1][1] = -4 tam = 4 count = 0
x[0][2] = 2 tam = 5 count = 0
x[0][3] = -1 tam = 6 count = 0
x[1][2] = 2 tam = 7 count = 0
x[2][1] = -2 tam = 8 count = 0
x[3][0] = 0 tam = 9 count = 0
x[4][0] = 0 tam = 9 count = 1
x[3][1] = 0 tam = 9 count = 2
x[2][2] = 0 tam = 9 count = 3
x[1][3] = 0 tam = 9 count = 4
x[0][4] = 2 tam = 9 count = 5
x[0][5] = 0 tam = 12 count = 0
x[1][4] = 0 tam = 12 count = 1
x[2][3] = 0 tam = 12 count = 2
x[3][2] = 1 tam = 12 count = 3
x[4][1] = 0 tam = 15 count = 0
x[5][0] = 0 tam = 15 count = 1
x[6][0] = 0 tam = 15 count = 2
x[5][1] = 0 tam = 15 count = 3
x[4][2] = 0 tam = 15 count = 4
x[3][3] = -1 tam = 15 count = 5
x[2][4] = -1 tam = 18 count = 0
x[1][5] = 0 tam = 19 count = 0
x[0][6] = 0 tam = 19 count = 1
x[0][7] = 1 tam = 19 count = 2
x[1][6] = 0 tam = 22 count = 0
x[2][5] = 0 tam = 22 count = 1
x[3][4] = 0 tam = 22 count = 2
x[4][3] = 0 tam = 22 count = 3
x[5][2] = -1 tam = 22 count = 4
x[6][1] = 0 tam = 25 count = 0
x[7][0] = 0 tam = 25 count = 1
x[7][1] = 0 tam = 25 count = 2
x[6][2] = 0 tam = 25 count = 3
x[5][3] = 0 tam = 25 count = 4
x[4][4] = 0 tam = 25 count = 5
x[3][5] = 0 tam = 25 count = 6
x[2][6] = -1 tam = 25 count = 7
x[1][7] = 0 tam = 28 count = 0
x[2][7] = 0 tam = 28 count = 1
x[3][6] = 0 tam = 28 count = 2
x[4][5] = 0 tam = 28 count = 3
x[5][4] = 0 tam = 28 count = 4
x[6][3] = 0 tam = 28 count = 5
x[7][2] = 0 tam = 28 count = 6
x[7][3] = 0 tam = 28 count = 7
x[6][4] = 0 tam = 28 count = 8
x[5][5] = 0 tam = 28 count = 9
x[4][6] = 0 tam = 28 count = 10
x[3][7] = 0 tam = 28 count = 11
x[4][7] = 0 tam = 28 count = 12
x[5][6] = 0 tam = 28 count = 13
x[6][5] = 0 tam = 28 count = 14
x[7][4] = 0 tam = 28 count = 15
x[7][5] = 0 tam = 28 count = 16
x[6][6] = 0 tam = 28 count = 17
x[5][7] = 0 tam = 28 count = 18
x[6][7] = 0 tam = 28 count = 19
x[7][6] = 0 tam = 28 count = 20
x[7][7] = 0 tam = 28 count = 21
*/
