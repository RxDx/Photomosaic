#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define n_max 500

/*** ESTRUTURA DE UM NO, DO TIPO ARQUIVO ***/
typedef struct no {
    char *nome, *path;
	int max_h, max_s, max_v;
    struct no *proximo;
} No;

/*** ESTRUTURA DE UMA LISTA, DO TIPO LISTA ***/
typedef struct lista {
    No *inicio;
    unsigned int tam;
} Lista;

/*** FUNCOES ***/

/*** CRIA UMA LISTA ***/
Lista *constroiLista(void){
      Lista *l = (Lista *) malloc(sizeof(Lista)); // ALOCA ESPACO PARA A LISTA
      if (!l) return NULL; // VERIFICA SE FOI POSSIVEL ALOCAR
      l->tam = 0; // INICIALIZA O TAMANHO DA LISTA
      l->inicio = NULL; // CORRIGE OS PONTEIROS
      return l;	// RETORNA A LISTA CRIADA
}

void leArquivo(FILE *arq){
	
	
}

/* Recebe como argumento um diretorio e retorna quantos arquivos existem nesse diretorio */
int nArquivos(char *dir) {
	DIR* diretorio = opendir(dir);
	struct dirent* arquivo;
	int tam = 0;

	if (!diretorio) {
			perror("opendir");
			return -1;
	}
	
	arquivo = readdir(diretorio);
	while (arquivo)	{
		tam++;
		arquivo = readdir(diretorio);
	}
	
	closedir(diretorio);

	return tam;
}

void maxHSV(IplImage *img, No *imagem){
	/* variaveis do histograma */
	int numBins = 256;
	float range[] = {0, 255};
	float *ranges[] = {range};
	
	/* cria um histograma */
	CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
	cvClearHist(hist);
	
	/* aloca as imagens auxiliares */
	IplImage *imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage *imgH = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *imgS = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *imgV = cvCreateImage(cvGetSize(img), 8, 1);

	cvCvtColor(img, imgHSV, CV_BGR2HSV);			// converte a img (RGB) para imgHSV (HSV)
	cvSplit(imgHSV, imgH, imgS, imgV, NULL);		// separa os canais da imgHSV
	
	cvCalcHist(&imgH, hist, 0, 0);					// calcula o histograma da imgH
	cvGetMinMaxHistValue(hist, NULL, NULL, NULL, &imagem->max_h); // coleta a cor dominante
	cvClearHist(hist);
	
	cvCalcHist(&imgS, hist, 0, 0);
	cvGetMinMaxHistValue(hist, NULL, NULL, NULL, &imagem->max_s);
	cvClearHist(hist);
	
	cvCalcHist(&imgV, hist, 0, 0);
	cvGetMinMaxHistValue(hist, NULL, NULL, NULL, &imagem->max_v);
	cvClearHist(hist);
	
	/* libera as imagens auxiliares */
	cvReleaseImage(&imgHSV);
	cvReleaseImage(&imgH);
	cvReleaseImage(&imgS);
	cvReleaseImage(&imgV);
}

No *analisaImagem(No *imagem){
	IplImage *img = cvLoadImage(imagem->nome, 1); 	// carrega a imagem
	if (!img){
		printf("%s erro!\n", imagem->nome);
		return 0;
	}
		
	maxHSV(img, imagem);
	cvReleaseImage(&img);
		
	return imagem;
}

