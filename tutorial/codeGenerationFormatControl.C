// This example will be made more sophisticated later, for now it just
// modifies the indentation of nested code blocks (from 2 spaces/block 
// to 5 spaces/block).

#include "rose.h"
#include "unparseFormatHelp.h"

class CustomCodeFormat : public UnparseFormatHelp
   {
     public:
          CustomCodeFormat();
         ~CustomCodeFormat();

          virtual int getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);
          virtual int getCol ( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt);

       // return the value for indentation of code (part of control over style)
          virtual int tabIndent ();

       // return the value for where line wrapping starts (part of control over style)
          virtual int maxLineLength ();

     private:
          int defaultLineLength;
          int defaultIndentation;
   };


CustomCodeFormat::CustomCodeFormat()
   {
  // default values here!
     defaultLineLength = 20;
     defaultIndentation = 5;
   }

CustomCodeFormat::~CustomCodeFormat()
   {}

// return: > 0: start new lines; == 0: use same line; < 0:default
int
CustomCodeFormat::getLine( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt)  
   {
  // Use default mechanism to select the line where to output generated code
     return -1;
   }

// return starting column. if < 0, use default
int
CustomCodeFormat::getCol( SgLocatedNode*, SgUnparse_Info& info, FormatOpt opt) 
   {
  // Use default mechanism to select the column where to output generated code
     return -1;
   }

int
CustomCodeFormat::tabIndent()
   {
  // Modify the indentation of the generated code (trival example of tailoring code generation)
     return defaultIndentation;
   }

int
CustomCodeFormat::maxLineLength()
   {
     return defaultLineLength;
   }


int main (int argc, char* argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project = new SgProject(argc,argv);

     CustomCodeFormat* formatControl = new CustomCodeFormat();

     return backend(project,formatControl);
   }
