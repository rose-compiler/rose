/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * visualizer FLTK group
 *
 *****************************************************************************/



#include "vizgroup.h"

// menu callback functions
void menuCallback(Fl_Widget *w, void *v);

// menu contents for each visualizer window
#define VMENU_UNDEFINED         0
#define VMENU_LOAD                              1
#define VMENU_CLOSE                             2
#define VMENU_DETAIL_HIGH       3
#define VMENU_DETAIL_MED        4
#define VMENU_DETAIL_LOW        5
Fl_Menu_Item vizMenuItems[] = {
        { "&Graph",                             0,0,0, FL_SUBMENU },
                { "&Load",                              0, (Fl_Callback *)DotVisualizer::menuLoadCB  },
                { "&Close",                     0, (Fl_Callback *)DotVisualizer::menuCloseCB },
                //{ "&Test",                            0, (Fl_Callback *)menuCallback , (void *)1 },
                { 0 },

        { "&Layout",                            0,0,0, FL_SUBMENU },
                { "&Dot",                               0, (Fl_Callback *)DotVisualizer::menuPerformDotLayout   },
                { "&Neato",                     0, (Fl_Callback *)DotVisualizer::menuPerformNeatoLayout },
                //{ "&Test",                            0, (Fl_Callback *)menuCallback , (void *)1 },
                { 0 },

        { "&Display",                           0,0,0, FL_SUBMENU },
                { "&Lock",                              0, (Fl_Callback *)DotVisualizer::menuToggleLock, (void *)0 , FL_MENU_TOGGLE },
                { "&Show FPS",          0, (Fl_Callback *)DotVisualizer::menuToggleFPS , (void *)0 , FL_MENU_TOGGLE | FL_MENU_DIVIDER },
                //{ "New File",                 0, (Fl_Callback *)menuCallback , (void *)11 , FL_MENU_DIVIDER },
                { "&Detail",                    0,0,0, FL_SUBMENU },
                        { "&High",                      0, (Fl_Callback *)DotVisualizer::menuSetDetailHigh },
                        { "&Normal",            0, (Fl_Callback *)DotVisualizer::menuSetDetailMed  },
                        { "&Low",                       0, (Fl_Callback *)DotVisualizer::menuSetDetailLow  },
                        { 0 },
                { 0 },

        { 0 }
};



void menuCallback(Fl_Widget *w, void *v) {
        //cout << " CB " << (int)v << endl;
        /* FIXME
        int vi = (int)v / 100;
        int event = (int)v - vi*100;
        if(vi>MAX_VIZ) {
                cerr << "Warning - invalid viz id: e"<< event <<" for "<< vi <<", " << (int)v << endl;
                return;
        }
        if(!mVizActive[vi]) {
                cerr << "Warning - invalid viz (not active): e"<< event <<" for "<< vi <<", " << (int)v << endl;
                return;
        }

        switch(event) {
                case VMENU_LOAD: {
                        Fl_File_Chooser *choose = new Fl_File_Chooser(".", "DOT File (*.dot)\tDOT File with Layout (*.pdot)", Fl_File_Chooser::SINGLE, "Load DOT Graph");
                        choose->show();
                        while(choose->visible()) Fl::wait();
                        if(!choose->value()) break;
                        string filename( choose->value() );
                        
                        viz[vi]->loadFile( filename );
                        } break;

                case VMENU_CLOSE: {
                        viz[vi]->closeGraph();
                        } break;

                default:
                        cerr << "Warning - invalid viz menu id: e"<< event <<" for "<< vi <<", " << (int)v << endl;
                        break;
        }
        */
}


//-----------------------------------------------------------------------------
//! constructor
VizGroup::VizGroup(int x,int y,int w,int h) : 
        Fl_Tile( x,y, w,h ) 
{
        for(int i=0; i<MAX_VIZ; i++) {
                mVizActive[i] = false;
                mVizVisible[i] = false;
                mpViz[i]= NULL;
                mpGroup[i]= NULL;
        }

        end();
}

//-----------------------------------------------------------------------------
//! destructor
VizGroup::~VizGroup()
{
        for(int i=0; i<MAX_VIZ; i++) {
                if(mVizActive[i]) {
                        delete mpViz[i];
                }
        }
};


