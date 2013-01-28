/***************************************************************************
 * Manipulando Imagens TIFF (Tag Image File Format)
 * autoras: Lissa Pesci
 *          Miriam Yumi Peixoto
 * compile: gcc -o ex_imagem ex_imagem.c imagem.c -lm -ltiff
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "imagem.h"

int IMAGE_WIDTH  = 512;
int IMAGE_HEIGHT = 512;
int NFRAMES;

int main (int argc, char *argv[]) {
	struct Image * image1, * image2;
	struct ComprImage compr_image;
	unsigned int fator = 5;

	if (argc == 2) {
		image1 = ReadTiffImage(argv[1]);
		compr_image = comprime(image1, fator);
		//compr_image = aplica_dct(image);
		//image = quantizacao(compr_image, 3);
		
		image2 = descomprime(compr_image, fator);
		SaveTiffImage("imagens/anothertest.tif", image2);
	} else {
		printf("Usage: ex_imagem filename.tiff\n");
	}
	
	return 0;
}
