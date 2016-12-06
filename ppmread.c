#include "ppmread.h"

ppmImage openImage(char* name){
  ppmImage image = (ppmImage) malloc(sizeof(*image));
  image->name = name;
  image->width = image->height = 0;

  //Opening file
  image->file = fopen(name, "r");
  if(image->file == NULL){
    free(image);
    return NULL;
  }

  //Checking file format
  char* buffer = (char*)malloc(2*sizeof(char));
  fread(buffer, sizeof(char), 2, image->file);
  if(strcmp(buffer, "P6")){
    fprintf(stderr, "Error : Wrong file format (format read %s)\n", buffer);
    exit(1);
  }
  free(buffer);

  //Reading comment
  buffer = (char*)malloc(sizeof(char));
  fread(buffer, sizeof(char), 1, image->file); //read end of line car
  fread(buffer, sizeof(char), 1, image->file); //Read first car to see if comment
  while((buffer != NULL) && (!strcmp(buffer, "#"))){
    while(strcmp(buffer, "\n")){
      fread(buffer, sizeof(char), 1, image->file);
    }
    fread(buffer, sizeof(char), 1, image->file);
  }

  //Get image size
  int numberSize = 1;
  char* number = (char*)malloc(sizeof(char)*numberSize);
  number[numberSize-1] = buffer[0];
  while((fread(buffer, sizeof(char), 1, image->file)) && (strcmp(buffer, " "))){
    if(buffer != NULL){
      numberSize++;
      number = realloc(number,sizeof(char)*numberSize);
      number[numberSize-1] = buffer[0];
    }
  }
  image->width = atoi(number);
  free(number);

  numberSize = 0;
  number = (char*)malloc(sizeof(char)*numberSize);
  while((fread(buffer, sizeof(char), 1, image->file)) && (strcmp(buffer, "\n"))){
    if(buffer != NULL){
      numberSize++;
      number = realloc(number,sizeof(char)*numberSize);
      number[numberSize-1] = buffer[0];
    }
  }
  image->height = atoi(number);
  free(number);

  //Reading max color value
  do{
    fread(buffer, sizeof(char), 1, image->file);
  }while(strcmp(buffer, "\n"));

  free(buffer);
  return image;
}

unsigned char* getImageData(ppmImage inputImage){
  //Read data
  unsigned char* data = (unsigned char*)malloc(inputImage->height * inputImage->width * 3 * sizeof(unsigned char));
  int read = 1;
  read = fread(data, sizeof(unsigned char), inputImage->height * inputImage->width * 3, inputImage->file);
  if( read != inputImage->height * inputImage->width * 3){
    fprintf(stderr, "Error : bad reading of file\n");
    exit(1);
  }

  /*unsigned char temp[3];

  //Rearange data
  for(int i = 0 ; i < inputImage->width ; i++){
    for(int j = 0 ; j < inputImage->height; j++){
      temp[0] = data[j*i];
      temp[1] = data[(j*i)+1];
      temp[2] = data[(j*i)+2];

      data[j*i] = data[(inputImage->height - j)*i];
      data[(j*i)+1] = data[((inputImage->height - j)*i)+1];
      data[(j*i)+2] = data[((inputImage->height - j)*i)+2];

      data[(inputImage->height - j)*i] = temp[0];
      data[((inputImage->height - j)*i)+1] = temp[1];
      data[((inputImage->height - j)*i)+2] = temp[2];

    }
  }*/
  return data;
}

void closeImage(ppmImage image){
  if(image->file != NULL){
    fclose(image->file);
  }
  free(image);
}
