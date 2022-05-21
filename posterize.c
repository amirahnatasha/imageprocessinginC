#include <stdio.h>
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

int max(long num1, long num2)
{
    return (num1 > num2 ) ? num1 : num2;
}

int min(long num1, long num2)
{
    return (num1 > num2 ) ? num2 : num1;
}

void RGBtoHSL(unsigned char Red, unsigned char Green, unsigned char Blue, double *pHue, double *pSaturation, double *pLightness)
{

    double Hue = 0, Saturation = 0, Lightness = 0;
    long Max, Min;

    Max = max(max(Red, Green), Blue);
    Min = min(min(Red, Green), Blue);

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

                Lightness = pow(Lightness, 1.0 / Gamma);

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

                Lightness = pow(Lightness, 1.0 / Gamma);

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

int main(void)
{
    static unsigned char Red[MAXS][MAXS];
    static unsigned char Green[MAXS][MAXS];
    static unsigned char Blue[MAXS][MAXS];

    long Width = 0, Height = 0;

    ReadImage("photo_yacht_std.ppm", Red, Green, Blue, &Width, &Height);
    //DisplayHistogram(Red, Green, Blue, Width, Height, 20);

    static unsigned char binarize_Red[MAXS][MAXS];
    static unsigned char binarize_Green[MAXS][MAXS];
    static unsigned char binarize_Blue[MAXS][MAXS];

    //Gamma Correction
    //ChangeLightness_gamma(Red, Green, Blue,gamma_Red, gamma_Green, gamma_Blue, Width, Height);
    //WriteImage("photo_correction.ppm", Red, Green, Blue, Width, Height);

    //Binarize Image
    BinarizeImage(Red, Green, Blue, binarize_Red, binarize_Green, binarize_Blue, Width, Height, 3.5, 0.5);
    DisplayHistogram(binarize_Red, binarize_Green, binarize_Blue, Width, Height, 20);

    static unsigned char posterize_Red[MAXS][MAXS];
    static unsigned char posterize_Green[MAXS][MAXS];
    static unsigned char posterize_Blue[MAXS][MAXS];

    //Posterize Image
    PosterizeImage(Red, Green, Blue, posterize_Red, posterize_Green, posterize_Blue, Width, Height, 3.5, 5.0);
    DisplayHistogram(posterize_Red, posterize_Green, posterize_Blue, Width, Height, 20);
    //WriteImage("photo_posterize.ppm", posterize_Red, posterize_Green, posterize_Blue, Width, Height);

    return 0;
}