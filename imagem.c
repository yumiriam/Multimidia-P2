#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>  /* biblioteca libtiff */
#include <math.h>
#include "imagem.h"

extern int IMAGE_WIDTH, IMAGE_HEIGHT;
extern int NFRAMES;

/****************************FUNCOES AUXILIARES****************************/

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

// Imprime matriz de ponto flutuante nxn
void printfm(float **m, int n) {
	int i, j;
	
	for (i = 0;i < n;i++) {
		for (j = 0;j < n;j++) {
			printf("%4d ", round(m[i][j]));
		}
		printf("\n");
	}
	printf("\n");
}
/*****************************Auxiliar DCT**********************************

	C(u), C(v) = sqrt(2) / 2 p/ u, v = 0
				 1 p/ u, v = 1, ..., (m - 1)
				 0 outra forma

***************************************************************************/
float c(int x) {
	if (x == 0) return (sqrt(2) / 2);
	if (x > 0 && x < N) return 1.0;

	return 0.0;
}
/*******************Transformada Discreta do Cosseno************************

	X[u, v] = (C(u)*C(v) / 4)*...
		sum( sum(x(i,j)*cos[(2*i + 1)*u*pi / ... 
			(2*m)]*cos[(2*j + 1)*v*pi / (2*m)]) ...
				from j = 0 to m) from i = 0 to m
	
	X : retorno, x : imagem

***************************************************************************/
float * dct2d(int * x) {
	int   u, v, i, j;
	float sum_i, sum_j;
	float * X;
	
	X = (float *) malloc(sizeof(float)*N*N);
	
	for (u = 0;u < N;u++) {
		for (v = 0;v < N;v++) {
			
			sum_i = 0;
			for (i = 0;i < N;i++) {
				sum_j = 0;
				for (j = 0;j < N;j++) {
					sum_j = sum_j + x[i*N + j]*cos((2*i + 1)*u*M_PI / (2*N))*cos((2*j + 1)*v*M_PI / (2*N));	
				}
				sum_i = sum_i + sum_j;
			}
			
			X[u*N + v] = ((c(u)*c(v)) / 4)*sum_i;
		}		
	}

	return X;
}
/****************Transformada Inversa da Discreta do Cosseno****************

	X[u, v] = ...
		sum( sum((C(i)*C(j) / 4)*x(i,j)*cos[(2*u + 1)*i*pi / ... 
			(2*m)]*cos[(2*v + 1)*j*pi / (2*m)]) ...
				from j = 0 to m) from i = 0 to m

	X : retorno, x : imagem

****************************************************************************/
int * idct2d(float * x) {
	int   u, v, i, j;
	float sum_i, sum_j;
	int   * X;
	
	X = (int *) malloc(sizeof(int)*N*N);
	for (u = 0;u < N;u++) {
		for (v = 0;v < N;v++) {
			
			sum_i = 0;
			for (i = 0;i < N;i++) {
				sum_j = 0;
				for (j = 0;j < N;j++) {
					sum_j = sum_j + ((c(i)*c(j)) / 4)*x[i*N + j]*cos((2*u + 1)*i*M_PI / (2*N))*cos((2*v + 1)*j*M_PI / (2*N));	
				}
				sum_i = sum_i + sum_j;
			}
			
			X[u*N + v] = round(sum_i);
		}		
	}

	return X;
}
/************************Aplicacao da DCT na Imagem************************/
struct ComprImage * aplica_dct(struct Image *image) {
	float *f_buffer_r, *f_buffer_g, *f_buffer_b;
	int   *i_buffer_r, *i_buffer_g, *i_buffer_b;
	int   i, j;
	struct ComprImage * result;

	i_buffer_r = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_g = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_b = (int *)   malloc(sizeof(int)*N*N);
	
