#include<opencv/cv.h>
#include<opencv/highgui.h>
#include<stdio.h>
/*1) A partir de uma imagem faça:
   a) Realize a operação de CROP na imagem, transformando em 16 Imagens;
   b) Com as 16 imagens criadas a partir da imagem original. Monte uma Janela contendo as 16 imagens.
   c) Extrair a cor dominante de cada região obtida pelo crop.*/

int main(int argc,char *argv[])
{
  IplImage *img1=cvLoadImage(argv[1],1);
  IplImage *img2 = cvCreateImage(cvGetSize(img1),img1->depth,img1->nChannels);
  CvPoint pt=cvPoint(0,0);
  CvPoint pt2=cvPoint(0,0);
  int i,j,min_idx,max_idx;
  // variaveis do histograma
  int numBins = 256;
  float range[] = {0, 255};
  float *ranges[] = { range };
  float min_value,max_value;
  // cria um histograma
  CvHistogram *hist = cvCreateHist(1, &numBins, CV_HIST_ARRAY, ranges, 1);
  cvClearHist(hist);

  IplImage* imgHSV = cvCreateImage(cvGetSize(img1), 8, 3);
  IplImage* imgH = cvCreateImage(cvGetSize(img1), 8, 1);
  IplImage* imgS = cvCreateImage(cvGetSize(img1), 8, 1);
  IplImage* imgV = cvCreateImage(cvGetSize(img1), 8, 1);  

  /*converte a imagem de rgb para hsv*/
  cvCvtColor(img1,imgHSV,CV_BGR2HSV);
  /*separa cada canal HSV em uma imagem*/
  cvSplit(imgHSV, imgH, imgS, imgV, NULL);

  cvNamedWindow("display",1);

  for(i=0;i<4;i++)
  { 
    for(j=0;j<4;j++)
    { 
      /*pega uma regiao do img1 e do imgH*/
      cvSetImageROI(img1, cvRect(pt.x, pt.y, img1->width/4, img1->height/4));
      cvSetImageROI(imgH, cvRect(pt.x, pt.y, img1->width/4, img1->height/4));      
      pt.x+=img1->width/4;
      pt2.x+=img1->width/4;
      /*calcula o histograma*/
      cvCalcHist(&imgH, hist, 0, 0);
      /*coleta a cor dominante*/
      cvGetMinMaxHistValue( hist, NULL, NULL,NULL,&max_idx);
      /*imprimi a cor dominante*/
      printf("max: %d\n", max_idx);
      /*limpa histograma*/
      cvClearHist(hist);
      /*pega uma regiao do img2*/
      cvSetImageROI(img2, cvRect(pt2.x,pt2.y,img1->width/4, img1->height/4));
      cvCopy(img1, img2, NULL);
      cvResetImageROI(img1);
      cvResetImageROI(img2);
    } 
    pt.y+=img1->height/4;
    pt2.y+=img1->height/4;
    pt.x=0;
    pt2.x=0;
  }  
  cvShowImage("display",img2);
  cvWaitKey(0);
  return 0;
}
