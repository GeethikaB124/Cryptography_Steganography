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
int width = 180;
int height = 180;
int k;

struct Pixel_LSB {
  int r, g, b, cluster;
};

struct Pixel {
  double r, g, b;
  int cluster;
  double mindis;
};

int w, h, k;

void kmeansclustering(struct Pixel *pixels, int k, int epochs, int tpixels,
                      int *clustercount) {
  float dis;
  struct Pixel centroids[k];
  for (int i = 0; i < tpixels; i++) {
    pixels[i].cluster = -1;
    pixels[i].mindis = DBL_MAX;
  }
  srand(time(0));
  for (int i = 0; i < k; i++)
    centroids[i] = pixels[rand() % tpixels];

  for (int i = 0; i < k; i++)
    centroids[i].cluster = i;

  for (int e = 0; e < epochs; e++) {
    // Assigning points to clusters by minimizing euclidean distance
    for (int i = 0; i < k; i++) {
      for (int j = 0; j < tpixels; j++) {
        // Calculate distance of the point to the centroid
        dis = sqrt(pow((pixels[j].r - centroids[i].r), 2) +
                   pow((pixels[j].g - centroids[i].g), 2) +
                   pow((pixels[j].b - centroids[i].b), 2));
        if (dis < pixels[j].mindis) {
          pixels[j].mindis = dis;
          pixels[j].cluster = centroids[i].cluster;
        }
      }
    }

    double num_pixels[k];
    double sum_r[k], sum_g[k], sum_b[k];

    for (int i = 0; i < k; i++) {
      num_pixels[i] = 0.0;
      sum_r[i] = 0.0;
      sum_g[i] = 0.0;
      sum_b[i] = 0.0;
    }

    for (int i = 0; i < tpixels; i++) {
      num_pixels[pixels[i].cluster] += 1;
      sum_r[pixels[i].cluster] += pixels[i].r;
      sum_g[pixels[i].cluster] += pixels[i].g;
      sum_b[pixels[i].cluster] += pixels[i].b;
      pixels[i].mindis = DBL_MAX;
    }

    printf("Epoch%d\n", e + 1);
    for (int i = 0; i < k; i++) {
      if (num_pixels[i] == 0) {
        centroids[i] = pixels[rand() % tpixels];
        centroids[i].cluster = i;
      } else {
        centroids[i].r = sum_r[i] / num_pixels[i];
        centroids[i].g = sum_g[i] / num_pixels[i];
        centroids[i].b = sum_b[i] / num_pixels[i];
      }
      printf("Centroid%d : (%f, %f, %f)\n", i + 1, centroids[i].r,
             centroids[i].g, centroids[i].b);
    }
    printf("\n");
  }

  for (int c = 0; c < k; c++)
    clustercount[c] = 0;

  for (int c = 0; c < k; c++) {
    for (int i = 0; i < tpixels; i++) {
      if (pixels[i].cluster == c)
        clustercount[c]++;
    }
  }

  // File saving
  FILE *fp;
  fp = fopen("clusters.txt", "w");
  for (int i = 0; i < tpixels; i++)
    fprintf(fp, "%.0lf %.0lf %.0lf %d\n", pixels[i].r, pixels[i].g, pixels[i].b,
            pixels[i].cluster);
  fclose(fp);
}

void kmeans() {
  int channels;
  int epochs;
  char imgname[20];

  printf("K-Means Clustering \n");
  printf("Enter filename of image: ");
  scanf("%s", imgname);
  printf("Enter number of clusters: ");
  scanf("%d", &k);
  printf("Enter number of iterations: ");
  scanf("%d", &epochs);

  int tpixels;
  int clustercount[k];
  struct Pixel pixels[MAX];

  unsigned char *img = stbi_load(imgname, &w, &h, &channels, 3);
  if (img == NULL) {
    printf("Error in loading the image\n");
    exit(1);
  }
  tpixels = w * h;

  int counter = 0;
  for (int i = 0; i < 3 * tpixels; i += 3) {
    pixels[counter].r = (double)*(img + i);
    pixels[counter].g = (double)*(img + i + 1);
    pixels[counter].b = (double)*(img + i + 2);
    counter++;
  }

  kmeansclustering(pixels, k, epochs, tpixels, clustercount);

  unsigned char *imgs[k];

  for (int i = 0; i < k; i++)
    imgs[i] = malloc(3 * (int)sqrt(clustercount[i]) *
                     (int)sqrt(clustercount[i]) * sizeof(uint8_t));

  for (int c = 0; c < k; c++) {
    counter = 0;
    for (int i = 0; i < tpixels; i++) {
      if (pixels[i].cluster == c && counter < 3 * (int)sqrt(clustercount[c]) *
                                                  (int)sqrt(clustercount[c])) {
        *(imgs[c] + counter++) = (uint8_t)pixels[i].r;
        *(imgs[c] + counter++) = (uint8_t)pixels[i].g;
        *(imgs[c] + counter++) = (uint8_t)pixels[i].b;
      }
    }
  }

  int channo = 3;

  char name[30];
  for (int cnt = 0; cnt < k; cnt++) {
    sprintf(name, "cluster%d.png", cnt + 1);
    printf("%s saved\n", name);
    stbi_write_png(name, (int)sqrt(clustercount[cnt]),
                   (int)sqrt(clustercount[cnt]), channo, imgs[cnt],
                   (int)sqrt(clustercount[cnt]) * channo);
  }
}

