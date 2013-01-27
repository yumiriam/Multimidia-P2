/*
  avi_to_raw.cpp
  Need to compile together with MPEG4IP avilib.c
  Compile: g++ -o avi_to_raw avi_to_raw.cpp avilib.c
  Execute: ./avi_to_raw infile [video_output_file audio_output_file]

  http://www.webkinesia.com/games/index.php
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "avilib.h"

using namespace std;

int main( int argc, char *argv[] )
{
  FILE *fpo = NULL;
  avi_t *avi = NULL;		//points to opened avi file
  char infilename[30], outfilename[30], *pc;
  double videoFrameRate;
  unsigned  numVideoFrames; 
  unsigned long fileDuration;
  long numBytes = 0, totalBytes = 0;
  unsigned long videoFramesRead = 0;
  unsigned long emptyFramesRead = 0;
  unsigned long short_frames_len = 4;

  if ( argc < 2 ) {
    printf("\nUsage: avi_to_raw infile [outfile1 outfile2]\n");
    exit ( 1 );
  } 
  strcpy ( infilename, argv[1] );

  avi = AVI_open_input_file( infilename, 1 );
  if (avi == NULL) {
    fprintf(stderr, "error %s: %s\n", infilename, AVI_strerror());
    exit( 2 );
  }
 
  videoFrameRate = AVI_video_frame_rate( avi );	//frame per second
  numVideoFrames = AVI_video_frames( avi );
  fileDuration = ( unsigned long ) ceil( numVideoFrames / videoFrameRate);

  printf("\n---------------------------------------------------------");
  printf("\nVideo Frame Rate:\t%5.0f \tfps", videoFrameRate );
  printf("\nNumber of Frames:\t%5d", numVideoFrames );
  printf("\nFile Duration:   \t%5ld \tsec", fileDuration );
  printf("\nVideo width:     \t%5d \tpixels", avi->width );
  printf("\nVideo height:     \t%5d \tpixels", avi->height );
  printf("\nCompressor type: \t%5s", avi->compressor );
  printf("\nAudio channels:  \t%5d", avi->a_chans );
  printf("\nAudio sample rate:\t%5d \tHz", avi->a_rate  );
  printf("\nBits per audio sample:\t%5d", avi->a_bits );

  printf("\nVideo starts at:  \t%5ld ( 0x%lx )", avi->movi_start, avi->movi_start );
  printf("\n---------------------------------------------------------\n");
 
  //extract video 
  if ( argc < 3 ) {				//construct raw video output file name
    strcpy ( outfilename, infilename );
    pc = strrchr ( outfilename, '.' );          //pc points to '.'
    if ( pc != NULL ) *pc = 0;
    strcat ( outfilename, ".rawv" );             //extenstion .rawv
  } else
    strcpy ( outfilename, argv[2] ); 
  if ( ( fpo = fopen ( outfilename, "wb" ) ) == NULL ) {
    fprintf(stderr, "error opening file %s for output\n", outfilename );
    exit ( 3 );
  }
  // get a buffer large enough to handle a frame of raw SDTV 
  u_char *buf = ( u_char * ) malloc ( 768 * 576 * 4 );
  if ( buf == NULL ) {
    fprintf(stderr, "Error allocating memory\n");
    exit ( 4 );
  }
  //move file pointer pointing to beginning of video data
  if (AVI_seek_start( avi )) {
     fprintf(stderr, "bad seek 0: %s\n", AVI_strerror());
     exit( 5 );
  }
  if (AVI_set_video_position( avi, 0, NULL)) {
     fprintf(stderr, "bad seek 1: %s\n", AVI_strerror());
     exit( 6 );
  }

  videoFramesRead = 0;
  while ( ( numBytes = AVI_read_frame( avi, (char *)buf ) ) > -1 ) {
    totalBytes += numBytes;
    videoFramesRead++;
    printf("frame %d - length %u total %u\r", videoFramesRead, numBytes, totalBytes);
    //eliminate short frames
    if ( numBytes > short_frames_len ) {
      if (  fwrite ( buf, 1, numBytes, fpo ) != numBytes ) {
        fprintf ( stderr, "Error writing %s\n", outfilename );    
        break;
      }
    } else {
      emptyFramesRead++;
    }
    if ( videoFramesRead >= numVideoFrames ) break;
  } //while
  // read error
  if (numBytes < 0) 
    fprintf(stderr, "read error\n" );
  if (emptyFramesRead) {
    fprintf(stderr, "warning: %u zero length frames ignored\n", emptyFramesRead);
  }

  fclose ( fpo );			//done with video

  if ( avi->a_chans == 0 ) {
    printf("\nno audio channel\n" );
    exit ( 7 );
  }
  //extract audio
  if ( argc < 4 ) {                             //construct raw audio output file name
    strcpy ( outfilename, infilename );
    pc = strrchr ( outfilename, '.' );          //pc points to '.'
    if ( pc != NULL ) *pc = 0;
    strcat ( outfilename, ".rawa" );       	//extenstion .rawa
  } else
    strcpy ( outfilename, argv[3] );
  if ( ( fpo = fopen ( outfilename, "wb" ) ) == NULL ) {
    fprintf(stderr, "error opening file %s for output\n", outfilename );
    exit ( 9 );
  }

  unsigned audioFrames = 0;
  totalBytes = 0;
  if (AVI_seek_start( avi )) {
    fprintf(stderr, "Audio bad seek 2: %s\n", AVI_strerror());
    exit( 10 );
  }
  if (AVI_set_audio_position( avi, 0)) {
    fprintf(stderr, "Audio bad seek 3: %s\n", AVI_strerror());
    exit( 11 );
  }
  while ((numBytes = AVI_read_audio( avi, (char *)buf, 8192)) > 0) {
    totalBytes += numBytes;
    if (fwrite ( buf, 1, numBytes, fpo ) != numBytes) {
      fprintf(stderr, "error writing %s\n", outfilename );
      break;
    }
    audioFrames++;
    printf("frame %d - length %u total %u\r", audioFrames, numBytes, totalBytes);
  }
 
  printf("\n");
  fclose ( fpo );	

  return 0;
}
 
