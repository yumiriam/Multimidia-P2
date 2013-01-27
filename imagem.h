#define MAX 512
#define N 8          // bloco de trabalho da DCT
#define round(x) (int)((x) + 0.5)

#define COM 0
#define DEC 1

// Estrutura da Imagem
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
	int *red, *green, *blue;
};

// Funcao auxiliar para criar bloco NxN
int   ** aloca_i(int n, int m);
float ** aloca_f(int n, int m);
// Funcao auxiliar para imprimir matriz
void printim(int **m, int n);
void printfm(float **m, int n);

/*********************************DCT**********************************/

// Funcao auxiliar da DCT
float c(int x);
// Transformada Discreta do Cosseno
int dct2d(int **x, float **X);
// Transformada Inversa da Discreta do Cosseno
int idct2d(float **x, int **X);

// Aplicacao da DCT na imagem
struct ComprImage * aplica_dct(struct Image *image);
// Aplicacao da IDCT na imagem
struct Image * aplica_idct(struct ComprImage *image);

/*****************************QUANTIZACAO*****************************/

// Efetua quantizacao
int ** quantiza(float **x, int fator);
struct QuantImage * quantizacao(struct ComprImage *image, int fator);
// Efetua dequantizacao
float ** dequantiza(int **x, int fator);
struct ComprImage * dequantizacao(struct QuantImage *quant, int fator);


// Compressao JPEG
struct QuantImage * comprime(struct Image * image, unsigned int fator);
// Decompressao JPEG
struct Image * descomprime(struct QuantImage * image, unsigned int fator);

// Leitura de imagem TIFF 
struct Image * ReadTiffImage(char * name);
// Escrita de imagem TIFF
void SaveTiffImage(char * name, struct Image * t);
