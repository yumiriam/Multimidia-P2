// gcc convert.c -o convert -lm
// ./convert sample_video.raw sample_video2.raw 320 240 50

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct ImageChar {
     uint8_t red, green, blue;
     };
     
struct ImageInt {
     uint32_t red, green, blue;
     };   
     
int main(int argc, char ** argv) {
  FILE                  * fpin, * fpout;
  struct ImageChar      * frame;
  struct ImageInt       * image;
  int                    m, NFRAMES, IMAGE_HEIGHT, IMAGE_WIDTH;
  
  IMAGE_WIDTH = (int) atoi(argv[3]);
  IMAGE_HEIGHT = (int) atoi(argv[4]);
  NFRAMES = (int) atoi(argv[5]);

  if ((fpin=fopen(argv[1],"rb"))==NULL) exit(1);
  if ((fpout=fopen(argv[2],"wb"))==NULL) exit(1);
  
  frame = (struct ImageChar *) malloc(NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ImageChar));
  if (!frame) exit(1);
  image = (struct ImageInt *) malloc(NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ImageInt));
  if (!image) exit(1);
 
  while (1) {
    if (fread(frame,NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ImageChar),1,fpin)<=0) break;
    for(m=0;m<NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT;m++) {
	image[m].red  = frame[m].red;
	image[m].green = frame[m].green;
	image[m].blue = frame[m].blue;
      }
    fwrite(image,NFRAMES*IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct ImageInt),1,fpout);
    }
    
  fclose(fpin);
  fclose(fpout);
  
  return 0;
  }
