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

void ChangeLightness_gamma(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height)
{
    double gammaCorrection = 1.0 / 2.0;
    double R = 0, G = 0, B = 0;

    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            R = pow(255 * (Red[j][i] / 255), gammaCorrection);
            G = pow(255 * (Green[j][i] / 255), gammaCorrection);
            B = pow(255 * (Blue[j][i] / 255), gammaCorrection);

            Red2[j][i] = (unsigned char)R;
            Green2[j][i] = (unsigned char)G;
            Blue2[j][i] = (unsigned char)B;
        }
    }
}

void DisplayHistogram(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], long Width, long Height, long Interval)
{

        static long Histogram[MAX_HISTOGRAM];
        double Hue = 0.0, Saturation = 0.0, Lightness = 0.0;
        long i, j;
        long Index = 0;
        long Total = 0;

        //Initialize histogram array
        for( i = 0; i < Interval; i++) {
            Histogram[i] = 0;
        }

        for ( j = 0; j < Height; j++) {
            for ( i = 0; i < Width; i++) {

                //Change RGB to HSL
                RGBtoHSL(Red[j][i], Green[j][i], Blue[j][i], &Hue, &Saturation, &Lightness);

                //Histogram for Lightness
                Index = (long)(Interval * Lightness);

                if (Index >= Interval) {
                    Index = Interval - 1;
                }

                Histogram[Index]++;
                Total++;
            }
        }

        //print histogram
        printf("=====================\n");
        for(i = 0; i < Interval; i++)
        {
            printf("%.3f %ld\n", (i * 1.0 / Interval),Histogram[i]);
        }
        printf("=====================\n");
}

void BinarizeImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, double Gamma, double Threshold)
{
        long i, j;
        double Hue = 0.0, Saturation = 0.0, Lightness = 0.0;
        unsigned char R = 0, G =0, B = 0;
        double threshold = 0.5;       //しきい値

        for (j = 0; j < Height; j++) {
            for (i = 0; i < Width; i++) {

                RGBtoHSL (Red[j][i], Green[j][i], Blue[j][i], &Hue, &Saturation, &Lightness);

                //Binarize pixel
                if (Lightness < Threshold){
                    Lightness = 0.0;
                }else {
                    Lightness = 1.0;
                }

                HSLtoRGB (Hue, Saturation, Lightness, &R, &G, &B);

                Red2[j][i] = R;
                Green2[j][i] = G;
                Blue2[j][i] = B;
            }
        }
}

void PosterizeImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, double Gamma, long Level)
{
        long i, j, k;
        double Hue = 0.0, Saturation = 0.0, Lightness = 0.0;
        unsigned char R = 0, G =0, B = 0;

        for (j = 0; j < Height; j++) {
            for (i = 0; i < Width; i++) {

                RGBtoHSL (Red[j][i], Green[j][i], Blue[j][i], &Hue, &Saturation, &Lightness);

                //Posterize pixel
                for (k = 0.0; k < Level; k++) {

                    if(Lightness >= k / Level && Lightness < (k+1.0) / Level) {
                        Lightness = k / (Level - 1.0);
                        break;
                    }else{
                        continue;
                    }
                }

                HSLtoRGB (Hue, Saturation, Lightness, &R, &G, &B);

                Red2[j][i] = R;
                Green2[j][i] = G;
                Blue2[j][i] = B;
            }
        }
}

void AlphablendImage (unsigned char (*Red1)[MAXS], unsigned char (*Green1)[MAXS], unsigned char (*Blue1)[MAXS],unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], unsigned char (*alphablend_Red)[MAXS], unsigned char (*alphablend_Green)[MAXS], unsigned char (*alphablend_Blue)[MAXS], long Width, long Height, double Alpha)
{
    long i, j;
    unsigned char R = 0, G = 0, B = 0;

    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++) {

            R = Alpha * Red1[j][i] + (1.0 - Alpha) * (Red2[j][i]);
            G = Alpha * Green1[j][i] + (1.0 - Alpha) * (Green2[j][i]);
            B = Alpha * Blue1[j][i] + (1.0 - Alpha) * (Blue2[j][i]);

            Red2[j][i] = R;
            Green2[j][i] = G;
            Blue2[j][i] = B;

        }
    }
}

void EmbossImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, long Alpha)
{
    long i, j;
    unsigned char R = 0, G = 0, B = 0;
    double Hue = 0.0, Saturation = 0.0, Lightness = 0.0;
    double Hue2 = 0.0, Saturation2 = 0.0, Lightness2 = 0.0;

    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++) {

            RGBtoHSL (Red[j][i], Green[j][i], Blue[j][i], &Hue, &Saturation, &Lightness);
            RGBtoHSL (Red[j + Alpha][i + Alpha], Green[j + Alpha][i + Alpha], Blue[j + Alpha][i + Alpha], &Hue2, &Saturation2, &Lightness2);

            Hue = 0.0;
            Saturation = 0.0;

            Lightness = (Lightness - Lightness2 + 1.0) / 2.0;

            HSLtoRGB (Hue, Saturation, Lightness, &R, &G, &B);

            Red2[j][i] = R;
            Green2[j][i] = G;
            Blue2[j][i] = B;
        }
    }

}

void BlurImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, long FilterSize)
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

            Red2[i][j] = round(redSum / (float)squareSum);
            Green2[i][j] = round(greenSum / (float)squareSum);
            Blue2[i][j] = round(blueSum / (float)squareSum);
        }
    }
}

