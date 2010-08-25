/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * map overview display (header)
 *
 *****************************************************************************/

#ifndef MAPCODEGEN_H
#define MAPCODEGEN_H

#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "glf.h"
#include "ntl_vector3dim.h"

/* globals for drawing state */
//static float penCol[3];
//static float fillCol[3];
//static float Col[3];


// include dot/neato functions
extern "C" {
#include <dotneato.h>
};
void glmap_show_view( void );


// scale graph coordinates to map OpenGL ones
#define MSX(x) ( (x)*mapScale +mapOffsetX )
#define MSY(y) ( (y)*mapScale +mapOffsetY )

// scale/move graph info display
#define GIX(x) ( (x)*giScale +giOffsetX )
#define GIY(y) ( (y)*giScale +giOffsetY )


// map codegen state vars
class MapCodegenState {
        public:
                
                MapCodegenState( Agraph_t *G );
                ~MapCodegenState();
                
                void init_ortho( graph_t *pGraph );
                void pop_ortho( void );
                void set_mousepos( int x, int y );
                void begin_graph(graph_t* pGraph, box , point );
                void end_graph(void);
                void begin_node(node_t* node);
                void display( graph_t *pGraph );
                void calc_viewingregion(pointf f[], double *minz, double *maxz);
                void show_view( void );
                bool check_click(int x, int y, float *camX, float *camY);
                void init_view(int width, int height);
                void set_camera_position(float x, float y, float z, float lx, float ly );
                void set_fade(float fade);

                void MapCodegenState::graphinfo_display(graph_t *pGraph);

        protected:

                float zpos;                                     // fixed z position for all objects
                float mapScale;                 // scale map (in graph cooridnates) to ortho coords
                float mapOffsetX;               // map shift x (calculated for each window size)
                float mapOffsetY;               // map shift y
                float mapOffset;    // offset from window frame
                float mapDestSize;   // target size of smaller border of map
                point bbll, bbur;                                       // bounding box extent
                float gCamX, gCamY, gCamZ;              // camera position from DotViz
                float gLookatX, gLookatY;    // camera look at
                short gViewSizeX, gViewSizeY; // viewport size
                float orthoX, orthoY; // ortho view size
                bool  gMouseIn;                         // mouse in map?

                float gFade;                                    // fading variable
                short gMousePosX, gMousePosY; // current mouse position
                pointf gViewRegion[4];                  // vectors in z-plane of viewing region

                float giOffset;   // offset from window frame
                float giScale;                  // scale graph info
                float giOffsetX;                // graph info shift x (calculated for each window size)
                float giOffsetY;                // graph info shift y

                Agraph_t *pGraph;
};

#endif