/*** PROGRAMA PRINCIPAL ***/
int main(int argc, char *argv[]) {
	if (argc != 5){
		printf("Uso correto: photomosaic <FOTO> <DIR> <ARQ> <REPETE> <N_SEGMENTOS>.\n");
		return 1;
	}
	
	unsigned int r = atoi(argv[4]);
	if (r != 0 && r != 1){
		printf("Parametro REPETE invalido. Deve ser 0 ou 1.\n");
		return 3;
	}
	
	unsigned int n = atoi(argv[5]);
	if (n > n_max){
		printf("Parametro N invalido. Deve ser um valor positivo menor que %d.\n", n_max);
		return 4;
	}

	/*** CRIA UMA LISTA ***/
	Lista *l = malloc(sizeof(Lista));
	if (!l) return -1;
	l->inicio = NULL;
	l->tam = 0;

	/*** Calcula o histograma das imagens localizadas no diretorio passado por parametro ***/
	DIR *diretorio;								// ponteiro com o diretorio das imagens
	struct dirent *arquivo;						// ponteiro para a imagem do banco de dados
	
	diretorio = opendir(argv[2]);
	if (!diretorio) {
		perror("opendir");
		return -1;
	}

	arquivo = readdir(diretorio);	
	No *imagem;
	while (arquivo)	{
		if (l->tam == 0) {
			imagem = malloc(sizeof(No));
			if (!imagem) {
				printf("Erro na alocacao de memoria!");
				return -1;
			}
			l->inicio = imagem;
			imagem->proximo = NULL;
			imagem->nome = arquivo->d_name;
			imagem->path = diretorio;
			imagem = analisaImagem(imagem);
			l->tam++;
		}
		else {
			imagem = malloc(sizeof(No));
			if (!imagem){
				printf("Erro na alocacao de memoria");
				return -1;
			}
			imagem->proximo = l->inicio;
			l->inicio = imagem;
			imagem->nome = arquivo->d_name;
			imagem->path = diretorio;
			imagem = analisaImagem(imagem);
			l->tam++;
		}
		
		arquivo = readdir(diretorio);
	}
	
	closedir(diretorio);
	
	CvPoint pt = cvPoint(0, 0);
	int i, j, min_idx, max_idx;
	

	
	/* variaveis do histograma */
	int numBins = 256;
	float range[] = {0, 255};
	float *ranges[] = {range};
	
	/* cria um histograma */
	CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
	cvClearHist(hist);


	IplImage *img = cvLoadImage(argv[1], 1);
	if (!img) {
		printf("Erro ao carregar a imagem %s.", argv[1]);
		return 2;
	}
	
	IplImage *img_aux = cvCreateImage(cvGetSize(img), 8, 1);
	
	IplImage *imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	IplImage *imgH = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *imgS = cvCreateImage(cvGetSize(img), 8, 1);
	IplImage *imgV = cvCreateImage(cvGetSize(img), 8, 1);
	
	/* converte a imagem RGB para HSV */
	cvCvtColor(img, imgHSV, CV_BGR2HSV);
	
	/* separa cada canal HSV em uma imagem */
	cvSplit(imgHSV, imgH, imgS, imgV, NULL);

	for (j=0; j<=n; j++) {
		for (i=0; i<=n; i++) {
			/* seleciona uma regiao da img e da imgH */
			cvSetImageROI(imgH, cvRect(pt.x, pt.y, img->width/n, img->height/n));
			cvSetImageROI(img_aux, cvRect(pt.x, pt.y, img->width/n, img->height/n));			     

			/* calcula o histograma */
			cvCalcHist(&imgH, hist, 0, 0);

			/* coleta a cor dominante */
			cvGetMinMaxHistValue(hist, NULL, NULL, &min_idx, &max_idx);

			/* imprime a cor dominante */
			printf("max_idx = %d\n", max_idx);
			
			/* sobrepoe a imgH mais proxima no segmento */
			
			
			/* limpa histograma */
			cvClearHist(hist);

			/* reseta as imagens */
			cvResetImageROI(imgH);
			cvResetImageROI(img_aux);
			
			pt.x+=img->width/n;
		} 
		pt.y+=img->height/n;
		pt.x=0;
	}

	cvNamedWindow("imgH", 1);
	cvNamedWindow("imgHSV", 1);
	cvNamedWindow("imgAUX", 1);
	cvNamedWindow("img", 1);


	cvShowImage("img", img);
	cvShowImage("imgH", imgH);
	cvShowImage("imgHSV", imgHSV);
	cvShowImage("imgAUX", img_aux);
	cvWaitKey(0);

	return 0;
}
