#include <stdio.h>
#include <stdlib.h>
#include "tiffio.h"  /* biblioteca libtiff */
#include <math.h>
#include "imagem.h"

extern int IMAGE_WIDTH, IMAGE_HEIGHT;

// Funcao auxiliar para criar bloco NxN
int ** aloca_i(int n, int m) {
	int **a;
	int i;

	a = (int **) malloc(sizeof(int *) * n);
	for (i = 0;i < n;i++)
		a[i] = (int *) malloc(sizeof(int) * m);
	
	return a;
}

float ** aloca_f(int n, int m) {
	float **a;
	int   i;

	a = (float **) malloc(sizeof(float *) * n);
	for (i = 0;i < n;i++)
		a[i] = (float *) malloc(sizeof(float) * m);
		
	return a;
}

// Imprime matriz inteira nxn
void printim(int **m, int n) {
	int i, j;
	
	for (i = 0;i < n;i++) {
		for (j = 0;j < n;j++) {
			printf("%4d ", m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// Imprime matriz inteira nxn
void printfm(float **m, int n) {
	int i, j;
	
	for (i = 0;i < n;i++) {
		for (j = 0;j < n;j++) {
			printf("%4d ", (int) m[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}
/************************Auxiliar DCT**************************************

	C(u), C(v) = sqrt(2) / 2 p/ u, v = 0
				 1 p/ u, v = 1, ..., (m - 1)
				 0 outra forma

***************************************************************************/
float c(int x) {
	if (x == 0) return (sqrt(2) / 2);
	if (x > 0 && x < N) return 1.0;

	return 0.0;
}
/************************Transformada Discreta do Cosseno*******************

	X[u, v] = (C(u)*C(v) / 4)*...
		sum( sum(x(i,j)*cos[(2*i + 1)*u*pi / ... 
			(2*m)]*cos[(2*j + 1)*v*pi / (2*m)]) ...
				from j = 0 to m) from i = 0 to m
	
	X : retorno, x : imagem

***************************************************************************/
int dct2d(int **x, float **X) {
	int   u, v, i, j;
	float sum_i, sum_j;
	
	for (u = 0;u < N;u++) {
		for (v = 0;v < N;v++) {
			
			sum_i = 0;
			for (i = 0;i < N;i++) {
				sum_j = 0;
				for (j = 0;j < N;j++) {
					sum_j = sum_j + x[i][j]*cos((2*i + 1)*u*M_PI / (2*N))*cos((2*j + 1)*v*M_PI / (2*N));	
				}
				sum_i = sum_i + sum_j;
			}
			
			X[u][v] = ((c(u)*c(v)) / 4)*sum_i;
		}		
	}

	return 0;
}
/************************Transformada Inversa da Discreta do Cosseno********

	X[u, v] = ...
		sum( sum((C(i)*C(j) / 4)*x(i,j)*cos[(2*u + 1)*i*pi / ... 
			(2*m)]*cos[(2*v + 1)*j*pi / (2*m)]) ...
				from j = 0 to m) from i = 0 to m

	X : retorno, x : imagem

***************************************************************************/
int idct2d(float **x, int **X) {
	int   u, v, i, j;
	float sum_i, sum_j;
	
	for (u = 0;u < N;u++) {
		for (v = 0;v < N;v++) {
			
			sum_i = 0;
			for (i = 0;i < N;i++) {
				sum_j = 0;
				for (j = 0;j < N;j++) {
					sum_j = sum_j + ((c(i)*c(j)) / 4)*x[i][j]*cos((2*u + 1)*i*M_PI / (2*N))*cos((2*v + 1)*j*M_PI / (2*N));	
				}
				sum_i = sum_i + sum_j;
			}
			
			X[u][v] = round(sum_i);
		}		
	}

	return 0;
}
/************************Aplicacao da DCT na Imagem************************/
struct ComprImage * aplica_dct(struct Image *image) {
	float **or, **og, **ob;
	int   **ir, **ig, **ib;
	int   i, j, x, y;
	struct ComprImage * result;

	or = aloca_f(N, N);
	og = aloca_f(N, N);
	ob = aloca_f(N, N);
	
	ir = aloca_i(N, N);
	ig = aloca_i(N, N);
	ib = aloca_i(N, N);
	
	result = (struct ComprImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ComprImage));

	for (x = 0;x < IMAGE_WIDTH;x+=8) {
		for (y = 0;y < IMAGE_HEIGHT;y+=8) {
			for (i = 0;i < N;i++) {
				for (j = 0;j < N;j++) {
					ir[i][j] = image[(x+i)*N + (y+j)].red;
					ig[i][j] = image[(x+i)*N + (y+j)].green;
					ib[i][j] = image[(x+i)*N + (y+j)].blue;
				}
			}

			dct2d(ir, or);
			dct2d(ig, og);
			dct2d(ib, ob);
	
			for (i = 0;i < N;i++) {
				for (j = 0;j < N;j++) {
					result[(x+i)*N + (y+j)].red = or[i][j];
					result[(x+i)*N + (y+j)].green = og[i][j];
					result[(x+i)*N + (y+j)].blue = ob[i][j];
				}
			}
		}
	}
	
	return result;
}
/************************Efetua a quantizacao******************************
	Recebe a imagem depois de aplicada a DCT e o fator de qualidade
***************************************************************************/
int ** quantiza(float **x, int fator) {
	int  **X;
	int  i, j, q;
	
	X = aloca_i(N, N);
	
	for (i = 0;i < N;i++) {
		for (j = 0;j < N;j++) {
			q = 1 + (1 + i + j)*fator;
			X[i][j] = round(x[i][j] / q);
		}
	}
	
	return X;
	
}

struct QuantImage * quantizacao(struct ComprImage *image, int fator) {
	float **ir, **ig,  **ib;
	int   **or, **og, **ob;
	int   i, j, x, y;
	struct Image * result;


	ir = aloca_f(N, N);
	ig = aloca_f(N, N);
	ib = aloca_f(N, N);
	
	result = (struct QuantImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct QuantImage));

	for (x = 0;x < IMAGE_WIDTH;x+=8) {
		for (y = 0;y < IMAGE_HEIGHT;y+=8) {
			for (i = 0;i < N;i++) {
				for (j = 0;j < N;j++) {
					ir[i][j] = image[(x+i)*N + (y+j)].red;
					ig[i][j] = image[(x+i)*N + (y+j)].green;
					ib[i][j] = image[(x+i)*N + (y+j)].blue;
				}
			}

			or = quantiza(ir, fator);
			og = quantiza(ig, fator);
			ob = quantiza(ib, fator);
	
			for (i = 0;i < N;i++) {
				for (j = 0;j < N;j++) {
					result[(x+i)*N + (y+j)].red = or[i][j];
					result[(x+i)*N + (y+j)].green = og[i][j];
					result[(x+i)*N + (y+j)].blue = ob[i][j];
				}
			}
		}
	}
}

// Funcao de codificacao entropica retirando os 0s
struct CodifImage * codifica(struct Image *image) {
	struct CodifImage r1, r2;
	int zeros_r, zeros_b, zeros_g;
	int count_r, count_g, count_b;
	int x, y;
	
	r1.red = (int *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(int));
	r1.green = (int *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(int));
	r1.blue = (int *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(int));
	
	zeros_r = zeros_b = zeros_g = 0;
	count_r = count_b = count_g = 0;
	
	for (x = 0;x < IMAGE_WIDTH;x++) {
		for (y = 0;y < IMAGE_HEIGHT;y++) {
			if (image[x*IMAGE_WIDTH + y].red == 0) {
				if (zeros_r < 1)
					r1.red[count_r++] = image[x*IMAGE_WIDTH + y].red;
				zeros_r++;
			} else {
				if (zeros_r > 0) {
					r1.red[count_r++] = zeros_r - 1;
					zeros_r = 0;
				}
				r1.red[count_r++] = image[x*IMAGE_WIDTH + y].red;
			}
			
			if (image[x*IMAGE_WIDTH + y].green == 0) {
				if (zeros_g < 1)
					r1.green[count_g++] = image[x*IMAGE_WIDTH + y].green;
				zeros_g++;
			} else {
				if (zeros_g > 0) {
					r1.green[count_g++] = zeros_g - 1;
					zeros_g = 0;
				}
				r1.green[count_g++] = image[x*IMAGE_WIDTH + y].green;
			}
			
			if (image[x*IMAGE_WIDTH + y].blue == 0) {
				if (zeros_b < 1)
					r1.blue[count_b++] = image[x*IMAGE_WIDTH + y].blue;
				zeros_b++;
			} else {
				if (zeros_b > 0) {
					r1.blue[count_b++] = zeros_b - 1;
					zeros_b = 0;
				}
				r1.blue[count_b++] = image[x*IMAGE_WIDTH + y].blue;
			}
		}
	}
	
	r2.red =  (int *) malloc(count_r*sizeof(int));
	r2.green =  (int *) malloc(count_g*sizeof(int));
	r2.blue =  (int *) malloc(count_b*sizeof(int));
	
	// memcpy
}

// Funcao de decodificacao entropica devolvendo os 0s
struct Image * decodifica(struct Image *image) {

}

/************************Leitura de imagem TIFF****************************/
struct Image * ReadTiffImage(char * name) {
	TIFF * tif;
	uint32 * raster;
	struct Image * t1, * t2;
	register int i, j, k;

	tif = TIFFOpen(name,"r");

	if (tif) {
		TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&IMAGE_WIDTH);
		TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&IMAGE_HEIGHT);
		t2 = (struct Image *) malloc(IMAGE_HEIGHT*IMAGE_WIDTH*sizeof(struct Image));
		/*
		t2 = (struct Image **) malloc(IMAGE_WIDTH*sizeof(struct Image *));
		for (i = 0;i < IMAGE_WIDTH;i++)
			t2[i] = (struct Image *) malloc(IMAGE_HEIGHT*sizeof(struct Image));
		*/
		if (!t2) exit(1);
		raster = (uint32 *) _TIFFmalloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(uint32));
		if (raster != NULL) {
			if (TIFFReadRGBAImage(tif,IMAGE_WIDTH,IMAGE_HEIGHT,raster,0)) {
				k = 0;
				for (i = IMAGE_HEIGHT - 1; i >= 0; i--) {
					for (j = 0; j < IMAGE_WIDTH; j++) {
						t2[i*IMAGE_WIDTH+j].red = (unsigned int) TIFFGetR(raster[k]);
						t2[i*IMAGE_WIDTH+j].green = (unsigned int) TIFFGetG(raster[k]);
						t2[i*IMAGE_WIDTH+j].blue = (unsigned int) TIFFGetB(raster[k]);	  
						k++;
					}
				}
			} else { 
				exit(1); 
			}
			_TIFFfree(raster);
		} else { 
			exit(1);
		}
		t1 = t2;
	} else { 
		exit(1); 
	}

	TIFFClose(tif);

	return (struct Image **) t1;
}

/************************Salva a imagem TIFF*******************************/
void SaveTiffImage(char * name, float * t) {  
/* 
 * Salva uma matriz com entradas em ponto-flutuante em uma imagem no formato tiff em tons de cinza.
 * Exercício: estenda a função para salvar as três componentes RGB que devem ser armazenadas em uma variável uint32.
 */
  
	TIFF * tif;
	float mv;
	register int i, j, k;
	unsigned char * buffer;

	for (i = 0;i < IMAGE_HEIGHT;i++) {
		for (j = 0;j < IMAGE_WIDTH;j++) {
			if (t[i*IMAGE_WIDTH+j]<0.0) t[i*IMAGE_WIDTH+j] = 0.0;
		}
	}
	mv = 0.0;
	for (i = 0;i < IMAGE_HEIGHT;i++) {
		for (j = 0;j < IMAGE_WIDTH;j++) {
			if (t[i*IMAGE_WIDTH+j]>mv) mv = t[i*IMAGE_WIDTH+j];
		}
	}
	for (i = 0;i < IMAGE_HEIGHT;i++) {
		for (j = 0;j < IMAGE_WIDTH;j++) {
			if (mv != 0.0) t[i*IMAGE_WIDTH+j] = 255.0*(t[i*IMAGE_WIDTH+j]/mv);
			else t[i*IMAGE_WIDTH+j] = 0.0;
		}
	}
	tif = TIFFOpen(name,"w");
	buffer = (unsigned char*) malloc(IMAGE_HEIGHT*IMAGE_WIDTH*sizeof(unsigned char));
	if (tif) {
		if (buffer) {
			k = 0;
			for (i = 0;i < IMAGE_HEIGHT;i++) {
				for (j = 0;j < IMAGE_WIDTH;j++) {
					buffer[k]=(unsigned char)t[i*IMAGE_WIDTH+j];
					k++;
				}
			}
			TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,IMAGE_WIDTH);
			TIFFSetField(tif,TIFFTAG_IMAGELENGTH,IMAGE_HEIGHT);
			TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,8);
			TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,1);
			TIFFSetField(tif,TIFFTAG_ROWSPERSTRIP,IMAGE_HEIGHT);
			TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
			TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_MINISBLACK);
			TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
			TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
			TIFFSetField(tif,TIFFTAG_XRESOLUTION,150.0);
			TIFFSetField(tif,TIFFTAG_YRESOLUTION,150.0);
			TIFFSetField(tif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_INCH);
			TIFFWriteEncodedStrip(tif,0,buffer,IMAGE_HEIGHT*IMAGE_WIDTH);
		} else { 
			exit(1); 
		}
	} else { 
		exit(1); 
	}

	TIFFClose(tif);
}
