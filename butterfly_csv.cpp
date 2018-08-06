#include "butterfly.h"
#include "phasing.c"

int main(int argc, char** argv)
{
    init_from_file(argv[1]);

    for(int i=0; i<SENSORS; i++)
    {
        sensors[i].pos.y = Y*0.999;
        sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i);
        sensors[i].writing.clear();
    }

    FILE* setup = fopen(argv[2], "r");
    if (!setup)
    {
        printf("No file opened\n");
        exit(-1);
    }
    fscanf(setup, "%d%lf%lf", &rays_num, &focus, &PIES);
    fclose(setup);

    char fname[100];
    for(int i=0; i<SENSORS; i++)
    {
        sprintf(fname, "%s_%s_%03d.csv", argv[1], argv[2], i);
        f_csv = fopen(fname, "w");
        if (!f_csv)
        {
            printf("No file opened\n");
            exit(-1);
        }   
        init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - i), Y*0.999);

        T_START = SENSORS*DX_SENSORS*1.2/C0;
        T_FINISH = 2.2*Y/C0;
        written = 0;

        while(T_FINISH > 0) calc_a_step();
        fclose(f_csv);
        printf("%3dth sensor of %d calculated\n", i, SENSORS);
    }

    sprintf(fname, "%s_%s", argv[1], argv[2]);
    phasing(fname, 0.0);

    finalize();

    return 0;
}

