#include "rose.h"

#ifdef ROSEQT_EXPERIMENTAL
    #include "PTracer.h"
#endif

namespace InstructionCountAnnotator  {

using namespace std;


void annotate(SgProject * proj,
              std::vector<std::string> args = std::vector<std::string>() )
{

#ifdef ROSEQT_EXPERIMENTAL
    PTracer tracer;

    // Get path of executable
    if(proj->numberOfFiles() <= 0)
    {
        cerr << "InstructionCountAnnotator - Error: No Files in project" << endl;
        return;
    }

    assert( proj->get_numberOfTraversalSuccessors() > 0);
    if( ! isSgBinaryFile(proj->get_traversalSuccessorByIndex(0)))
    {
        cerr << "InstructionCountAnnotator - Error: Not a binary AST" << endl;
        return;
    }

    // Build an array with path at the first entry
    Sg_File_Info * fi = (*proj)[0]->get_file_info();
    vector<string> newArgs;
    newArgs.reserve(args.size()+1);
    newArgs.push_back(fi->get_filenameString());
    for(int i=0; i< args.size(); i++)
        newArgs.push_back(args[i]);


    // Load the process
    tracer.loadProcess(newArgs);

    // Add a breakpoint at each assembler instruction
    typedef Rose_STL_Container<SgNode*>::iterator StatementIter;
    Rose_STL_Container<SgNode *> stmts = NodeQuery::querySubTree(proj,V_SgAsmInstruction);
    assert(stmts.size() > 0);

    for(StatementIter i= stmts.begin(); i != stmts.end(); ++i)
    {
        SgAsmStatement * s = isSgAsmStatement(*i);
        assert(s);
        tracer.addStatementForTracing(s);
    }


    tracer.run();

    // Annotate the AST
    const map<address,PTracer::BreakpointInfo> & bpInfo = tracer.getBreakpointInfoMap();
    for (PTracer::const_BpIter i= bpInfo.begin(); i != bpInfo.end(); ++i)
    {
        const PTracer::BreakpointInfo & info = i->second;
        info.getNode()->addNewAttribute("Execution Count", new MetricAttribute(info.getHitCount()));
        //if(info.getHitCount() > 0)
        //    cerr << "Annotating: " << info.getNode()->class_name() << " with " << dec << info.getHitCount()<< endl;
    }

#else
    cerr<< "Annotating ast with PTracer experimental (not ported to all platforms), use ROSEQT_EXPERIMENTAL flag" << endl;

#endif


}




} //namespace