//-----------------------------------------------------------------------------
void VizGroup::addViz(int x,int y,int w,int h, int vi,Agraph_t *G)
{
        Fl_Group *group;
        //DotVisualizer *viz = new DotVisualizer( G, 20, 40+100+20, 260, 220 );
        DotVisualizer *vizz = NULL;
        int menuHeight = 24;

        group = new Fl_Group( x,y, w,h );

        // add border
        Fl_Box *border = new Fl_Box( FL_ENGRAVED_BOX, x,y, w,h, "" );
        border->show();
        x += 3; 
        y += 3; 
        w -= 6; 
        h -= 6;

        if(!mVizActive[vi]) {
                vizz = new DotVisualizer( G, x, y +menuHeight, w, h-menuHeight );
                vizz->callback( DotVisualizer::staticCallback, vizz );
                vizz->setGraphInfo( mpGraphInfo );
                //vizz->loadFile( string("./prelayout_small.dot")); // DEBUG std init
                mpViz[vi] = vizz;
        } else {
                vizz = mpViz[vi];
                vizz->resize( x,y +menuHeight, w, h-menuHeight );
                group->add( vizz );
                //group = mpGroup[vi];
                //group->resize( x,y, w,h );
        }

        Fl_Menu_Bar *menu = new Fl_Menu_Bar( x, y, w, menuHeight );
        for(size_t l=0; l< sizeof(vizMenuItems)/sizeof(Fl_Menu_Item); l++) {
                vizMenuItems[l].user_data_ = (void *)vizz;
        }
        menu->copy( vizMenuItems );

        mpGroup[vi] = group;
        group->resizable( vizz );
        group->end();
        group->show();
        menu->show();

        mVizActive[vi] = true;
        mVizVisible[vi] = true;
        //add(group);
}



//-----------------------------------------------------------------------------
//! init the vizgroup layout
void VizGroup::vizLayout(VizGroupLayout slayout)
{
        int x=this->x(), y=this->y();

        for(int i=0; i<MAX_VIZ; i++) {
                if((mVizActive[i])&&(mVizVisible[i])) {
                        mpGroup[i]->remove( mpViz[i] );
                        // hide them - this is important, otherwise they will still be drawn somewhere...
                        mpViz[i]->hide();
                }
                mVizVisible[i] = false;
                mpGroup[i] = NULL;
        }
        clear();
        current(this);
        
        switch(slayout) {
                case single:
                        addViz( x, y, w(), h() ,0,NULL);
                        break;
                case splitHoriz:
                        addViz( x, y,                   w(), h()/2 ,0,NULL);
                        addViz( x, y+h()/2, w(), h()/2 ,1,NULL);
                        break;
                case splitVert:
                        addViz( x,                              y, w()/2, h() ,0,NULL);
                        addViz( x+w()/2,        y, w()/2, h() ,1,NULL);
                        break;
                case triple:
                        addViz( x+w()/3*1, y,                           w()/3*2,        h()             ,0,NULL);
                        addViz( x+w()/3*0, y+h()/2*0, w()/3,            h()/2           ,1,NULL);
                        addViz( x+w()/3*0, y+h()/2*1, w()/3,            h()/2           ,2,NULL);
                        break;
                case quad:
                        addViz( x      , y      , w()/2, h()/2 ,0,NULL);
                        addViz( x+w()/2, y      , w()/2, h()/2 ,1,NULL);
                        addViz( x      , y+h()/2, w()/2, h()/2 ,2,NULL);
                        addViz( x+w()/2, y+h()/2, w()/2, h()/2 ,3,NULL);
                        break;
                case threeone:
                        addViz( x+w()/4*1, y,                           w()/4*3,        h()             ,0,NULL);
                        addViz( x        , y+h()/3*0, w()/4,            h()/3           ,1,NULL);
                        addViz( x        , y+h()/3*1, w()/4,            h()/3           ,2,NULL);
                        addViz( x        , y+h()/3*2, w()/4,            h()/3           ,3,NULL);
                        break;
                default:
                        break;
        }

        end();
        focus( mpViz[0] );
}

//-----------------------------------------------------------------------------
//! redraw all visualizers when idle
void VizGroup::doIdleRedraw()
{

        for(int i=0; i<MAX_VIZ; i++) {
                if(mVizVisible[i]) {
                        //cerr << "idle " << i <<endl;
                        mpViz[i]->idle();
                        mpViz[i]->redraw();
                }
        }

}

//-----------------------------------------------------------------------------
//! show all viz's
void VizGroup::showAll()
{
        for(int i=0; i<MAX_VIZ; i++) {
                if(mVizVisible[i]) {
                        mpGroup[i]->show();
                        // for some reason, the visualizers have to be explicitly shown...
                        mpViz[i]->show();
                }
        }
        show();
}



//-----------------------------------------------------------------------------
//! load multiple files for startup
void VizGroup::vizMultipleLoad( vector<string> files )
{
        if(files.size()==0) {
                // do default empty layout...
                vizLayout( triple );
                return;
        }
        // layout depending on no. of files
        switch(files.size()) {
                case 1:
                        vizLayout( single );
                        break;
                case 2:
                        vizLayout( splitHoriz );
                        break;
                case 3:
                        vizLayout( triple );
                        break;
                default:
                        vizLayout( quad );
                        break;
        }

        // load them
        for(size_t i=0; ((i<files.size())&&(i<4)); i++) {
                if(mVizVisible[i]) {
                        mpViz[i]->loadFile( files[i] );
                }
        }
        
}



