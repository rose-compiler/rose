// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>

using namespace std;


// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

string localOutputVisitorSupport ( string name ) {
     string s;
     s += string("          virtual void visit(");
     s += name;
     s += " *variable_";
     s += name;

  // DQ (11/26/2005): Not clear if these should be pure virtual
  // s += ") = 0;\n";

  // DQ (12/23/2005): Need to build the implementation and let 
  // derived classed overload as appropriate
     // DXN (08/27/2010): instead of do nothing, call visitDefault() which does nothing.
     s += ") { visitDefault(variable_" + name +"); }\n";

     return s;
}

/** the visit method for a given node type.
 * @param name a node type name
 */
string delegate2strategy ( string name ) {
     string s;
     s += string("    virtual void visit(");
     s += name;
     s += " *variable_";
     s += name;
     // delegates visit call to strategy:
     s += ") { _strategy->visit(variable_" + name +"); }\n";

     return s;
}

string Grammar::buildVisitorBaseClass() {
     string s = string("class ROSE_VisitorPattern  {\npublic:\n    virtual ~ROSE_VisitorPattern() {};\n");

     for (unsigned int i=0; i < terminalList.size(); i++) {
          string name = terminalList[i]->name;
          s += localOutputVisitorSupport(name);
        }
     // DXN (08/27/210): add the default case that does nothing as a catch-all;
     // Let the derived class override if so desired.
     s += "    virtual void visitDefault(SgNode* n) {}\n";
     s += "};\n\n";

     // DXN (08/28/2010): add template base class for visitors that return results.
     s += "template <class R> class Visitor_R: public ROSE_VisitorPattern {\n ";
     s += "protected:\n	 /** Result of a visit method.\n    */\n    R _result;\npublic:\n";
     s += "    virtual ~Visitor_R() {}\n	/**@return the result of the visitor's computation.\n    */\n";
     s += "    virtual R& getResult() {	return _result;	}\n";
     s += "    /**@return the result of the visitor's computation.\n	*/\n";
     s += "    virtual const R& getResult() const { return _result; }\n};\n\n";

     // DXN: (08/29/2010): add template base class for the strategy pattern.
     s += "/** Strategy pattern applied to visitors: serves as a context to a strategy,\n";
     s += " * which itself is a visitor; delegates all requests to the current strategy;\n";
     s += " * capable of switching strategy at will.  Derived classes may declare the\n";
     s += " * known strategies as friend to facilitate communication between the strategies\n";
     s += " * and their context.\n";
     s += " */\ntemplate <class R> class ContextVis_R: public Visitor_R<R> {\n";
     s += "protected:\n	/** Strategy to process a node, can be set dynamically.\n	*/\n";
     s += "    ROSE_VisitorPattern* _strategy; // not owned by this context.\n public:\n";
     s += "    virtual ~ContextVis_R() {_strategy = NULL; }\n";
     s += "    /** Allows the strategy to be set dynamically.\n";
     s += "     * @param strategy a visitor to process a node.\n    */\n";
     s += "    void setStrategy(ROSE_VisitorPattern* strategy)  {_strategy = strategy; }\n\n";
     for (int i = 0; i < terminalList.size(); i++) {
         string name = terminalList[i]->name;
         s += delegate2strategy(name);
     }
     s += "    virtual void visitDefault(SgNode* n) { _strategy->visitDefault(n); }\n";
     s += "};\n";

     return s;
}



