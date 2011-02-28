/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * class to handle glut output to display graph+map
 *
 *****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glu.h>

// FLTK
#include <FL/Fl_File_Chooser.h>


#include "dotvisualizer.h"
#include "glcodegen.h"
#include "mapcodegen.h"

// signal quit to main progam
extern bool globalQuit;

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::string;

//-----------------------------------------------------------------------------
/* defines - not needed anywhere else currently */

/* default camera position */
#define VIS_CAMDEFAULTZ -1.75

/* store buttons in one variable */
#define VIS_BUTTONL 1
#define VIS_BUTTONM 2
#define VIS_BUTTONR 4

/* vis effects on/off */
#define VIS_FOG            1
#define VIS_LIGHT          1
#define VIS_FULLSCREEN     0

/* animation parameters */
#define MAP_FADEOUT_DELAY               4000
#define MAP_FADEOUT_SPEED               5000.0
#define MAP_FADEIN_SPEED                        1000.0
#define MAP_ZOOMSPEED                                   500
#define TRACK_CAMSPEED                          1500
#define TRACK_LOOKATSPEED                       500

// anti alias lines?
bool antiAlias = false;

// speed of rotation, translation
double rotScale = 0.25;
double transScale = 0.5;
double wheelSpeed = 40.0;
double minZDist = 10.0;

/* 
 * Not-so-nice-workaround to pass the display ... functions to glut
 * and still access all member variables
 */
//static DotVisualizer *localVisualizer;



//-----------------------------------------------------------------------------
// Constructor
DotVisualizer::DotVisualizer(Agraph_t *g, int x,int y, int w,int h) :
        Fl_Gl_Window( x,y, w,h, NULL),
  /* window stuff */
  mCamX(-115.0), mCamY(-170.0), mCamZ( -200.0 ),
  mWindowAspect(1.0),
  mButtonCurrent(0), 
  mMouseLastX(0), mMouseLastY(0),
        mpGraph( g ), mOnMap( false ), mOnNode( false ),
        mLastTime( -1 ), mLastAction( -1 ),
        mMapFade( 1.0 ),
        mWindowX( -1 ), mWindowY( -1 ),
        mMaxZDistance( 0.0 ),
        mTrackTarget( false ), mTargetX( 0.0 ), mTargetY( 0.0 ), mTargetZ( 0.0 ), mTargetLookX( 0.0 ), mTargetLookY( 0.0 ),
        mFirstSelect( false ), mShowFPS( false ), mShowLock( false )
{
        mFirstTime = 0;
        mFirstTime = getTime();
        reshape(w,h);
        if(mpGraph) initGraph();
        mpGLC = NULL;
        mpMapC = NULL;
}


//-----------------------------------------------------------------------------
// 
void DotVisualizer::initGraph() {
  mCamX = (mpGraph->u.bb.LL.x+mpGraph->u.bb.UR.x)*-0.5;
  mCamY = (mpGraph->u.bb.LL.y+mpGraph->u.bb.UR.y)*-0.5;
  mCamZ = ((-mpGraph->u.bb.LL.x+mpGraph->u.bb.UR.x)+(-mpGraph->u.bb.LL.y+mpGraph->u.bb.UR.y))*-0.25;
        mLookatX = mCamX; mLookatY = mCamY;
        mTargetX = mTargetLookX = mCamX; mTargetY = mTargetLookY = mCamY; mTargetZ = mCamZ;
        mCamZoomPos = mCamZ;

        // init node selection  
        node_t *selnode = agfstnode(mpGraph);
        selnode = agnxtnode(mpGraph,selnode);

  // prepare hack
  //localVisualizer = this;
        if(mpGLC) delete mpGLC;
        mpGLC = new GlCodegenState( mpGraph );
        mpGLC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        mpGLC->select_node( mpGraph, selnode );
        mpGLC->setGraphInfo( mpGraphInfo ); 

        if(mpMapC) delete mpMapC;
        mpMapC = new MapCodegenState( mpGraph );
        mpMapC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        reshape(mWindowX,mWindowY);
}




//-----------------------------------------------------------------------------
// Destructor
DotVisualizer::~DotVisualizer()
{
  /* close graph */
        if(mpGraph) closeGraph();
}