	result = (struct ComprImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ComprImage));

	for (i = 0;i < IMAGE_WIDTH*IMAGE_HEIGHT;i+=64) {
		
		// separa blocos NxN (8x8)
		for (j = 0;j < N*N;j++) {
			i_buffer_r[j] = image[i+j].red;
			i_buffer_g[j] = image[i+j].green;
			i_buffer_b[j] = image[i+j].blue;
		}

		f_buffer_r = dct2d(i_buffer_r); // aplica a DCT nos blocos NxN (8x8)
		f_buffer_g = dct2d(i_buffer_g);
		f_buffer_b = dct2d(i_buffer_b);

		// retorna blocos NxN (8x8)
		for (j = 0;j < N*N;j++) {
			result[i+j].red   = f_buffer_r[j];
			result[i+j].green = f_buffer_g[j];
			result[i+j].blue  = f_buffer_b[j];
		}
		
	}
	// libera a memoria !importante!
	free(f_buffer_r);
	free(i_buffer_r);
	free(f_buffer_g);
	free(i_buffer_g);
	free(f_buffer_b);
	free(i_buffer_b);
	
	return result;
}

struct Image * aplica_idct(struct ComprImage *image) {
	float *f_buffer_r, *f_buffer_g, *f_buffer_b;
	int   *i_buffer_r, *i_buffer_g, *i_buffer_b;
	int   i, j;
	struct Image * result;

	f_buffer_r = (float *) malloc(sizeof(float)*N*N);
	f_buffer_g = (float *) malloc(sizeof(float)*N*N);
	f_buffer_b = (float *) malloc(sizeof(float)*N*N);
	
	result = (struct Image *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image));

	for (i = 0;i < IMAGE_WIDTH*IMAGE_HEIGHT;i+=64) {
		
		for (j = 0;j < N*N;j++) { // separa blocos NxN (8x8)
			f_buffer_r[j] = image[i+j].red;
			f_buffer_g[j] = image[i+j].green;
			f_buffer_b[j] = image[i+j].blue;
		}

		i_buffer_r = idct2d(f_buffer_r); // aplica a IDCT nos blocos NxN (8x8)
		i_buffer_g = idct2d(f_buffer_g);
		i_buffer_b = idct2d(f_buffer_b);

		for (j = 0;j < N*N;j++) { // retorna blocos NxN (8x8)
			result[i+j].red   = i_buffer_r[j];
			result[i+j].green = i_buffer_g[j];
			result[i+j].blue  = i_buffer_b[j];
		}
		
	}
	// libera a memoria !importante!
	free(f_buffer_r);
	free(i_buffer_r);
	free(f_buffer_g);
	free(i_buffer_g);
	free(f_buffer_b);
	free(i_buffer_b);
	
	return result;
}
/************************Efetua a quantizacao******************************
	Funcoes para a quantizacao da imagem e reversao deste processo
***************************************************************************/
// Efetua quantizacao no bloco NxN
int * quantiza(float *x, int fator) {
	int  i, j, q;
	int  *X;

	X = (int *) malloc(sizeof(int)*N*N);
	
	for (i = 0;i < N;i++) {
		for (j = 0;j < N;j++) {
			q = 1 + (1 + i + j)*fator;
			X[i*N + j] = round(x[i*N + j] / q);
		}
	}
	
	return X;
}
// Efetua a quantizacao na imagem IMAGE_WIDTHxIMAGE_HEIGHT
struct QuantImage * quantizacao(struct ComprImage *image, int fator) {
	float *f_buffer_r, *f_buffer_g, *f_buffer_b;
	int   *i_buffer_r, *i_buffer_g, *i_buffer_b;
	int   i, j;
	struct QuantImage * result;

	f_buffer_r = (float *)   malloc(sizeof(float)*N*N);
	f_buffer_g = (float *)   malloc(sizeof(float)*N*N);
	f_buffer_b = (float *)   malloc(sizeof(float)*N*N);
	
