#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"
#define MAX 100000
#define KMAX 10

int counter = 0;
int clustercounter = 0;
int bincount = 0;
char cipherarr[MAX];
char finalarr[MAX];
int clustercount[KMAX];
int w1 = 180;
int h1 = 180;

void lsb_fetch(char filename[], int total_pixels) {
  FILE *file;
  file = fopen(filename, "r+");
  int r, g, b, cluster, lsb_r, lsb_g, lsb_b;
  int buffer[8];
  for (int i = 0; i < total_pixels; i++) {
    fscanf(file, "%d %d %d", &r, &g, &b);
    lsb_r = r % 2;
    if (lsb_r == 0) {
      strcat(finalarr, "0");
    } else {
      strcat(finalarr, "1");
    }
    buffer[counter] = lsb_r;
    counter++;
    if (counter == 8) {
      int temp = 0;
      for (int i = 0; i < 8; i++) {
        if (buffer[i] == 0) {
          temp++;
        }
      }
      if (temp == 8) {
        break;
      }
      temp = 0;
      counter = counter % 8;
    }
    lsb_g = g % 2;
    if (lsb_g == 0) {
      strcat(finalarr, "0");
    } else {
      strcat(finalarr, "1");
    }
    buffer[counter] = lsb_g;
    counter++;
    if (counter == 8) {
      int temp = 0;
      for (int i = 0; i < 8; i++) {
        if (buffer[i] == 0) {
          temp++;
        }
      }
      if (temp == 8) {
        break;
      }
      temp = 0;
      counter = counter % 8;
    }
    lsb_b = b % 2;
    if (lsb_b == 0) {
      strcat(finalarr, "0");
    } else {
      strcat(finalarr, "1");
    }
    buffer[counter] = lsb_b;
    counter++;
    if (counter == 8) {
      int temp = 0;
      for (int i = 0; i < 8; i++) {
        if (buffer[i] == 0) {
          temp++;
        }
      }
      if (temp == 8) {
        break;
      }
      temp = 0;
      counter = counter % 8;
    }
  }

  counter = 0;
  for (int i = 0; i < (strlen(finalarr) - 8); i++) {
    if (finalarr[i] == '0') {
      strcat(cipherarr, "0");
    } else {
      strcat(cipherarr, "1");
    }
  }
  strcpy(finalarr, "");
}

void lsb_decrypt(int k, int l) {

  printf("\nLSB Decryption\n");
  for (int i = 0; i < k; i++) {
    char name[30];
    sprintf(name, "Cluster%d lsb.png", i);
    int w1, h1, channels;
    unsigned char *img = stbi_load(name, &w1, &h1, &channels, 3);

    FILE *fp;
    char pixfile[30];
    sprintf(pixfile, "Cluster%d.txt", i);
    fp = fopen(pixfile, "w");

    for (int j = 0; j < 3 * w1 * h1; j += 3) {
      fprintf(fp, "%d %d %d\n", *(img + j), *(img + j + 1), *(img + j + 2));
    }
    fclose(fp);
    lsb_fetch(pixfile, (w1 * h1));
  }
  cipherarr[l]='\0';
  printf("Final: %s\n", cipherarr);
  FILE *file3;
  file3 = fopen("../Decryption-AES/cipher_afterlsb.txt", "w");
  fprintf(file3, "%s", cipherarr);
  fclose(file3);
  printf("Cipher saved in cipher_afterlsb.txt\n");
}

int main() {
  int k,l;
  printf("Enter number of clusters k and length of binarry array: ");
  scanf("%d %d", &k,&l);
  lsb_decrypt(k,l);
}
