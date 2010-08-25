/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * class to handle glut output to display graph+map
 *
 *****************************************************************************/

#ifndef MFFSLBM_VISUALIZER
#define MFFSLBM_VISUALIZER

#ifdef _WIN32
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <string>

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::string;

// FTLK
#include <FL/Fl.h>
#include <FL/gl.h>
#include <FL/Fl_Gl_Window.h>


// include dot/neato functions
extern "C" {
#include <dotneato.h>
};
class GlCodegenState;
class MapCodegenState;
class GraphInfo;


/*! class to display a DOT graph with OpenGL */
class DotVisualizer : public Fl_Gl_Window {

        public:
                /*! Constructor */
                DotVisualizer(Agraph_t *g, int x,int y, int w,int h);
                /*! Destructor */
                ~DotVisualizer();

                /*! init camera position and orientation */
                void initCamera( void );

                /*! function for idle animation */
                void idle( void );
                /*! handle an FLTK keydown event */
                int handleKey(int key);
                /*! handle dragging mouse motion */
                void handleDragging(int x, int y);
                /*! glut function for the mouse */
                void handleMouse(int button, int state, int x, int y);

                /*! glut function for window reshaping */
                void reshape(int width, int height);
                /*! glut function for displaying */
                void draw( void );

                /*! handle animations */
                void animate( long t );

                /*! finish init, set up solver viz */
                void finishInit( void );

                /*! get current time in ms */
                unsigned long getTime();

                /*! load a DOT file from disk */
                void loadFile(string filename);

                /*! init a graph that was just loaded */
                void initGraph();

                /*! release a graph previously opened */
                void closeGraph();

                /*! set pointer to graph info widget */
                void setGraphInfo(GraphInfo *set) { mpGraphInfo = set; }

        protected:

        private:

                /*! camera position */
                float mCamX, mCamY, mCamZ;
                /*! camera look at */
                float mLookatX, mLookatY;
                /*! window aspect */
                float mWindowAspect;

                /*! state of the mouse keys */
                int mButtonCurrent;
                /*! position of mouse click */
                int mMouseLastX, mMouseLastY;

                /*! pointer to the graph to display */
                Agraph_t *mpGraph;

                /*! has the user clicked on the map? */
                int mOnMap;

                /*! has the user clicked on a node? */
                int mOnNode;

                /*! time of last animation frame */
                long mLastTime;

                /*! time of last user action */
                long mLastAction;

                /*! map fading state */
                float mMapFade;

                /*! animated zooming */
                float mCamZoomPos;

                /*! current window size */
                int mWindowX, mWindowY;

                /*! maximum, minimum Z distance in scene */
                double mMaxZDistance, mMinZDistance;

                /*! target tracking enabled? */
                int mTrackTarget;

                /*! node selection position */
                float mTargetX, mTargetY, mTargetZ, mTargetLookX, mTargetLookY;

                /*! gl code generation state variables */
                GlCodegenState *mpGLC;

                /*! gl code generation state variables */
                MapCodegenState *mpMapC;

                /*! viz init time */
                unsigned long mFirstTime;

                /*! graph info widget pointer */
                GraphInfo *mpGraphInfo;

                /*! has a first node been selected? */
                bool mFirstSelect;

                /*! show FPS? */
                bool mShowFPS;

                /*! display lock on? */
                bool mShowLock;

        public: 

                /* FLTK functions */

                int handle( int event );
                //void resize(int x,int y,int w, int h);
                void callbackFunc(Fl_Widget *w);
                static void staticCallback(Fl_Widget *w, void *that) { ((DotVisualizer *)that)->callbackFunc( w ); }

                //! menu callback functions, for loading a graph
                void displayLoadFileDialog();
                static void menuLoadCB(Fl_Widget *w, void *v) { ((DotVisualizer *)v)->displayLoadFileDialog(); }

                //! menu callback functions, for closing the current graph
                void displayCloseDialog();
                static void menuCloseCB(Fl_Widget *w, void *v) {        ((DotVisualizer *)v)->displayCloseDialog(); }

                //! menu callback functions, for layouting graph with DOT
                void performDotLayout();
                static void menuPerformDotLayout(Fl_Widget *w, void *v) {       ((DotVisualizer *)v)->performDotLayout(); }
                //! menu callback functions, for layouting graph with NEATO
                void performNeatoLayout();
                static void menuPerformNeatoLayout(Fl_Widget *w, void *v) {     ((DotVisualizer *)v)->performNeatoLayout(); }

                //! menu callback functions, for setting display detail
                void setDisplayDetail(int detail);
                static void menuSetDetailHigh(Fl_Widget *w, void *v) {  ((DotVisualizer *)v)->setDisplayDetail(2); }
                static void menuSetDetailMed (Fl_Widget *w, void *v) {  ((DotVisualizer *)v)->setDisplayDetail(1); }
                static void menuSetDetailLow (Fl_Widget *w, void *v) {  ((DotVisualizer *)v)->setDisplayDetail(0); }

                //! menu callback functions, for toggling FPS display
                void setToggleFPS();
                static void menuToggleFPS(Fl_Widget *w, void *v) {      ((DotVisualizer *)v)->setToggleFPS(); }

                //! menu callback functions, for toggling display lock
                void setToggleLock();
                static void menuToggleLock(Fl_Widget *w, void *v) {     ((DotVisualizer *)v)->setToggleLock(); }
};

                


#endif
