/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * 
 *
 *****************************************************************************/

#include "glcodegen.h"
#include "graphinfo.h"

// prototypes
static void gl_polyline(point* A, int n);

// global pointer for current codegen state
GlCodegenState *g;


//-----------------------------------------------------------------------------
// codegen object funtions

GlCodegenState::GlCodegenState( Agraph_t *G ) {
        reset();
        pGraph = G;
}

void GlCodegenState::reset() {
        ZPos = 0.0;
        cs = 0;
        fontScale = 1.0; 
        EnhanceDisplay = true;                  
        Culling = CULL_UNDECIDED;
        CullTests = 0;                                          
        ObjectsDisplayed = 0;   
        ObjectsTotal = 0;               
        DispState = false;      
        NodeSelected = NULL;            
        NodeUndermouse = NULL;
        EdgeSelected = 0;               
        SelNodeParent = NULL;
        LastViewTrack = 0; 
        pGraph = NULL;
        mpGraphInfo = NULL;
        mDetailFactor = 1.0;
}



//-----------------------------------------------------------------------------
// tell DotViz camera position to OpenGL display
void GlCodegenState::set_camera_position(float x, float y, float z, float lx, float ly )
{
        CamX = x;
        CamY = y;
        CamZ = z;
        LookatX = lx;
        LookatY = ly;
}

//-----------------------------------------------------------------------------
// tell DotViz screen size
void GlCodegenState::set_screensize(int sx, int sy) {
        ViewSizeX = sx;
        ViewSizeY = sy;
}
//-----------------------------------------------------------------------------
// tell DotViz mouse position to OpenGL display
void GlCodegenState::set_mousepos(int x, int y)
{
        // remember position
        MouseX = x;
        MouseY = y;
        if((x<0)||(y<0)) return;

        // calculate intersection with graph plane
        ntlVec3d camera( -CamX, -CamY, -CamZ );
        ntlVec3d lookat( -LookatX, -LookatY, 0.0 );
        ntlVec3d direction = lookat - camera;
        double fovy = 90.0;
        double aspect = (double)ViewSizeX/ViewSizeY;
        /* calculate width of screen using perpendicular triangle diven by
         * viewing direction and screen plane */
        double screenWidth = direction.getNorm()*tan( (fovy*0.5/180.0)*M_PI );

        /* calculate vector orthogonal to up and viewing direction */
        ntlVec3d upVec(0.0, 1.0, 0.0);
        ntlVec3d rightVec( upVec.crossProd(direction) );
        rightVec.normalize();

        /* calculate screen plane up vector, perpendicular to viewdir and right vec */
        upVec = ntlVec3d( rightVec.crossProd(direction) );
        upVec.normalize();

        /* check if vectors are valid FIXME what to do if not? */
        if( (upVec==ntlVec3d(0.0)) || (rightVec==ntlVec3d(0.0)) ) {
          return;
        }

        /* length from center to border of screen plane */
        rightVec *= (screenWidth*aspect * -1.0);
        upVec *= (screenWidth * -1.0);

        /* calc edges positions */
        double zplane = 0.0;
        ntlVec3d mousedir =  direction - rightVec + upVec;
        mousedir += rightVec*2.0* (float)x/(float)ViewSizeX;
        mousedir -= upVec*2.0* (float)y/(float)ViewSizeY;

        if((zplane-mousedir[2])>1.0) { // only treat negative directions
                mousedir[0] /= (zplane-mousedir[2]); 
                mousedir[1] /= (zplane-mousedir[2]); 
        }
        MousePlaneX = camera[0] + mousedir[0]*camera[2];
        MousePlaneY = camera[1] + mousedir[1]*camera[2];
        //cout << gMousePlaneX<<","<<gMousePlaneY<<"   "<<gCamX<<","<<gCamY << endl;
}

//-----------------------------------------------------------------------------
// get  viewing region calculated by map
void GlCodegenState::set_viewingregion(pointf f[]) {
        for(int i=0;i<4;i++) {
                ViewRegion[i] = f[i];
        }
        VRLL = VRUR = ViewRegion[0];
        for(int i=1;i<4;i++) {
                if(ViewRegion[i].x<VRLL.x) VRLL.x = ViewRegion[i].x;
                if(ViewRegion[i].y<VRLL.y) VRLL.y = ViewRegion[i].y;
                if(ViewRegion[i].x>VRUR.x) VRUR.x = ViewRegion[i].x;
                if(ViewRegion[i].y>VRUR.y) VRUR.y = ViewRegion[i].y;
        }
}

//-----------------------------------------------------------------------------
// perform incodes visibilty test for float point array
int GlCodegenState::isVisibleF(pointf A[],int n) {
        //return false; // debug !!!
        CullTests++;
        short toc = 0;
        for(int i = 0; (i < n); i++) {
                short oc=0;
                if(A[i].x>=VRLL.x) oc |= (1<<0);
                if(A[i].x<=VRUR.x) oc |= (1<<1);
                if(A[i].y>=VRLL.y) oc |= (1<<2);
                if(A[i].y<=VRUR.y) oc |= (1<<3); 
                if(oc == (1|2|4|8))     return true; // trivial accept
                toc |= oc;
        }
        if(toc != (1|2|4|8))    return false; // trivial reject
        return true;
}

//-----------------------------------------------------------------------------
// perform incodes visibilty test for integer point array
int GlCodegenState::isVisibleI(point A[],int n) {
        //return false; // debug !!!
        CullTests++;
        short toc = 0;
        for(int i = 0; (i < n); i++) {
                short oc=0;
                if(A[i].x>=VRLL.x) oc |= (1<<0);
                if(A[i].x<=VRUR.x) oc |= (1<<1);
                if(A[i].y>=VRLL.y) oc |= (1<<2);
                if(A[i].y<=VRUR.y) oc |= (1<<3); 
                if(oc == (1|2|4|8))     return true; // trivial accept
                toc |= oc;
        }
        if(toc != (1|2|4|8))    return false; // trivial reject
        return true;
}

