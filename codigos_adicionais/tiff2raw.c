// gcc tiff2raw.c -o tiff2raw -ltiff
// ./tiff2raw lena512c.tif lena512c.raw

#include <stdio.h>
#include <stdlib.h>
#include <tiffio.h>

struct Image {
	unsigned int red, green, blue;
};
     
int IMAGE_HEIGHT, IMAGE_WIDTH;

struct Image * ReadTiffImage(char * name);

int main(int argc, char ** argv) {
	FILE * fp;
	struct Image * image;

	image = ReadTiffImage(argv[1]);

	if ((fp = fopen(argv[2],"wb"))==NULL) {
		exit(1);
	} else {
		fwrite(image,IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image),1,fp);
		fclose(fp);
	}

	return 0;
}

struct Image * ReadTiffImage(char * name) {
	TIFF * tif;
	uint32 * raster;
	struct Image * t1, * t2;
	register int i, j, k;

	tif = TIFFOpen(name,"r");

	if (tif) {
		TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&IMAGE_WIDTH);
		TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&IMAGE_HEIGHT);
		t2 = (struct Image *) malloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(struct Image));
		if (!t2) exit(1);
		raster = (uint32 *) _TIFFmalloc(IMAGE_WIDTH*IMAGE_HEIGHT*sizeof(uint32));
		if (raster != NULL) {
			if (TIFFReadRGBAImage(tif,IMAGE_WIDTH,IMAGE_HEIGHT,raster,0)) {
				k = 0;
				for (i = IMAGE_HEIGHT - 1; i >= 0; i--) {
					for (j = 0; j < IMAGE_WIDTH; j++) {
						t2[i*IMAGE_WIDTH+j].red = (unsigned int) TIFFGetR(raster[k]);
						t2[i*IMAGE_WIDTH+j].green = (unsigned int) TIFFGetG(raster[k]);
						t2[i*IMAGE_WIDTH+j].blue = (unsigned int) TIFFGetB(raster[k]);	  
						k++;
					}
				}
			} else {
				exit(1);
			}
			_TIFFfree(raster);
		} else {
			exit(1);
		}
		t1 = t2;
	} else {
		exit(1);
	}

	TIFFClose(tif);

	return t1;
}
