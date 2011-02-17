/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * graph info display widget
 *
 *****************************************************************************/

#include "graphinfo.h"

#include <FL/Fl_Group.h>
#include <FL/Fl_Scroll.h>
#include <FL/Fl_Text_Editor.h>
#include <FL/Fl_Text_Display.h>
#include <FL/Fl_Menu_Bar.h>
#include <sstream>
#include <iostream>

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::ostringstream;
using std::endl;
using std::istringstream;
using std::cerr;

// this includes the source code for syntax highlightin (also fucntion definitions)
#include "style.h"

//! predefined attributes from dot, that are usually hidden in the display
char *dotPredefAttr[] = {
        "Damping",
        "URL",
        "arrowhead",
        "arrowsize",
        "arrowtail",
        "bb",
        "bgcolor",
        "bottomlabel",
        "center",
        "clusterrank",
        "color",
        "comment",
        "compound",
        "concentrate",
        "constraint",
        "decorate",
        "dim",
        "dir",
        "distortion",
        "epsilon",
        "fillcolor",
        "fixedsize",
        "fontcolor",
        "fontname",
        "fontpath",
        "fontsize",
        "group",
        "headURL",
        "headclip",
        "headlabel",
        "headport",
        "headtooltip",
        "height",
        "label",
        "labelangle",
        "labeldistance",
        "labelfloat",
        "labelfontcolor",
        "labelfontname",
        "labelfontsize",
        "labeljust",
        "labelloc",
        "layer",
        "layers",
        "len",
        "lhead",
        "lp",
        "ltail",
        "margin",
        "maxiter",
        "mclimit",
        "minlen",
        "model",
        "nodesep",
        "normalize",
        "nslimit",
        "nslimit1",
        "ordering",
        "orientation",
        "orientation",
        "outputorder",
        "overlap",
        "pack",
        "packmode",
        "page",
        "pagedir",
        "pencolor",
        "peripheries",
        "pin",
        "pos",
        "quantum",
        "rank",
        "rankdir",
        "ranksep",
        "ratio",
        "rects",
        "regular",
        "remincross",
        "resolution",
        "root",
        "rotate",
        "samehead",
        "sametail",
        "samplepoints",
        "searchsize",
        "sep",
        "shape",
        "shapefile",
        "showboxes",
        "sides",
        "size",
        "skew",
        "splines",
        "start",
        "style",
        "stylesheet",
        "tailURL",
        "tailclip",
        "taillabel",
        "tailport",
        "tailtooltip",
        "tooltip",
        "toplabel",
        "truecolor",
        "vertices",
        "voro_margin",
        "weight",
        "width",
        "z" };


//-----------------------------------------------------------------------------
//! constructor
GraphInfo::GraphInfo(int x,int y,int w,int h) :
        Fl_Tabs( x,y, w,h, "NodeInfo" ),
        mpInfBox( NULL ),
        mInfo( "---" ), mHideStdAttr( true )
{
        int tabHeight = 26;

        // the group for the node info display
        Fl_Group *ninfo = new Fl_Scroll( x, y+tabHeight, w,h-tabHeight, "Info");
        mpInfBox = new Fl_Box( ninfo->x(),ninfo->y(),ninfo->w(),ninfo->h(), "No node selected..." );
        mpInfBox->align( FL_ALIGN_TOP | FL_ALIGN_LEFT | FL_ALIGN_INSIDE );
        //mpInfBox->align( FL_ALIGN_TOP );
        mpInfBox->box(FL_NO_BOX);
        //mpInfBox->labelsize(26);
        //mpInfBox->resizable();
        ninfo->end();
        add( ninfo );
        //resizable(ninfo);

        // make a new group for the editor
        Fl_Group *editgrp = new Fl_Group( x, y+tabHeight, w,h-tabHeight, "Source");

        // create empty dummy buffer
        Fl_Text_Buffer *buffer = new Fl_Text_Buffer();
        /*FILE *textfile = fopen("./main.cpp", "r");
                if(! textfile) { exit(1); }
                size_t filesize = 9766;
                char *textbuf = (char *)malloc( sizeof(char) * filesize );
                cout << " asfhashf " << fread( textbuf, sizeof(char), filesize, textfile) << endl;
                fclose(textfile);
                free( textbuf );
                buffer->loadfile( textbuf, (int)filesize ); */
        //buffer->loadfile( "./main.cpp" ); // DEBUG test file
        style_init( buffer );
        mBuffers.push_back(buffer);
        mBufferFiles.push_back( "<dummy_buffer>" );
        mLastBuffer = 0;

        EDIT_TYPE *editor = new EDIT_TYPE( editgrp->x(),editgrp->y(),editgrp->w(),editgrp->h(), "" );
        editor->buffer( buffer );
        // init syntax highlighting
        editor->highlight_data(glob_stylebuf, styletable,
                        sizeof(styletable) / sizeof(styletable[0]),
                        'A', style_unfinished_cb, 0);                   
        buffer->add_modify_callback( style_update, editor);
        editor->show();
        mpEditor = editor;

        /*Fl_Box *box = new Fl_Box( editgrp->x(),editgrp->y(),editgrp->w(),editgrp->h(), "INFO" );
                box->box(FL_UP_BOX);
                box->labelsize(36);*/
        editgrp->end();
        add( editgrp );

        // tabs are finished...
        end();
        value( ninfo );
        //value( editor );
        show();
}

