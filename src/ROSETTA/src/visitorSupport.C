// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <string>

// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

static void
emitLocalOutputVisitorSupport(std::ostream &header, std::ostream &impl, const std::string &visitorType,
                              const std::string &nodeType) {
    header <<"    virtual void visit(" <<nodeType <<"*);\n";

    // DQ (12/23/2005): Need to build the implementation and let derived classed overload as appropriate
    // DXN (08/27/2010): instead of do nothing, call visitDefault() which does nothing.
    impl <<"\n"
         <<"void " <<visitorType <<"::visit(" <<nodeType <<" *x) {\n"
         <<"    visitDefault(x);\n"
         <<"}\n";
}

static void
emitLocalOutputVisitorParentSupport(std::ostream &header, std::ostream &impl, const std::string &visitorType,
                                    const std::string &nodeType) {
    if (nodeType == "SgNode") {
        emitLocalOutputVisitorSupport(header, impl, visitorType, nodeType);

    } else {
        header <<"    void visit(" <<nodeType <<"*);\n";

        impl <<"\n"
             <<"void " <<visitorType <<"::visit(" <<nodeType <<" *x) {\n"
             <<"    visit(static_cast<" <<nodeType <<"::base_node_type*>(x));\n"
             <<"}\n";
    }
}

// The visit method for a given node type. Generates code that delegates visit call to strategy.
static void
emitDelegateToStrategy(std::ostream &header, const std::string &nodeType) {
    header <<"\n"
           <<"    virtual void visit(" <<nodeType <<" *x) {\n"
           <<"        _strategy->visit(x);\n"
           <<"    }\n";
}

void
Grammar::emitVisitorBaseClass(std::ostream &header, std::ostream &impl) {
    impl <<"#include <Cxx_GrammarVisitorSupport.h>\n"
         <<"\n"
         <<"#include <Cxx_Grammar.h>\n";

    header <<"#ifndef ROSE_CxxGrammarVisitorSupport_H\n"
           <<"#define ROSE_CxxGrammarVisitorSupport_H\n"
           <<"// #line " <<__LINE__ <<" \"" <<__FILE__ <<"\"\n"
           <<"#include <Cxx_GrammarDeclarations.h>\n"
           <<"\n"
           <<"#ifndef SWIG\n"
           <<"class ROSE_VisitorPattern  {\n"
           <<"public:\n"
           <<"    virtual ~ROSE_VisitorPattern() {};\n"
           <<"    virtual void visitDefault(SgNode*) {}\n"
           <<"\n";

    for (const auto &terminal: terminalList)
        emitLocalOutputVisitorSupport(header, impl, "ROSE_VisitorPattern", terminal->name);

    header <<"};\n"
           <<"\n"
           <<"class ROSE_VisitorPatternDefaultBase: public ROSE_VisitorPattern {\n"
           <<"public:\n";
    for (const auto &terminal: terminalList)
        emitLocalOutputVisitorParentSupport(header, impl, "ROSE_VisitorPatternDefaultBase", terminal->name);

    header <<"};\n"
           <<"\n";

    // DXN (08/28/2010): add template base class for visitors that return results.
    header << "template<class R>\n"
           <<"class Visitor_R: public ROSE_VisitorPattern {\n"
           <<"protected:\n"
           <<"    /** Result of a visit method. */\n"
           <<"    R _result;\n"
           <<"\n"
           <<"public:\n"
           <<"    virtual ~Visitor_R() {}\n"
           <<"\n"
           <<"    /** Return the result of the visitor's computation. */\n"
           <<"    virtual R& getResult() {\n"
           <<"        return _result;\n"
           <<"    }\n"
           <<"\n"
           << "   /** Return the result of the visitor's computation. */\n"
           << "   virtual const R& getResult() const {\n"
           <<"        return _result;\n"
           <<"    }\n"
           <<"};\n"
           <<"\n";

    // DXN: (08/29/2010): add template base class for the strategy pattern.
    header <<"/** Strategy pattern applied to visitors.\n"
           <<" *\n"
           <<" * Serves as a context to a strategy,\n"
           <<" * which itself is a visitor; delegates all requests to the current strategy;\n"
           <<" * capable of switching strategy at will.  Derived classes may declare the\n"
           <<" * known strategies as friend to facilitate communication between the strategies\n"
           <<" * and their context. */\n"
           <<"template<class R>\n"
           <<"class ContextVis_R: public Visitor_R<R> {\n"
           <<"protected:\n"
           <<"    /** Strategy to process a node, can be set dynamically. */\n"
           <<"    ROSE_VisitorPattern* _strategy; // not owned by this context.\n"
           <<"\n"
           <<"public:\n"
           <<"    virtual ~ContextVis_R() {\n"
           <<"        _strategy = nullptr;\n"
           <<"    }\n"
           <<"\n"
           <<"    /** Allows the strategy to be set dynamically.\n"
           <<"      *\n"
           <<"      * @param strategy a visitor to process a node. */\n"
           <<"    void setStrategy(ROSE_VisitorPattern* strategy) {\n"
           <<"        _strategy = strategy;\n"
           <<"    }\n"
           <<"\n"
           <<"    virtual void visitDefault(SgNode *x) {\n"
           <<"        _strategy->visitDefault(x);\n"
           <<"    }\n";
    for (const auto &terminal: terminalList)
        emitDelegateToStrategy(header, terminal->name);

    header <<"};\n"
           <<"\n"
           <<"#endif // SWIG\n"
           <<"#endif // include-once\n";

}