//-----------------------------------------------------------------------------
// perform incodes visibilty test for reduced viewsize (needed for tree traversal)
int GlCodegenState::isVisibleReducedF(pointf A[],int n) {
        //return false; // debug !!!
        pointf VRLL, VRUR;
        pointf center, diag;
        center.x        = (VRLL.x+VRUR.x)*0.5;
        center.y        = (VRLL.y+VRUR.y)*0.5;
        diag.x          = (VRUR.x-center.x)*0.6; // rescale view window
        diag.y          = (VRUR.y-center.y)*0.6;
        VRLL.x          = center.x - diag.x;
        VRLL.y          = center.y - diag.y;
        VRUR.x          = center.x + diag.x;
        VRUR.y          = center.y + diag.y;
        CullTests++;
        short toc = 0;
        for(int i = 0; (i < n); i++) {
                short oc=0;
                if(A[i].x>=VRLL.x) oc |= (1<<0);
                if(A[i].x<=VRUR.x) oc |= (1<<1);
                if(A[i].y>=VRLL.y) oc |= (1<<2);
                if(A[i].y<=VRUR.y) oc |= (1<<3); 
                //cout <<"  OC "<<" -    c["<<center.x<<","<<center.y<<"]  , d["<<diag.x<<","<<diag.y<<"]  - "<<gVRLL.x<<","<<gVRLL.y<<"<"<<gVRUR.x<<","<<gVRUR.y << endl;
                //cout <<"  OC "<<oc<<"    "<<A[i].x<<","<<A[i].y<<" - "<<VRLL.x<<","<<VRLL.y<<"<"<<VRUR.x<<","<<VRUR.y << endl;
                if(oc == (1|2|4|8))     return true; // trivial accept
                toc |= oc;
        }
        //cout <<" TOC "<<toc << endl;
        if(toc != (1|2|4|8))    return false; // trivial reject
        return true;
}

//-----------------------------------------------------------------------------
// caclulate minimal camera distance to points
double GlCodegenState::getCameraDistanceF(pointf A[],int n) {
        double min = 0.0;
        double zdist = ABS(CamZ-ZPos);
        min = zdist + ABS(CamX-A[0].x) + ABS(CamY-A[0].y);
        for(int i = 1; (i < n); i++) {
                double mt = zdist + ABS(CamX-A[i].x) + ABS(CamY-A[i].y);
                if(mt<min) min = mt;
        }
        return min;
}

//-----------------------------------------------------------------------------
// calculate screen distance for two scene points
inline double GlCodegenState::getScreenDistanceF2(float a1x, float a1y, float a1z, float a2x, float a2y, float a2z ) {
        //return 10000.0; // debug, switches off adaptive LOD
        GLdouble  wx1,wy1,wz1;
        GLdouble  wx2,wy2,wz2;
        gluProject (a1x, a1y, a1z          , Mvmat, Projmat, Viewp, &wx1, &wy1, &wz1);
        gluProject (a2x, a2y, a2z          , Mvmat, Projmat, Viewp, &wx2, &wy2, &wz2);
        return sqrt( SQR(wx1-wx2)+SQR(wy1-wy2) );
}

//-----------------------------------------------------------------------------
// set detail settings
void GlCodegenState::setDetail(int detail)
{
        switch(detail) {
                case 2:
                        mDetailFactor = 1.50;
                        break;
                case 0:
                        mDetailFactor = 0.50;
                        break;
                default:
                        mDetailFactor = 1.0;
                        break;
        }
}

//-----------------------------------------------------------------------------
// choose fill color by node/select state and node color
void GlCodegenState::select_fill_color( void ) {
        ntlVec3d color;

        if(DispState == DISPSTATE_ONNODE) {
                //glColor3f(0.5,0.5,0.5);
                if( ( getCS()->fillCol[0] > 0.99 ) && ( getCS()->fillCol[1] > 0.99 ) && ( getCS()->fillCol[2] > 0.99 ) ) {
                        color = ntlVec3d(0.8,0.8,0.8);
                } else if( ( getCS()->fillCol[0] < 0.01 ) && ( getCS()->fillCol[1] < 0.01 ) && ( getCS()->fillCol[2] < 0.01 ) ) {
                        color = ntlVec3d(0.8,0.8,0.8);
                } else {
                        color = ntlVec3d(getCS()->fillCol[0],getCS()->fillCol[1],getCS()->fillCol[2]);
                }
        } else if(DispState == DISPSTATE_NODESEL) {
                color = ntlVec3d(1.0,0.0,0.0);
        } else if(DispState == DISPSTATE_EDGESEL) {
                color = ntlVec3d(1.0,0.0,0.0);
        } else if(DispState == DISPSTATE_NODEHIGH) {
                color = ntlVec3d(0.0,1.0,0.0);
        } else {
                // standard
                color = ntlVec3d(getCS()->fillCol[0],getCS()->fillCol[1],getCS()->fillCol[2]);
        }

        if(EnhanceDisplay) {
                // FIXME create display lists?
                //glColor3f( color[0], color[1], color[2] );
                //GLfloat MaterialAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f };
                GLfloat MaterialDiffuse[]= { color[0], color[1], color[2], 1.0f };
                //GLfloat MaterialSpecular[]= { 0.8f, 0.8f, 0.8f, 1.0f };
                //GLfloat MaterialSpecular[]= { 0.0f, 0.0f, 0.0f, 1.0f };
                GLfloat MaterialShininess[]= { 10.0f };
                //glMaterialfv(GL_FRONT, GL_AMBIENT, MaterialAmbient);
                glMaterialfv(GL_FRONT, GL_DIFFUSE, MaterialDiffuse);
                //glMaterialfv(GL_FRONT, GL_SPECULAR, MaterialSpecular);
                glMaterialfv(GL_FRONT, GL_SHININESS, MaterialShininess);
        } else {
                glColor3f( color[0], color[1], color[2] );
        }
}