	result = (struct QuantImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct QuantImage));

	for (i = 0;i < IMAGE_WIDTH*IMAGE_HEIGHT;i+=64) {
	
		for (j = 0;j < N*N;j++) { // separa em blocos NxN (8x8)
			f_buffer_r[j] = image[i+j].red;
			f_buffer_g[j] = image[i+j].green;
			f_buffer_b[j] = image[i+j].blue;
		}

		i_buffer_r = quantiza(f_buffer_r, fator); // aplica a quantizacao nos blocos NxN (8x8)
		i_buffer_g = quantiza(f_buffer_g, fator);
		i_buffer_b = quantiza(f_buffer_b, fator);

		for (j = 0;j < N*N;j++) { // retorna blocos NxN (8x8)
			result[i+j].red   = i_buffer_r[j];
			result[i+j].green = i_buffer_g[j];
			result[i+j].blue  = i_buffer_b[j];
		}
		
	}
	// libera a memoria !importante!
	free(f_buffer_r);
	free(i_buffer_r);
	free(f_buffer_g);
	free(i_buffer_g);
	free(f_buffer_b);
	free(i_buffer_b);
	
	return result;
}
// Efetua dequantizacao no bloco NxN
float * dequantiza(int *x, int fator) {
	float *X;
	int   i, j, q;
	
	X = (float *)   malloc(sizeof(float)*N*N);
	
	for (i = 0;i < N;i++) {
		for (j = 0;j < N;j++) {
			q = 1 + (1 + i + j)*fator;
			X[i*N + j] = q * x[i*N + j];
		}
	}
	
	return X;
}
// Efetua a dequantizacao na imagem IMAGE_WIDTHxIMAGE_HEIGHT
struct ComprImage * dequantizacao(struct QuantImage *image, int fator) {
	float *f_buffer_r, *f_buffer_g, *f_buffer_b;
	int   *i_buffer_r, *i_buffer_g, *i_buffer_b;
	int   i, j;
	struct ComprImage * result;

	i_buffer_r = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_g = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_b = (int *)   malloc(sizeof(int)*N*N);
	
	result = (struct ComprImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ComprImage));

	for (i = 0;i < IMAGE_WIDTH*IMAGE_HEIGHT;i+=64) {
	
		for (j = 0;j < N*N;j++) { // separa em blocos NxN (8x8)
			i_buffer_r[j] = image[i+j].red;
			i_buffer_g[j] = image[i+j].green;
			i_buffer_b[j] = image[i+j].blue;
		}

		f_buffer_r = dequantiza(i_buffer_r, fator); // aplica a quantizacao nos blocos NxN (8x8)
		f_buffer_g = dequantiza(i_buffer_g, fator);
		f_buffer_b = dequantiza(i_buffer_b, fator);

		for (j = 0;j < N*N;j++) { // retorna blocos NxN (8x8)
			result[i+j].red   = f_buffer_r[j];
			result[i+j].green = f_buffer_g[j];
			result[i+j].blue  = f_buffer_b[j];
		}
		
	}
	// libera a memoria !importante!
	free(f_buffer_r);
	free(i_buffer_r);
	free(f_buffer_g);
	free(i_buffer_g);
	free(f_buffer_b);
	free(i_buffer_b);
	
	return result;
}

