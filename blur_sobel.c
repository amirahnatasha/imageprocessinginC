#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAXS (2000)
#define MAX_HISTOGRAM (1000)

void ReadImage (char *pFileName, unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], long *pWidth, long *pHeight)
{
    if ((pFileName != NULL) && (Red != NULL) && (Green != NULL) && (Blue != NULL) && (pWidth!=NULL) && (pHeight!=NULL))
    {
        FILE *pFile = NULL;

        if ((pFile = fopen(pFileName, "r")) != NULL)
        {
            char Line1[200];
            char Line2[200];
            long Maximum = 0;
            long Width = 0, Height = 0;
            long R = 0, G = 0, B = 0;
            long i = 0, j = 0;

            fgets(Line1, sizeof(Line1) - 1, pFile);
            fgets(Line2, sizeof(Line2) - 1, pFile);

            fscanf(pFile, "%ld %ld", &Width, &Height);

            fscanf(pFile, "%ld", &Maximum);

            *pWidth = Width;
            *pHeight = Height;

            for(j = 0; j < Height; j++) {
                for(i = 0; i < Width; i++)
                {
                    fscanf(pFile, "%ld %ld %ld", &R, &G, &B);

                    Red[j][i] = (unsigned char)R;
                    Green[j][i] = (unsigned char)G;
                    Blue[j][i] = (unsigned char)B;
                }
            }

            fclose(pFile);
            pFile = NULL;
        }
        else
        {
            printf("File cannot be opened.\n");
        }
    }
}

void WriteImage(char *pFileName, unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], long Width, long Height)
{
    if((pFileName != NULL) && (Red != NULL) && (Green != NULL) && (Blue != NULL))
    {
        FILE *pFile = NULL;

        if((pFile = fopen(pFileName, "w")) != NULL)
        {
            long i = 0, j = 0;

            fprintf(pFile, "P3 \n");
            fprintf(pFile, "#comment line \n");

            fprintf(pFile, "%ld %ld\n", Width, Height);
            fprintf(pFile, "255\n");

            for(j = 0; j < Height; j++) {
                for(i = 0; i < Width; i++) {
                    fprintf(pFile, "%d %d %d\n", Red[j][i], Green[j][i], Blue[j][i]);
                }
            }

            fclose(pFile);
            pFile = NULL;
        }
        else
        {
            printf("File cannot be opened.\n");
        }
    }
}

void RGBtoHSL(unsigned char Red, unsigned char Green, unsigned char Blue, double *pHue, double *pSaturation, double *pLightness)
{

    double Hue = 0, Saturation = 0, Lightness = 0;
    long Max, Min;

    Max = 0;
    if (Max < Red) Max = Red;
    if (Max < Green) Max = Green;
    if (Max < Blue) Max = Blue;
    Min = 0;
    if (Min > Red) Min = Red;
    if (Min > Green) Min = Green;
    if (Min > Blue) Min = Blue;

    if ( Max == Min)
    {
        Lightness = ((Max + Min) / 2) / 255.0;
        *pLightness = Lightness;

        Saturation = 0.0;
        *pSaturation = Saturation;

        Hue = 0.0;
        *pHue = Hue;
    }
    else
    {
        Lightness = ((Max + Min) / 2) / 255.0;
        *pLightness = Lightness;

        if (Lightness < 0.5)
        {
            Saturation = (Max - Min) / (Max + Min);
        }else
        {
            Saturation = (Max - Min) / (510.0 - Max - Min);
        }

        *pSaturation = Saturation;

        if(Max == Red)
        {
            Hue = 60 * ((Green - Blue) / (Max - Min));
        }
        else if (Max == Green)
        {
            Hue = 60 * ((Blue - Red) / (Max - Min)) + 120.0;
        }
        else
        {
            Hue = 60 * ((Red - Green) / (Max - Min)) + 240.0;
        }

        *pHue = Hue;
    }
}

void HSLtoRGB(double Hue, double Saturation, double Lightness, unsigned char *pRed, unsigned char *pGreen, unsigned char *pBlue)
{
    double Max = 0, Min = 0;
    unsigned char R, G, B;

    if(Lightness < 0.5)
    {
        Max = Lightness * (1 + Saturation) * 255.0;
        Min = Lightness * (1 - Saturation) * 255.0;
    }
    else
    {
        Max = (Lightness + (1-Lightness) * Saturation) * 255.0;
        Min = (Lightness - (1 - Lightness) * Saturation) * 255.0;
    }

    if (Hue >= 0 && Hue < 60)
    {
        R = Max;
        G = Min + (Max - Min) * (Hue - 0) / 60.0;
        B = Min;
    }
    else if (Hue >= 60 && Hue < 120)
    {
        R = Max - (Max - Min) * (Hue - 60) / 60.0;
        G = Max;
        B = Min;
    }
    else if (Hue >= 120 && Hue < 180)
    {
        R = Min;
        G = Max;
        B = Min + (Max - Min) * (Hue - 120) / 60.0;
    }
    else if (Hue >= 180 && Hue < 240)
    {
        R = Min;
        G = Max - (Max - Min) * (Hue - 180) / 60.0;
        B = Max;
    }
    else if (Hue >= 240 && Hue < 300)
    {
        R = Min + (Max - Min) * (Hue - 240) / 60.0;
        G = Min;
        B = Max;
    }
    else
    {
        R = Max;
        G = Min;
        B = Max - (Max - Min) * (Hue - 300) / 60.0;
    }

    *pRed = R;
    *pGreen = G;
    *pBlue = B;
}

void BlurImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*blur_Red)[MAXS], unsigned char (*blur_Green)[MAXS], unsigned char (*blur_Blue)[MAXS], long Width, long Height, long FilterSize)
{
    long i, j, row, col;
    unsigned char R = 0, G = 0, B = 0;
    long radius = (FilterSize - 1.0) / 2.0;

    for (i = 0 ; i < Height ; i++)
    {
        for (j = 0 ; j < Width ; j++)
        {
            int redSum = 0, greenSum = 0, blueSum = 0, squareSum = 0;

            for (row = (i - radius) ; row <= (i + radius) ; row++)
            {
                for (col = (j - radius) ; col <= (j + radius) ; col++)
                {
                    // If trying to get value for non-existent pixel, skip
                    if (row == -1 || row == Height || col == -1 || col == Width)
                    {
                        continue;
                    }
                    redSum += Red[row][col];
                    greenSum += Green[row][col];
                    blueSum += Blue[row][col];

                    squareSum++;
                }
            }

            blur_Red[i][j] = round(redSum / (float)squareSum);
            blur_Green[i][j] = round(greenSum / (float)squareSum);
            blur_Blue[i][j] = round(blueSum / (float)squareSum);
        }
    }
}

//Edge Detection using Sobel Filtering
void DifferentiateImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*diff_Red)[MAXS], unsigned char (*diff_Green)[MAXS], unsigned char (*diff_Blue)[MAXS], long Width, long Height)
{
    long i, j, row, col;
    unsigned char R = 0, G = 0, B = 0, rgbGray = 0;
    static long mx[3][3] = {
        {-1, 0 , 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    static long my[3][3] = {
        {-1, -2, -1},
        {0, 0, 0},
        {1, 2, 1}
    };

    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++) {

            unsigned char xRed = 0, xGreen = 0, xBlue = 0, yRed = 0, yGreen = 0, yBlue = 0;

            //Convolution
            for ( row = -1; row < 2; row++) {
                for ( col = -1; col < 2; col++) {
                    if (j + row > Height || i + col > Width)
                    {
                        continue;
                    }
                    xRed += Red[j + row][i + col] * mx[row + 1][col + 1];
                    xGreen += Green[j + row][i + col] * mx[row + 1][col + 1];
                    xBlue += Blue[j + row][i + col] * mx[row + 1][col + 1];

                    yRed += Red[j + row][i + col] * my[row + 1][col + 1];
                    yGreen += Green[j + row][i + col] * my[row + 1][col + 1];
                    yBlue += Blue[j + row][i + col] * my[row + 1][col + 1];
                }
            }

            R = sqrt(xRed*xRed + yRed*yRed);
            G = sqrt(xGreen*xGreen + yGreen*yGreen);
            B = sqrt(xBlue*xBlue + yBlue*yBlue);

            unsigned char Max = 0;
            if (Max < R) Max = R;
            if (Max < G) Max = G;
            if (Max < B) Max = B;

            if (Max > 200) {
                Max = 255;
            }else {
                Max = 0;
            }

            diff_Red[j][i] = Max;
            diff_Green[j][i] = Max;
            diff_Blue[j][i] = Max;
        }
    }
}

int main(void)
{
    static unsigned char Red[MAXS][MAXS];
    static unsigned char Green[MAXS][MAXS];
    static unsigned char Blue[MAXS][MAXS];

    long Width = 0, Height = 0;

    //Blur Image
    static unsigned char blur_Red[MAXS][MAXS];
    static unsigned char blur_Green[MAXS][MAXS];
    static unsigned char blur_Blue[MAXS][MAXS];

    ReadImage("photo_yacht_std.ppm", Red, Green, Blue, &Width, &Height);
    BlurImage(Red, Green, Blue, blur_Red, blur_Green, blur_Blue, Width, Height, 5.0);
    WriteImage("photo_blur.ppm", blur_Red, blur_Green, blur_Blue, Width, Height);

    //Differentiate Image
    static unsigned char diff_Red[MAXS][MAXS];
    static unsigned char diff_Green[MAXS][MAXS];
    static unsigned char diff_Blue[MAXS][MAXS];

    DifferentiateImage(Red, Green, Blue, diff_Red, diff_Green, diff_Blue, Width, Height);
    WriteImage("photo_diff.ppm", diff_Red, diff_Green, diff_Blue, Width, Height);

    return 0;
}