void GlCodegenState::select_pen_color( void ) {
        if(DispState == DISPSTATE_NODESEL) {
                glColor3f(1.0,0.0,0.0);
        } else if(DispState == DISPSTATE_EDGESEL) {
                glColor3f(1.0,0.0,0.0);
        } else {
                // standard
                glColor3f(getCS()->penCol[0],getCS()->penCol[1],getCS()->penCol[2]);
                //glColor3f(1,1,1);
        }
}

//-----------------------------------------------------------------------------
// graph navigation functions
void GlCodegenState::select_node(graph_t *pGraph, node_t *node)
{
        SelNodeEdges.clear();
        // first find all successors
        for (Agedge_t *edge = agfstout(pGraph,node); edge!=NULL; edge = agnxtout(pGraph,edge)) {
                SelNodeEdges.push_back( edge );
        }
        // find all edges pointing to this one
        int parentCnt = 0;
        SelNodeParent = NULL;
        for (Agnode_t *inode = agfstnode(pGraph); inode!=NULL; inode = agnxtnode(pGraph,inode)) {
                for (Agedge_t *edge = agfstout(pGraph,inode); edge!=NULL; edge = agnxtout(pGraph,edge)) {
                        if(edge->head == node) {
                                SelNodeEdges.push_back( edge );
                                SelNodeParent = edge->tail;
                                parentCnt++;
                        }
                }
        }
        EdgeSelected = 0;
        NodeSelected = node;
        if(mpGraphInfo) {
                mpGraphInfo->showInfo( NodeSelected );
        }
}

void GlCodegenState::select_next_edge( void )
{
        if(!NodeSelected) return;
        if(SelNodeEdges.size() == 0) return;
        EdgeSelected++;
        if(EdgeSelected >= (int)SelNodeEdges.size()) EdgeSelected = 0;
}

void GlCodegenState::select_previous_edge( void )
{
        if(!NodeSelected) return;
        if(SelNodeEdges.size() == 0) return;
        EdgeSelected--;
        if(EdgeSelected<0) EdgeSelected = SelNodeEdges.size()-1;
}

void GlCodegenState::select_follow_edge( graph_t *pGraph )
{
        if(!NodeSelected) return;
        if(SelNodeEdges.size() == 0) return;
        node_t *target;
        edge_t *source = SelNodeEdges[EdgeSelected];
        if(source->tail == NodeSelected) target = source->head;
        else target = source->tail;

        select_node( pGraph, target );
}

void GlCodegenState::select_parent( graph_t *pGraph )
{
        if(!NodeSelected) return;
        if(!SelNodeParent) return;
        select_node( pGraph, SelNodeParent );
}

int GlCodegenState::select_click( graph_t *pGraph )
{
        if(!NodeUndermouse) return false;
        select_node( pGraph, NodeUndermouse );
        return true;
}

//-----------------------------------------------------------------------------
// return lookat position for open gl display (focus on selected node)
int GlCodegenState::get_nodeposition(float *tx, float *ty, float *tz, float *tlx, float *tly )
{
        if(!NodeSelected) return false; // dont modify
        // check if target is visible
        pointf A[1];
        A[0].x = NodeSelected->u.coord.x;
        A[0].y = NodeSelected->u.coord.y;

        if(LastViewTrack == 1)
        {
                // always override last edge movement
                if(isVisibleReducedF(A, 1)) 
                        return false;
        }

        *tx = -A[0].x;
        *ty = -A[0].y;
        *tz = ZPos;
        *tlx = -A[0].x;
        *tly = -A[0].y;
        LastViewTrack = 1;
        return true;
}

//-----------------------------------------------------------------------------
// return lookat position for open gl display (focus on selected edge)
int GlCodegenState::get_edgeposition(float *tx, float *ty, float *tz, float *tlx, float *tly )
{
        if(!NodeSelected) return false; // dont modify
        if(SelNodeEdges.size() == 0) return false; // there are no edges
        // check if target is visible
        pointf A[2];
        edge_t *source = SelNodeEdges[EdgeSelected];
        A[0].x = NodeSelected->u.coord.x;
        A[0].y = NodeSelected->u.coord.y;
        A[1].x = (source->head->u.coord.x + source->tail->u.coord.x)*0.5;
        A[1].y = (source->head->u.coord.y + source->tail->u.coord.y)*0.5;
        if( (isVisibleReducedF(&A[0], 1)) &&
            (isVisibleReducedF(&A[1], 1)) ) return false;

        *tx = -A[0].x;
        *ty = -A[0].y;
        *tz = ZPos;
        *tlx = -A[1].x;
        *tly = -A[1].y;
        LastViewTrack = 2;
        return true;
}


//-----------------------------------------------------------------------------
// the following function interface with the graphviz package, and
// need the global "g" pointer to function properly

//-----------------------------------------------------------------------------
// as of yet unused dot codegen emitting functions

static void gl_reset(void) { }

static void gl_begin_job(FILE *ofp,graph_t *g, char **lib, char *user, char *info[], point pages) { }

static  void gl_end_job(void) { }

static void gl_comment(void* obj, attrsym_t* sym) { }

static void gl_begin_page(graph_t *g, point page, double scale, int rot, point offset) { }

static void gl_end_page(void) { }

static void gl_begin_cluster(graph_t* g) { }

static void gl_end_cluster(void) { }

static void gl_begin_nodes(void) { }
    
static void gl_end_nodes(void) { }

static void gl_begin_edges(void) { }            

