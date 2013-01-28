/*
  player.cpp
  A simple program to demonstrate playing raw video using SDL.  Assume that you have
  a raw video clip named "sample_video.raw" in the directory and assume that the image
  size is 320 x 240 and is 24 bits/pixel.
  compile: g++ -o player player.cpp -I/usr/include -L/usr/local/lib -lSDL
  execute: ./player
  see 'http://www.webkinesia.com/games/index.php'
*/
#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main()
{   
  SDL_Surface *screen;
  FILE *fp;
  char *buf;
  int n;

  //hard-code file name and parameters just for demo purposes
  if ( ( fp = fopen ( "../videos/sample_video.raw", "rb" ) ) == NULL ) {
    printf("\nError opening file \n");
    exit ( 0 );
  }

  //allocate memory to hold one frame of data
  n = 320 * 240 * 3;
  buf = ( char *) malloc ( n );
  assert ( buf );			//make sure memory has been allocated successfully
  //initialize video system
  if ( SDL_Init( SDL_INIT_VIDEO ) < 0 ) {
        fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
  }
  //ensure SDL_Quit is called when the program exits
  atexit(SDL_Quit);
   
  //set video mode of 320 x 240 with 24-bit pixels
  screen = SDL_SetVideoMode( 320, 240, 24, SDL_SWSURFACE);
  if ( screen == NULL ) {
        fprintf(stderr, "Unable to set 320x240 video: %s\n", SDL_GetError());
        exit(1);
  }

  screen->pixels = buf;	//point framebuffer to data buffer
  while ( 1 ) {
    //read data ( one frame of n bytes ) into buffer
    if ( fread ( buf, 1, n, fp )  <= 0 )
	break;
    SDL_Delay ( 50 );				//50 ms per frame
    SDL_UpdateRect ( screen, 0, 0, 0, 0 );	//blit data to screen
  }  
  fclose ( fp );
  SDL_Delay ( 2000 );	//delay 2 seconds before exit
  printf("Playing video successful!\n");
  return 0;
}
 
