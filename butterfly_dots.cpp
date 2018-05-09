#include "butterfly.h"
#include <GL/glut.h>

//==================================================================================================================
//=== graphics

void init_gl()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);   /* black background */
    glMatrixMode(GL_PROJECTION);        /* In World coordinates: */
    glLoadIdentity();                   /* position the "clipping rectangle" */
    gluOrtho2D( 0, X, 0, Y);            /* at -B/2, its right edge at +B/2, its bottom */
    glMatrixMode(GL_MODELVIEW);         /* edge at -B/2 and its top edge at +B/2 */
}

void draw_obstacles()
{
    for (int i=0; i<OBSTACLES; i++)
    {
        glColor3f(obstacles[i].c_rel, 0, 0);
        glBegin(GL_POLYGON);
        for (int j=0; j<VERTICES; j++)
            glVertex2i(obstacles[i].pos[j].x, obstacles[i].pos[j].y);
        glEnd();
        /*glBegin(GL_LINES);
        for (int j=0; j<VERTICES-1; j++)
        {
            glVertex2i(obstacles[i].pos[j].x, obstacles[i].pos[j].y);
            glVertex2i(obstacles[i].pos[j+1].x, obstacles[i].pos[j+1].y);
        }*/
        glEnd();
    }
    glColor3f(0, 0, 1.0);
    for (int i=0; i<DOTS; i++)
    {
        glTranslatef(dots[i].pos.x, dots[i].pos.y, 0);
        glBegin(GL_QUADS);
            glVertex2i(2, 2);
            glVertex2i(2, -2);
            glVertex2i(-2, -2);
            glVertex2i(-2, 2);
        glEnd();
        glTranslatef(-dots[i].pos.x, -dots[i].pos.y, 0);
    }
    glColor3f(0, 1, 0);
    for (int i=0; i<SENSORS; i++)
    {
        glTranslatef(sensors[i].pos.x, sensors[i].pos.y, 0);
        glBegin(GL_QUADS);
            glVertex2i(2, 2);
            glVertex2i(2, -2);
            glVertex2i(-2, -2);
            glVertex2i(-2, 2);
        glEnd();
        glTranslatef(-sensors[i].pos.x, -sensors[i].pos.y, 0);
    }
}

void draw_nodes()
{
    double in;
    for (int i=0; i<n_nodes; i++)
    {
        //glColor3f(1.0 * nodes[i]->intensity, 1.0 * nodes[i]->intensity, (nodes[i]->material >= 0 ? obstacles[nodes[i]->material].c_rel : 1.0) * nodes[i]->intensity);
        in = 0.5 + nodes[i]->intensity;
        glColor3f(1.0 * in, 1.0 * in, 1.0 * in);
        if (nodes[i]->t_encounter == -1) glColor3f(1.0, 0.0, 1.0);
        else if (nodes[i]->t_encounter == INFINITY) glColor3f(0.0, 1.0, 1.0);
        glTranslatef(nodes[i]->pos.x, nodes[i]->pos.y, 0);
        glBegin(GL_QUADS);
            glVertex2i(2, 2);
            glVertex2i(2, -2);
            glVertex2i(-2, -2);
            glVertex2i(-2, 2);
        glEnd();
        glTranslatef(-nodes[i]->pos.x, -nodes[i]->pos.y, 0);
    }
}

