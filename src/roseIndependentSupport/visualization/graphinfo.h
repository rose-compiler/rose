/******************************************************************************
 *
 * DOT3D - An OpenGL dot file viewer
 * Nils Thuerey, 2003
 *
 * graph info display widget
 *
 *****************************************************************************/

#ifndef GRAPHINFO_H
#define GRAPHINFO_H

#include <FL/Fl.h>
#include <FL/Fl_Tabs.h>
#include <FL/Fl_Box.h>
#include <string>
#include <vector>

// DQ (12/6/2003): Added to support compilation with g++ 3.x
using std::string;
using std::vector;

class Fl_Text_Buffer;
class Fl_Text_Editor;
class Fl_Text_Display;
#define EDIT_TYPE Fl_Text_Display

// ------------------------------------------------------------------------------
// patch necessary (v 1.1.4) to view selection even when not focused:
// src/Fl_Text_Display.cxx, line 1583:
// < if ( style & (HIGHLIGHT_MASK | PRIMARY_MASK) && Fl::focus() == this) {
// > if ( style & (HIGHLIGHT_MASK | PRIMARY_MASK) ) {
// and line 1588:  
// < } else if ( style & (HIGHLIGHT_MASK | PRIMARY_MASK) && Fl::focus() == this ) {
// > } else if ( style & (HIGHLIGHT_MASK | PRIMARY_MASK) ) {
//
                
                        

// include dot/neato functions
extern "C" {
#include <dotneato.h>
};


class GraphInfo :
        public Fl_Tabs
{
        public:

                //! constructor
                GraphInfo(int x,int y,int w,int h);

                //! destructor
                ~GraphInfo();

                //! show info about this node
                void showInfo(Agnode_t *node);

                //! set hide standard attr flag
                void setHideStdAttr(bool set) { mHideStdAttr = set; }

        private:

                //! store source file locations
                typedef struct {
                        string  filename;
                        int             line;
                        int             column;
                } SourceLocation;
                

                //! pointer to the box containing the node info
                Fl_Box *mpInfBox;

                //! string storing the info about a node
                string mInfo;

                //! hide standard dot attributes?
                bool mHideStdAttr;

                //! list source file locations in the current node
                vector<SourceLocation> mSrcLocs;

                //! save a certain number of file buffers for quicker access
                vector<Fl_Text_Buffer *> mBuffers;

                //! the filenames of the loaded buffers
                vector<string> mBufferFiles;

                //! remember buffer that was last loaded
                int mLastBuffer;

                //! editor widget
                EDIT_TYPE *mpEditor;
                
};

#endif


