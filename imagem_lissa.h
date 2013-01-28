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

struct RLEImage {
	unsigned int r_size, g_size, b_size;
	signed char *red, *green, *blue;
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

/*****************************QUANTIZACAO*****************************/
// Efetua quantizacao
int ** quantiza(float **x, int fator);
struct QuantImage * quantizacao(struct ComprImage *image, int fator);
// Efetua dequantizacao
float ** dequantiza(int **x, int fator);
struct ComprImage * dequantizacao(struct QuantImage *image, int fator);

/*********************************RLE*********************************/
// Funcao auxiliar do rle (compressao)
signed char* rle(int** , int *);
// Aplicacao do rle na imagem (compressao)
int aplica_rle(struct QuantImage * , signed char * );
// Funcao auxiliar do rle (descompressao)
int** rle_d(signed char * , int *);
// Aplicacao do rle na imagem (descompressao)
void aplica_rle_d(struct QuantImage *, signed char * , int );


// Compressao JPEG
//struct QuantImage * comprime(struct Image * image, unsigned int fator);
// Decompressao JPEG
//struct Image * descomprime(struct QuantImage * image, unsigned int fator);

// Leitura de imagem TIFF 
struct Image * ReadTiffImage(char * name);
// Escrita de imagem TIFF
void SaveTiffImage(char * name, float * t);