static void gl_end_edges(void) { }            

static void gl_user_shape(char *name, point *A, int sides, int filled) { }

//static void gl_arrowhead( void ) { }

static void gl_set_style(char** s) { }

char * gl_string(char *s) { return NULL; }


//-----------------------------------------------------------------------------
static void gl_begin_graph(graph_t* pGraph, box bb, point pb)
{
        g->cs = 0;
        g->getCS()->penCol[0] = 1.0;
        g->getCS()->penCol[1] = 1.0;
        g->getCS()->penCol[2] = 1.0;
        g->getCS()->fillCol[0] = 1.0;
        g->getCS()->fillCol[1] = 1.0;
        g->getCS()->fillCol[2] = 1.0;
        g->getCS()->sameFillCol = true;
        g->CullTests = 0;
        g->ObjectsDisplayed = 0;
        g->ObjectsTotal = 0;

        // determine font scale with test string
        float xmin,ymin,xmax,ymax;
        glfGetStringBounds( "MQlit",&xmin,&ymin,&xmax,&ymax);
        g->fontScale = 1.0/(ymax-ymin);

        // arial font also needs some scaling to fix size calculation
        g->fontScale *= 0.8;
        g->NodeUndermouse = NULL;

        // store projection matrix for unprojection
        glGetIntegerv (GL_VIEWPORT, g->Viewp); 
        glGetDoublev (GL_MODELVIEW_MATRIX, g->Mvmat); 
        glGetDoublev (GL_PROJECTION_MATRIX, g->Projmat); // note viewport[3] is height of window in pixels  

}


//-----------------------------------------------------------------------------
static void gl_end_graph(void)
{
        //cout << " graph end " << endl;
}



//-----------------------------------------------------------------------------
static void gl_begin_node(node_t* node) 
{ 
        g->DispState = DISPSTATE_ONNODE;
        if(node==g->NodeSelected) {
                g->DispState = DISPSTATE_NODESEL;
        }
        double s = (PSinputscale/2)+1;

        pointf A[4];
        A[0].x = node->u.coord.x - node->u.width*s;     A[0].y = node->u.coord.y - node->u.height*s;
        A[1].x = node->u.coord.x - node->u.width*s;     A[1].y = node->u.coord.y + node->u.height*s;
        A[2].x = node->u.coord.x + node->u.width*s;     A[2].y = node->u.coord.y - node->u.height*s;
        A[3].x = node->u.coord.x + node->u.width*s;     A[3].y = node->u.coord.y + node->u.height*s;
        if(g->isVisibleF(A,4)) {
                // node is visible
                g->Culling = CULL_DISPLAY;
                if( (g->MousePlaneX>=A[0].x)&&(g->MousePlaneX<=A[2].x)&&
                                (g->MousePlaneY>=A[0].y)&&(g->MousePlaneY<=A[1].y) ){
                        g->NodeUndermouse = node;
                        if(g->DispState != DISPSTATE_NODESEL) g->DispState = DISPSTATE_NODEHIGH;
                }
        } else {
                g->Culling = CULL_DONTDISPLAY;
        }

        g->ZPos = EXTRUDE_DEPTH/2;
}

//-----------------------------------------------------------------------------
static void gl_end_node (void) 
{ 
        g->DispState = DISPSTATE_OFF;
        g->Culling = CULL_UNDECIDED;
        g->ZPos = 0.0;
}

//-----------------------------------------------------------------------------
static void gl_begin_edge (edge_t* e) 
{ 
        g->DispState = DISPSTATE_OFF;
        if(g->SelNodeEdges.size() > 0) {
                if(e == g->SelNodeEdges[ g->EdgeSelected ]) {
                        g->DispState = DISPSTATE_EDGESEL;
                }
        }
}

//-----------------------------------------------------------------------------
static void gl_end_edge (void) 
{ 
        g->DispState = DISPSTATE_OFF;
}

//-----------------------------------------------------------------------------
static void gl_begin_context(void)
{
        if( g->cs >= CONTEXT_STACK_DEPTH-1 ) return;
        g->cStack[ g->cs+1 ] = g->cStack[ g->cs ];
        g->cs++;
}


//-----------------------------------------------------------------------------
static void gl_end_context(void)
{
        if( g->cs <= 0 ) return;
        g->cs--;
}


//-----------------------------------------------------------------------------
static void gl_set_font(char* name, double size)
{
        g->getCS()->fontName = name;
        g->getCS()->fontSize = size;
}



//-----------------------------------------------------------------------------
static void gl_set_pencolor(char* name)
{
        if(g->Culling==CULL_DONTDISPLAY) return;
        color_t col;
        colorxlate(name, &col, RGBA_BYTE);
        g->getCS()->penCol[0] = col.u.rgba[0]/255.0f;
        g->getCS()->penCol[1] = col.u.rgba[1]/255.0f;
        g->getCS()->penCol[2] = col.u.rgba[2]/255.0f;

        if(     (g->getCS()->penCol[0] == g->getCS()->fillCol[0]) &&
                        (g->getCS()->penCol[1] == g->getCS()->fillCol[1]) &&
                        (g->getCS()->penCol[2] == g->getCS()->fillCol[2]) ) g->getCS()->sameFillCol = true;
        else g->getCS()->sameFillCol = false;
}


//-----------------------------------------------------------------------------
static void gl_set_fillcolor(char* name)
{
        if(g->Culling==CULL_DONTDISPLAY) return;
        //cout << " fillcol " << name << endl; // debug
        color_t col;
        colorxlate(name, &col, RGBA_BYTE);
        g->getCS()->fillCol[0] = col.u.rgba[0]/255.0f;
        g->getCS()->fillCol[1] = col.u.rgba[1]/255.0f;
        g->getCS()->fillCol[2] = col.u.rgba[2]/255.0f;

        if(     (g->getCS()->penCol[0] == g->getCS()->fillCol[0]) &&
                        (g->getCS()->penCol[1] == g->getCS()->fillCol[1]) &&
                        (g->getCS()->penCol[2] == g->getCS()->fillCol[2]) ) g->getCS()->sameFillCol = true;
        else g->getCS()->sameFillCol = false;
}


