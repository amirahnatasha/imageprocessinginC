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

void ChangeLightness_gamma(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*gamma_Red)[MAXS], unsigned char (*gamma_Green)[MAXS], unsigned char (*gamma_Blue)[MAXS], long Width, long Height)
{
    double gammaCorrection = 1.0 / 2.0;
    double R = 0, G = 0, B = 0;

    for (int j = 0; j < Height; j++) {
        for (int i = 0; i < Width; i++) {
            R = pow(255 * (Red[j][i] / 255), gammaCorrection);
            G = pow(255 * (Green[j][i] / 255), gammaCorrection);
            B = pow(255 * (Blue[j][i] / 255), gammaCorrection);

            gamma_Red[j][i] = (unsigned char)R;
            gamma_Green[j][i] = (unsigned char)G;
            gamma_Blue[j][i] = (unsigned char)B;
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

void BinarizeImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*binarize_Red)[MAXS], unsigned char (*binarize_Green)[MAXS], unsigned char (*binarize_Blue)[MAXS], long Width, long Height, double Gamma, double Threshold)
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

                binarize_Red[j][i] = R;
                binarize_Green[j][i] = G;
                binarize_Blue[j][i] = B;
            }
        }
}

void PosterizeImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS],unsigned char (*posterize_Red)[MAXS], unsigned char (*posterize_Green)[MAXS], unsigned char (*posterize_Blue)[MAXS], long Width, long Height, double Gamma, long Level)
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

                posterize_Red[j][i] = R;
                posterize_Green[j][i] = G;
                posterize_Blue[j][i] = B;
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

            alphablend_Red[j][i] = R;
            alphablend_Green[j][i] = G;
            alphablend_Blue[j][i] = B;

        }
    }
}

void EmbossImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*emboss_Red)[MAXS], unsigned char (*emboss_Green)[MAXS], unsigned char (*emboss_Blue)[MAXS], long Width, long Height, long Alpha)
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

            emboss_Red[j][i] = R;
            emboss_Green[j][i] = G;
            emboss_Blue[j][i] = B;
        }
    }

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
void DifferentiateImage (unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*diff_Red)[MAXS], unsigned char (*diff_Green)[MAXS], unsigned char (*diff_Blue)[MAXS], long Width, long Height, long FilterSize)
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

            diff_Red[j][i] = Max;
            diff_Green[j][i] = Max;
            diff_Blue[j][i] = Max;

        }
    }
}

void LaplacianImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*lap_Red)[MAXS], unsigned char (*lap_Green)[MAXS], unsigned char (*lap_Blue)[MAXS], long Width, long Height, long Threshold)
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
            lap_Red[j][i] = sumRed;
            lap_Green[j][i] = sumGreen;
            lap_Blue[j][i] = sumBlue;

            if(j + 1 > Height || i + 1 > Width) {
                continue;
            }

            //Detect zero intersections
            if (
            (Red[j][i] * lap_Red[j + 1][i] < 0.0 || Red[j][i] - Red[j + 1][i] > Threshold) ||
            (Green[j][i] * lap_Green[j + 1][i] < 0.0 || Green[j][i] - Green[j + 1][i] > Threshold) ||
            (Blue[j][i] * lap_Blue[j + 1][i] < 0.0 || Blue[j][i] - Blue[j + 1][i] > Threshold) )
            {
                lap_Red[j][i] = 255;
                lap_Green[j][i] = 255;
                lap_Blue[j][i] = 255;
            }
            else if (
            (Red[j][i] * lap_Red[j][i + 1] < 0.0 || Red[j][i] - Red[j][i + 1] > Threshold) ||
            (Green[j][i] * lap_Green[j][i + 1] < 0.0 || Green[j][i] - Green[j][i + 1] > Threshold) ||
            (Blue[j][i] * lap_Blue[j][i + 1] < 0.0 || Blue[j][i] - Blue[j][i + 1] > Threshold) )
            {
                lap_Red[j][i] = 255;
                lap_Green[j][i] = 255;
                lap_Blue[j][i] = 255;
            }
            else {
                lap_Red[j][i] = 0;
                lap_Green[j][i] = 0;
                lap_Blue[j][i] = 0;
            }
        }
    }
}

int main(void)
{
    static unsigned char Red[MAXS][MAXS];
    static unsigned char Green[MAXS][MAXS];
    static unsigned char Blue[MAXS][MAXS];

    long Width = 0, Height = 0;

    //Differentiate Image
    static unsigned char diff_Red[MAXS][MAXS];
    static unsigned char diff_Green[MAXS][MAXS];
    static unsigned char diff_Blue[MAXS][MAXS];

    DifferentiateImage(Red, Green, Blue, diff_Red, diff_Green, diff_Blue, Width, Height, 3.0);
    WriteImage("photo_diff.ppm", diff_Red, diff_Green, diff_Blue, Width, Height);

    //Laplacian filter
    static unsigned char lap_Red[MAXS][MAXS];
    static unsigned char lap_Green[MAXS][MAXS];
    static unsigned char lap_Blue[MAXS][MAXS];

    ReadImage("photo_yacht_std.ppm", Red, Green, Blue, &Width, &Height);
    LaplacianImage(Red, Green, Blue, lap_Red, lap_Green, lap_Blue, Width, Height, 20.0);
    WriteImage("photo_laplacian.ppm", lap_Red, lap_Green, lap_Blue, Width, Height);

    return 0;
}