#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "maplec.h"

void curveFit(void)
{
   ALGEB r, vertexArray;
   static int prev_num_vertices = 0;
   static char* prev_fit_function = NULL;

   if( num_vertices < 2 || !FitFunction || (prev_num_vertices == num_vertices
       && prev_fit_function == FitFunctionName) )
return;

   prev_num_vertices = num_vertices;
   prev_fit_function = FitFunctionName;
   fit_size = 0;  

   vertexArray = convertArrayToMaple(vertex,num_vertices);

   r = EvalMapleProc(kv,FitFunction,2,vertexArray,varX);

   if( !r || IsMapleNULL(kv,r) )
return;

   MapleAssign(kv,varEqn,r);

   r = EvalMapleStatement(kv,
"Array(op([1,1],plot(eqn,x=-1..1)),datatype=float[8],order=C_order);");

   /* Make sure the result of the above command is an rtable (Array) */
   if( !r || !IsMapleRTable(kv,r) ) {
printf("error, not an rtable\n");
return;
   }

   /* extract the data pointer from the array */
   fit_size = RTableUpperBound(kv,r,1);
   fit = (double*)RTableDataBlock(kv,r);
}#define FONT (void *)GLUT_BITMAP_8_BY_13

#ifdef _MSC_VER
 /* windows */
 #define CDECL __cdecl
#else
 #define CDECL
#endif

/* global variables */
double width, height, xPos = 0.0, yPos = 0.0;
double *vertex = NULL, *fit = NULL;
int num_vertices = 0, vertex_size = 0;
int fit_size = 0;
MKernelVector kv;
char *FitFunctionName;
ALGEB FitFunction, varX, varEqn;

static char *FitFunctions[6] = {
   "BSplineCurve",
   "LeastSquares",
   "PolynomialInterpolation",
   "Spline",
   "ThieleInterpolation",
   NULL
};

/* OpenGL: called when the window is resized */
void CDECL changeSize( int w, int h )
{
   /* we're storing these values for later use in the
    mouse motion functions */
   if( h < 0 ) h = 1;
   width = w;
   height = h;

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   
   /* Set the viewport to be the entire window */
   glViewport(0,0,w,h);
}

/* OpenGL: draw a floating point number (for tic mark labels) */
void drawNumber( double num )
{
   char buf[30];
   int i;

   sprintf(buf,"%.1f",num);
   for( i=0; i<strlen(buf); ++i ) {
glutBitmapCharacter(FONT,buf[i]);
   }
}

/* OpenGL: draws the axes tic marks and labels them */
void drawTic( double tic, int X, int big )
{
   /* label tic mark */
   if( big && tic != 0.0 ) {
if( X )
   glRasterPos2d(tic<-0.6 ? tic : tic-.06, -0.1);
else
   glRasterPos2d(tic<0 ? -0.23 : -0.2, tic<-0.6 ? tic+.03 : tic-.05);
       drawNumber(tic);
   }

   /* draw the tic */
   glPushMatrix();
   glBegin(GL_LINE_STRIP);
if( X ) {
   glVertex2d(tic,big ? -0.04 : -0.02);
   glVertex2d(tic,big ? 0.04 : 0.02);
}
else {
   glVertex2d(big ? -0.04 : -0.02,tic);
   glVertex2d(big ? 0.04 : 0.02,tic);
}
   glEnd();
   glPopMatrix();
}

/* OpenGL draws the x/y axes */
void drawAxes()
{
   double tic;
   int big;

   /* x axis */
   glPushMatrix();
   glBegin(GL_LINE_STRIP);
   glVertex2d(1.0,0.0);
   glVertex2d(-1.0,0.0);
   glEnd();
   glPopMatrix();

   /* y axis */
   glPushMatrix();
   glBegin(GL_LINE_STRIP);
   glVertex2d(0.0,1.0);
   glVertex2d(0.0,-1.0);
   glEnd();
   glPopMatrix();

   /* tic marks */
   for( big=1,tic=-1.0; tic<1.1; tic+=.25 ) {
drawTic(tic,1,big); /* x tic marks */
drawTic(tic,0,big); /* y tic marks */
big ^= 1;
   }

}

/* OpenGL: writes the point cursor position on the top left corner */
void drawCoords(void)
{
   char buf[50];
   int i;

   glPushMatrix();
   sprintf(buf,"[%.3f,%.3f]",xPos,yPos);
   glRasterPos2d(-.99,.85);
   for( i=0; i<strlen(buf); ++i ) {
glutBitmapCharacter(FONT,buf[i]);
   }
   glPopMatrix();
}

