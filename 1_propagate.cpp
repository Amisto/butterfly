#include "butterfly.h"
#include <string>
#include <string.h>
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
    int rays_num;
    double focus;
    fscanf(setup, "%d%lf%lf", &rays_num, &focus, &PIES);
    fclose(setup);

    char ftemplate[100];
    double c_rel = obstacles[0].c_rel;


    for(int i=0; i<SENSORS; i++)
    {
        sensors[i].pos.y = Y*0.999;
    	sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i);
    	sensors[i].writing.clear();
    }

        double shift = 0.0, ds = 10.0 / N_S;
        for (int i=0; i<VERTICES; i++)
            obstacles[0].pos[i].x += shift;

        for (int s = 0; s < N_S; s++)
        {
            sprintf(ftemplate, "%s_shift_%.2lf", argv[1], shift);

            char fname[100];
            for(int i=0; i<SENSORS; i++)
            {
                char data_after_propagate_path[100];
                sprintf(data_after_propagate_path, "./data/baseline/Sensor%d/", i);
                sprintf(fname, "%s_%03d.csv", ftemplate, i);

                strcat(data_after_propagate_path, fname);
                f_csv = fopen(data_after_propagate_path, "w");

                //f_csv = fopen(fname, "w");
                if (!f_csv)
                {
                    printf("No file opened\n");
                    exit(-1);
                }   
                init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - i), Y*0.999);
    
                T_START = T_START_BASE;
                T_FINISH = T_FINISH_BASE;
                written = 0;
                for(int i=0; i<SENSORS; i++)
                    sensors[i].writing.clear();
                nonidiocy = 1;
                while(T_FINISH > 0) calc_a_step();
                fclose(f_csv);
                printf("%3dth sensor of %d calculated\n", i, SENSORS);
                finalize();
                //p[100] = '\0';
                //delete[] p;
            }
            printf("c %lf shift %d of %d\n", c_rel, s, N_S);

            for (int i=0; i<VERTICES; i++)
                obstacles[0].pos[i].x += ds;
            shift += ds;
        }

    setup = fopen(argv[2], "w");
    if (!setup)
    {
        printf("No file opened\n");
        exit(-1);
    }
    fprintf(setup, "%d %lf %lf %d", rays_num, focus, PIES, written);
    fclose(setup);

    return 0;
}
