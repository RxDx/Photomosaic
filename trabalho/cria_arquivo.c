#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <stdlib.h>

/*** ESTRUTURA DE UM NO, DO TIPO ARQUIVO ***/
typedef struct no {
    char nome[16];
    char path[16];
	int max_h, max_s, max_v, erro;
    struct no *prox;
} No;

/*** ESTRUTURA DE UMA LISTA, DO TIPO LISTA ***/
typedef struct lista {
    No *inicio;
    unsigned int tam;
} Lista;

/*** CRIA UMA LISTA ***/
Lista *constroiLista(void){
      Lista *l = (Lista *) malloc(sizeof(Lista)); // ALOCA ESPACO PARA A LISTA
      if (!l) return NULL; // VERIFICA SE FOI POSSIVEL ALOCAR
      l->tam = 0; // INICIALIZA O TAMANHO DA LISTA
      l->inicio = NULL; // CORRIGE OS PONTEIROS
      return l;	// RETORNA A LISTA CRIADA
}

/*** LE A ENTRADA (STDIN) E ARMAZENA NA LISTA ***/
void leEntrada(Lista *l){
	No *aux;
	
	while(!feof(stdin)) // ENQUANTO A ENTRADA (STDIN) NAO ACABAR
	{
		if (l->tam == 0){
			aux = (No *) malloc(sizeof(No));
			l->inicio = aux;
			scanf("%s", aux->nome);
			
			aux->prox = NULL;
			l->tam++;
		}
		else{
			aux = (No *) malloc(sizeof(No));
			aux->prox = l->inicio;
			l->inicio = aux;
			
			scanf("%s", aux->nome);

			if (feof(stdin)) return; // PARA NAO BUGAR
			l->tam++;
		}
	}
	free(aux); // DESALOCA O NO AUXILIAR
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

void analisaImagem(No *imagem){
	IplImage *img = cvLoadImage(imagem->nome, 1);
	if (!img){
		imagem->erro = 1;
		return;
	}
	else
		imagem->erro = 0;
	
	maxHSV(img, imagem);
	cvReleaseImage(&img);
}

int main(void){
	Lista *l = constroiLista();
	
	leEntrada(l);
	
	No *aux = l->inicio;
	while (aux){
		analisaImagem(aux);
		if (aux->erro == 0)
			printf("%s %d %d %d %d\n", aux->nome, aux->max_h, aux->max_s, aux->max_v, aux->erro);		
		aux = aux->prox;
	}

	return 1;
}
