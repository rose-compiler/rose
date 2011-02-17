/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * map overview display 
 *
 *****************************************************************************/

#include "mapcodegen.h"


//-----------------------------------------------------------------------------
// init opengl for ortho drawing

MapCodegenState::MapCodegenState( Agraph_t *G ) {
        zpos = 0.0;     
        mapScale = 1.0;         
        mapOffsetX = 0.1;
        mapOffsetY = 0.1;
        mapOffset = 0.05; 
        mapDestSize = 0.3;
        orthoX=1.0, orthoY=1.0;
        gMouseIn = 0;   
        gFade = 1.0;    
        giOffset = 0.05;
        giScale = 1.0;  
        giOffsetX = 0.1;
        giOffsetY = 0.1;

        pGraph = G;
}
MapCodegenState::~MapCodegenState() {
}

void MapCodegenState::init_ortho( graph_t *pGraph ) {
        // init open gl
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

        glPushMatrix();
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-orthoX, orthoX, -orthoY, orthoY, zpos-1.0, zpos+1.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // save bounding box info
        bbll = pGraph->u.bb.LL;
        bbur = pGraph->u.bb.UR;
}

void MapCodegenState::pop_ortho( void ) {
        // clean up open gl
        glPopMatrix();
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
}

void MapCodegenState::set_mousepos( int x, int y ) {
        gMousePosX = x;
        gMousePosY = y;
        //cout << " pmp " << gMousePosX<<","<<gMousePosY<< endl; // debug
}

//-----------------------------------------------------------------------------

void MapCodegenState::begin_graph(graph_t* pGraph, box , point )
{
        // draw map
        double llx = pGraph->u.bb.LL.x;
        double lly = pGraph->u.bb.LL.y;
        double urx = pGraph->u.bb.UR.x;
        double ury = pGraph->u.bb.UR.y;
        double scaleX = (urx-llx);
        double scaleY = (ury-lly);

        //if(scaleY<scaleX) mapScale = (1.0/scaleX)*2.0;
        //else                                                  mapScale = (1.0/scaleY)*2.0;
        //mapScale *= 0.3;      

        if(scaleY<scaleX) {
                mapScale = ( mapDestSize /scaleY);
                if(( scaleX * mapScale )> (orthoX*2.0-mapOffset*2.0) ) {
                        mapScale = ( (orthoX*2.0-mapOffset*2.0)/scaleX);
                }
        } else {
                mapScale = ( mapDestSize /scaleX);
                if(( scaleY * mapScale )> (orthoX*2.0-mapOffset*2.0) ) {
                        mapScale = ((orthoX*2.0-mapOffset*2.0)/scaleY);
                }
        }

        //gMousePosY = gViewSizeY-gMousePosY;
        short sw = (short)( (MSX(bbur.x) - MSX(bbll.x))/(2.0*orthoX) * gViewSizeX );
        short sh = (short)( (MSY(bbur.y) - MSY(bbll.y))/(2.0*orthoY) * gViewSizeY );
        short ssx = (short)( (MSX(bbll.x))/(2.0*orthoX) * gViewSizeX + (gViewSizeX/2) );
        short ssy = (short)( (MSY(bbll.y))/(2.0*orthoY) * gViewSizeY + (gViewSizeY/2) );
        short sex = (short)( ssx + sw );
        short sey = (short)( ssy + sh );
        if( (gMousePosX>=ssx) && (gMousePosX<=sex) && 
                        ((gViewSizeY-gMousePosY)>=ssy) && ((gViewSizeY-gMousePosY)<=sey) ) {
                gMouseIn = true;
                //gFade = 1.0;
                //cout << " in " ; // debug
        } else {
                gMouseIn = false;
        }

        if(!gMouseIn) {
                glColor4f( 0,0, 0.25 , 0.75*gFade);
                //glColor4f( 0.0,0.0,0.0, 0.5*gFade);
        } else {
                //glColor4f( 0.25,0.25, 0.5 , 0.75*gFade);
                glColor4f( 0,0, 0.35 , 0.75*gFade);
                //glColor4f( 0.0,0.0,0.0, 0.5*gFade);
        }
        //glBegin(GL_QUADS);
        box bound;
        bound = pGraph->u.bb;
        //glVertex3f( MSX(bbll.x),MSY(bbll.y), zpos );
        //glVertex3f( MSX(bbur.x),MSY(bbll.y), zpos );
        //glVertex3f( MSX(bbur.x),MSY(bbur.y), zpos );
        //glVertex3f( MSX(bbll.x),MSY(bbur.y), zpos );
        //glEnd();

        //cout << " GL map start " << mapScale << endl;

        // setup stencil buffer display
        glEnable( GL_STENCIL_TEST );
        glStencilFunc( GL_ALWAYS, 0x01, 0x01 );
        glStencilOp( GL_REPLACE, GL_REPLACE, GL_REPLACE );
        
        glBegin(GL_QUADS);
        glVertex3f( MSX(bbll.x),MSY(bbll.y), zpos );
        glVertex3f( MSX(bbur.x),MSY(bbll.y), zpos );
        glVertex3f( MSX(bbur.x),MSY(bbur.y), zpos );
        glVertex3f( MSX(bbll.x),MSY(bbur.y), zpos );
        glEnd();

        glStencilFunc( GL_EQUAL, 0x01, 0x01 );
        glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

        // draw the rest
        glLineWidth(1.0);
        if(!gMouseIn) {
                glColor4f( 0.0,0.0,0.0, 0.5*gFade);
        } else {
                glColor4f( 0.0,0.0,1.0, 0.5*gFade);
        }
        glBegin(GL_LINES);
        glVertex3f( MSX(bbll.x),MSY(bbll.y), zpos );
        glVertex3f( MSX(bbur.x),MSY(bbll.y), zpos );
        glVertex3f( MSX(bbur.x),MSY(bbur.y), zpos );
        glVertex3f( MSX(bbll.x),MSY(bbur.y), zpos );
        glVertex3f( MSX(bbll.x),MSY(bbur.y), zpos );
        glEnd();

}