//-----------------------------------------------------------------------------
//! destructor
GraphInfo::~GraphInfo()
{
}


//-----------------------------------------------------------------------------
//! show info about this node
void GraphInfo::showInfo(Agnode_t *node)
{
        // search file source to display in editor
        string filesource;
        bool filefound = false;
        char *filePrefix = "file://";
        
        if(!mpInfBox) return;
        Agsym_t **list = node->graph->univ->nodeattr->list;
        ostringstream infostr;
        char *nodename = agget( node, "label" );
        if(!nodename) nodename = "[undefined]";
        infostr << endl << "Node Information for: '"<<nodename<<"' " << endl << endl;
        for(int i=0; list[i] != NULL; i++) {
                char *value = agget( node, list[i]->name );

                // do filtering
                bool dispthis = true;
                if(!strcmp(list[i]->name, "label")) dispthis = false;
                if(mHideStdAttr) {
                        for(int j=0;j<(int)(sizeof(dotPredefAttr)/sizeof(char *)); j++) {
                                if(!strcmp(list[i]->name, dotPredefAttr[j])) dispthis = false;
                        }
                }
                
                if((list[i])&&(value)&&(dispthis)) { 
                        infostr << list[i]->name<<"="<< value <<endl;
                        if((strlen(value)> strlen(filePrefix)) && (!filefound)) {
                                filefound = true;
                                for(size_t i=0;i<strlen(filePrefix);i++) {
                                        if(value[i]!=filePrefix[i]) {
                                                filefound = false;
                                                break;
                                        }
                                }
                                if(filefound) {
                                        char *name = value+strlen(filePrefix);
                                        filesource = name;
                                }
                        }
                }
        }

        // load file in editor?
        if(filefound) {
                bool ok = true;
                //cerr << " FOUND " << filesource << endl;
                string::size_type pos = 0, lastpos = 0;
                string filename, fileline, filecolumn, fileendline, fileendcolumn;
                int line = -1, column = -1, endline=-1, endcolumn=-1;
                pos = filesource.find( ':', lastpos);
                if(pos != string::npos) {
                        filename = filesource.substr(lastpos, pos-lastpos );
                        lastpos = pos;
                } else {
                        // no lines?
                        filename = filesource;
                        ok = false;
                }

                int valueCount = 0;
                while(valueCount<4) {
                        if(ok) {
                                lastpos++;
                                pos = filesource.find( ':', lastpos);
                                string tline("");
                                if(pos != string::npos) {
                                        tline = filesource.substr(lastpos, pos-lastpos );
                                        lastpos = pos;
                                } else {
                                        tline = filesource.substr(lastpos, filesource.length() );
                                        ok = false;
                                }
                                istringstream ist(tline);
                                int value = -1;
                                ist >> std::dec >> value;
                                if( ist.rdstate() != istringstream::goodbit ) value = -2;
                                switch(valueCount) {
                                        case 0: line = value; break;
                                        case 1: column = value; break;
                                        case 2: endline = value; break;
                                        case 3: endcolumn = value; break;
                                }
                        }
                        valueCount++;
                }
                //cerr << " FLINE " << line<<":"<< column << " to "<< endline<<":"<< endcolumn<< endl; // debug

                /*if(ok) {
                        lastpos++;
                        pos = filesource.find( ':', lastpos);
                        if(pos != string::npos) {
                                filecolumn = filesource.substr(lastpos, pos-lastpos );
                                lastpos = pos;
                        } else {
                                filecolumn = filesource.substr(lastpos, filesource.length() );
                                ok = false;
                        }
                                //line = atoi( fileline.c_str() );
                        istringstream ist(filecolumn);
                        ist >> std::dec >> column;
                        if( ist.rdstate() != istringstream::goodbit ) column = -2;
                        cerr << " FCOLM " << filecolumn<<" : "<< column<<" : "<< column << endl; // debug
                }

                if(ok) {
                        lastpos++;
                        pos = filesource.find( ':', lastpos);
                        if(pos != string::npos) {
                                filecolumn = filesource.substr(lastpos, pos-lastpos );
                                lastpos = pos;
                        } else {
                                filecolumn = filesource.substr(lastpos, filesource.length() );
                                ok = false;
                        }
                                //line = atoi( fileline.c_str() );
                        istringstream ist(filecolumn);
                        ist >> std::dec >> column;
                        if( ist.rdstate() != istringstream::goodbit ) column = -2;
                        cerr << " FCOLM " << filecolumn<<" : "<< column<<" : "<< column << endl; // debug
                }
                cerr << " FOUND " << filename<<" : "<< line<<" : "<< column << endl; // debug
                 */

                // check if file is already loaded
                Fl_Text_Buffer *buffer = NULL;
                if(mBuffers.size() != mBufferFiles.size()) {
                        cerr << "DOT3D FATAL: mBuffers and mBufferFiles are different size: "<< mBuffers.size()<<"-"<<mBufferFiles.size() << endl;
                        return;
                }
                for(size_t i=0; i<mBuffers.size(); i++ ) {
                        if(mBufferFiles[i] == filename) {
                                buffer = mBuffers[i];
                                break;
                        }
                }

                if(!buffer) {
                        // create new buffer and load
                        if(mBuffers.size() < 1) {
                                buffer = new Fl_Text_Buffer();
                                mBuffers.push_back(buffer);
                                mBufferFiles.push_back( filename );
                        } else {
                                // currently always use buffer 0
                                mLastBuffer = 0;
                                buffer = mBuffers[mLastBuffer];
                                mBufferFiles[mLastBuffer] = filename;
                        }
                        buffer->loadfile( filename.c_str() );
                }
                
                if((buffer)&&(mpEditor->buffer() != buffer)) {
                        // only use 1 buffer
                }

                if(line>0) {
                        int pos = buffer->skip_lines(0, line-1);
                        if(column>0) pos += (column-1);
                        int endpos = pos;
                        if(endline>0) {
                                endpos = buffer->skip_lines(pos, (endline-line));
                                if(endcolumn>0) endpos += (endcolumn-1);
                        } else {
                                // highlight whole line
                                endpos = buffer->skip_lines(pos, 1);
                        }
                        if(pos>=0) {
                                buffer->highlight( pos, endpos );
                                mpEditor->insert_position( pos );
                                mpEditor->show_insert_position();
                        }
                        //cerr << " LINES "<< pos<<"-"<<endpos <<" of "<< buffer->length() << " l"<< line << endl;
                }

        } // filefound

        
        mInfo = infostr.str();
        mpInfBox->label( mInfo.c_str() );
        //mpEditor->redraw();
        redraw();
}


