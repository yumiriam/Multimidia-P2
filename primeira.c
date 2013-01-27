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

int main (int argc, char *argv[]) {
	struct Image * image1, * image2;
	struct QuantImage * compr_image;
	unsigned int fator = 25;

	if (argc == 2) {
		image1 = ReadTiffImage(argv[1]);
		compr_image = comprime(image1, fator);
		//compr_image = aplica_dct(image);
		//image = quantizacao(compr_image, 3);
		printf("\n\n\n");
		image2 = descomprime(compr_image, fator);
		SaveTiffImage("anothertest.tif", image2);
	} else {
		printf("Usage: primeira filename.tiff\n");
	}
	
	return 0;
}
