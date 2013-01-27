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
	int *red, *green, *blue;
};

// Funcao auxiliar para criar bloco NxN
int   ** aloca_i(int n, int m);
float ** aloca_f(int n, int m);
// Funcao auxiliar para imprimir matriz
void printim(int **m, int n);
void printfm(float **m, int n);
// Funcao auxiliar da DCT
float c(int x);
// Transformada Discreta do Cosseno
int dct2d(int **x, float **X);
// Transformada Inversa da Discreta do Cosseno
int idct2d(float **x, int **X);
// Efetua quantizacao
int ** quantiza(float **x, int fator);
struct QuantImage * quantizacao(struct ComprImage *image, int fator);
// Aplicacao da DCT na imagem
struct ComprImage * aplica_dct(struct Image *image);
// Funcao de codificacao entropica retirando os 0s
int * codifica(unsigned int *);
// Funcao de decodificacao entropica devolvendo os 0s
unsigned int * decodifica(int *);
// Aplicacao da codificacao na imagem
int * codificar(struct Image *image);
// Aplicacao da decodificacao na imagem
struct Image * decodificar(int *image);

// Leitura de imagem TIFF 
struct Image * ReadTiffImage(char * name);
// Escrita de imagem TIFF
void SaveTiffImage(char * name, float * t);
