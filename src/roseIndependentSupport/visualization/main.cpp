/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * main FLTK program
 *
 *****************************************************************************/

// TODO
// - menu display errors when updating view layout
// - redraw of scroll/box graph info doesnt work?


#include <iostream>
#include <vector>
#include <string>

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::cerr;
using std::endl;
using std::cout;

// fltk GUI
#include <FL/Fl.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Box.h>
#include <FL/Fl_Menu_Bar.h>
#include <FL/Fl_File_Chooser.h>
#include <FL/Fl_Tile.h>



#include "vizgroup.h"
#include "graphinfo.h"
#include "glf.h"
#include "font_arial.h"

// debug output switch
bool myDebugOut = false;

// global leave program variable
bool globalQuit = false;

Fl_Tile *dotParent = NULL;                      // main window
VizGroup *vizgroup = NULL;              // visualizer group (DOT windows)
GraphInfo *graphinfo = NULL;    // graph info window
bool graphInfoView = true;              // graphInfoView on?

// menu callback functions
void mainCallback(Fl_Widget *w, void *v);


// main menu height
#define MAIN_MENU_HEIGHT 24
// the main menu
#define MAIN_UNDEFINED  0
#define MAIN_OPENPROJ           1
#define MAIN_SAVEPROJ           2
#define MAIN_QUIT                               3
#define MAIN_GRAPHINFO  4
#define MAIN_ABOUT                      5
#define MAIN_SETLAYOUT  1000
Fl_Menu_Item mainMenuItems[] = {
        { "&File",                                      0,0,0, FL_SUBMENU },
                { "&Open Project",              0, (Fl_Callback *)mainCallback , (void *)MAIN_OPENPROJ },
                { "&Save Project",              0, (Fl_Callback *)mainCallback , (void *)MAIN_SAVEPROJ, FL_MENU_DIVIDER },
                { "&Quit",                                              0, (Fl_Callback *)mainCallback , (void *)MAIN_QUIT },
                { 0 },

        { "&View",                                      0,0,0, FL_SUBMENU },
                { "&GraphInfo",                 0, (Fl_Callback *)mainCallback , (void *)MAIN_GRAPHINFO , FL_MENU_TOGGLE | FL_MENU_VALUE | FL_MENU_DIVIDER },
                { "&Single",                            0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::single) },
                { "Split &Horiz.",      0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::splitHoriz) },
                { "Split &Vert.",       0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::splitVert) },
                { "&Triple",                            0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::triple) },
                { "&Quad",                                      0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::quad) },
                { "&Thee:One",                  0, (Fl_Callback *)mainCallback , (void *)(MAIN_SETLAYOUT+VizGroup::threeone) },
                { 0 },

        { "&Help",                                      0,0,0, FL_SUBMENU },
                { "&About",                             0, (Fl_Callback *)mainCallback , (void *)MAIN_ABOUT },
                { 0 },

        { 0 }
};


void mainCallback(Fl_Widget *w, void *v) {
        //cout << " MAINCB " << (int)v << endl;
        int event = (int)v;
        VizGroup::VizGroupLayout layout = VizGroup::single;
        if(event > MAIN_SETLAYOUT) {
                layout = (VizGroup::VizGroupLayout)(event-MAIN_SETLAYOUT);
                event = MAIN_SETLAYOUT;
        }

        switch(event) {
                case MAIN_QUIT: {
                        globalQuit = true;
                        } break;
                case MAIN_GRAPHINFO: {
                        graphInfoView = !graphInfoView;
                        //int x=0, y=MAIN_MENU_HEIGHT;
                        //int h = dotParent->h() - MAIN_MENU_HEIGHT;
                        int x=0, y=0;
                        int h = dotParent->h();
                        // redo layout
                        if(graphInfoView) {
                                dotParent->add(graphinfo);
                                graphinfo->resize( x+dotParent->w()/4*3, y,     dotParent->w()/4*1, h );
                                vizgroup->resize( x, y, dotParent->w()/4*3, h );
                                //window->add(vizgroup);
                        } else {
                                //graphinfo = new GraphInfo( x()+w()/4*3, y(), w()/4,h );
                                //window->add(graphinfo);
                                dotParent->remove(graphinfo);
                                vizgroup->resize( x, y, dotParent->w(), h );
                                //vizgroup->resize( window->x(), window->y(),   window->w()/4*3, h );
                                //window->add(vizgroup);
                        }
                        } break;
                case MAIN_SETLAYOUT: {
                        // change layout of vizgroup
                        vizgroup->vizLayout( layout );
                        vizgroup->showAll();
                        } break;
                case MAIN_ABOUT: {
                        fl_message("DOT3D - Nils Thuerey, 2003.\nThis tool is a part of the ROSE project, see\nhttp://www.llnl.gov/rose for details.");
                        } break;
                default:
                        cerr << "Warning - invalid main menu id: e"<< event <<", " << (int)v << endl;
                        break;
        }
}

