/*
  Source code for blog post http://thesourcecodenotes.blogspot.co.uk/2015/09/imaage-processing-in-c-enbossing-filter.html
  The code is an implementation of an embossing filter in c for PPM images.
  by Antonio Molinaro, 2015.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned char r,g,b;
}Pixel;

typedef struct {
	unsigned int width, height;
    Pixel * data;
}Image;

unsigned int getImgWidth(const Image * i){ return i->width; }

unsigned int getImgHeight(const Image * i){ return i->height; }

void setPixel(int x, int y, Image ** data, const Pixel data_new)
{
  Pixel * p = (*data)->data;
  p[x + y * getImgWidth(*data)].r = data_new.r;
  p[x + y * getImgWidth(*data)].g = data_new.g;
  p[x + y * getImgWidth(*data)].b = data_new.b;

}

Pixel  getPixel(Image * i, int x, int y){ return i->data[getImgWidth(i)*y + x]; }

Image * new_image(int width, int height)
{
	Image * i = malloc(sizeof(Image));

	if(!i) {
		fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
	}
	i->width = width;
	i->height = height;

	i->data = malloc(sizeof(Pixel)*width*height);

	if(!i->data) {
		fprintf(stderr, "Unable to allocate memory\n");
        exit(1);
	}
	return i;
}

void destroy_image(Image ** i)
{
	free(*i);
	free((*i)->data);
}

#define RGB 255

Image * read_PPM(const char *filename)
{
     char buff[16];
     Image * img;
     int ch;
     int rgb;

     //open and read image file
     FILE * fp = fopen(filename, "rb");
     if (!fp) {
          fprintf(stderr, "Unable to open file '%s'\n", filename);
          exit(1);
     }

     //read image format
     if (!fgets(buff, sizeof(buff), fp)) {
          perror(filename);
          fclose(fp);
          exit(1);
     }

    //parse magic number
    if (buff[0] != 'P' || buff[1] != '6') {
         fprintf(stderr, "Invalid image format (must be 'P6')\n");
         fclose(fp);
         exit(1);
    }

    //parse comment
    ch = getc(fp);
    while (ch == '#') {
    	while (getc(fp) != '\n')
        	 ch = getc(fp);
    }
	ungetc(ch, fp);

    int width, height;
    //parse width and height
    if (fscanf(fp, "%d %d", & width, & height) != 2) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
         exit(1);
    }

    //parse rgb depth
    if (fscanf(fp, "%d", &rgb) != 1) {
         fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
         fclose(fp);
         exit(1);
    }

    //check max rgb value correctness
    if (rgb !=  RGB) {
         fprintf(stderr, "'%s' is not a 24-bits image.\n", filename);
         fclose(fp);
         exit(1);
    }


	//create new image
    img = new_image(width, height);

    int bytes;
    //read image data from file and store inside pixel array
    if ((bytes=fread(img->data, 3 * img->width, img->height, fp)) != img->height) {
         fprintf(stderr, "Error loading image '%s'\n", filename);
         exit(1);
    }

    fclose(fp);
    return img;
}

void write_PPM(const char * filename, Image * img)
{
    unsigned int w =  getImgWidth(img);
    unsigned int h =  getImgHeight(img);

    FILE * fp = fopen(filename, "wb");

    if (!fp) {
          fprintf(stderr, "Unable to open file '%s'\n", filename);
          exit(1);
    }

    fprintf(fp, "P6\n %d\n %d\n 255\n", w, h);

	fwrite(img->data, 3, getImgWidth(img) * getImgHeight(img), fp);

    fclose(fp);
}

#define max(a,b) ((a>b)?(a):(b))

Image * emboss_image(Image * img)
{
    unsigned int w = getImgWidth(img);
    unsigned int h = getImgHeight(img);
    Image * emb_img = new_image(w, h);
    Pixel diff;
    Pixel upleft;
    Pixel curr;
    char maxDiff, tmp = 0;
    unsigned char v;
    int x, y;
    for(y=1; y<h; y++)
    {
        for(x=1; x<w; x++)
        {

        		 if(maxDiff > tmp)
        		 	 tmp = maxDiff;
        		 else
        		 	 maxDiff = tmp;


        		 upleft = getPixel(img, x-1,y-1);
        		 curr = getPixel(img,x,y);
        		 diff.r = curr.r - upleft.r;
         		 diff.g = curr.g - upleft.g;
        		 diff.b = curr.b - upleft.b;
        		 if((diff.r==diff.g && diff.r > diff.b)||(diff.r==diff.b && diff.r > diff.g))
        		 		maxDiff = diff.r;
        		 else
        		  	maxDiff = max(diff.r,max(diff.g,diff.b));

        		 v = 128 + maxDiff;
        		 if(v<0)v=0;
        		 if(v>255)v=255;

        		 Pixel val2 = {v,v,v};
        		 setPixel(x,y,&emb_img,val2);
        }
    }
	return emb_img;
}


int main() {

	char filename[80] = "hiroshima6.ppm";

	//read image file
	Image * img = read_PPM(filename);

	//apply emboss filter
	Image * result = emboss_image(img);

	//write processed image to file
	write_PPM("result.ppm", result);

	destroy_image(&img);
	destroy_image(&result);

	return 0;
}