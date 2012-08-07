#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#define n_max 500

/*** ESTRUTURA DE UM NO, DO TIPO ARQUIVO ***/
typedef struct no {
    char *nome, *path;
	int max_h, max_s, max_v, flag;
    struct no *prox, *ant;
} No;

/*** ESTRUTURA DE UMA LISTA, DO TIPO LISTA ***/
typedef struct lista {
    No *inicio, *fim;
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

/*** INSERE NO ***/
void insereNo(Lista *l, No *n){	
	if (l->tam == 0){
		l->inicio = n;
		n->prox = NULL;
		l->tam++;
	}
	else{
		n->prox = l->inicio;
		l->inicio = n;
		l->tam++;		
	}	
}

void leDiretorio(char *dir, Lista *l){
	struct dirent **namelist;
	int n;
	No *aux;

	n = scandir(dir, &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else{
		while(n--){
			aux = malloc(sizeof(No));
			aux->nome = namelist[n]->d_name;
			insereNo(l, aux);
			free(namelist[n]);
		}
	free(namelist);
	}
	
	
}     

char *convertePath(char *dir, char *arq){
	/*** path = diretorio/arquivo.ext ***/
	char *path = calloc(32, sizeof(char));
	strcat(path, dir);
	strcat(path, "/");
	strcat(path, arq);
	return path;
}

void leArquivo(char *dir, char *arq, Lista *l){
	char *path = convertePath(dir, arq);

	FILE *histogramas = fopen(path, "r+");
	if (!histogramas){
		printf("Erro ao abrir o arquivo com os histogramas do BD.");
		return;
	}
	
	printf("%s\n\n\n", path);
	
	No *aux;
	char nome[32];
	int max_h, max_s, max_v, flag;
	while (!feof(histogramas)){
		//AUX MALLOC;
		fscanf(histogramas, "%s %d %d %d %d", nome, &max_h, &max_s, &max_v, &flag);
		aux->nome = nome;
		aux->max_h = max_h;
		aux->max_s = max_s;
		aux->max_v = max_v;
		aux->flag = flag;
		if (feof(histogramas)) return;
		//insereNo(l, *aux);
	}
	
	free(aux);
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
	cvClearHist(hist);								// reseta o histograma
	
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

IplImage *adicionaImagem(No *imagem){
	IplImage *img = cvLoadImage(imagem->nome, 1); 	// carrega a imagem
	if (!img){
		printf("%s erro!\n", imagem->nome);
		return 0;
	}
		
	maxHSV(img, imagem);
	cvReleaseImage(&img);
		
	return img;
}

No *analisaImagem(IplImage *img){
	No *aux = (No *) malloc(sizeof(No));
		
	maxHSV(img, aux);
		
	return aux;
}

int calcDistEuclidiana(No *p, No *q){
	return sqrt( pow((p->max_h - q->max_h), 2) + pow((p->max_s - q->max_s), 2) + pow((p->max_v - q->max_v), 2) );
}

No *comparaDiferenca(No *img, Lista *l){
	int dist_min = 99999;
	int dist;
	
	No *aux = l->inicio;
	No *no_min = aux;
	
	while (aux){
		dist = calcDistEuclidiana(img, aux);
		if (dist < dist_min){
			no_min = aux;
		}
		aux = aux->prox;
	}
	
	return no_min;
}

/*** PROGRAMA PRINCIPAL ***/
int main(int argc, char *argv[]) {
	if (argc != 6){
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
	
	char *dir = argv[2];
	char *arq = argv[3];

	/*** CRIA UMA LISTA ***/
	Lista *l = constroiLista();
	
	leDiretorio(dir, l);
	
	No *aux = l->inicio;
	while (aux){
		printf("%s\n", aux->nome);
		aux = aux->prox;
	}
	
	
	
	
	
	

	//~ leArquivo(dir, arq, l);
	//~ 
			//~ 
	//~ 
//~ 
	//~ IplImage *img = cvLoadImage(argv[1], 1);
	//~ IplImage *img_aux;
	//~ No *aux = (No *) malloc(sizeof(No));
	//~ 
	//~ aux = l->inicio;
	//~ while (aux){
		//~ printf("%s", aux->nome);
		//~ aux = aux->prox;
		//~ 
	//~ }
	//~ 
	//~ CvPoint pt = cvPoint(0, 0);
	//~ int i, j;
		//~ 
	//~ for (j=0; j<=n; j++) {
		//~ for (i=0; i<=n; i++) {
			//~ /* seleciona uma regiao das imagens */
			//~ cvSetImageROI(img, cvRect(pt.x, pt.y, img->width/n, img->height/n));
//~ 
			//~ /* analisa H, S, V e armazena as informacoes em aux */
			//~ aux = analisaImagem(img);
			//~ 
			//~ /* compara a diferenca da distancia euclidiana do segmento com os do banco de dados */
			//~ comparaDiferenca(aux, l);
			//~ 
			//~ /* redimensiona a imagem adequada */
			//~ img_aux = cvLoadImage(convertePath(dir, aux->nome), 1);
			//~ cvResize(img_aux, img, CV_INTER_NN);
			//~ 
			//~ /* copia a imagem para o segmento */
			//~ cvCopy(img_aux, img, NULL);
			//~ 
			//~ /* reseta as imagens e desaloca memoria */
			//~ cvResetImageROI(img);
			//~ cvReleaseImage(&img_aux);
			//~ free(aux);
			//~ 
			//~ pt.x+=img->width/n;
		//~ }
		//~ pt.y+=img->height/n;
		//~ pt.x=0;
	//~ }
	
	/* salva a imagem do photomosaico */
	//cvSaveImage(saida, img);

	//cvNamedWindow("img", 1);
	//cvShowImage("img", img);
	//cvWaitKey(0);

	return 0;
}
