#if 0
#include "rose.h"

int
main ( int argc, char** argv )
{
  CppToPdfTranslator c;
  return c.translate(argc,argv);
}












#endif

#include "rose.h"
int
main ( int argc, char * argv[] )
   {
    SgProject* project = frontend(argc,argv);

    AST_FILE_IO::startUp( project ) ;
//    AST_FILE_IO::writeASTToFile("binaryAST.bin" );

    AstPDFGeneration pdf;
    pdf.generateInputFiles (project);
    return backend(project);
   }
