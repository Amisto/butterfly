#include "butterfly.h"

int main(int argc, char** argv)
{
    init_from_file();

    for(int i=0; i<SENSORS; i++)
    {
        sensors[i].pos.y = Y*0.999;
        sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i);
        sensors[i].writing_time.clear();
    }

    char fname[100];
    for(int i=0; i<SENSORS; i++)
    {
        sprintf(fname, "%03d.csv", i);
        f_csv = fopen(fname, "w");
        if (!f_csv)
        {
            printf("No file opened\n");
            exit(-1);
        }   
        init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - i), Y*0.999);

        T_START = SENSORS*DX_SENSORS*1.2/C0;
        T_FINISH = Y/C0;

        while(n_nodes > 0) calc_a_step();
        fclose(f_csv);
    }

    finalize();

    return 0;
}