class myWindow : public Fl_Window {
        public:
                myWindow(int x,int y) : Fl_Window(x,y) { };
                virtual ~myWindow() { };
                void resize(int x,int y,int w,int h) {
                        int wv1=0,wv2=0, wg1=0,wg2=0;
                        if((vizgroup)&&(graphinfo)) {
                                wv1 = vizgroup->w();
                                wg1 = graphinfo->w();
                        }
                        Fl_Window::resize(x,y,w,h);
                        if((vizgroup)&&(graphinfo)) {
                                wv2 = vizgroup->w();
                                wg2 = graphinfo->w();
                                if((wg1!=wg2)&& (wg2>(w/4)) ) {
                                        dotParent->position( (dotParent->w()-wg2), 0, (dotParent->w()-wg1), 0);
                                }
                        }
                };
};


//-----------------------------------------------------------------------------
// main prog, load dot, setup FLTK
int main(int argc, char **argv) {
        int ret=0;                              // main prog return value

        // init dot
        dotneato_initialize( 1, argv );
        cout << "DOT - init " << endl;

        // files to load?
        vector<string> graphFiles;
        for(int i=1; i<argc; i++) {
                graphFiles.push_back( string( argv[i] ));
        }

  // glf init, for fonts in dotviz
  glfInit();
  //glfLoadFont("./fonts/times_new1.glf");
  //glfLoadFont("./fonts/arial1.glf");
  glfLoadMemFont( FONT_ARIAL );
  //glfLoadBMFFont("./fonts/fixedsys.bmf");  

                //} // debug out
                
                // open gl display
        //A DotVisualizer *viz = new DotVisualizer( &argc, argv, G);
                //A viz->finishInit();
                //A viz->start();
                {
                        //Fl_Window *window = new Fl_Window( 900, 450 );
                        Fl_Window *window = new myWindow( 900, 450 );
                        // fullscreen size
                        window->size( Fl::w()-400, Fl::h()-400 );
                        window->begin();
                        int x = window->x();
                        int y = window->y();
                        int w = window->w();
                        int h = window->h();

                        Fl_Menu_Bar *menu = new Fl_Menu_Bar(0,0, w, MAIN_MENU_HEIGHT );
                        menu->copy( mainMenuItems );

                        y += MAIN_MENU_HEIGHT;
                        h -= MAIN_MENU_HEIGHT;

                        Fl_Tile *tile = new Fl_Tile( x,y,w,h );
                        //Fl_Group *tile = new Fl_Group( x,y,w,h );
                        //x+=5; y+=5; w-=10; h-=10;

                        //Fl_Tabs *tabs = new Fl_Tabs( x+w/4*3, y, w/4,h, "");
                        graphinfo = new GraphInfo( x+w/4*3, y, w/4,h );
                        //graphinfo->resizable();
                        //tile->add(graphinfo);

                        // triple layout
                        vizgroup = new VizGroup( x, y,  w/4*3, h );
                        vizgroup->setGraphInfo( graphinfo );
                        //vizgroup->vizLayout( VizGroup::triple );
                        vizgroup->vizMultipleLoad( graphFiles );
                        vizgroup->resizable();
                        //tile->add(vizgroup);

                        //Fl_Group *group1 = addVizGroup( x+w/4*1, y,                   w/4*2,  h               ,0,NULL);
                        //Fl_Group *group2 = addVizGroup( x+w/4*0, y+h/2*0, w/4,                h/2             ,1,NULL);
                        //Fl_Group *group3 = addVizGroup( x+w/4*0, y+h/2*1, w/4,                h/2             ,2,NULL);

                        //DotVisualizer *viz2 = new DotVisualizer( G, 20, 40+100+20+220+20, 260, 220 );
                        //viz2->callback( DotVisualizer::staticCallback, viz2 );

                        //Fl_Box *tileSize = new Fl_Box( 10,10, tile->w()-20, tile->h()-20 );
                        //tile->resizable( tileSize );
                        tile->end();
                        dotParent = tile;

                        window->end();
                        window->size_range(300,300);
                        window->resizable( dotParent );
                        int wargc = 1;
                        char *wargv[] = {"dotgl"};
                        window->show(wargc,wargv);
                        //group1->show(); group2->show(); group3->show();
                        vizgroup->showAll();
                        graphinfo->show();

                        for (;;) {
                                if(window->visible()) {
                                        if (!Fl::check()) break;        // returns immediately
                                } else {
                                        if (!Fl::wait()) break; // waits until something happens
                                }

                                vizgroup->doIdleRedraw();
                                // set by some viz object or menu
                                if(globalQuit) break;
                        }
                }

        //}

        // whats this for?
        dotneato_terminate(); // doesnt return?
  exit( ret );
}



