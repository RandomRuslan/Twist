#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <math.h>
#define angle 5
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#pragma pack(push, 1)
struct Hat {
    unsigned char  B, M; 		    // Символы BM (2 байта)
    unsigned long  size; 			// Размер файла (4 байта)
    unsigned long  DontUse; 		// Не используются(4 байта)
    unsigned long  MDRM; 		   	// Местонахождение данных растрового масива (4 байта)
    unsigned long  hatLength;    	// Длинна этого заголовка (4 байта)
    unsigned long  width; 			// Ширина изображения (4 байта)
    unsigned long  height; 			// Высота изображения (4 байта)
    unsigned short ChCP; 	        // Число цветовых плоскостей (2 байта)
    unsigned short bitpxl; 		// Бит/пиксель (2 байта)
    unsigned long  MS; 		        // Метод сжатия (4 байта)
    unsigned long  DRM; 		    // Длина растрового массива (4 байта)
    unsigned long  GorR;		    // Горизонтальное разрешение (4 байта)
    unsigned long  VerR; 	        // Вертикальное разрешение(4 байта)
    unsigned long  ChCI; 	        // Число цветов изображения (4 байта)
    unsigned long  ChOC;	        // Число основных цветов (4 байта)
} hat;
#pragma pack(pop)

typedef struct{
	unsigned char a,b,c;
} pxl;
pxl** img; //данная картинка

//вычисление новых Х и У по данным
long newX(long x, long y, double sinn, double coss){
	return (long)(x*coss - y*sinn);
}
long newY(long x, long y, double sinn, double coss){
	return (long)(x*sinn + y*coss);
}

long readBMP(char* path) {
	long i, j;
    FILE* file = fopen(path,"rb");
    if(file == NULL) return 0;
    fread(&hat, sizeof(hat), 1, file);
    if (hat.B != 'B' || hat.M != 'M' || hat.bitpxl != 24) {
        printf("Wrong file");
        return 0;
    }
	char dump[3]; 
	img=(pxl**)malloc(hat.height * sizeof(pxl*));
	for(i = 0; i < hat.height; i++)
		img[i] = (pxl*)malloc(hat.width * sizeof(pxl));
	
	for(i=0; i<hat.height; i++){
		for(j=0; j<hat.width; j++)
			fread(&img[i][j], 1, sizeof(pxl), file);
		fread(dump, 1, hat.width%4, file);
	}
    fclose(file);
    return 1;
}

long main(long argc, char * argv[]) {
    long n, m, i, j;
	if (readBMP("in.bmp") == 0) {
        printf("Error");
        return 0;
    }
    double ang =(double)angle/180.0 * M_PI; //угол задается define'ом
	long top, bot, lft, rgt; 
	double sinn=sin(ang);
	double coss=cos(ang); 
	n=hat.height;
	m=hat.width;	
	
	//Вычиление координат углов после поворота
	long d1=newY(0,0,sinn,coss);
    long d2=newY(0,n-1,sinn,coss);
    long d3=newY(m-1,0,sinn,coss);
    long d4=newY(m-1,n-1,sinn,coss);
	top=max(d1, max(d2, max(d3, d4)));
	bot=min(d1, min(d2, min(d3, d4)));
	
	d1=newX(0,0,sinn,coss);
    d2=newX(0,n-1,sinn,coss);
    d3=newX(m-1,0,sinn,coss);
    d4=newX(m-1,n-1,sinn,coss);	
    rgt=max(d1, max(d2, max(d3, d4)));
	lft=min(d1, min(d2, min(d3, d4)));
	
	pxl pic[top-bot+1][rgt-lft+1]; //новая картинка
	
	for(i=0; i<=top-bot; i++)
		for(j=0; j<=rgt-lft; j++){
			//вычисление старых координат по новым
			d1=(i+bot)*sinn+(j+lft)*coss; 
			d2=-(j+lft)*sinn+(i+bot)*coss;
	
			if(d1>=0 && d1<m && d2<n && d2>=0){
				pic[i][j].a=img[d2][d1].a;
				pic[i][j].b=img[d2][d1].b;
				pic[i][j].c=img[d2][d1].c;
			}
			else{
				pic[i][j].a=255;
				pic[i][j].b=255;
				pic[i][j].c=255;
			}			
		}	
	
	//изменение hat 
	hat.width=rgt-lft+1;
	hat.height=top-bot+1;
	hat.size=(3*hat.width+hat.width%4)*hat.height+sizeof(hat);

	FILE* file = fopen("out.bmp","wb");
    if(file == NULL) return 0;
    fwrite(&hat, sizeof(hat), 1, file);
	char dump[3]={255};
	for(i=0; i<hat.height; i++){
		for(j=0; j<hat.width; j++)
			fwrite(&pic[i][j], sizeof(pxl), 1, file);
		fwrite(dump, sizeof(char), hat.width%4, file);
	}
	    
    fclose(file);	
    return 0;
}
