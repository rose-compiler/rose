#include "rose.h"
#include "aterm1.h"
#include "atermTranslation.h"

int main (int argc, char* argv[]) {
  ATerm bottom;
  SgProject* sageProject = frontend(argc,argv);
  ATinit(argc, argv, &bottom);

  ATerm term = convertNodeToAterm(sageProject /*.get_file(0).get_root()*/);
  ROSE_ASSERT (term);
#if 0
  AST_FILE_IO::startUp(sageProject);
  std::string astBlob = AST_FILE_IO::writeASTToString();
  term = ATsetAnnotation(term, 
                         ATmake("ast"),
                         ATmake("<blob>", astBlob.length(), astBlob.data()));
#endif
  ATwriteToBinaryFile(term, stdout);

  return 0;
}