/***************************Compactacao RLE*********************************
	Compactacao entropica sem perda RLE (Run Length Encode)
***************************************************************************/
// Funcao auxiliar do rle
signed char * rle(int *x, int *tam){
	signed char blocoAux[N*N*2]; // maximo que o vetor pode ocupar
	signed char *bloco;
	int i, count, c, l, dir;
	
	l = 0; c = 0; *tam = 0; count = 0; dir = 0;
	for (i = 0; i < N*N; i++) {
		//printf("x[%d][%d] = %d tam = %d count = %d\n", l, c, x[l][c], *tam, count);
		//fflush(stdout);
		if (x[l*N + c] == 0){
				count++;
		} else {
			if (count > 0){
				blocoAux[*tam] = 0;		
				*tam = *tam + 1;
				blocoAux[*tam] = count-1;		
				*tam = *tam + 1;
			} 
			blocoAux[*tam] = x[l*N + c];
			*tam = *tam + 1;
			count = 0;
		}
		// atualiza os indices (zigue-zague)
		// dir = 0 subindo a seta
		if (dir == 0) {
			if (l-1 < 0){
				c++;
				dir = 1;
			} else {
				if (c+1 > 7){
					l++;
					dir = 1;
				} else {
					l--;
					c++;
				}
			}
		} else { // dir = 1 descendo a seta
			if (c - 1 < 0){
				if (l + 1 > 7){
					c++;
					dir = 0;
				} else {
					l++;
					dir = 0;
				}
			} else {
				if (l+1 > 7){
					c++;
					dir = 0;
				} else {
					l++;
					c--;
				}
			}
		}
	}
	if (count > 0) {
		blocoAux[*tam] = 0;		
		*tam = *tam + 1;
		blocoAux[*tam] = count-1;		
		*tam = *tam + 1;
		count = 0;
	}	
	
	bloco = (signed char *) malloc(sizeof(signed char) * (*tam));
	for (i = 0; i < *tam; i++){
		bloco[i] = blocoAux[i];
	}
		
	return bloco;
}

// Aplicacao do rle na imagem
struct CodifImage aplica_rle(struct QuantImage *image) {
	int         *i_buffer_r, *i_buffer_g, *i_buffer_b;
	int         tam, tam_total = 0;
	signed char *o_buffer_r, *o_buffer_g, *o_buffer_b;
	int         i, j, count_r, count_g, count_b;
	signed char *aux_r, *aux_g, *aux_b;
	struct CodifImage result;
	
	count_r = count_g = count_b = 0;

	i_buffer_r = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_g = (int *)   malloc(sizeof(int)*N*N);
	i_buffer_b = (int *)   malloc(sizeof(int)*N*N);
	
	aux_r = (signed char *)   malloc(sizeof(signed char)*IMAGE_WIDTH*IMAGE_HEIGHT*2);
	aux_g = (signed char *)   malloc(sizeof(signed char)*IMAGE_WIDTH*IMAGE_HEIGHT*2);
	aux_b = (signed char *)   malloc(sizeof(signed char)*IMAGE_WIDTH*IMAGE_HEIGHT*2);

	for (i = 0;i < IMAGE_WIDTH*IMAGE_HEIGHT;i+=64) {
	
		for (j = 0;j < N*N;j++) { // separa em blocos NxN (8x8)
			i_buffer_r[j] = image[i+j].red;
			i_buffer_g[j] = image[i+j].green;
			i_buffer_b[j] = image[i+j].blue;
		}

		o_buffer_r = rle(i_buffer_r, &tam); // aplica a quantizacao nos blocos NxN (8x8)
		for (j = 0;j < tam;j++) { // retorna blocos NxN (8x8)
			aux_r[count_r+j] = o_buffer_r[j];
			count_r++;
		}
			
		o_buffer_g = rle(i_buffer_g, &tam);
		for (j = 0;j < tam;j++) { // retorna blocos NxN (8x8)
			aux_g[count_g+j] = o_buffer_g[j];
			count_g++;
		}
		
		o_buffer_b = rle(i_buffer_b, &tam);
		for (j = 0;j < tam;j++) { // retorna blocos NxN (8x8)
			aux_b[count_b+j] = o_buffer_b[j];
			count_b++;
		}
		
	}
	// libera a memoria !importante!
	free(i_buffer_r);
	free(i_buffer_g);
	free(i_buffer_b);
	free(o_buffer_r);
	free(o_buffer_g);
	free(o_buffer_b);

	result.r_size = count_r;
	result.g_size = count_g;
	result.b_size = count_b;
	
