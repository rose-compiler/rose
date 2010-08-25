/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * visualizer FLTK group
 *
 *****************************************************************************/

#ifndef VIZGROUP_H
#define VIZGROUP_H

#include <vector>
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Menu_Bar.h>
#include <FL/Fl_Tile.h>
#include "dotvisualizer.h"

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::vector;

class GraphInfo;

// viz array size
#define MAX_VIZ 4

class VizGroup :
        public Fl_Tile
{
        public:
                typedef enum {
                        single = 1,
                        splitHoriz,
                        splitVert,
                        triple,
                        quad,
                        threeone } VizGroupLayout;

                //! constructor
                VizGroup(int x,int y,int w,int h);

                //! destructor
                ~VizGroup();

                //! add visualizer widgets
                void addViz(int x,int y,int w,int h, int vi,Agraph_t *G);

                //! init the vizgroup layout
                void vizLayout(VizGroupLayout layout);

                //! redraw all visualizers when idle
                void doIdleRedraw();

                //! show all viz's
                void showAll();

                //! set pointer to graph info widget
                void setGraphInfo(GraphInfo *set) { mpGraphInfo = set; }

                //! load multiple files for startup
                void vizMultipleLoad( vector<string> files );
                
        private:

                //! which visualizer is active?
                bool mVizActive[MAX_VIZ];

                //! which visualizer is currently visible
                bool mVizVisible[MAX_VIZ];

                //! array of viz objects
                DotVisualizer *mpViz[MAX_VIZ];

                //! array of viz groups
                Fl_Group *mpGroup[MAX_VIZ];

                //! pointer to graph info widget for GLCodeGen
                GraphInfo *mpGraphInfo;
                
};


#endif