void draw_neighbors()
{
    for (int i=0; i<n_nodes; i++)
    {

        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glColor3f(0, 0, 1.0 * (nodes[i]->intensity + 0.5));
        if (nodes[i]->left)
        {
                glVertex2i(nodes[i]->pos.x, nodes[i]->pos.y);
                glVertex2i(nodes[i]->left->pos.x, nodes[i]->left->pos.y);
        }
        if (nodes[i]->right)
        {
                glVertex2i(nodes[i]->pos.x, nodes[i]->pos.y);
                glVertex2i(nodes[i]->right->pos.x, nodes[i]->right->pos.y);
        }
        glColor3f(0, 1.0, 0);
        for (int j=0; j<nodes[i]->neighbors_left.size(); j++)
        {
                glVertex2i(nodes[i]->pos.x, nodes[i]->pos.y);
                glVertex2i(nodes[i]->neighbors_left[j]->pos.x, nodes[i]->neighbors_left[j]->pos.y);
        }
        glColor3f(0, 1.0, 0);
        for (int j=0; j<nodes[i]->neighbors_right.size(); j++)
        {
                glVertex2i(nodes[i]->pos.x, nodes[i]->pos.y);
                glVertex2i(nodes[i]->neighbors_right[j]->pos.x, nodes[i]->neighbors_right[j]->pos.y);
        }
        glEnd();

    }
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);     /* clear the window */

    //printf("drawing obstacles\n");
    draw_obstacles();
    //printf("drawing neighbors\n");
    //draw_neighbors();
    //printf("drawing nodes\n");
    draw_nodes();

    glLoadIdentity();
    glFlush();                        /* send all commands */

}

//==================================================================================================================
//=== csv drawing

/*FILE* f_csv = NULL;

double signal(double t)
{
    return sin(M_PI * t / DT_WIDTH) * sin(M_PI * t / DT_WIDTH);// * sin(2 * M_PI * t / DT_CARRYING);
}

void write_to_csv()
{
    for(int i=0; i<SENSORS; i++)
    {
        double _signal = 0;
        for(int j=0; j<sensors[i].writing_time.size(); j++)
        {
            if (sensors[i].writing_time[j] > 0) _signal += signal(sensors[i].writing_time[j]);
            sensors[i].writing_time[j] += DT_DIGITIZATION;
        }
        fprintf(f_csv, "%5.2lf ", _signal);

        bool nulls_exist = true;
        while (nulls_exist)
        {
            nulls_exist = false;
            for (int j=0; j<sensors[i].writing_time.size(); j++)
                if (sensors[i].writing_time[j] > DT_WIDTH)
                {
                    sensors[i].writing_time.erase(sensors[i].writing_time.begin() + j);
                    nulls_exist = true;
                    break;
                }
        }
    }

    fprintf(f_csv, "\n");
}*/


//==================================================================================================================
//=== general management

void TimerFunction(int value)
{
    glutPostRedisplay();  //call display function
    glutTimerFunc(1,TimerFunction,1);
}

void key_exit(unsigned char c, int x, int y)
{
    switch (c)
    {
        case 27: exit(0); break;
        //case 32: ScreenShot(A, A); break;
    }
}

void key_pause(int key, int xx, int yy)
{
    switch (key)
    {
        case GLUT_KEY_LEFT:  break;
        case GLUT_KEY_RIGHT:  calc_a_step(); break;
        case GLUT_KEY_END:
            break;
    }
}

int main(int argc, char** argv)
{
    //graphic initialization
    glutInit(&argc,argv);
    glutInitWindowSize( X, Y );       /* A x A pixel screen window  */
    glutInitDisplayMode( GLUT_RGB | GLUT_SINGLE);
    glutCreateWindow("Butterfly effect");
    glutDisplayFunc(display);
    glutTimerFunc(1,TimerFunction,1);
    init_gl();
    glutKeyboardFunc(key_exit);
    glutSpecialFunc(key_pause);

    init_from_file();
    init_explosion(X/2 - DX_SENSORS * (SENSORS / 2 - 0), Y*0.9);

    f_csv = fopen("000.csv", "w");
    if (!f_csv)
    {
        printf("No file opened\n");
        exit(1);
    }

    for(int i=0; i<SENSORS; i++)
    {
        sensors[i].pos.y = Y*0.999;
        sensors[i].pos.x = X/2 - DX_SENSORS * (SENSORS / 2 - i);
        sensors[i].writing_time.clear();
    }

    glutMainLoop();                   /* pass control to the main loop  */

    finalize();

    return 0;
}

