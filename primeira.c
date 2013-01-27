/***************************************************************************
 * Manipulando Imagens TIFF (Tag Image File Format)
 * autoras: Lissa Pesci
 *          Miriam Yumi Peixoto
 * compile me: gcc -o primeira primeira.c teste_imagem.c -lm -ltiff
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "teste_imagem.h"

int IMAGE_WIDTH  = 512;
int IMAGE_HEIGHT = 512;

int main (int argc, char *argv[]) {
	struct Image * image;
	struct ComprImage * compr_image;

	if (argc == 2) {
		image = ReadTiffImage(argv[1]);
		compr_image = aplica_dct(image);
		image = quantizacao(compr_image, 3);
		
		SaveTiffImage("teste.tif", image);
	} else {
		printf("Usage: primeira filename.tiff\n");
	}
	
	return 0;
}