void MapCodegenState::end_graph(void)
{
        show_view( );
        glDisable( GL_STENCIL_TEST );
        //cout << " GL graph end " << endl;
}

void MapCodegenState::begin_node(node_t* node)
{
        double sx = MSX(node->u.coord.x);
        double sy = MSY(node->u.coord.y);

        glColor4f( 0.0, 1.0, 0.0, 0.5*gFade);
        glPointSize( 3.0 );
        glBegin(GL_POINTS);
        glVertex3f( sx,sy, zpos );

        glEnd();
}


//-----------------------------------------------------------------------------
// helper functions
//-----------------------------------------------------------------------------




//-----------------------------------------------------------------------------
// own graph display function (no need to traverse the whole tree for the map
// so this should replace dotneato_write_one( G );
void MapCodegenState::display( graph_t *pGraph ) {
        box tmpbox;
        point tmppnt;
        begin_graph(pGraph, tmpbox , tmppnt );
        for (Agnode_t *node = agfstnode(pGraph); node!=NULL; node = agnxtnode(pGraph,node)) {
                begin_node( node );
        }
        end_graph();
}




        
//-----------------------------------------------------------------------------
// calculate viewing region
void MapCodegenState::calc_viewingregion(pointf f[], double *minz, double *maxz) {
        /* calculate rays through projection plane */
        ntlVec3d camera( -gCamX, -gCamY, -gCamZ );
        ntlVec3d lookat( -gLookatX, -gLookatY, 0.0 );
        ntlVec3d direction = lookat - camera;
        double fovy = 90.0;
        double aspect = (double)gViewSizeX/gViewSizeY;
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
        double maxzcnt = ABS(camera[2]-zplane);
        double minzcnt = maxzcnt;
        ntlVec3d e[4];
        e[0] = direction + rightVec + upVec;    
        e[1] = direction - rightVec + upVec;    
        e[2] = direction + rightVec - upVec;    
        e[3] = direction - rightVec - upVec;    
        for(int i=0;i<4;i++) {
                if((zplane-e[i][2])>1.0) { // only treat negative directions
                        e[i][0] /= (zplane-e[i][2]); 
                        e[i][1] /= (zplane-e[i][2]); 
                }
                //ntlVec3d p1( camera[0] + e[i][0] * camera[2], camera[1] + e[i][1] * camera[2], 0.0 );
                f[i].x = gViewRegion[i].x = camera[0] + e[i][0] * camera[2];
                f[i].y = gViewRegion[i].y = camera[1] + e[i][1] * camera[2];

                //double currz = ABS(e[i][2] + zplane);
                ntlVec3d fiVec = ntlVec3d(f[i].x,f[i].y,zplane);
                double currz = (fiVec-camera).getNorm();
                //cout << "  f"<<i<<" "<<fiVec<<" c:"<< camera <<" d:"<<(fiVec-camera)<<"  curr:"<<currz<<endl;
                //if(i==0) {
                        //minzcnt = currz;
                        //maxzcnt = currz;
                //} else {
                        if( minzcnt > currz) minzcnt = currz;
                        if( maxzcnt < currz) maxzcnt = currz;
                //}
        }

        // save min./max. z distance
        *maxz = maxzcnt;
        *minz = minzcnt;
}

