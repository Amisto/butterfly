#include "butterfly.h"
#include "phasing.c"
#define N_C 11
#define N_S 1

int main(int argc, char** argv)
{
    init_from_file(argv[1]);
    FILE* setup = fopen(argv[2], "r");
    if (!setup)
    {
        printf("No file opened\n");
        exit(-1);
    }
    fscanf(setup, "%d%lf%lf", &rays_num, &focus, &PIES);
    fclose(setup);

    char ftemplate[100];
    double c_rel = obstacles[0].c_rel;//, dc = 0.01;
    //for (int c = 0; c < N_C; c++)
    //{
    obstacles[0].c_rel = c_rel;
    for(int i=0; i<SENSORS; i++)
    {
        sensors[i].pos.y = Y*0.999;
    	sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i);
    	sensors[i].writing.clear();
    }
    init_from_file(argv[1]);
    //for (int i=0; i<2; i++)
    //{
        //obstacles[0].c_rel = c_rel;
        double shift = 0.0, ds = 10.0 / N_S;
        for (int i=0; i<VERTICES; i++)
            obstacles[0].pos[i].x += shift;

        for (int s = 0; s < N_S; s++)
        {
            sprintf(ftemplate, "finfin_%s_shift_%.2lf_c_%.1lf", argv[1], shift, c_rel);

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
                init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - i), Y*0.999);
    
                T_START = SENSORS*DX_SENSORS*1.0/C0;
                T_FINISH = 1.8*Y/C0;
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
            printf("c %lf shift %d of %d\n", c_rel, s, N_S);

            for (int i=0; i<VERTICES; i++)
                obstacles[0].pos[i].x += ds;
            shift += ds;
        }
        //c_rel += 0.2;
    //}

    //    c_rel += dc;
    //}

    return 0;
}