//-----------------------------------------------------------------------------
static void gl_textline(point p, textline_t *line)
{
        char *text = line->str;                 // text to write
        float xmin,ymin,xmax,ymax;      // font dimension
        float scale = 1.0;                                      // scale to fit graph
        float tpx,tpy;                                                  // point to actually put the font
        float textZOffset = 1.0;                // Z offset for text
        g->ObjectsTotal++;
        if(g->Culling==CULL_DONTDISPLAY) return;

        glfGetStringBounds( text,&xmin,&ymin,&xmax,&ymax);
        scale = g->getCS()->fontSize * g->fontScale;
        xmin *= scale;
        xmax *= scale;
        ymin *= scale;
        ymax *= scale;
        tpx = p.x - xmin;
        tpy = p.y - ymin;
        //tpx = p.x; tpy = p.y;

        switch(line->just) {
                case 'l':
                        break;
                case 'r':
                        tpx = p.x - xmax;
                        break;
                case 'n':
                        tpx = p.x - xmax*0.5;
                        break;
                default:
                        tpx = p.x - xmax*0.5;
                        break;
        }

        //fprintf(Output_file,"%d %d moveto %d %.1f %s alignedtext\n",
        //p.x,p.y,line->width,adj,ps_string(line->str));
        //glColor4f( 1.0, 0.0, 0.0, 0.5); glPointSize( 5.0 ); glBegin(GL_POINTS); glVertex3f( p.x,p.y, gZPos ); glEnd(); // debug
        //glColor4f( 0.0, 1.0, 0.0, 0.5); glPointSize( 5.0 ); glBegin(GL_POINTS); glVertex3f( tpx,tpy, gZPos ); glEnd(); // debug
        //glColor4f( 0.0, 0.0, 1.0, 0.5); glPointSize( 5.0 ); glBegin(GL_POINTS); glVertex3f( tpx+xmax,tpy+ymax, gZPos ); glEnd(); // debug
        //glColor4f( 0.0, 0.0, 0.0, 0.5); glBegin(GL_QUADS); glVertex3f( tpx ,tpy , gZPos ); glVertex3f( tpx+xmax,tpy , gZPos ); glVertex3f( tpx+xmax,tpy+ymax, gZPos ); glVertex3f( tpx ,tpy+*ymax, gZPos ); glEnd(); // debug
        
        if(g->Culling==CULL_UNDECIDED) {
                pointf A[4];
                A[0].x = tpx+xmin; A[0].y = tpy+ymin;
                A[1].x = tpx+xmax; A[1].y = tpy+ymin;
                A[3].x = tpx+xmax; A[3].y = tpy+ymax;
                A[2].x = tpx+xmin; A[2].y = tpy+ymax;
                // debug - visualize bounding box
                //glColor3f(0,1,0); glBegin(GL_QUADS); glVertex3f(A[0].x,A[0].y,0); glVertex3f(A[2].x,A[2].y,0); glVertex3f(A[3].x,A[3].y,0); glVertex3f(A[1].x,A[1].y,0); glEnd();
                if(!g->isVisibleF(A,4)) return;
        }

        pointf A[4];
        A[0].x = tpx+xmin; A[0].y = tpy+ymin;
        A[1].x = tpx+xmin; A[1].y = tpy+ymax;

        double detail = DETAIL_TEXT_BOX+1.0;
        detail = g->getScreenDistanceF2( A[0].x,A[0].y,(g->ZPos+textZOffset), A[1].x,A[1].y,(g->ZPos+textZOffset) );
        detail *= g->mDetailFactor;
        //cout << " T " << text <<"\t\t\t det "<< detail<<" ppu:"<<pixPerUnit<<" "<<wx1<<","<<wy1<<" - "<<wx2<<","<<wy2<< endl;
        
        if(detail< DETAIL_TEXT_BOX) {
                pointf A[4];
                A[0].x = tpx+xmin;                      A[0].y = tpy+ymin;
                A[1].x = tpx+xmax;      A[1].y = tpy+ymin;
                A[3].x = tpx+xmax;      A[3].y = tpy+ymax;
                A[2].x = tpx+xmin;                      A[2].y = tpy+ymax;
                // debug - visualie bounding box
                glColor3f(g->getCS()->penCol[0],g->getCS()->penCol[1],g->getCS()->penCol[2]);
                glBegin(GL_LINES); 
                glVertex3f(A[0].x,A[0].y,       g->ZPos+textZOffset); 
                glVertex3f(A[3].x,A[3].y,       g->ZPos+textZOffset); 
                glEnd();
                return;
        }       else if(detail< DETAIL_TEXT_REMOVE) {
                return;
        }

        // for some reason, fonts are misaligned a bit -> fix it roughly
        tpx += scale*1.0;

        g->ObjectsDisplayed++;
        glPushMatrix();
        glTranslatef( tpx, tpy, g->ZPos+textZOffset);
        glScalef( scale,scale,scale );
        glColor3f(g->getCS()->penCol[0],g->getCS()->penCol[1],g->getCS()->penCol[2]);
        glfDrawSolidString( text );
        glPopMatrix();
        return;
}