//-----------------------------------------------------------------------------
// helper function to determine current time
unsigned long DotVisualizer::getTime()
{
        unsigned long long ret = 0;
#ifdef _WIN32
        static LARGE_INTEGER liTimerFrequency = time_freq();
        LARGE_INTEGER liLastTime;
        QueryPerformanceCounter(&liLastTime);
        ret = (INT)( ((double)liLastTime.QuadPart / liTimerFrequency.QuadPart)*1000 ) - mFirstTime;
#else
        struct timeval tv;
        struct timezone tz;
        tz.tz_minuteswest = 0;
        tz.tz_dsttime = 0;
        gettimeofday(&tv,&tz);
        ret = (tv.tv_sec*1000)+(tv.tv_usec/1000)-mFirstTime;
        //fprintf(stderr, " Tp s%lu us%lu \n", tv.tv_sec,  tv.tv_usec );
#endif
        //cout << " Tret " << ret <<endl;
        return (unsigned long)ret;
}




//-----------------------------------------------------------------------------
// init camera position and orientation
void
DotVisualizer::initCamera( void )
{
  /* perspective projection viewport */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

        // account for label offset and extrude depth, safety region
        double max = mMaxZDistance+(5.0+10.0);
        double min = (mMinZDistance-(10.0+5.0+10.0))*0.5; // FIXME - why *0.5?
        if(min<0.01) min = 0.01;
  gluPerspective(90.0,mWindowAspect, min,max );
        //cout << " mz "<< min<<"-"<<max<<"   camZ:"<< mCamZ<< endl; // debug

  /* reset */
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* translate camera */
        gluLookAt( 
                        -mCamX,-mCamY,-mCamZ,
                        -mLookatX,-mLookatY, 0.0, 
                        0.0, 1.0, 0.0
                        );
                        
        /* camera done... */
}

  


