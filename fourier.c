#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define MAXS (2000)
#define MAX_HISTOGRAM (1000)
#define PI 3.14159265358979323846

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

void ScaleImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], unsigned char (*Red2)[MAXS], unsigned char (*Green2)[MAXS], unsigned char (*Blue2)[MAXS], long Width, long Height, double Sx, double Sy)
{
    long i, j;
    double Mat[3][3] = {
        {Sx, 0, 0},
        {0, Sy, 0},
        {0, 0, 1}
    };
    double Matout[3][1];

    for (i = 0; i < Height; i++) {
        for (j = 0; j < Width; j++) {

            //Get coordinates of pixels and store in array
            double Vec[3][1] = {{(double)i}, {(double)j}, {1}};

            //Multiply vector to get new coordinates
            MultiplyVector(Mat, Vec, Matout);

            int newX = (int)Matout[0][0];
            int newY = (int)Matout[1][0];

            //Rewrite image with new coordinates
            Red2[newX][newY] = Red[i][j];
            Green2[newX][newY] = Green[i][j];
            Blue2[newX][newY] = Blue[i][j];
        }
    }
}


void FourierTransformationImage(unsigned char (*Red)[MAXS], unsigned char (*Green)[MAXS], unsigned char (*Blue)[MAXS], long Width, long Height, char *filename, char *imageFile)
{
    long i, j, u, v;
    double Hue = 0.0, Saturation = 0.0, Lightness = 0.0;

    //Array for DFT
    static double complex Data1[MAXS][MAXS];
    static double complex Data2[MAXS][MAXS];
    unsigned char red, green, blue;

    //Output text file
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
    }

    fprintf(fp,"Width Height 実数部\t 虚数部\n");

    //Output image file
    FILE *pFile = fopen(imageFile, "w");
    if (pFile == NULL)
    {
        printf("Error opening the file %s", imageFile);
    }

    fprintf(pFile, "P3 \n");
    fprintf(pFile, "#comment line \n");

    fprintf(pFile, "%ld %ld\n", Width, Height);
    fprintf(pFile, "255\n");

    //Loop inner equation
    for(j = 0; j < Height; j++) {
        for(u = 0; u < Width; u++) {

            Data1[j][u] = 0 + I*0;

            for(i = 0; i < Width; i++) {

                //Get brightness
                RGBtoHSL(Red[j][i], Green[j][i], Blue[j][i], &Hue, &Saturation, &Lightness);

                if(Hue < 0)
                {
                    Hue += 360;
                }

                double a = (2 * PI * u * i) / (double)Width;

                Data1[j][u] += Lightness * (cos(a) - I * sin(a));
            }
        }
    }

    //Loop outer equation
    for(v = 0; v < Height; v++) {
        for(u = 0; u < Width; u++) {

            Data2[v][u] = 0 + I * 0;

            for(j = 0; j < Height; j++) {

                double b = (2 * PI * v * j) / (double)Height;

                Data2[v][u] += Data1[j][u] * (cos(b) - I * sin(b));
            }

            Data2[v][u] /= (Width * Height);

            fprintf(fp, "%.2ld %.2ld %e %e\n", u, v, creal(Data2[v][u]), cimag(Data2[v][u]));

            //Output image
            Hue = 0.0;
            Saturation = 0.0;

            Lightness = sqrt(pow(creal(Data2[v][u]), 2) + pow(cimag(Data2[v][u]), 2));

            //Binarize Image
            if (Lightness < 0.001){
                Lightness = 0.0;
            }else {
                Lightness = 1.0;
            }

            HSLtoRGB(Hue, Saturation, Lightness, &red, &green, &blue);

	        fprintf(pFile, "%d %d %d\n", red, green, blue);
        }
    }


    fclose(fp);
    fclose(pFile);
    fp = NULL;
    pFile = NULL;
}




int main(void)
{
    static unsigned char Red[MAXS][MAXS];
    static unsigned char Green[MAXS][MAXS];
    static unsigned char Blue[MAXS][MAXS];

    long Width = 0, Height = 0;

    //Fourier transformation

    ReadImage("circular_waves.ppm", Red, Green, Blue, &Width, &Height);

    FourierTransformationImage(Red, Green, Blue, Width, Height, "fourier.txt", "fourier_out.ppm");

    return 0;
}