	result.red   = (signed char *) malloc(sizeof(signed char)*count_r);
	result.green = (signed char *) malloc(sizeof(signed char)*count_g);
	result.blue  = (signed char *) malloc(sizeof(signed char)*count_b);
	
	for (i = 0;i < count_r;i++)
		result.red[i] = aux_r[i];
	for (i = 0;i < count_r;i++)
		result.green[i] = aux_g[i];
	for (i = 0;i < count_r;i++)
		result.blue[i] = aux_b[i];
		
	free(aux_r);
	free(aux_g);
	free(aux_b);
	
	return result;
}

// Funcao auxiliar do rle (descompactacao)
int * rle_d(signed char* bloco, int *tam) {
	int i, j, tamAux, l, c, dir;
	int aux[N*N];
	int *x;
	
	x = (int *) malloc(sizeof(int)*N*N);
	
	// tira o rle
	tamAux = 0; i = 0;
	while (tamAux < 64) {
		if (bloco[i] == 0){
			aux[tamAux] = 0;
			tamAux++;
			i++;
			for (j = 0; j < bloco[i]; j++){
				aux[tamAux] = 0;
				tamAux++;
			}
		} else {
			aux[tamAux] = bloco[i];
			tamAux++;
		}
		i++;
	}

	*tam = i;

	l = 0; c = 0; dir = 0;
	for (i = 0; i < N*N; i++) {
		x[l*N + c] = aux[i];
		// atualiza os indices (zigue-zague)
		// dir = 0 subindo a seta
		if (dir == 0) {
			if (l-1 < 0){
				c++;
				dir = 1;
			} else {
				if (c+1 > 7){
					l++;
					dir = 1;
				} else {
					l--;
					c++;
				}
			}
		} else { // dir = 1 descendo a seta
			if (c - 1 < 0){
				if (l + 1 > 7){
					c++;
					dir = 0;
				} else {
					l++;
					dir = 0;
				}
			} else {
				if (l+1 > 7){
					c++;
					dir = 0;
				} else {
					l++;
					c--;
				}
			}
		}
	}
	return x;
}

// Aplicacao do rle na imagem (descompactacao)
struct QuantImage * aplica_rle_d(struct CodifImage image) {
	int         *i_buffer;
	int         i, count, tam;
	struct QuantImage * result;
	
	i_buffer = (int *)   malloc(sizeof(int)*N*N);
	
	result = (struct QuantImage *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct QuantImage));

	count = 0;
	while (count < image.r_size) {
		i_buffer = rle_d(&(image.red[count]), &tam);
		
		for (i = count;i < N*N;i++) {
			result[i].red = i_buffer[i];
		}
		count += tam;
	}
	
	count = 0;
	while (count < image.g_size) {
		i_buffer = rle_d((&image.green[count]), &tam);
		
		for (i = count;i < N*N;i++) {
			result[i].green = i_buffer[i];
		}
		count += tam;
	}
	
	count = 0;
	while (count < image.b_size) {
		i_buffer = rle_d((&image.blue[count]), &tam);
		
		for (i = count;i < N*N;i++) {
			result[i].blue = i_buffer[i];
		}
		count += tam;
	}
	// libera a memoria !importante!
	free(i_buffer);

	return result;
}

/***************************Compressao JPEG*********************************
	Recebe a imagem e efetua todos os passos para uma compressao JPEG
***************************************************************************/

// Compressao JPEG
struct QuantImage * comprime(struct Image * image, unsigned int fator) {
	struct ComprImage * compr;
	struct QuantImage * quant;
	int x, y;

	compr = aplica_dct(image);
	quant = quantizacao(compr, fator);
	
	return quant;
}

// Decompressao JPEG
struct Image * descomprime(struct QuantImage * quant, unsigned int fator) {
	struct ComprImage * compr;
	struct Image * image;
	int x, y;

	compr = dequantizacao(quant, fator);
	image = aplica_idct(compr);
		
	return image;
}