//-----------------------------------------------------------------------------
// open gl(ut) display method
int ctt = 0;
void DotVisualizer::draw( void )
{
        // measure fps
        long startTime = getTime();
        bool demo = false;

        glPushMatrix();

        ctt++;
        reshape(w(), h());

        // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
        /* setup open gl */
        glShadeModel(GL_SMOOTH);
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);
        //glClearColor(0.0, 0.0, 0.0, 0.0);
        glClearColor(1.0, 1.0, 1.0, 0.0);
        glDisable(GL_DITHER);

        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);                                                 // Set Line Antialiasing

        glClearDepth(1.0f);                                 // Depth Buffer Setup
        glEnable(GL_DEPTH_TEST);                            // Enables Depth Testing
        glDepthFunc(GL_LEQUAL);                             // The Type Of Depth Testing To Do
        glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);                     // Blending Function For Translucency Based On Source Alpha Value

        if(VIS_LIGHT) {
                /* set up light */
                GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };    // Ambient Light Values
                GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };    // Diffuse Light Values
                GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };   // Specular Light Values
                //GLfloat LightPosition[]= { -100.0f, 200.0f, 00.0f, 1.0f };   // Light Position
                //GLfloat LightPosition[]= { 0.0f, 0.0f, 0.0f, 1.0f };   // Light Position
                glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);        // Setup The Ambient Light
                glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);        // Setup The Diffuse Light
                glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);
                //glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);       // Position The Light
                glLightf (GL_LIGHT1, GL_SPOT_EXPONENT, 1.0);
                glEnable(GL_LIGHT1);                                   // Enable Light One

                /* set up global light model */
                GLfloat LightViewer[] = { 1.0 };
                glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightAmbient);
                glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, LightViewer);

                /* set up material */
                //GLfloat MaterialAmbientO[]= { 0.0f, 0.0f, 0.3f, 1.0f };
                //GLfloat MaterialDiffuseO[]= { 0.2f, 0.2f, 0.8f, 1.0f };
                //GLfloat MaterialSpecularO[]= { 0.5f, 0.5f, 1.0f, 1.0f };

                GLfloat MaterialAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f };
                GLfloat MaterialDiffuse[]= { 0.5f, 0.5f, 0.5f, 1.0f };
                GLfloat MaterialSpecular[]= { 0.8f, 0.8f, 0.8f, 1.0f };
                GLfloat MaterialShininess[]= { 10.0f };
                glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
                glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
                glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);

                /* lighting is enabled only for the surface */
        }

        /* enable fog */
        if(VIS_FOG) {
                GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR };       // Storage For Three Types Of Fog
                GLuint fogfilter= 2;                                    // Which Fog To Use
                GLfloat fogColor[4]= {0.0f, 0.0f, 0.0f, 1.0f};          // Fog Color  
                glFogi(GL_FOG_MODE, fogMode[fogfilter]);                // Fog Mode
                glFogfv(GL_FOG_COLOR, fogColor);                        // Set Fog Color
                glFogf(GL_FOG_DENSITY, 0.35f);                          // How Dense Will The Fog Be
                glHint(GL_FOG_HINT, GL_DONT_CARE);                      // Fog Hint Value
                glFogf(GL_FOG_START, 1.0f);                             // Fog Start Depth
                glFogf(GL_FOG_END, 4.0f);                               // Fog End Depth
                //glEnable(GL_FOG);                                       // Enables GL_FOG
        }

        // also show backsides of triangles...
        glDisable( GL_CULL_FACE );
        // :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

        if(mpGraph) {

                // calculate viewing region, for Z-Buffer init
                pointf viewReg[4];
                mpMapC->calc_viewingregion(viewReg,&mMinZDistance, &mMaxZDistance );
                mpGLC->set_viewingregion(viewReg);

                initCamera();
                /* position light next to camera*/
                if(VIS_LIGHT) {
                        GLfloat LightPosition[]= { -mCamX-200.0f, -mCamY+200.0f, -mCamZ+200.0f, 1.0f };
                        glLightfv(GL_LIGHT1, GL_POSITION, LightPosition); 
                }
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ); 


                // do "real" display?
                if(!demo) {
                        // draw graph
                        mpGLC->draw();

                        // draw map etc.
                        mpMapC->init_ortho( mpGraph );
                        mpMapC->display( mpGraph );
                        mpMapC->graphinfo_display( (graph_t *)mpGraph );
                        mpMapC->pop_ortho();
                        mpMapC->show_view( ); //mCamX, mCamY, mCamZ );

                        // done...
                }
                /* */

        } else {
                glClearColor(0.5, 0.5, 0.5, 0.0);
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT ); 
        }

        /* TESTS */
        if(demo) {
                // two points
                glColor3f( 0,1,0); // green
                glBegin(GL_POINTS);
                glVertex3f( -1,1,0 );
                glVertex3f( -1,-1,0 );
                glEnd();


                // a line
                glColor3f( 0,1,1); // cyan
                glBegin(GL_LINES);
                glVertex3f( 0,2,0 );
                glVertex3f( 0,0,0 );
                glEnd();


                // simple triangle
                glColor3f( 1,1,0); // yellow
                glBegin(GL_TRIANGLES);
                glVertex3f( 1,0,1 );
                glVertex3f( -1,0,1 );
                glVertex3f( 1,0,-1 );
                glEnd();


                // "complex" triangle, effects on
                glEnable(GL_LIGHTING);
                glEnable(GL_FOG);
                glEnable(GL_BLEND);

                glPushMatrix();
                glTranslatef( 0.5,0.5,0.5 );

                glBegin(GL_TRIANGLES); // draw triangle
                glNormal3f(0,1,0);
                glVertex3f( 1,1,0 ); // now the material above is used
                glVertex3f( 1,-1,0 );
                glVertex3f( -1,1,0 );
                glEnd();

                glPopMatrix(); // restore old state
                glDisable(GL_LIGHTING);
                glDisable(GL_FOG);
                glDisable(GL_BLEND);


                // draw text
                glPushMatrix();
                glClear( GL_DEPTH_BUFFER_BIT ); // draw above everything else
                glfStartBitmapDrawing();

                glLoadIdentity();
                glTranslatef(-2.0, -2.0, -2.0);
                glScalef(3, 3, 1);

                char statusBuffer[512];
                snprintf(statusBuffer, 510, "test %d", 123);
                glColor3f(0.5,0.5,0.5);
                glfDrawBString( statusBuffer );

                glfStopBitmapDrawing();
                glPopMatrix();
        }

        // display fps
        long stopTime = getTime();
        //if(stopTime-startTime >5) {
        float fpsec = 1000.0/(float)(stopTime-startTime);
        bool showFps = mShowFPS;
        //cout << " fps " << fpsec << "    " << stopTime << " " << startTime << endl; return;

        if(showFps) {
                glDisable(GL_DEPTH_TEST);
                glColor3f(0.0, 0.0, 0.0);
                glPushMatrix();
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();

                char statusBuffer[512];
                if(fpsec<100.0) {
                        snprintf(statusBuffer, 510, "fps: %6.3f", fpsec);
                } else {
                        snprintf(statusBuffer, 510, "fps: >100");
                }
                int len, i;
                glRasterPos3f( 0.8,  0.9, 0);
                len = (int) strlen(statusBuffer);
                for (i = 0; i < len; i++) {
                        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, statusBuffer[i]);
                }

                glPopMatrix();
                glEnable(GL_DEPTH_TEST);
                //glFlush();
        }
        glFlush();

  /* and show... */
        glPopMatrix();
}



