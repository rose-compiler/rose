/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * 
 *
 *****************************************************************************/

#ifndef GLCODEGEN_H
#define GLCODEGEN_H


#include <stdio.h>
#include <iostream.h>
#include <string.h>
#include <vector>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "glf.h"
#include "dotvisualizer.h"
#include "ntl_vector3dim.h"

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::vector;

// subdivision for ellipses and bezier curves
#define SUBDIVISION_STEPS 25

// depth to extrude nodes
#define EXTRUDE_DEPTH 10.0

//-----------------------------------------------------------------------------

// no need to include the header file...
class GraphInfo;

// include dot/neato functions
extern "C" {
#include <dotneato.h>
};

// square macro
#define SQR(x) ((x)*(x))

#define CONTEXT_STACK_DEPTH 		10
typedef struct contextType {
        float 	penCol[3], fillCol[3];
	char 	*fontName;
	double  fontSize;
	int	sameFillCol;
} contextType;

// culling status - undecided, tested - dont display, tested - display
#define CULL_UNDECIDED	 	0
#define CULL_DONTDISPLAY 	1
#define CULL_DISPLAY 		2

// node display status
#define DISPSTATE_OFF		0		// nothing special...
#define DISPSTATE_ONNODE	1		// normal node display
#define DISPSTATE_NODESEL	2		// selected node
#define DISPSTATE_EDGESEL	3		// selected edge
#define DISPSTATE_NODEHIGH	4		// node highlighted by mouseover

// level of detail screen sizes
#define DETAIL_TEXT_BOX		1.5
#define DETAIL_TEXT_REMOVE	0.75
#define DETAIL_BEZIER_LINE	10.0
#define DETAIL_POLY_DEPTH	2.0
#define DETAIL_ELLIPSE_LINE	5.0
#define DETAIL_ELLIPSE_MINSUBDIV 8
#define DETAIL_ELLIPSE_BOX	7.0



// gl codegen state struct
class GlCodegenState {
public:
	GlCodegenState( Agraph_t *g );
	void draw( void );
	void reset();

	//! common access functions
	inline contextType *getCS() { return &(cStack[cs]); };
	void set_viewingregion(pointf f[]);
	void set_mousepos(int x, int y);
	void set_screensize(int sx, int sy);
	void set_camera_position(float x, float y, float z, float lx, float ly );

	//! set graph info widet pointer
	void setGraphInfo(GraphInfo *set) { mpGraphInfo = set; }

	int isVisibleF(pointf A[],int n);
	int isVisibleI(point A[],int n);
	int isVisibleReducedF(pointf A[],int n);
	double getCameraDistanceF(pointf A[],int n);
	double getScreenDistanceF2(float a1x, float a1y, float a1z, float a2x, float a2y, float a2z );
	void select_fill_color( void );
	void select_pen_color( void );
	void select_node(graph_t *pGraph, node_t *node);
	void select_next_edge( void );
	void select_previous_edge( void );
	void select_follow_edge( graph_t *pGraph );
	void select_parent( graph_t *pGraph );
	int select_click( graph_t *pGraph );
	int get_nodeposition(float *tx, float *ty, float *tz, float *tlx, float *tly );
	int get_edgeposition(float *tx, float *ty, float *tz, float *tlx, float *tly );

	void setDetail(int detail);

	// member variables, currently almost everthing is public :(

	contextType cStack[CONTEXT_STACK_DEPTH];	// context stack
	int 		cs;																// context stack size
	double 	fontScale; 				// scale for loaded font
	//node_t *currNode;

	int	EnhanceDisplay;		                // enhance DOT display? all effects are off when this is false
	int	Culling;			        // culling enabled? dont display things not in viewing region
	int 	CullTests;			        // how many culling tests were performed per frame?
	int 	ObjectsDisplayed;		        // how many object were displayed?
	int 	ObjectsTotal;			        // how many objects in total?
	pointf 	ViewRegion[4];				// vectors in z-plane of viewing region
	pointf 	VRLL,VRUR;				// maximized rectangular viewing region
	int	DispState;				// for node display enhancement
	float 	CamX, CamY, CamZ;			// camera position from DotViz
	float 	LookatX, LookatY;    			// camera look at
	short 	MouseX, MouseY;				// mouse position from dotviz
	short 	ViewSizeX, ViewSizeY; 			// viewport/window size
	float	MousePlaneX, MousePlaneY;		// mouse position in graph plane, for node selection
	GLint 	Viewp[4];			        // gl viewport, for unproject
	GLdouble Mvmat[16], Projmat[16];		// projection matrices, for unproject

	// graph navigation
	node_t	*NodeSelected;				// selected node
	node_t	*NodeUndermouse;			// node under the mouse cursor
	int	EdgeSelected;		                // selected edge (index to gSelNodeEdges vector)
	vector<edge_t*> SelNodeEdges;			// all edges of the selected node
	node_t	*SelNodeParent;				// pointer to the parent node (if unique)

	int 	LastViewTrack; 				// remember if last view movement was for edge or node
	double 	ZPos;					// z position currently fixed

	double mDetailFactor;				// detail settings low/med/high

protected:
	
	Agraph_t *pGraph;				// graph to work with	
	GraphInfo *mpGraphInfo;				// graph info FLTK widget, for node info display upon selection

};


#endif