//-----------------------------------------------------------------------------
static void gl_polygon(point *A, int n, int filled)
{
        g->ObjectsTotal++;
        if(g->Culling==CULL_DONTDISPLAY) return;
        else if(g->Culling==CULL_UNDECIDED) {
                g->CullTests++;
                if(!g->isVisibleI(A,n)) return;
        }
        g->ObjectsDisplayed++;
        if( (   (g->DispState==DISPSTATE_ONNODE) ||
                        (g->DispState==DISPSTATE_NODEHIGH) ||
                                (g->DispState==DISPSTATE_NODESEL) ) 
                        && (g->EnhanceDisplay) ){
                filled = 2;
                // check if its visible
                double detail = g->getScreenDistanceF2( A[0].x,A[0].y,(g->ZPos+0.0), A[0].x,A[0].y,(g->ZPos-EXTRUDE_DEPTH) );
                detail *= g->mDetailFactor;
                if(detail< DETAIL_POLY_DEPTH) {
                        filled = 1;
                }
        }
        if(filled && g->EnhanceDisplay) {
                glEnable( GL_LIGHTING );
        }

        if(filled) {
                g->select_fill_color();

                if(n==3) {
                        glBegin( GL_TRIANGLES );
                } else  if(n==4) {
                        glBegin( GL_QUADS );
                } else {
                        // warning: this doesnt work for all polygons!
                        glBegin( GL_POLYGON );
                }
                if(filled) glNormal3f(0.0, 0.0, 1.0); // all in same plane
                for (int j=0; j<n; j++) {
                        glVertex3f( A[j].x, A[j].y, g->ZPos );
                }
                glEnd( );

                if(filled == 2) {

                        // approxmiate polygon center
                        float cx=0.0, cy=0.0;
                        for (int j=0; j<n; j++) {
                                cx += A[j].x;
                                cy += A[j].y;
                        }
                        cx /= (float)n;
                        cy /= (float)n;
                        
                        // calculate vertex normals
                        ntlVec3d *vertNormals = new ntlVec3d[n];
                        for (int j=0; j<n; j++) {
                                vertNormals[j] = ntlVec3d( A[j].x-cx, A[j].y-cy, 0.0 );
                                vertNormals[j].normalize();
                        }
                
                        glBegin( GL_TRIANGLES );
                        for (int j=1; j<n; j++) {
                                glNormal3f(vertNormals[j][0], vertNormals[j][1], vertNormals[j][2]);
                                glVertex3f(A[j].x, A[j].y, g->ZPos );
                                glNormal3f(vertNormals[j-1][0], vertNormals[j-1][1], vertNormals[j-1][2]);
                                glVertex3f(A[j-1].x, A[j-1].y, g->ZPos );
                                glNormal3f(vertNormals[j-1][0], vertNormals[j-1][1], vertNormals[j-1][2]);
                                glVertex3f(A[j-1].x, A[j-1].y, g->ZPos-EXTRUDE_DEPTH );

                                glNormal3f(vertNormals[j][0], vertNormals[j][1], vertNormals[j][2]);
                                glVertex3f(A[j].x, A[j].y, g->ZPos );
                                glNormal3f(vertNormals[j-1][0], vertNormals[j-1][1], vertNormals[j-1][2]);
                                glVertex3f(A[j-1].x, A[j-1].y, g->ZPos-EXTRUDE_DEPTH );
                                glNormal3f(vertNormals[j][0], vertNormals[j][1], vertNormals[j][2]);
                                glVertex3f(A[j].x, A[j].y, g->ZPos-EXTRUDE_DEPTH );
                        }
                        glNormal3f(vertNormals[0][0], vertNormals[0][1], vertNormals[0][2]);
                        glVertex3f(A[0].x, A[0].y, g->ZPos );
                        glNormal3f(vertNormals[n-1][0], vertNormals[n-1][1], vertNormals[n-1][2]);
                        glVertex3f(A[n-1].x, A[n-1].y, g->ZPos );
                        glNormal3f(vertNormals[n-1][0], vertNormals[n-1][1], vertNormals[n-1][2]);
                        glVertex3f(A[n-1].x, A[n-1].y, g->ZPos-EXTRUDE_DEPTH );

                        glNormal3f(vertNormals[0][0], vertNormals[0][1], vertNormals[0][2]);
                        glVertex3f(A[0].x, A[0].y, g->ZPos );
                        glNormal3f(vertNormals[n-1][0], vertNormals[n-1][1], vertNormals[n-1][2]);
                        glVertex3f(A[n-1].x, A[n-1].y, g->ZPos-EXTRUDE_DEPTH );
                        glNormal3f(vertNormals[0][0], vertNormals[0][1], vertNormals[0][2]);
                        glVertex3f(A[0].x, A[0].y, g->ZPos-EXTRUDE_DEPTH );
                        glEnd( );
                }
        } 
        // in case it was enabled for node display
        glDisable( GL_LIGHTING );

        if( (!filled) ||
                        ( (filled) && (!g->getCS()->sameFillCol) )
                        )       {
                // draw only if visible
        double lastX = -2.0, lastY = -2.0;

                g->select_pen_color();
                glBegin( GL_LINES );
                for(int j=0; j<n; j++) {
                        double px = A[j].x;
                        double py = A[j].y;
                        if((lastX>-2.0)&&(lastY>-2.0)) {
                                glVertex3f( lastX,lastY,g->ZPos );
                                glVertex3f( px,py,g->ZPos );
                        }
                        lastX = px; lastY = py;
                }
                glVertex3f( lastX,lastY,g->ZPos );
                glVertex3f( A[0].x,A[0].y,g->ZPos );
                glEnd( );
        }

}



//-----------------------------------------------------------------------------
// use gd tables
extern int gdCosT[];
extern int gdSinT[];