/********************************IO TIFF************************************
	Funcoes de leitura e escrita
***************************************************************************/
// Leitura de imagem TIFF
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

	return t1;
}

// Escrita da imagem TIFF
void SaveTiffImage(char * name, struct Image * t) {  
	TIFF * tif;
	float mv;
	register int i, j, k;
	unsigned char * buffer;

	tif = TIFFOpen(name,"w");
	buffer = (unsigned char*) malloc(IMAGE_HEIGHT*IMAGE_WIDTH*sizeof(unsigned char)*3);
	if (tif) {
		if (buffer) {
			k = 0;
			for (i = 0;i < IMAGE_HEIGHT;i++) {
				for (j = 0;j < IMAGE_WIDTH;j++) {
					buffer[k++] = (unsigned char) t[i*IMAGE_WIDTH+j].red;
					buffer[k++] = (unsigned char) t[i*IMAGE_WIDTH+j].green;
					buffer[k++] = (unsigned char) t[i*IMAGE_WIDTH+j].blue;
				}
			}
			TIFFSetField(tif,TIFFTAG_IMAGEWIDTH,IMAGE_WIDTH);
			TIFFSetField(tif,TIFFTAG_IMAGELENGTH,IMAGE_HEIGHT);
			TIFFSetField(tif,TIFFTAG_BITSPERSAMPLE,8);
			TIFFSetField(tif,TIFFTAG_SAMPLESPERPIXEL,3);
			TIFFSetField(tif,TIFFTAG_ROWSPERSTRIP,IMAGE_HEIGHT);
			TIFFSetField(tif,TIFFTAG_COMPRESSION,COMPRESSION_NONE);
			TIFFSetField(tif,TIFFTAG_PHOTOMETRIC,PHOTOMETRIC_RGB);
			TIFFSetField(tif,TIFFTAG_FILLORDER,FILLORDER_MSB2LSB);
			TIFFSetField(tif,TIFFTAG_PLANARCONFIG,PLANARCONFIG_CONTIG);
			TIFFSetField(tif,TIFFTAG_XRESOLUTION,150.0);
			TIFFSetField(tif,TIFFTAG_YRESOLUTION,150.0);
			TIFFSetField(tif,TIFFTAG_RESOLUTIONUNIT,RESUNIT_INCH);
			TIFFWriteEncodedStrip(tif,0,buffer,IMAGE_HEIGHT*IMAGE_WIDTH*3);
		} else { 
			exit(1); 
		}
	} else { 
		exit(1); 
	}

	TIFFClose(tif);
}

/**********************************VIDEO************************************/

void comprime_video(char * input, char * output) {
	FILE * fin, * fout;
	struct Image * frame, * image;
	int  i, j;
	char * name;
  
	if ( (fin  = fopen(input,"rb")) == NULL ) {
		printf("Couldn't open input file.");
		exit(1);
	}
	if ( (fout = fopen(output,"wb")) == NULL ) {
		printf("Couldn't open output file.");
		exit(1);
	}
  
	frame = (struct Image *) malloc(NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image));
	if (!frame)	exit(1);
	image = (struct Image *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image));
	if (!image)	exit(1);
	
	while (1) {
		
		if (fread(frame, NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image), 1, fin) < 1)
			break;
		
		for (i = 0;i < NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT;i+=IMAGE_WIDTH*IMAGE_HEIGHT) {
			for (j = 0;j < IMAGE_WIDTH*IMAGE_HEIGHT;j++) {
				image[j].red   = frame[i+j].red;
				image[j].green = frame[i+j].green;
				image[j].blue  = frame[i+j].blue;
			}
			
			//sprintf(name, "teste_%2d.tif", i/(IMAGE_WIDTH*IMAGE_HEIGHT));
			//SaveTiffImage(name, image);
			
		}
	
	}
	
	free(frame);
	free(image);

	fclose(fin);
	fclose(fout);
}