//Edge Detection using Sobel Filtering
void DifferentiateImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, long FilterSize)
{
    long i, j, row, col;
    unsigned char R = 0, G = 0, B = 0;
    static long mx[3][3] = {
        {0, 0, 0},
        {0, -1, 1},
        {0, 0, 0}
    };
    static long my[3][3] = {
        {0, 0, 0},
        {0, -1, 0},
        {0, 1, 0}
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

            unsigned char Max = R;
            if (Max < G) Max = G;
            if (Max < B) Max = B;

            if (Max > 255) {
                Max = 255;
            }

            Red2[j][i] = Max;
            Green2[j][i] = Max;
            Blue2[j][i] = Max;

        }
    }
}

void LaplacianImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, long Threshold)
{
    long i, j, row, col;
    unsigned char R = 0, G = 0, B = 0;
    static long filter[3][3] = {
        {0, 1, 0},
        {1, -4, 1},
        {0, 1, 0}
    };

    for (j = 0; j < Height; j++) {
        for (i = 0; i < Width; i++) {

            unsigned char sumRed = 0, sumGreen = 0, sumBlue = 0;

            //Convolution
            for ( row = -1; row < 2; row++) {
                for ( col = -1; col < 2; col++) {
                    if (j + row > Height || i + col > Width)
                    {
                        continue;
                    }
                    sumRed += Red[j + row][i + col] * filter[row + 1][col + 1];
                    sumGreen += Green[j + row][i + col] * filter[row + 1][col + 1];
                    sumBlue += Blue[j + row][i + col] * filter[row + 1][col + 1];
                }
            }
            //Obtain Laplacian pixel
            Red2[j][i] = sumRed;
            Green2[j][i] = sumGreen;
            Blue2[j][i] = sumBlue;

            if(j + 1 > Height || i + 1 > Width) {
                continue;
            }

            //Detect zero intersections
            if (
            (Red[j][i] * Red2[j + 1][i] < 0.0 || Red[j][i] - Red[j + 1][i] > Threshold) ||
            (Green[j][i] * Green2[j + 1][i] < 0.0 || Green[j][i] - Green[j + 1][i] > Threshold) ||
            (Blue[j][i] * Blue2[j + 1][i] < 0.0 || Blue[j][i] - Blue[j + 1][i] > Threshold) )
            {
                Red2[j][i] = 255;
                Green2[j][i] = 255;
                Blue2[j][i] = 255;
            }
            else if (
            (Red[j][i] * Red2[j][i + 1] < 0.0 || Red[j][i] - Red[j][i + 1] > Threshold) ||
            (Green[j][i] * Green2[j][i + 1] < 0.0 || Green[j][i] - Green[j][i + 1] > Threshold) ||
            (Blue[j][i] * Blue2[j][i + 1] < 0.0 || Blue[j][i] - Blue[j][i + 1] > Threshold) )
            {
                Red2[j][i] = 255;
                Green2[j][i] = 255;
                Blue2[j][i] = 255;
            }
            else {
                Red2[j][i] = 0;
                Green2[j][i] = 0;
                Blue2[j][i] = 0;
            }
        }
    }
}

void SharpenImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, long FilterSize, long K)
{
    long i,j, row, col;
    unsigned char R = 0, G = 0, B = 0;
    float average[3][3] = {
        {-1, -1, -1},
        {-1, 8, -1},
        {-1, -1, -1}
    };
    long ori[3][3] = {
        {0, 0, 0},
        {0, 1, 0},
        {0, 0, 0}
    };

    //Create unsharp masking filter
    for (i = 0; i < FilterSize; i++){
        for (j = 0; j < FilterSize; j++) {
            average[i][j] /= 9;
            average[i][j] *= K;
            average[i][j] += ori[i][j];
            //printf("%.04f\n", average[i][j]);
        }
    }

    for (j = 0 ; j < Height ; j++)
    {
        for (i = 0 ; i < Width ; i++)
        {
            long sumRed = 0, sumGreen = 0, sumBlue = 0, squareSum = 0;

            for ( row = -1; row < FilterSize-1; row++) {
                for ( col = -1; col < FilterSize-1; col++) {

                    sumRed += Red[j + row][i + col] * average[row + 1][col + 1];
                    sumGreen += Green[j + row][i + col]* average[row + 1][col + 1];
                    sumBlue += Blue[j + row][i + col]* average[row + 1][col + 1];
                }
            }

            if (sumRed < 0) sumRed = 0;
            if (sumGreen < 0) sumGreen = 0;
            if (sumBlue < 0) sumBlue = 0;
            if (sumRed > 255) sumRed = 255;
            if (sumGreen > 255) sumGreen = 255;
            if (sumBlue > 255) sumBlue = 255;

            Red2[j][i] = (unsigned char)sumRed;
            Green2[j][i] = (unsigned char)sumGreen;
            Blue2[j][i] = (unsigned char)sumBlue;

        }
    }
}

int main(void)
{
    static unsigned char Red[MAXS][MAXS];
    static unsigned char Green[MAXS][MAXS];
    static unsigned char Blue[MAXS][MAXS];

    long Width = 0, Height = 0;

    //Sharpen Image
    static unsigned char Red2[MAXS][MAXS];
    static unsigned char Green2[MAXS][MAXS];
    static unsigned char Blue2[MAXS][MAXS];

    ReadImage("photo_yacht_std.ppm", Red, Green, Blue, &Width, &Height);
    SharpenImage(Red, Green, Blue, Red2, Green2, Blue2, Width, Height, 3.0, 10.0);
    WriteImage("photo_sharp.ppm", Red2, Green2, Blue2, Width, Height);

    return 0;
}