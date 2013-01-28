/***************************************************************************
 * Manipulando Imagens TIFF (Tag Image File Format)
 * autoras: Lissa Pesci
 *          Miriam Yumi Peixoto
 * compile: gcc -o ex_video ex_video.c imagem.c -lm -ltiff
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "imagem.h"

int IMAGE_WIDTH, IMAGE_HEIGHT, NFRAMES;

int main (int argc, char *argv[]) {
	struct Image * image1, * image2;
	struct QuantImage * compr_image;
	unsigned int fator = 5;

	if (argc == 6) {
		IMAGE_WIDTH  = (int) atoi(argv[3]);
		IMAGE_HEIGHT = (int) atoi(argv[4]);
		NFRAMES      = (int) atoi(argv[5]);
		
		comprime_video(argv[1], argv[2]);
	} else {
		printf("Usage: ./ex_video input.tiff output.tiff  WIDTH HEIGHT NFRAMES\n");
	}
	
	return 0;
}
