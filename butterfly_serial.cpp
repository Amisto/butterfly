#include "butterfly.h"
#include "phasing.c"
#define N_C 11
#define N_S 15

int main(int argc, char** argv)
{
    init_from_file(argv[1]);
    FILE* setup = fopen("setup.txt", "r");
    if (!setup)
    {
        printf("No file opened\n");
        exit(-1);
    }
    fscanf(setup, "%d%lf", &rays_num, &focus);
    fclose(setup);

    char ftemplate[100];
    double c_rel = 0.9, dc = 0.01;
    for (int c = 0; c < N_C; c++)
    {
        obstacles[0].c_rel = c_rel;

        double shift = -175.0, ds = (350.0 - DX_SENSORS*SENSORS) /N_S;
        for (int s = 0; s < N_S; s++)
        {
            sprintf(ftemplate, "c_%.2lf_shift_%.2lf", c_rel, shift);
            for(int i=0; i<SENSORS; i++)
            {
                sensors[i].pos.y = Y*0.999;
                sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i) + shift;
                sensors[i].writing.clear();
            }
    
            char fname[100];
            for(int i=0; i<SENSORS; i++)
            {
                sprintf(fname, "%s_%03d.csv", ftemplate, i);
                f_csv = fopen(fname, "w");
                if (!f_csv)
                {
                    printf("No file opened\n");
                    exit(-1);
                }   
                init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - i) + shift, Y*0.999);
    
                T_START = SENSORS*DX_SENSORS*1.2/C0;
                T_FINISH = 2.2*Y/C0;
                written = 0;
                for(int i=0; i<SENSORS; i++)
                    sensors[i].writing.clear();
                nonidiocy = 1;
                while(T_FINISH > 0) calc_a_step();
                fclose(f_csv);
                printf("%3dth sensor of %d calculated\n", i, SENSORS);
                finalize();
            }
            phasing(ftemplate, 0.0);
            printf("c_rel %d of %d, shift %d of %d\n", c, N_C, s, N_S);

            shift += ds;
        }

        c_rel += dc;
    }

    return 0;
}

