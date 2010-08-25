/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>
#include <qrose.h>

using namespace qrs;
using namespace std;

class QueryCollection {
public:

   bool findTopLevelLoops(SgNode *node, string &info) {
      if (!isLoop(node)) return false;
      SgNode *parent = node->get_parent();
      while (parent) {
        if (isLoop(parent)) return false;
        if (isSgFunctionDeclaration(parent)) {
            SgFunctionDeclaration *fdecl = (SgFunctionDeclaration *) parent;
            if (fdecl) {
               info = string(fdecl->get_name().str()) + "()";
               return true;
            }
        }
        parent = parent->get_parent();
      }

      return true;
   }


   bool findTopLevelConditionals(SgNode *node, string &) {
      if (!isConditional(node)) return false;
      SgNode *parent = node->get_parent();
      while (parent) { if (isConditional(parent)) return false;
                       parent = parent->get_parent();
                     }

      return true;
   }

protected:
   bool isLoop(SgNode *node) {
      return (isSgDoWhileStmt(node) || isSgForStatement(node) || isSgWhileStmt(node));
   }

   bool isConditional(SgNode *node) {
      return (isSgIfStmt(node));
   }
};

void installQueries() {

   // retrieve the QueryBox
   QRWindow &win = *QROSE::getWindow("query");
   QRQueryBox *queryBox = win["querybox"];

   // install two variant queries
   queryBox->insertVariantQuery("Loops", VariantVector(V_SgDoWhileStmt) +
                                VariantVector(V_SgForStatement) +
                                VariantVector(V_SgWhileStmt));

   queryBox->insertVariantQuery("Conditionals",
        VariantVector(V_SgIfStmt));

   // install two custom queries
   queryBox->insertCustomQuery<QueryCollection>("top-level loops",
             &QueryCollection::findTopLevelLoops);
   queryBox->insertCustomQuery<QueryCollection>("top-level conditionals",
             &QueryCollection::findTopLevelConditionals);
}


int main(int argc, char **argv) {

   SgProject *project = frontend(argc, argv);

   // initialize QRose
   QROSE::init(argc, argv);

   // Let's create a window with a splitter
   QRWindow &win = *new QRWindow("query", QROSE::UseSplitter);

   // And place a component in each dialog-box, by specifying each dialog-box
   // as its parent.
   win["sourcebox"] << new QRSourceBox;
   win["querybox"]  << new QRQueryBox(project);
   installQueries();

   // give 50% for each component
   win.setTileSize(50);


   // set position and size
   win.setGeometry(100,100,623,800);

   // start the main event loop
   return QROSE::exec();
}

