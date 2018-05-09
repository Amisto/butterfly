#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define sensors 32
#define DX      1  //any other number shits the b-scan

double ***values;
double **res;

int main()
{
    int i, j, k, l, delayed;

    int max_read = 800;
    int rays_num = 200;
    int skip = 0;
    int max_length = max_read - skip;
    double focus = 500;
    double magic = 1.0;
    double tmp;

    values = (double***)malloc(sizeof(double**)*sensors);
    for (i=0; i<sensors; i++)
    {
        values[i] = (double**)malloc(sizeof(double*)*sensors);
        for (j=0; j<sensors; j++)
            values[i][j] = (double*)malloc(sizeof(double)*max_length);
    }
    res = (double**)malloc(sizeof(double*)*rays_num);
    for (i=0; i<rays_num; i++)
        res[i] = (double*)malloc(sizeof(double)*max_length);
    for (i=0; i<rays_num; i++)
        for (j=0; j<max_length; j++)
            res[i][j] = 0;
    
    FILE *fi, *fo;
    char fname[100];

    for (i=0; i<sensors; i++)
    {    
        sprintf(fname, "%03d.csv", i);
        printf("reading %s\n", fname);
        fi = fopen(fname, "r");
        if (!fi) {printf("no file"); exit(-1);}
        for (j=0; j<skip; j++)
        {
            //printf("    reading %d\n", j);
            for (k=0; k<sensors; k++)
                fscanf(fi, "%lf", &tmp);
        }
        for (; j<max_read; j++)
        {
            //printf("    reading %d\n", j);
            for (k=0; k<sensors; k++)
                fscanf(fi, "%lf", &(values[i][k][j]));
        }
        fclose(fi);
    }

    double delay[sensors] = {0};

    for (i=0; i<rays_num; i++)
    {
        printf("%d of %d\n", i, rays_num);
        double alpha = (M_PI*2.0/3.0/rays_num)*(rays_num/2-i);
        double s = sin(alpha);
        double c = cos(alpha);
        for (j=0; j<sensors; j++)
        {
            l = DX*(sensors/2 - j);
            delay[j] = focus - sqrt(focus*c*focus*c + (focus*s - l)*(focus*s - l));
        }

        for (j=0; j<sensors; j++)
            for (k=0; k<sensors; k++)
                for (l=0; l<max_length; l++)
                {
                    delayed = magic*l - magic*(delay[j] + delay[k]);
                    if (delayed < 0) delayed = 0;
                    if (delayed > max_length - 1) delayed = max_length - 1;
                    res[i][l] += values[j][k][delayed];
                }
    }
    fo = fopen("out.csv", "w");
    if (!fo) {printf("no file"); exit(-1);}
    for (i=0; i<rays_num; i++)
    {
        for (j=0; j<max_length; j++)
            fprintf(fo, "%lf ", res[i][j]);
        fprintf(fo, "\n");
    }
    fclose(fo);
    return 0;
}

