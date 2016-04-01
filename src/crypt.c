#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const short crypt[] = {8,13,11,17,10,7,15,10,18,17,8,13,2,9,3,10,18,5,8,11,12,4,14,16,9,2,1,16,17,7,16,12,13,16,12,5,2,16,7,17,14,18,3,13,10,14,7,6,3,8,4};



char * app_Crypt(char *dataIn)
{
  char *data = (char*)malloc(strlen(dataIn)+1);
  strcpy(data, dataIn);
  for (char *tmp2 = data; tmp2[0]; tmp2++) tmp2[0] -= crypt[tmp2 - data];
  return data;
}

char * app_DeCrypt(char *dataIn, int len)
{
  char *data = (char*)malloc(len+1);
  memset(data, 0, len+1);
  memcpy(data, dataIn, len);
  for (char *tmp2 = data; tmp2[0]; tmp2++) tmp2[0] += crypt[tmp2 - data];
  return data;
}
