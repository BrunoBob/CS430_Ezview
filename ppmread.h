#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
  char* name;
  FILE* file;
  int width, height;
} *ppmImage;


ppmImage openImage(char* name);
unsigned char* getImageData(ppmImage inputImage);
void closeImage(ppmImage image);