/* OpenGL: writes the title on the top left corner */
void drawTitle(void)
{
   int i;

   glPushMatrix();
   glColor3f(0.0,1.0,1.0);
   glRasterPos2d(-.99,.93);
   for( i=0; i<strlen(FitFunctionName); ++i ) {
glutBitmapCharacter(FONT,FitFunctionName[i]);
   }
   glPopMatrix();
}

/* OpenGL: draws a curve from the points given */
void drawLine( double *vertex, int num_vertices, int color )
{
   int i;

   if( num_vertices == 0 )
return;

   if( color == 1 )
glColor3f(1.0,0.0,0.0);
   else
glColor3f(0.0,1.0,0.0);

   glBegin(GL_LINE_STRIP);
   for( i=0; i<num_vertices; ++i ) {
glVertex2d(vertex[2*i+0],vertex[2*i+1]);
   }
   glEnd();

   if( color == 1 ) {
glColor3f(0.0,0.0,1.0);
glPointSize(5.0);
   }
   else {
glColor3f(1.0,1.0,1.0);
glPointSize(3.0);
   }
   glBegin(GL_POINTS);
   for( i=0; i<num_vertices; ++i ) {
glVertex2d(vertex[2*i+0],vertex[2*i+1]);
   }
   glEnd();
}

/* OpenMaple: Turns a C array into something Maple recognizes.
  Note: Generally it is better to create the array in Maple
        and get a pointer to the data block.  This way the
        data can be changed in-place with no conversions
        needed.  In this case we are constantly resizeing
        the array, so, we create new Maple Array headers
        that point to the C allocated data.  The data is
        not copied over to Maple each time this function is
        called.  Maple directly accesses the data allocated
        by C in this program.
*/
ALGEB convertArrayToMaple( double *array, int size )
{
   ALGEB vertexArray;
   RTableSettings rts;
   int bounds[4] = { 1, 100, 1, 2 };  /* 2x100 rtable */

   /* setup the array details */
   RTableGetDefaults(kv,&rts);
   rts.data_type = RTABLE_FLOAT64;
   rts.num_dimensions = 2;
   rts.foreign = 1;
   rts.order = RTABLE_C;
   bounds[1] = size;

   /* create the vertex Array */
   vertexArray = RTableCreate(kv,&rts,vertex,bounds);

   return( vertexArray );
}

/* OpenMaple: Calls Maple to find a function that closely
  approximates the chosen points using the selected method.  
*/
void curveFit(void)
{
   ALGEB r, vertexArray;
   static int prev_num_vertices = 0;
   static char* prev_fit_function = NULL;

   /* don't regenerate the curve-fit if too few vertices, or
      if no new points have been added
   */
   if( num_vertices < 2 || !FitFunction || (prev_num_vertices == num_vertices
       && prev_fit_function == FitFunctionName) )
return;
   prev_num_vertices = num_vertices;
   prev_fit_function = FitFunctionName;
   fit_size = 0;  /* don't draw any curve if there is an error */

   /* convert the vertex array to a Maple Array */
   vertexArray = convertArrayToMaple(vertex,num_vertices);

   /* call the appropriate CurveFitting function */
   r = EvalMapleProc(kv,FitFunction,2,vertexArray,varX);

   /* r will be NULL if there was an error (eg. BSplineCurve was
      used to fit too few points.  Don't draw anything in this case.
   */
   if( !r || IsMapleNULL(kv,r) )
return;

   /* assign the result to the maple variable `eqn` (created at init time) */
   MapleAssign(kv,varEqn,r);

   /* useful to uncomment when debugging */
   /* MapleALGEB_Printf(kv,"Result is: %a\n", r); */

   /* Use Maple's plot command to generate a list of points worthy of
      plotting.  The guts of the following line is
      op([1,1],plot(r,x=-2..2));
   */
   r = EvalMapleStatement(kv,
"Array(op([1,1],plot(eqn,x=-1..1)),datatype=float[8],order=C_order);");

   /* Make sure the result of the above command is an rtable (Array) */
   if( !r || !IsMapleRTable(kv,r) ) {
printf("error, not an rtable\n");
return;
   }

   /* extract the data pointer from the array */
   fit_size = RTableUpperBound(kv,r,1);
   fit = (double*)RTableDataBlock(kv,r);
}

