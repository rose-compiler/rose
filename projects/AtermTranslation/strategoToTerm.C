#include "rose.h"
#include "aterm1.h"
#include "atermTranslation.h"

using namespace std;

int main (int argc, char* argv[]) {
  ATerm bottom;
  ATinit(argc, argv, &bottom);

  ROSE_ASSERT (argc == 1);

  ATerm term = ATreadFromFile(stdin);
  ROSE_ASSERT (term);
  SgNode* node = convertAtermToNode(term);
  ROSE_ASSERT (isSgProject(node));
  if (node) {
    cleanAstMadeFromAterm(node);
    cout << node->unparseToCompleteString() << endl;
#if 0
    AstPDFGeneration pdf;
    pdf.generateInputFiles(isSgProject(node));
#endif
  } else {
    cout << "NULL" << endl;
  }
  // isSgProject(node)->unparse();

  return 0;
}