//-----------------------------------------------------------------------------
// function called by DotVisualizer to display view info
void MapCodegenState::show_view( void )
{
        double sx = MSX(-gCamX);
        double sy = MSY(-gCamY);
        double lsx = MSX(-gLookatX);
        double lsy = MSY(-gLookatY);
        //double cx = gLookatX-gCamX, cy = gLookatY-gCamY, cz = 0.0-gCamZ;

        if(!gMouseIn) {
                glColor4f( 1.0, 0.0, 0.2, 0.25*gFade );
        } else {
                glColor4f( 1.0, 0.0, 0.2, 0.50*gFade );
        }
        glPointSize( 5.0 );
        glBegin(GL_QUADS);
        glVertex3f( MSX(gViewRegion[0].x),MSY(gViewRegion[0].y), 0.0 );
        glVertex3f( MSX(gViewRegion[2].x),MSY(gViewRegion[2].y), 0.0 );
        glVertex3f( MSX(gViewRegion[3].x),MSY(gViewRegion[3].y), 0.0 );
        glVertex3f( MSX(gViewRegion[1].x),MSY(gViewRegion[1].y), 0.0 );
        glEnd();

        /* draw map and viewing region */
        glColor4f( 0.0, 0.0, 0.7, 0.5*gFade);
        glLineWidth( 1.0 );
        glBegin(GL_LINES);
        glVertex3f( sx,sy, zpos );
        glVertex3f( lsx,lsy, zpos );
        glEnd();

        glColor4f( 0.5, 0.5, 1.0, 0.5*gFade);
        glPointSize( 10.0 );
        glBegin(GL_POINTS);
        glVertex3f( sx,sy, zpos );
        glEnd();

        glColor4f( 0.0, 0.0, 0.2, 0.5*gFade);
        glPointSize( 2.0 );
        glBegin(GL_POINTS);
        glVertex3f( lsx,lsy, zpos );
        glEnd();

        //cout << " sv " << sx<<","<<sy << " - " << x <<","<<y << endl; // debug
}

//-----------------------------------------------------------------------------
// check if click landed on the map, and if yes move camera
bool MapCodegenState::check_click(int x, int y, float *camX, float *camY)
{
        bool ret = false;

        y = gViewSizeY-y;
        short sw = (short)( (MSX(bbur.x) - MSX(bbll.x))/(2.0*orthoX) * gViewSizeX );
        short sh = (short)( (MSY(bbur.y) - MSY(bbll.y))/(2.0*orthoY) * gViewSizeY );
        short ssx = (short)( (MSX(bbll.x))/(2.0*orthoX) * gViewSizeX + (gViewSizeX/2) );
        short ssy = (short)( (MSY(bbll.y))/(2.0*orthoY) * gViewSizeY + (gViewSizeY/2) );
        short sex = (short)( ssx + sw );
        short sey = (short)( ssy + sh );

        if( (x>=ssx) && (x<=sex) && (y>=ssy) && (y<=sey) ) {
                //cout << " cc in " << (*camX) <<","<<(*camY) << endl; // debug
                ret = true;
        } else {
                if(x<=ssx) x = ssx;
                if(x>=sex) x = sex;
                if(y<=ssy) y = ssy;
                if(y>=sey) y = sey;
        }

        *camX = -(bbur.x-bbll.x) * ((float)(x-ssx)/(float)(sex-ssx));
        *camY = -(bbur.y-bbll.y) * ((float)(y-ssy)/(float)(sey-ssy));

        //cout << " cc " << ssx<<","<<ssy << " - " <<sex<<","<<sey<< endl; // debug
        //cout << " c2 " << x<<","<<y << " - " <<gViewSizeX<<","<<gViewSizeY<< endl; // debug
        return ret;
}