// ellipse drawing routine - same as in libgd
static void gl_draw_ellipse(int cx, int cy, int w, int h, int s, int e, int filled, short subdivide) {
        //gdPoint pts[3];
        float i;
        float lx = 0, ly = 0;
        float fx = 0, fy = 0;
        float step = (e-s)/subdivide;
        while (e < s) { e += 360; }

        if(filled == 0) {
                g->select_pen_color();
                glBegin( GL_LINES );
        } else {
                g->select_fill_color();
                glBegin( GL_TRIANGLES );
        }

        for (i = s; (i <= e); i+= step ) {
                float x, y;
                int ii = (int)i;
                x = ((float) gdCosT[ii % 360] * (float) w / (2 * 1024)) + cx;
                y = ((float) gdSinT[ii % 360] * (float) h / (2 * 1024)) + cy;
                if (i != s)
                {
                        if(filled > 0) {
                                glNormal3f(0.0, 0.0, 1.0); // all in same plane
                                glVertex3f(cx,cy, g->ZPos );
                        }
                        glVertex3f(lx,ly, g->ZPos );
                        glVertex3f(x,y, g->ZPos );

                        if(filled == 2) {
                                // precalculate normals?
                                ntlVec3d norm( x-cx, y-cy, 0.0 );
                                norm.normalize();
                                ntlVec3d norml( lx-cx, ly-cy, 0.0 );
                                norml.normalize();

                                glNormal3f( norm[0], norm[1], norm[2] );
                                glVertex3f(x,y, g->ZPos );
                                glNormal3f( norml[0], norml[1], norml[2] );
                                glVertex3f(lx,ly, g->ZPos );
                                glNormal3f( norml[0], norml[1], norml[2] );
                                glVertex3f(lx,ly, g->ZPos-EXTRUDE_DEPTH );

                                glNormal3f( norm[0], norm[1], norm[2] );
                                glVertex3f(x,y, g->ZPos );
                                glNormal3f( norml[0], norml[1], norml[2] );
                                glVertex3f(lx,ly, g->ZPos-EXTRUDE_DEPTH );
                                glNormal3f( norm[0], norm[1], norm[2] );
                                glVertex3f(x,y, g->ZPos-EXTRUDE_DEPTH );
                        }
                } else {
                        fx = x;
                        fy = y;
                }
                lx = x;
                ly = y;
        }

        if(filled > 0) {
                glNormal3f(0.0, 0.0, 1.0); // all in same plane
                glVertex3f(cx,cy, g->ZPos );
        }
        glVertex3f(fx,fy, g->ZPos );
        glVertex3f(lx,ly, g->ZPos );
        if(filled == 2) {
                // precalculate normals?
                ntlVec3d norm( fx-cx, fy-cy, 0.0 );
                norm.normalize();
                ntlVec3d norml( lx-cx, ly-cy, 0.0 );
                norml.normalize();

                glNormal3f( norm[0], norm[1], norm[2] );
                glVertex3f(fx,fy, g->ZPos );
                glNormal3f( norml[0], norml[1], norm[2] );
                glVertex3f(lx,ly, g->ZPos );
                glNormal3f( norml[0], norml[1], norm[2] );
                glVertex3f(lx,ly, g->ZPos-EXTRUDE_DEPTH );

                glNormal3f( norm[0], norm[1], norm[2] );
                glVertex3f(fx,fy, g->ZPos );
                glNormal3f( norml[0], norml[1], norm[2] );
                glVertex3f(lx,ly, g->ZPos-EXTRUDE_DEPTH );
                glNormal3f( norm[0], norm[1], norm[2] );
                glVertex3f(fx,fy, g->ZPos-EXTRUDE_DEPTH );
        }

        glEnd();
}

//-----------------------------------------------------------------------------
static void gl_ellipse(point p, int rx, int ry, int filled)
{
        short subdivide = SUBDIVISION_STEPS*2;
        g->ObjectsTotal++;
        if(g->Culling==CULL_DONTDISPLAY) return;
        else if(g->Culling==CULL_UNDECIDED) {
                g->CullTests++;
                /* FIXME */ } 
        g->ObjectsDisplayed++;

        if( (   (g->DispState==DISPSTATE_ONNODE) ||
                        (g->DispState==DISPSTATE_NODEHIGH) ||
                                (g->DispState==DISPSTATE_NODESEL) ) 
                        && (g->EnhanceDisplay) ){
                filled = 2;
                // check if its visible
                double detail = g->getScreenDistanceF2( p.x,p.y,(g->ZPos+0.0), p.x,p.y,(g->ZPos-EXTRUDE_DEPTH) );
                detail *= g->mDetailFactor;
                if(detail< DETAIL_POLY_DEPTH) {
                        filled = 1;
                }
        }
        if(filled && g->EnhanceDisplay) {
                glEnable( GL_LIGHTING );
        }

        double detail;
        if(ry>rx) {
                detail = g->getScreenDistanceF2( p.x,p.y-ry,(g->ZPos), p.x,p.y+ry,(g->ZPos) );
        } else {
                detail = g->getScreenDistanceF2( p.x-rx,p.y,(g->ZPos), p.x+rx,p.y,(g->ZPos) );
        }
        detail *= g->mDetailFactor;
        subdivide = (short)(detail/DETAIL_ELLIPSE_LINE)*2;
        if(subdivide>SUBDIVISION_STEPS*2) subdivide = SUBDIVISION_STEPS*2;
        if(subdivide<DETAIL_ELLIPSE_MINSUBDIV) subdivide=DETAIL_ELLIPSE_MINSUBDIV;
        if(detail<DETAIL_ELLIPSE_BOX) {
                // draw ellipse as box
                float rscale = 0.8;
                pointf A[4];
                A[0].x = p.x-(rx*rscale); A[0].y = p.y-(ry*rscale);
                A[1].x = p.x-(rx*rscale); A[1].y = p.y+(ry*rscale);
                A[2].x = p.x+(rx*rscale); A[2].y = p.y-(ry*rscale);
                A[3].x = p.x+(rx*rscale); A[3].y = p.y+(ry*rscale);
                if(filled) {
                        g->select_fill_color();
                        glBegin( GL_TRIANGLES );
                        glVertex3f(A[0].x,A[0].y,g->ZPos);
                        glVertex3f(A[1].x,A[1].y,g->ZPos);
                        glVertex3f(A[2].x,A[2].y,g->ZPos);
                        glVertex3f(A[1].x,A[1].y,g->ZPos);
                        glVertex3f(A[3].x,A[3].y,g->ZPos);
                        glVertex3f(A[2].x,A[2].y,g->ZPos);
                        glEnd();
                }
                // in case it was enabled for node display
                glDisable( GL_LIGHTING );

                g->select_pen_color();
                glBegin( GL_LINES );
                glVertex3f(A[0].x,A[0].y,g->ZPos);
                glVertex3f(A[1].x,A[1].y,g->ZPos);

                glVertex3f(A[1].x,A[1].y,g->ZPos);
                glVertex3f(A[3].x,A[3].y,g->ZPos);

                glVertex3f(A[3].x,A[3].y,g->ZPos);
                glVertex3f(A[2].x,A[2].y,g->ZPos);

                glVertex3f(A[2].x,A[2].y,g->ZPos);
                glVertex3f(A[0].x,A[0].y,g->ZPos);
                glEnd();
                return;
        }

        // real ellipse display
        if(filled) {
                gl_draw_ellipse(p.x,p.y, rx+rx,ry+ry, 0,360, filled,subdivide );
        }
        // in case it was enabled for node display
        glDisable( GL_LIGHTING );

        gl_draw_ellipse(p.x,p.y, rx+rx,ry+ry, 0,360, false,subdivide );
}


