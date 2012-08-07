#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<stdio.h>
//A partir de uma imagem, verifique qual a cor predominante da mesma.
int main(int argc,char *argv[])
{
  // variaveis do histograma
  int max_idx,numBins = 256;
  float range[] = {0, 255};
  float *ranges[] = { range };
  float min_value,max_value;
  // cria um histograma
  CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
  cvClearHist(hist);

  IplImage *img1=cvLoadImage(argv[1],1);
  IplImage* imgHSV = cvCreateImage(cvGetSize(img1), 8, 3);
  IplImage* imgH = cvCreateImage(cvGetSize(img1), 8, 1);
  IplImage* imgS = cvCreateImage(cvGetSize(img1), 8, 1);
  IplImage* imgV = cvCreateImage(cvGetSize(img1), 8, 1);  

  /*converte a imagem de rgb para hsv*/
  cvCvtColor(img1,imgHSV,CV_BGR2HSV);
  /*separa cada canal HSV em uma imagem*/
  cvSplit(imgHSV, imgH, imgS, imgV, NULL);

  /*calcula o histograma*/
  cvCalcHist(&imgH, hist, 0, 0);
  /*coleta a cor dominante*/
  cvGetMinMaxHistValue( hist, NULL, NULL,NULL,&max_idx);
  /*imprimi a cor dominante*/
  printf("max: %d\n", max_idx);
  /*limpa histograma*/
  cvClearHist(hist);

  return 0;
}


/*coleta uma região da img1
      cvSetImageROI(img1, cvRect(10,10,50,50)); //imagem, cvRect(posicao x, posicao y, largura,altura)
      cvCopy(img1, img2, NULL);//copia a regiao da imagem 1 para imagem 2
      cvResetImageROI(img1);//retorna a imagem ao tamanho original
*/


/* Exercício
A partir de uma imagem faça:
   a) Divida a imagem em 16 imagens de mesmo tamanho;
   b) Com as 16 imagens criadas a partir da imagem original. Monte uma Janela contendo-as.
   c) Extrair a cor dominante de cada região obtida na letra a.
*/