//-----------------------------------------------------------------------------
// init ortho view
void MapCodegenState::init_view(int width, int height)
{
        gViewSizeX = width;
        gViewSizeY = height;
        //float aspect = width/height;
        orthoX = orthoY = 1.0;
        if(width>height) {
                orthoX = (float)width/height;
        } else {
                orthoY = (float)height/width;
        }
        mapOffsetX = -orthoX + mapOffset;
        mapOffsetY = -orthoY + mapOffset;
                
        //cout << " iv " << width<<","<<height<< " - " <<orthoX<<","<<orthoY<< endl; // debug
}

//-----------------------------------------------------------------------------
// tell DotViz camera position to map display
void MapCodegenState::set_camera_position(float x, float y, float z, float lx, float ly )
{
        gCamX = x;
        gCamY = y;
        gCamZ = z;
        gLookatX = lx;
        gLookatY = ly;
}

//-----------------------------------------------------------------------------
// set fading state (0=not visible, 1=fully visible)
void MapCodegenState::set_fade(float fade)
{
        gFade = fade;
        //cout << " f " << fade << endl;
}



//-----------------------------------------------------------------------------
// display graph info boxes
void MapCodegenState::graphinfo_display(graph_t *pGraph)
{
        float height;                                                           // height of text
        float width;                                                            // width of text
        float xmin,ymin,xmax,ymax;      // font dimension
        float scale = 0.04;                                     // font scale

        char *text = pGraph->name;
        glfGetStringBounds( text,&xmin,&ymin,&xmax,&ymax);
        width  = (xmax-xmin)*scale+0.025;
        height = (ymax-ymin)*scale+0.025;
        //width = height = 0.3;
        
        //box bound;
        pointf LL, UR;
        giOffsetX = -orthoX+ giOffset;
        giOffsetY =  orthoY-( giOffset +height);
        LL.x = 0.0;
        LL.y = 0.0;
        UR.x = LL.x+ width;
        UR.y = LL.y+ height;
        
        glColor4f( 0,0, 0.25 , 0.75*gFade);
        glBegin(GL_QUADS);
        //bound = pGraph->u.bb;
        glVertex3f( GIX(LL.x),GIY(LL.y), zpos );
        glVertex3f( GIX(UR.x),GIY(LL.y), zpos );
        glVertex3f( GIX(UR.x),GIY(UR.y), zpos );
        glVertex3f( GIX(LL.x),GIY(UR.y), zpos );
        glEnd();

        glColor4f( 0.0,0.0,0.0, 0.5*gFade);
        glBegin(GL_LINES);
        glVertex3f( GIX(LL.x),GIY(LL.y), zpos );
        glVertex3f( GIX(UR.x),GIY(LL.y), zpos );
        glVertex3f( GIX(UR.x),GIY(UR.y), zpos );
        glVertex3f( GIX(LL.x),GIY(UR.y), zpos );
        glVertex3f( GIX(LL.x),GIY(UR.y), zpos );
        glEnd();

        float tpx = 0.0 - (scale*xmin);
        float tpy = 0.0 - (scale*ymin);
        tpx += scale*1.0;

        glPushMatrix();
        glTranslatef( GIX(tpx), GIY(tpy), zpos );
        glScalef( scale,scale,scale );

        glColor4f( 0.0, 1.0, 0.0, 0.9*gFade);
        glfDrawSolidString( text );
        glPopMatrix();
}

//-----------------------------------------------------------------------------
// check if graph info was clicked
bool graphinfo_check_click(int x, int y)
{
        return false;
}