//-----------------------------------------------------------------------------
// open gl(ut) reshape method 
void DotVisualizer::reshape(int width, int height)
{
  mWindowX = (GLint)width;
  mWindowY  = (GLint)height;
  mWindowAspect = (GLdouble)mWindowX / (GLdouble)mWindowY;
  
  glViewport(0, 0, mWindowX, mWindowY);  
        if(!mpGraph) return;
        mpMapC->init_view(mWindowX,mWindowY);
        mpGLC->set_screensize(mWindowX,mWindowY);
}

//-----------------------------------------------------------------------------
// handle mouse press
void DotVisualizer::handleMouse(int button, int state, int x, int y)
{
        float newx, newy; // map click?
        if(!mpGraph) return;

  mMouseLastX = x;
  mMouseLastY = y;
  switch (button) {
  case 1:
    if(state == 0) {
      mButtonCurrent = mButtonCurrent & (~VIS_BUTTONL);
                        mOnMap = false;
                        mOnNode = false;
    } else {
      mButtonCurrent = mButtonCurrent | VIS_BUTTONL;
                        mOnMap = mpMapC->check_click(x,y, &newx, &newy);
                        if(!mOnMap) {
                                // try to select node under cursor
                                mOnNode = mpGLC->select_click( mpGraph );
                                if(mOnNode) {
                                        if(mpGLC->get_nodeposition( &mTargetX, &mTargetY, &mTargetZ, &mTargetLookX, &mTargetLookY )) 
                                                mTrackTarget = true;
                                } else {
                                        // not clicked on map or node...
                                        mTrackTarget = false;
                                }
                        }
    }
    break;
  case 2:
    if(state == 0) {
      mButtonCurrent = mButtonCurrent & (~VIS_BUTTONM);
    } else {
      mButtonCurrent = mButtonCurrent | VIS_BUTTONM;
    }
    break;
  case 3:
    if(state == 0) {
      mButtonCurrent = mButtonCurrent & (~VIS_BUTTONR);
    } else {
      mButtonCurrent = mButtonCurrent | VIS_BUTTONR;
    }
                mTrackTarget = false;
    break;
  case 4: // wheel up
                if(mCamZoomPos<mCamZ) mCamZoomPos = mCamZ;
    mCamZoomPos += wheelSpeed * transScale * (mCamZoomPos/-100.0);
                break;
  case 5: // wheel up
                if(mCamZoomPos>mCamZ) mCamZoomPos = mCamZ;
    mCamZoomPos -= wheelSpeed * transScale * (mCamZoomPos/-100.0);
                break;
  default:
    //cout << "DotVisualizer::glutFuncMouse Error: unknown button ??? " << button << "\n"; // debug
    break;
  }

        // check if the map was clicked, and cam moved
        if(mOnMap) {
                float dx = newx - mCamX;
                float dy = newy - mCamY;
                mCamX += dx;
                mCamY += dy;
                mLookatX += dx;
                mLookatY += dy;
                mTrackTarget = false;

                redraw();
                mpGLC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
                mpMapC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        } else {
        }
        mLastAction = getTime();
}


