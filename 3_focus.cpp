#include "butterfly.h"
#include "phasing.c"
#define N_C 11
#define N_S 1

int main(int argc, char** argv)
{
    FILE* setup = fopen(argv[2], "r");
    if (!setup)
    {
        printf("No file opened\n");
        exit(-1);
    }

    fscanf(setup, "%d%lf%lf%d", &rays_num, &focus, &PIES, &written);
    fclose(setup);

    char ftemplate[100];

    double shift = 0.0, ds = 10.0 / N_S;
    for (int s = 0; s < N_S; s++)
    {
        sprintf(ftemplate, "%s_shift_%.2lf", argv[1], shift);
        phasing(ftemplate, 0.0);
        shift += ds;
    }
    return 0;
}

