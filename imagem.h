#ifndef IMGLIB_H
#define IMGLIB_H
#define N 8          // bloco de trabalho da DCT
#define round(x) (int)((x) + 0.5)

// Estruturas da Imagem
struct Image {
	unsigned int red, green, blue;
};

struct QuantImage {
	int red, green, blue;
};

struct ComprImage {
	float red, green, blue;
};

struct CodifImage {
	unsigned int r_size, g_size, b_size;
	signed char  *red, *green, *blue;
};

// Funcao auxiliar para imprimir matriz
void printim(int **, int );
void printfm(float **, int );

/*********************************DCT**********************************/

// Funcao auxiliar da DCT
float c(int );
// Transformada Discreta do Cosseno
float * dct2d(int *);
// Transformada Inversa da Discreta do Cosseno
int   * idct2d(float *);

// Aplicacao da DCT na imagem
struct ComprImage * aplica_dct(struct Image *);
// Aplicacao da IDCT na imagem
struct Image * aplica_idct(struct ComprImage *);

/*****************************QUANTIZACAO*****************************/

// Efetua quantizacao
int   * quantiza(float *, int );
struct QuantImage * quantizacao(struct ComprImage *, int );
// Efetua dequantizacao
float * dequantiza(int *, int );
struct ComprImage * dequantizacao(struct QuantImage *, int );

/*****************************COMPRESSAO*****************************/

// Compressao JPEG
struct QuantImage * comprime(struct Image *, unsigned int );
// Decompressao JPEG
struct Image * descomprime(struct QuantImage *, unsigned int );

/*********************************RLE*********************************/

// Funcao auxiliar do rle (compressao)
signed char * rle(int* , int *);
// Aplicacao do rle na imagem (compressao)
struct CodifImage aplica_rle(struct QuantImage *);
// Funcao auxiliar do rle (descompressao)
int * rle_d(signed char * , int *);
// Aplicacao do rle na imagem (descompressao)
struct QuantImage * aplica_rle_d(struct CodifImage );

/*******************************IO TIFF******************************/

// Leitura de imagem TIFF 
struct Image * ReadTiffImage(char * );
// Escrita de imagem TIFF
void SaveTiffImage(char * , struct Image * );

/*******************************VIDEO********************************/

void comprime_video(char * , char * );
#endif
