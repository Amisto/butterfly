double focus;
int rays_num;

double ***values;
double **res;

void phasing()
{
    int i, j, k, l, delayed;
    int max_length = written;
    double magic = DX_SENSORS/(DT_DIGITIZATION*C0);
    int res_length = max_length / magic;

    values = (double***)malloc(sizeof(double**)*SENSORS);
    for (i=0; i<SENSORS; i++)
    {
        values[i] = (double**)malloc(sizeof(double*)*SENSORS);
        for (j=0; j<SENSORS; j++)
            values[i][j] = (double*)malloc(sizeof(double)*max_length);
    }
    res = (double**)malloc(sizeof(double*)*rays_num);
    for (i=0; i<rays_num; i++)
        res[i] = (double*)malloc(sizeof(double)*res_length);
    for (i=0; i<rays_num; i++)
        for (j=0; j<res_length; j++)
            res[i][j] = 0;
    
    FILE *fi, *fo;
    char fname[100];

    for (i=0; i<SENSORS; i++)
    {    
        sprintf(fname, "%03d.csv", i);
        //printf("reading %s\n", fname);
        fi = fopen(fname, "r");
        if (!fi) {printf("no file"); exit(-1);}
        for (j=0; j<max_length; j++)
        {
            for (k=0; k<SENSORS; k++)
                fscanf(fi, "%lf", &(values[i][k][j]));
        }
        fclose(fi);
    }

    double delay[SENSORS] = {0};

    for (i=0; i<rays_num; i++)
    {
        //printf("%d of %d\n", i, rays_num);
        double alpha = M_PI/6.0 + ((M_PI*2.0/3.0)/rays_num)*i;
        double s = sin(alpha);
        double c = cos(alpha);
        for (j=0; j<SENSORS; j++)
        {
            l = SENSORS/2 - j;
            delay[j] = focus - sqrt(focus*focus*s*s + (focus*c - l)*(focus*c - l));//focus - sqrt(focus*c*focus*c + (focus*s - l)*(focus*s - l));
        }

        for (j=0; j<SENSORS; j++)
            for (k=0; k<SENSORS; k++)
                for (l=0; l<res_length; l++)
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
        for (j=0; j<res_length; j++)
            fprintf(fo, "%lf ", res[i][j]);
        fprintf(fo, "\n");
    }
    fclose(fo);
}

