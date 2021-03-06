
// gcc xdisplayraw.c -o xdisplayraw -lX11
// ./xdisplayraw lena512c.raw 512 512

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define EventMask (KeyPressMask | ExposureMask)

struct Image {
     uint32_t red, green, blue;
     };
     
int main(int argc, char ** argv) {
  FILE                  * fp;
  struct Image          * image;
  Display               * display;
  XImage                * ximage;
  Window                window;
  XEvent                an_event;
  GC                    gc;
  Visual                * visual;
  XGCValues             values;
  int                   m, n, FOREGROUNDP = 0, BACKGROUNDP = 1, screen, dplanes;
  int                   IMAGE_HEIGHT, IMAGE_WIDTH;
  
  IMAGE_WIDTH = (int) atoi(argv[2]);
  IMAGE_HEIGHT = (int) atoi(argv[3]);

  if ((fp=fopen(argv[1],"rb"))==NULL) exit(1);
  else {
    image = (struct Image *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image));
    if (!image) exit(1);
    fread(image,IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image),1,fp);
    fclose(fp);
    }  

  if ((display = XOpenDisplay(NULL)) == NULL) {
     printf("Incapaz de conectar ao display...\n");
     exit(1);
     }
    
  screen = DefaultScreen(display);
  dplanes = DisplayPlanes(display,screen);   
  visual = XDefaultVisual(display,screen);
     
  FOREGROUNDP=BlackPixel(display,screen);
  BACKGROUNDP=WhitePixel(display,screen);
  
  if (!(window=XCreateSimpleWindow(display,RootWindow(display,screen),0,0,IMAGE_WIDTH,IMAGE_HEIGHT,1,FOREGROUNDP,BACKGROUNDP))) {
     printf("Não foi possível criar a janela...\n");
     exit(1);
     }

  XSelectInput(display, window, EventMask);
  gc = XCreateGC(display, window, 0, &values);
  
  XMapWindow(display,window);
  XSync(display,False);
  
  ximage = XCreateImage(display, visual, dplanes, ZPixmap, 0, malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(int32_t)), IMAGE_WIDTH, IMAGE_HEIGHT, 8, 0);

  for(m=0;m<IMAGE_HEIGHT;m++) {
    for(n=0;n<IMAGE_WIDTH;n++) {
      ximage -> data[(m*4)*IMAGE_WIDTH+n*4]   = (uint8_t) image[m*IMAGE_WIDTH+n].blue;
      ximage -> data[(m*4)*IMAGE_WIDTH+n*4+1] = (uint8_t) image[m*IMAGE_WIDTH+n].green;
      ximage -> data[(m*4)*IMAGE_WIDTH+n*4+2] = (uint8_t) image[m*IMAGE_WIDTH+n].red;
      ximage -> data[(m*4)*IMAGE_WIDTH+n*4+3] = (uint8_t) 0;
      }
    }

  XPutImage(display, window, gc, ximage, 0, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
  
  XFlush(display);

  sleep(10);
  
  return 0;
  }