/* OpenGL: draw the scene */
void CDECL renderScene(void)
{
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   drawAxes();
   drawCoords();
   drawLine(vertex,num_vertices,1);
   curveFit();
   drawLine(fit,fit_size,0);
   drawTitle();
   glutSwapBuffers();
}

/* OpenGL: handle mouse clicks -- add a new point to the vertex array */
void CDECL processMouse(int button, int state, int x, int y)
{
   if( state == GLUT_DOWN ) {
int i = num_vertices;
if( vertex == NULL ) {
   /* alloc initial array of vertices */
   vertex_size = 50;
   vertex = (double*)malloc(2*vertex_size*sizeof(double));
       }
else if( num_vertices == vertex_size ) {
   /* grow array of vertices */
   vertex_size *= 2;
   vertex = (double*)realloc(vertex,2*vertex_size*sizeof(double));
       }

vertex[2*i+0] =  (double)(2*x-width)/width;
vertex[2*i+1] =  (double)(height-2*y)/height;
num_vertices++;
   }
}

/* OpenGL: handle mouse motion -- display position of cursor */
void CDECL setCursorPosition(int x, int y)
{
   xPos =  (double)(2*x-width)/width;
   yPos =  (double)(height-2*y)/height;
}

/* OpenGL: quit when 'q' is pressed, clear screen when 'c' is pressed */
void CDECL processKeys(unsigned char key, int x, int y)
{
   if( key == 'q' || key == 'Q' )
exit(0);
   else if( key == 'c' || key == 'C' ) {
       num_vertices = 0;
       fit_size = 0;
   }
}

/* OpenMaple: right-click menu -- allow choice of CurveFitting function */
void CDECL pickCurveFittingFunction( int option )
{
   char fullname[100];

   FitFunctionName = FitFunctions[option];

   /* get the Maple function name for the selected member of
      the CurveFitting package.
   */
   sprintf(fullname,"CurveFitting:-%s;",FitFunctionName);
   FitFunction = EvalMapleStatement(kv,fullname);

   if( !FitFunction ) {
       /* perhaps the Maple library couldn't be found? */
       char *dir;

       /* set libname to $MAPLE/lib if $MAPLE exists in the environment */
       if( (dir=getenv("MAPLE")) || (dir=getenv("MAPLE_ROOT")) ) {
   char *libpath;
   libpath = (char*)malloc((5+strlen(dir))*sizeof(char));
   sprintf(libpath,"%s/lib",dir);
   MapleLibName(kv,ToMapleString(kv,libpath));
   FitFunction = EvalMapleStatement(kv,fullname);
}
   }
}

/* initialize OpenGL */
void initOpenGL( int argc, char **argv )
{
   int i;

   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
   glutInitWindowPosition(100,100);
   glutInitWindowSize(320,320);
   glutCreateWindow("Line");
   glutDisplayFunc(renderScene);
   glutIdleFunc(renderScene);
   glutReshapeFunc(changeSize);
   glutKeyboardFunc(processKeys);

   glutMouseFunc(processMouse);
   glutPassiveMotionFunc(setCursorPosition);

   glutCreateMenu(pickCurveFittingFunction);
   for( i=0; FitFunctions[i] != NULL; ++i )
glutAddMenuEntry(FitFunctions[i],i);
   glutAttachMenu(GLUT_RIGHT_BUTTON);

}

/* OpenMaple: text output callback function */
static void M_DECL textCallBack( void *data, int tag, char *output )
{
   /* to see output on stdout, uncomment the following. */
   /* printf("%s\n",output); */
}

/* OpenMaple: error output callback function */
static void M_DECL errorCallBack ( void *data, M_INT offset, char *msg )
{
   /* show errors on stdout */
   printf("%s\n",msg);
}

/* initialize Maple */
void initMaple( int argc, char *argv[] )
{
   MCallBackVectorDesc cb = { textCallBack, errorCallBack, 0, 0, 0, 0, 0, 0 };
   char err[2048];
   ALGEB r;

   /* start Maple */
   if( (kv=StartMaple(argc,argv,&cb,NULL,NULL,err)) == NULL ) {
printf("Initialization error: %s\n");
exit(1);
   }

   /* get the function and variable names */
   pickCurveFittingFunction(1);
   varX = ToMapleName(kv,"x",1);
   varEqn = ToMapleName(kv,"eqn",1);

}

/* main entry point */
int main( int argc, char **argv )
{
   initOpenGL(0,argv);
   initMaple(argc,argv);
   glutMainLoop();

   return 0;
}