//-----------------------------------------------------------------------------
void DotVisualizer::handleDragging(int x, int y)
{
        if(!mpGraph) return;
  /* translate? */
        if( (!mOnMap) &&
                        (!mOnNode) 
                ){
                if(mButtonCurrent==VIS_BUTTONL) {
                        mCamX += (mMouseLastX-x) * -transScale * (mCamZ/((mWindowY/(float)mWindowX)*mWindowX*-0.25));
                        mCamY += (mMouseLastY-y) *  transScale * (mCamZ/(mWindowY*-0.25));
                        mLookatX += (mMouseLastX-x) * -transScale * (mCamZ/((mWindowY/(float)mWindowX)*mWindowX*-0.25));
                        mLookatY += (mMouseLastY-y) *  transScale * (mCamZ/(mWindowY*-0.25));
                }
                if( (mButtonCurrent== (VIS_BUTTONL|VIS_BUTTONR) ) ||
                                (mButtonCurrent==VIS_BUTTONM) ) {
                        mCamZ += (mMouseLastY-y) * transScale * (mCamZ/-100.0);
                        if(mCamZ > -minZDist) mCamZ = -minZDist;
                        mCamZoomPos = mCamZ;
                }
                /* rotate? */
                if(mButtonCurrent==VIS_BUTTONR) {
                        mLookatX += (mMouseLastX-x) * -transScale * (mCamZ/((mWindowY/(float)mWindowX)*mWindowX*-0.25));
                        mLookatY += (mMouseLastY-y) *  transScale * (mCamZ/(mWindowY*-0.25));
                }
                mpGLC->set_mousepos(x,y);
        } else {
                if(mOnMap) {
                        float newx, newy; // map click?
                        mpMapC->check_click(x,y, &newx, &newy);
                        float dx = newx - mCamX;
                        float dy = newy - mCamY;
                        mCamX += dx;
                        mCamY += dy;
                        mLookatX += dx;
                        mLookatY += dy;
                        mpGLC->set_mousepos(-1,-1);
                }
                if(mOnNode) {
                        return;
                }
        }

        redraw();
        mpGLC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        mpMapC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );

  mMouseLastX = x;
  mMouseLastY = y;
        mLastAction = getTime();
}


//-----------------------------------------------------------------------------
// there's nothing else to do, so animat&redraw
void DotVisualizer::idle( void )
{
  /* animation? */
        long time = getTime();
        if((mLastTime>0)&&(time - mLastTime > 5)) {
                animate( time );
                mLastTime = time;
        }
        if(mLastTime<0) mLastTime = time;

  /* idle done... */
}


//-----------------------------------------------------------------------------
// handle animations
void DotVisualizer::animate( long t )
{
        if(!mpGraph) return;
        bool updateCam = false;
        // fading
        if((mLastAction>0)&&(t-mLastAction>MAP_FADEOUT_DELAY)) {
                if(t-mLastAction > (5000+MAP_FADEOUT_DELAY)) {
                        mMapFade = 0.2;
                } else {
                        float fadestate = 1.0- ((t-mLastAction-MAP_FADEOUT_DELAY)/ MAP_FADEOUT_SPEED);
                        mMapFade =  0.2+(fadestate*0.8);
                }
        } else {
                if(mMapFade<1.0) mMapFade += (t-mLastTime)/ MAP_FADEIN_SPEED;
                if(mMapFade>1.0) mMapFade = 1.0;
                if(mLastAction<0) mLastAction = getTime();
        }
        mpMapC->set_fade( mMapFade ); 

        // zoom
        if( mCamZoomPos > -minZDist ) mCamZoomPos = -minZDist;
        if( ABS(mCamZ-mCamZoomPos)>2.0 ) {
                if((t-mLastTime)> MAP_ZOOMSPEED) mCamZ = mCamZoomPos;
                else {
                        mCamZ += (-mCamZ+mCamZoomPos)*(t-mLastTime)/MAP_ZOOMSPEED;
                        //cout << mCamZ << " " << mCamZoomPos << endl;
                }
                updateCam = true;
        }

        // track targets
        if(mTrackTarget) {
                // camera pos
                if( ABS(mCamX-mTargetX)>2.0 ) {
                        if((t-mLastTime)> TRACK_CAMSPEED) mCamX = mTargetX;
                        else {
                                mCamX += (-mCamX+mTargetX)*(t-mLastTime)/TRACK_CAMSPEED;
                        }
                        updateCam = true;
                }
                if( ABS(mCamY-mTargetY)>2.0 ) {
                        if((t-mLastTime)> TRACK_CAMSPEED) mCamY = mTargetY;
                        else {
                                mCamY += (-mCamY+mTargetY)*(t-mLastTime)/TRACK_CAMSPEED;
                        }
                        updateCam = true;
                }
                // lookat
                if( ABS(mLookatX-mTargetLookX)>2.0 ) {
                        if((t-mLastTime)> TRACK_CAMSPEED) mLookatX = mTargetLookX;
                        else {
                                mLookatX += (-mLookatX+mTargetLookX)*(t-mLastTime)/TRACK_LOOKATSPEED;
                        }
                        updateCam = true;
                }
                if( ABS(mLookatY-mTargetLookY)>2.0 ) {
                        if((t-mLastTime)> TRACK_CAMSPEED) mLookatY = mTargetLookY;
                        else {
                                mLookatY += (-mLookatY+mTargetLookY)*(t-mLastTime)/TRACK_LOOKATSPEED;
                        }
                        updateCam = true;
                }
        } else {
                mTargetX = mCamX;
                mTargetY = mCamY;
                mTargetZ = mCamZ;
                mTargetLookX = mLookatX;
                mTargetLookY = mLookatY;
        }
        
        if(updateCam) {
                mpGLC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
                mpMapC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        }
}