void sort() {
  int total_pixels = width * height;
  int r, g, b, cluster;
  FILE *file;
  FILE *f[k];
  for (int i = 0; i < k; i++) {
    file = fopen("clusters.txt", "r");
    char filename[15];
    snprintf(filename, sizeof(filename), "%d.txt", i);
    f[i] = fopen(filename, "w");
    for (int j = 0; j < total_pixels; j++) {
      fscanf(file, "%d %d %d %d", &r, &g, &b, &cluster);
      if (cluster == i) {
        fprintf(f[i], "%d %d %d %d\n", r, g, b, cluster);
      }
    }
    fclose(f[i]);
    fclose(file);
  }
  printf("\nSorting Successful\n");
}

void lsb_insert(char filename[], char binarr[], int frame) {

  FILE *file, *fl;
  file = fopen(filename, "r+");

  int total_pixels = 0;

  for (char c = getc(file); c != EOF; c = getc(file)) {
    if (c == '\n') {
      total_pixels += 1;
    }
  }
  fclose(file);

  clustercount[clustercounter] = total_pixels;
  clustercounter++;

  int len = frame * 8;
  char flagged_binarr[len + 8];

  for (int i = 0; i < len; i++) {
    flagged_binarr[i] = binarr[bincount];
    bincount++;
  }
  for (int i = len; i < (len + 8); i++) {
    flagged_binarr[i] = '0';
  }

  int pixel_counter = 0;
  int r, g, b, cluster;
  file = fopen(filename, "r+");
  fl = fopen("clusters_lsb.txt", "a+");
  for (int j = 0; j < (len + 8); j++) {
    fscanf(file, "%d %d %d %d", &r, &g, &b, &cluster);
    if (flagged_binarr[j] == '0') {
      if (r % 2 == 0)
        r = r;
      else
        r = r - 1;
    } else if (flagged_binarr[j] == '1') {
      if (r % 2 == 1)
        r = r;
      else
        r = r + 1;
    } else {
      break;
    }
    j++;
    if (flagged_binarr[j] == '0') {
      if (g % 2 == 0)
        g = g;
      else
        g = g - 1;
    } else if (flagged_binarr[j] == '1') {
      if (g % 2 == 1)
        g = g;
      else
        g = g + 1;
    } else {
      break;
    }
    j++;
    if (flagged_binarr[j] == '0') {
      if (b % 2 == 0)
        b = b;
      else
        b = b - 1;
    } else if (flagged_binarr[j] == '1') {
      if (b % 2 == 1)
        b = b;
      else
        b = b + 1;
    } else {
      break;
    }
    fprintf(fl, "%d %d %d %d\n", r, g, b, cluster);
    pixel_counter++;
  }

  for (int i = pixel_counter; i < total_pixels; i++) {
    fscanf(file, "%d %d %d %d", &r, &g, &b, &cluster);
    fprintf(fl, "%d %d %d %d\n", r, g, b, cluster);
  }
  fclose(file);
  fclose(fl);
}

void lsb_encrypt() {

  char binarr[128];
  FILE *file2;
  file2 = fopen("example.txt", "r");
  fscanf(file2, "%s", binarr);
  fclose(file2);
  int a = strlen(binarr);
  printf("Length of binary array: %d\n",a);
  int frame_size = 4;
  int noofframes = (int)(strlen(binarr) / frame_size);
  int framepercluster = (int)(noofframes / k);
  int reminder = (int)(noofframes % k);

  for (int i = 0; i < k; i++) {
    char filename[15];
    snprintf(filename, sizeof(filename), "%d.txt", i);
    if (i < (k - 1)) {
      lsb_insert(filename, binarr, framepercluster);
    } else {
      lsb_insert(filename, binarr, (framepercluster + reminder));
    }
  }

  FILE *file;
  file = fopen("clusters_lsb.txt", "r");
  struct Pixel_LSB pixels[width * height];

  for (int i = 0; i < width * height; i++)
    fscanf(file, "%d %d %d %d", &pixels[i].r, &pixels[i].g, &pixels[i].b,
           &pixels[i].cluster);

  unsigned char *images[k];

  for (int i = 0; i < k; i++)
    images[i] = malloc(3 * (int)sqrt(clustercount[i]) *
                       (int)sqrt(clustercount[i]) * sizeof(uint8_t));

  for (int c = 0; c < k; c++) {
    int counter = 0;
    for (int i = 0; i < width * height; i++) {
      if (pixels[i].cluster == c && counter < 3 * (int)sqrt(clustercount[c]) *
                                                  (int)sqrt(clustercount[c])) {
        *(images[c] + counter++) = (uint8_t)pixels[i].r;
        *(images[c] + counter++) = (uint8_t)pixels[i].g;
        *(images[c] + counter++) = (uint8_t)pixels[i].b;
      }
    }
  }

  int nochannels = 3;

  char name[40];
  for (int c = 0; c < k; c++) {
    sprintf(name, "../LSB decryption/Cluster%d lsb.png", c);
    stbi_write_png(name, (int)sqrt(clustercount[c]), (int)sqrt(clustercount[c]),
                   nochannels, images[c],
                   (int)sqrt(clustercount[c]) * nochannels);
    printf("New Cluster%d.png saved\n",c);
  }
  printf("LSB encryption done.");
}

int main() {
  kmeans();
  sort();
  lsb_encrypt();
}
