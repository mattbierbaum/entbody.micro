#include <stdio.h>
#include <math.h>
#include "plot.h"

#define pi 3.14159265358

int keystates[256];
int plot_sizex;  
int plot_sizey;
int win;

#ifdef IMAGES
ILuint img;
#endif

void key_up(unsigned char key, int x, int y){
    keystates[key] = 0;
}

void key_down(unsigned char key, int x, int y){
    keystates[key] = 1;
}


void plot_init(){
  const int SIZE = 1000;//2400;
  plot_sizex = SIZE;
  plot_sizey = SIZE;
  win = 0;
  plot_init_opengl();
  
  #ifdef IMAGES
    plot_initialize_canvas();
  #endif
  int i;
  for (i=0; i<256; i++)
        keystates[i] = 0;
}

void plot_clean(){
  plot_end_opengl();
}


#ifdef IMAGES
void plot_initialize_canvas(){
  ilInit();
  ilGenImages(1, &img);
  ilBindImage(img);
  ilTexImage(plot_sizex, plot_sizey, 0, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
}


void plot_saveimage(const char* name){
  ILubyte *data = ilGetData();

  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  int x      = viewport[0];
  int y      = viewport[1];
  int width  = viewport[2];
  int height = viewport[3];

  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
  
  ilEnable(IL_FILE_OVERWRITE);
  ilSaveImage(name);
}
#endif

//=============================================================
// OpenGL functionality
// http://www.andyofniall.net/2d-graphics-with-opengl/
//=============================================================
void plot_init_opengl(){
  int argc = 1;
  char *argv = (char*)malloc(sizeof(char)*42);
  sprintf(argv, "./entbody");

  glutInit(&argc, &argv);	         
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
  glutInitWindowSize(plot_sizex, plot_sizey);
  glutInitWindowPosition(100,100);
  win = glutCreateWindow("EntBody Simulation");	

  glDisable(GL_DEPTH_TEST);
  glClearColor(1.0, 1.0, 1.0, 0.0);	/* set background to white */
  glutKeyboardUpFunc(key_up);
  glutKeyboardFunc(key_down);
  glViewport(0,0,plot_sizex, plot_sizey);

  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glutMainLoopEvent();
  free(argv);
}

void plot_end_opengl(){
  glutDestroyWindow(win);
}

int plot_clear_screen(){
  glClear(GL_COLOR_BUFFER_BIT);
  return 1;
}


int* plot_render_particles(double *x, double *rad, int *type, long N, double L, double *shade){
    // focus on the part of scene where we draw nice
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, L, L, 0, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // lets draw our viewport just in case its not square
    glBegin(GL_LINE_LOOP);
      glVertex2f(0, 0);
      glVertex2f(0, L);
      glVertex2f(L, L);
      glVertex2f(L, 0);
    glEnd();
    
    glDisable(GL_POINT_SMOOTH);

    #ifdef POINTS 
    glPointSize(rad[0]/L*plot_sizex);
    glBegin(GL_POINTS);
    #else
    double t=0;
    #endif

    int i; 
    for (i=0; i<N; i++){
        float tx = (float)x[2*i+0];
        float ty = (float)x[2*i+1];

        double c = fabs(shade[i]);
        if (c < 0) c = 0.0;
        if (c > 1.0) c = 1.0;

        float cr = c;
        float cg = c;
        float cb = c;
        float ca = 1.0;

        if (type[i] == 1) {
            cr = 0.9;
            cg = 0.05;
            cb = 0.05;
        }
        
        #ifdef POINTS
        glPointSize(2*rad[i]/L * plot_sizex);
        plot_set_draw_color(cr,cg,cb,ca);
        glVertex2f(tx, ty);
        #else
        double rx = 1.02*rad[i];
        uint secs = 15;
        plot_set_draw_color(cr,cg,cb,ca);
        glBegin(GL_POLYGON);
        for (t=0; t<2*pi; t+=2*pi/secs)
          glVertex2f(tx + rx*cos(t), ty + rx*sin(t));
        glEnd();
        plot_set_draw_color(0.0,0.0,0.0,1.0);
        glBegin(GL_LINE_LOOP);
        for (t=0; t<2*pi; t+=2*pi/secs)
          glVertex2f(tx + rx*cos(t), ty + rx*sin(t));
        glEnd();
        #endif
    }
    #ifdef POINTS
    glEnd();
    #endif

    glutSwapBuffers();
    glutMainLoopEvent();

    return keystates;
}


void plot_set_draw_color(float cr, float cg, float cb, float ca){
  glColor4f(cr, cg, cb, ca);
}