//-----------------------------------------------------------------------------
int DotVisualizer::handleKey(int key)
{
        if(!mpGraph) return false;
        //cout << " K " << key<< endl; // debug
  switch (key) {
    
  case FL_Escape:
  case 'q':
    /* quit program */
    cout << "\nQuit...\n";
                globalQuit = true;
                break;

  //case 'R':
  case 'r':
                if(Fl::event_state() & FL_SHIFT) {
    /* reset camera */
                mTargetX = (mpGraph->u.bb.LL.x+mpGraph->u.bb.UR.x)*-0.5;
                mTargetY = (mpGraph->u.bb.LL.y+mpGraph->u.bb.UR.y)*-0.5;
                mCamZoomPos = ((-mpGraph->u.bb.LL.x+mpGraph->u.bb.UR.x)+(-mpGraph->u.bb.LL.y+mpGraph->u.bb.UR.y))*-0.25;
                mTargetLookX = mTargetX; 
                mTargetLookY = mTargetY; 
                mTargetZ = mCamZoomPos;
                } else {
                //mCamZoomPos = mCamZoomPos;
                //mTrackTarget = true;
    //break;  
    /* reset lookat */
                if(!mTrackTarget) {
                        mTargetX = mCamX; 
                        mTargetY = mCamY; 
                } 
                }
                mTargetLookX = mTargetX; 
                mTargetLookY = mTargetY; 
                mTrackTarget = true;
    break;  

                // VIM like navigation
  case 'h':
  case FL_Right:
                mpGLC->select_previous_edge();
                if(mpGLC->get_edgeposition( &mTargetX, &mTargetY, &mTargetZ, &mTargetLookX, &mTargetLookY ))
                        mTrackTarget = true;
    break;
  case 'l':
  case FL_Left:
                mpGLC->select_next_edge();
                if(mpGLC->get_edgeposition( &mTargetX, &mTargetY, &mTargetZ, &mTargetLookX, &mTargetLookY ))
                        mTrackTarget = true;
    break;
  case 'k':
  case FL_Down:
                mpGLC->select_parent( mpGraph );
                if(mpGLC->get_nodeposition( &mTargetX, &mTargetY, &mTargetZ, &mTargetLookX, &mTargetLookY )) 
                        mTrackTarget = true;
    break;
  case 'j':
  case FL_Up:
                mpGLC->select_follow_edge( mpGraph );
                if(mpGLC->get_nodeposition( &mTargetX, &mTargetY, &mTargetZ, &mTargetLookX, &mTargetLookY )) 
                        mTrackTarget = true;
    break;
  case 'u': // same as wheel up
  case FL_Page_Up:
                if(mCamZoomPos<mCamZ) mCamZoomPos = mCamZ;
    mCamZoomPos += wheelSpeed * transScale * (mCamZoomPos/-100.0);
                break;
  case 'd': // same as wheel down
  case FL_Page_Down:
                if(mCamZoomPos>mCamZ) mCamZoomPos = mCamZ;
    mCamZoomPos -= wheelSpeed * transScale * (mCamZoomPos/-100.0);
                break;


  default:
    //cout << "DotVisualizer::glutFuncKeys : Pressed key "<< key <<"\n";
    return false;
  }

        mpGLC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        mpMapC->set_camera_position( mCamX, mCamY, mCamZ, mLookatX, mLookatY );
        mLastAction = getTime();
        redraw();
        return true;
}



/******************************************************************************
 * FLTK functions
 *****************************************************************************/



//-----------------------------------------------------------------------------
void DotVisualizer::callbackFunc(Fl_Widget *w) {
        cout << " CB " << (int)w << endl;
}

