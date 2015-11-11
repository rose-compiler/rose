#include <string>
#include <iostream>

#include "rose.h"

#include "annotatePragmas.h"

using namespace std;
using namespace SageBuilder;
using namespace SageInterface;

static bool debug = false;

static const string pragmaAttrib = "PragmaAttribute";

string ppPresRemUnd (PresRemUnd x) {
  switch (x) {
    case pragma_preserve : return "preserve";
    case pragma_remove   : return "remove";
    case pragma_undef    : return "undef";
    default              : assert(0);
  }
}

class PRUAttribute : public AstAttribute {
  public:
    PRUAttribute(PresRemUnd v) : state(v) {}
    PRUAttribute *copy() const {
      return new PRUAttribute(*this);
    }
    std::string attribute_class_name() { return pragmaAttrib; }
    std::string toString() { return pragmaAttrib+" "+ppPresRemUnd(state); }

    PresRemUnd state;
};

PresRemUnd getPresRemUnd (SgStatement *s) {
  if( s->getAttribute(pragmaAttrib) != NULL ){
    const PRUAttribute *attr = dynamic_cast<const PRUAttribute*>(s->getAttribute(pragmaAttrib));
    ROSE_ASSERT( attr != NULL );
    return attr->state;
  }
  return pragma_undef;
}

PRUAttribute attr_undef    (pragma_undef);
PRUAttribute attr_preserve (pragma_preserve);
PRUAttribute attr_remove   (pragma_remove);

class Processing_PresRemUnd : public AstTopDownProcessing<PRUAttribute*> {
 protected:
  virtual PRUAttribute* evaluateInheritedAttribute(SgNode* astNode,
                                                   PRUAttribute *a) {

    SgStatement *s = isSgStatement(astNode);
    if (!s) return a;
    if (debug) cout << "A: stmt: " << s->unparseToString() << endl;
    if (debug) cout << "A: a = " << a->toString() << endl;
    if (debug) printf ("A: ptr = %p\n", s);

    SgStatement *prev = getPreviousStatement(s);
    if (prev) {
      if (debug) printf ("A: L1\n");
      SgPragmaDeclaration *pd = isSgPragmaDeclaration(prev);
      if (pd) {
        // Note: because of the way ROSE parses and then outputs pragmas, the
        // following will work with extra whitespace: before, between, and after
        // the keywords.

        string pragmaText = pd->get_pragma()->get_pragma();
        if (debug) cout << "A: pragmatext: " << pragmaText << endl;
        if (pragmaText == "skel remove")
          a = &attr_remove;
        else if (pragmaText == "skel preserve") {
          if (a == &attr_remove)
            cout << "warning: skel preserve in skel remove scope: ignored"
                 << endl;
          else
            a = &attr_preserve;
        } else if (pragmaText.compare(0,4,"skel") == 0) {
          // all other skel pragmas imply preserve.
          if (debug) cout << "A: skel-pragma: ";
          a = &attr_preserve;
        }
      }
    }
    if (debug)  cout << "A: attribute: " << a->toString() << endl << endl;

    astNode->setAttribute(pragmaAttrib, a);
    return a;
  }
};

void annotatePragmas (SgProject *project) {
  Processing_PresRemUnd p;
  p.traverseInputFiles(project, &attr_undef);
}