//-----------------------------------------------------------------------------
static void gl_polyline(point* A, int n)
{
        double lastX = -2.0, lastY = -2.0;
        g->ObjectsTotal++;
        if(g->Culling==CULL_DONTDISPLAY) return;
        else if(g->Culling==CULL_UNDECIDED) {
                g->CullTests++;
                if(!g->isVisibleI(A,n)) return;
        }
        g->ObjectsDisplayed++;

        g->select_pen_color();
        glBegin( GL_LINES );
        for(int j=0; j<n; j++) {
                double px = A[j].x;
                double py = A[j].y;
                if((lastX>-2.0)&&(lastY>-2.0)) {
                        glVertex3f( lastX,lastY,g->ZPos );
                        glVertex3f( px,py,g->ZPos );
                }
                lastX = px; lastY = py;
        }
        glEnd( );
}


//-----------------------------------------------------------------------------
static void gl_bezier(point *A, int n, int arrow_at_start, int arrow_at_end)
{
        g->ObjectsTotal++;
        if(g->Culling==CULL_DONTDISPLAY) return;
        else if(g->Culling==CULL_UNDECIDED) {
                g->CullTests++;
                if(!g->isVisibleI(A,n)) return;
        }
        g->ObjectsDisplayed++;

        pointf p0, p1, V[4];
        short subdivide = SUBDIVISION_STEPS; // TODO

        double detail = g->getScreenDistanceF2( A[0].x,A[0].y,(g->ZPos+0.0), A[n-1].x,A[n-1].y,(g->ZPos+0.0) );
        detail *= g->mDetailFactor;
        if(detail< DETAIL_BEZIER_LINE) {
                g->select_pen_color();
                glBegin( GL_LINES );
                glVertex3f(A[0].x,A[0].y,g->ZPos);
                glVertex3f(A[n-1].x,A[n-1].y,g->ZPos);
                glEnd();
                return;
        } else {
                subdivide = (short)(detail/DETAIL_BEZIER_LINE);
                if(subdivide>SUBDIVISION_STEPS) subdivide = SUBDIVISION_STEPS;
        }

        g->select_pen_color();
        glBegin( GL_LINES );

        V[3].x = A[0].x; 
        V[3].y = A[0].y;
        for(int i = 0; i+3 < n; i += 3) {
                V[0] = V[3];
                for(int j=1; j<=3; j++) {
                        V[j].x = A[i+j].x; 
                        V[j].y = A[i+j].y;
                }
                p0 = V[0];
                for(int step=1; step<=subdivide; step++) {
                        p1 = Bezier(V, 3, (double)step/subdivide, NULL, NULL);
                        glVertex3f( p0.x,p0.y, g->ZPos );
                        glVertex3f( p1.x,p1.y, g->ZPos );
                        p0 = p1;
                }
        }

        glEnd( );
        return;
}


//-----------------------------------------------------------------------------
static point gl_textsize(char *str, char *fontname, double fontsz)
{ 
        point p;
        return p;
}


//-----------------------------------------------------------------------------
// setup codegen struct for DOT
codegen_t GL_CodeGen = {
        gl_reset,
        gl_begin_job, gl_end_job,
        gl_begin_graph, gl_end_graph,
        gl_begin_page, gl_end_page,
        gl_begin_cluster, gl_end_cluster,
        gl_begin_nodes, gl_end_nodes,
        gl_begin_edges, gl_end_edges,
        gl_begin_node, gl_end_node,
        gl_begin_edge, gl_end_edge,
        gl_begin_context, gl_end_context,
        gl_set_font, gl_textline,
        gl_set_pencolor, gl_set_fillcolor, gl_set_style,
        gl_ellipse, gl_polygon,
        gl_bezier, gl_polyline,
        0 /*gl_arrowhead*/ , gl_user_shape,
        gl_comment, gl_textsize 
};

void GlCodegenState::draw( void ) {
        // convince DOT to call the OpenGL output functions
        Output_lang = POSTSCRIPT;

        // important - restore old codegen for loading files!!!
        codegen_t *old = CodeGen;
        CodeGen = &GL_CodeGen;
        g = this;
        dotneato_write_one( pGraph );
        CodeGen = old;
};