//-----------------------------------------------------------------------------
int DotVisualizer::handle(int event) {
        if(!mpGraph) return false;
        //cout << " EV " << (int)event << endl; // debug
        switch( event ) {

                case FL_PUSH: {
                        int x = Fl::event_x();
                        int y = Fl::event_y();
                        int mbid = Fl::event_button();
                        handleMouse(mbid,1,x,y);
                        return true; }

                case FL_DRAG: {
                        int x = Fl::event_x();
                        int y = Fl::event_y();
                        handleDragging(x,y);
                        return true; }

                case FL_RELEASE: {
                        int x = Fl::event_x();
                        int y = Fl::event_y();
                        int mbid = Fl::event_button();
                        handleMouse(mbid,0,x,y);
                        return true; }

                case FL_KEYBOARD:
                        return handleKey( Fl::event_key() );

                case FL_SHORTCUT:
                        return handleKey( Fl::event_key() );

                case FL_MOVE: {
                        int x = Fl::event_x();
                        int y = Fl::event_y();
                        mpGLC->set_mousepos(x,y);
                        mpMapC->set_mousepos(x,y);
                        mLastAction = getTime();
                        return true; }
                case FL_MOUSEWHEEL: {
                        int dy = Fl::event_dy();
                        //cout << " wheel!!!!!!!!!!!11 " << (int)dy << endl; // debug
                        mCamZoomPos += wheelSpeed * transScale * (mCamZoomPos/100.0) * (float)dy;
                        if(dy<0) {
                                if(mCamZoomPos<mCamZ) mCamZoomPos = mCamZ;
                        } else {
                                if(mCamZoomPos>mCamZ) mCamZoomPos = mCamZ;
                        }
                        return true; }

                case FL_FOCUS:
                        // we got the focus...
                        return true;
                case FL_UNFOCUS:
                        // damn, its gone
                        return true;
                case FL_ENTER:
                        // get the focus if the mouse is here...
                        focus( this );
                        return true;
                case FL_LEAVE:
                        return true;
                default:
                        // do nothing...
                        return false;
        }
        return false;
}


//-----------------------------------------------------------------------------
// 
void DotVisualizer::loadFile(string filename) {

        // sth. open?
        if(mpGraph) {
                closeGraph();
        }

        // get first graph
        FILE *fp = fopen( filename.c_str(), "r" );
        if(!fp) {
                cerr << "dotgl - unable to load file " << filename << endl;
                return;
        }
        Agraph_t *G = agread( fp );
        fclose(fp);

        if(G) {
                cout << "DOT - graph name: "<< G->name << endl;

                int layout = 2;         // layout gaph with dot? 0=dot,1=neato,2=nop
                if(layout==0) {
                        dot_layout( G );
                } else if(layout==1) {
                        neato_layout( G );
                } else {
                        Nop = 2;
                        PSinputscale = POINTS_PER_INCH;
                        neato_init_graph( G );
                        //for (Agnode_t *node = agfstnode(G); node!=NULL; node = agnxtnode(G,node)) { if(E_label) { char *label         = agxget(node, E_label->index ); cout << label << " at " << node->u.coord.x <<","<< node->u.coord.y <<" - w:"<< node->u.width << ",h:" << node->u.height << endl; } }
                        //dot_init_graph( G );
                        init_nop( G );
                        dotneato_postprocess(G, neato_nodesize);
                }

                Agsym_t *E_label  = NULL;
                //E_label  = agfindattr( G->proto->n, "label" );

                Agsym_t *E_comment  = agfindattr( G->proto->n, "comment" );
                Agsym_t *E_test  = agfindattr( G->proto->n, "blub" );

                /*Agsym_t **list = G->univ->globattr->list;
                for(int i=0; list[i] != NULL; i++) {
                        if(list[i]) { cout << " ATTR "<<i<<" "<<list[i]->name<<"="<<list[i]->value <<endl; }
                }
                list = G->univ->nodeattr->list;
                for(int i=0; list[i] != NULL; i++) {
                        if(list[i]) { cout << " ATTR "<<i<<" "<<list[i]->name<<"="<<list[i]->value <<endl; }
                }
                list = G->univ->edgeattr->list;
                for(int i=0; list[i] != NULL; i++) {
                        if(list[i]) { cout << " ATTR "<<i<<" "<<list[i]->name<<"="<<list[i]->value <<endl; }
                }*/

                // output nodes
                long mNodeCnt = 0;
                long mEdgeCnt = 0;
                for (Agnode_t *node = agfstnode(G); node!=NULL; node = agnxtnode(G,node)) {
                        if(E_comment) {
                                char *comm      = agxget(node, E_comment->index );
                                cout << "Comment:"<<comm << endl;
                        }
                        if(E_test) {
                                char *comm      = agxget(node, E_test->index );
                                cout << "Blatest:"<<comm << endl;
                        }
                        if(E_label) {
                                char *label     = agxget(node, E_label->index );
                                cout << label << " at " << node->u.coord.x <<","<< node->u.coord.y <<" - w:"<< node->u.width << ",h:" << node->u.height << endl; // debug
                        }
                        mNodeCnt++;
                }

                // output edges
                for (Agnode_t *node = agfstnode(G); node!=NULL; node = agnxtnode(G,node)) {
                        for (Agedge_t *edge = agfstout(G,node); edge!=NULL; edge = agnxtout(G,edge)) {
                                mEdgeCnt++;
                                //cout << "edge at " << " " << ""; // debug
                                //cout << " head " << edge->u.head_port.p.x << " " << edge->u.head_port.p.y << endl; // debug
                                //if(ED_spl(edge)) {
                                        // loop over all bezier curves
                                        //for (int i = 0; i < ED_spl(edge)->size; i++) {
                                                //bezier bz = ED_spl(edge)->list[i];
                                                // print bezier points
                                                //cout << " s"<<bz.sp.x <<","<< bz.sp.y <<"   "; // debug
                                                //for (int j = 0; j < bz.size; j++) {
                                                        //cout << bz.list[j].x <<","<< bz.list[j].y <<"   "; // debug
                                                //}
                                                //cout << " e"<<bz.ep.x <<","<< bz.ep.y <<"   "; // debug
                                        //}

                                //} else {
                                        // no line?
                                        //cout << " NOL "; // debug
                                //}
                                //cout << endl;
                        }
                }
        //cout<<" gtt "<<glutGet(GLUT_ELAPSED_TIME) << endl;
                cout << " Graph stats for '" << "" <<"', nodes:"<<mNodeCnt<<", edges:"<<mEdgeCnt<<" "<<endl;

                mpGraph = G;             
                initGraph();
                mLastTime = getTime();
                if(!mFirstSelect) {
                        mpGLC->select_node( mpGraph, agfstnode(G) );
                        mFirstSelect = true;
                }
        } else {
                cerr << "dotgl - unable to load file " << filename << endl;
                mpGraph = NULL;
                return;
        }
        
}

//-----------------------------------------------------------------------------
// 
void DotVisualizer::closeGraph() {
        if(!mpGraph) return;
        // clean up, free graph
        dot_cleanup( mpGraph );
        agclose( mpGraph );
        mpGraph = NULL;

        delete mpGLC;
        delete mpMapC;
        mpGLC = NULL;
        mpMapC = NULL;
}


//-----------------------------------------------------------------------------
// menu callback functions, for loading a graph
void DotVisualizer::displayLoadFileDialog()
{
        Fl_File_Chooser *choose = new Fl_File_Chooser(".", "DOT File (*.dot)\tDOT File with Layout (*.pdot)", Fl_File_Chooser::SINGLE, "Load DOT Graph");
        choose->show();
        while(choose->visible()) Fl::wait();
        if(!choose->value()) return;
        string filename( choose->value() );
        delete choose;
                        
        loadFile( filename );
}


//-----------------------------------------------------------------------------
// menu callback functions, for closing the current graph
void DotVisualizer::displayCloseDialog()
{
        closeGraph();
}


//-----------------------------------------------------------------------------
// menu callback functions, for layouting the current graph
void DotVisualizer::performDotLayout()
{
        if(!mpGraph) return;
        dot_layout( mpGraph );
        initGraph();
}
void DotVisualizer::performNeatoLayout()
{
        if(!mpGraph) return;
        neato_layout( mpGraph );
        initGraph();
}


//-----------------------------------------------------------------------------
// menu callback functions, for setting display detail
void DotVisualizer::setDisplayDetail(int detail)
{
        switch(detail) {
                case 2: // high
                        mpGLC->setDetail(2);
                        break;
                case 1: // med
                        mpGLC->setDetail(1);
                        break;
                case 0: // low
                        mpGLC->setDetail(0);
                        break;
                default:
                        cerr << "DotVisualizer::setDisplayDetail invalid setting: " << detail << endl;
                        return;
        }
}

//-----------------------------------------------------------------------------
// menu callback functions, for setting FPS display
void DotVisualizer::setToggleFPS() {
        mShowFPS = !mShowFPS;
}


//-----------------------------------------------------------------------------
// menu callback functions, for setting FPS display
void DotVisualizer::setToggleLock() {
        mShowLock = !mShowLock;
}

                
