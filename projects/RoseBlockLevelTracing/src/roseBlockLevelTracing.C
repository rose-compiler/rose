#include "rose.h"
#include "compilationFileDatabase.h"
#include <iostream>
#include <sstream>
#include <functional>
#include <numeric>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>
#include "common.h"


using namespace std;
using namespace boost::algorithm;
using namespace SageBuilder;
using namespace SageInterface;

#define ROSE_BIN_DIR_INSTRUMENTATION_UTIL  ROSE_INSTALLATION_PATH + string("/include/roseTraceLib.c")

// Adds an "extern int ROSE_Tracing_Instrumentor(uint64_t)" declaration in each file
void BuildInstrumentationFunctionDeclaration(SgNode * node){
    static boost::unordered_map<SgGlobal*, bool> globalScopes;
    
    SgGlobal * globalScope = TransformationSupport::getGlobalScope(node);
    if(globalScopes.find(globalScope) == globalScopes.end()) {
        SgFunctionDeclaration *  functionDeclaration = SageBuilder::buildNondefiningFunctionDeclaration (
                                                                                                         SgName("ROSE_Tracing_Instrumentor") /*name*/,
                                                                                                         SageBuilder::buildSignedIntType() /*return_type*/,
                                                                                                         SageBuilder::buildFunctionParameterList(SageBuilder::buildInitializedName( SgName("id"), SageBuilder::buildUnsignedLongType())) /*parameter_list*/,
                                                                                                         globalScope  /* globalScope SgScopeStatement *scope=NULL*/);
        
        globalScope->insertStatementInScope(functionDeclaration ,  true);
        globalScopes[globalScope] = true;
    }
}


// Builds a statement of the form: {ROSE_Tracing_Instrumentor(1645UL);}
SgStatement * GetInstumentationStatement(SgNode * node, uint64_t id){
    BuildInstrumentationFunctionDeclaration(node);
    uint64_t uniqueId = id; //GetUniqueTraceIdForNode(node);
    SgExprStatement  * functionCallStmt = buildFunctionCallStmt (SgName("ROSE_Tracing_Instrumentor"), /* SgName name */
                                                                 buildSignedIntType() /* return type */,
                                                                 SageBuilder::buildExprListExp(SageBuilder::buildUnsignedLongVal (uniqueId)), /* SgExprListExp *parameters=NULL*/
                                                                 getScope(node) /* scope */);
    return  SageBuilder::buildBasicBlock (isSgStatement(functionCallStmt));
}


// Builds an expression o of the form: ROSE_Tracing_Instrumentor(1645UL)
SgExpression *  GetInstumentationExpression(SgNode * node, uint64_t id){
    BuildInstrumentationFunctionDeclaration(node);
    uint64_t uniqueId = id; //GetUniqueTraceIdForNode(node);
    SgFunctionCallExp * functionCallExp = SageBuilder::buildFunctionCallExp (SgName("ROSE_Tracing_Instrumentor"),
                                                                             buildSignedIntType() /* return type */,
                                                                             SageBuilder::buildExprListExp(SageBuilder::buildUnsignedLongVal (uniqueId)), /* SgExprListExp *parameters=NULL*/
                                                                             getScope(node) /* scope */);
    return isSgExpression(functionCallExp);
}


// Detects if the given file has main in it.
class MainDetector: public AstSimpleProcessing {
    SgFunctionDeclaration* mainFunc;
protected:
    void virtual visit(SgNode * node) {
        if (SageInterface::isMain(node))
            mainFunc = isSgFunctionDeclaration(node);
    }
    
public:
    MainDetector(): mainFunc(NULL){}
    SgFunctionDeclaration * GetMain() {
        return mainFunc;
    }
};



// If the file has main insert a #include directive to our ROSE instrumentation library
void BuildInstrumentationFunctionDefinitionIfMainIsDetected(int argc, char ** argv, SgNode * node){
    MainDetector mainDetector;
    mainDetector.traverse(node, preorder);
    if(!mainDetector.GetMain())
        return;
    string includeString = "#include \"" + string(ROSE_BIN_DIR_INSTRUMENTATION_UTIL) + "\"\n";
    PreprocessingInfo * preprocessingInfo = new  PreprocessingInfo (PreprocessingInfo::CpreprocessorIncludeDeclaration, includeString, string("rose-tracing-lib"), 0, 0, 0, PreprocessingInfo::before );
    mainDetector.GetMain()->addToAttachedPreprocessingInfo(preprocessingInfo);
    
}


// A mapping from SgNode to a unique trace id.
static boost::unordered_map<SgNode*, TraceId> nodeToIdMap;


// A simple preorder traversl to assign unique ids to each node.
// The same order is used again in IdToNodeMapper to retrieve the same ids.
class NodeToIdMapper: public AstSimpleProcessing {
    string m_fileUnderCompilation;
    uint32_t m_fileIdUnderCompilation;
    uint32_t m_counter;
    string m_roseDBFile;
public:
    NodeToIdMapper(SgFile * fileRoot, const string & dbFile){
        m_fileUnderCompilation = fileRoot->get_file_info()->get_physical_filename();
        m_roseDBFile = dbFile;
        m_counter = 0;
        m_fileIdUnderCompilation  = rose::GetProjectWideUniqueIdForPhysicalFile(m_roseDBFile, m_fileUnderCompilation);
        traverse(fileRoot, preorder);
    }
private:
    void virtual visit(SgNode * node) {
        TraceId id(m_fileIdUnderCompilation, m_counter++);
        nodeToIdMap.insert(std::make_pair<SgNode*, TraceId> (node, id));
        cout<<"\n"<< std::hex << id.GetSerializableId() << " : mapped to " << std::hex << node;
        
    }
    
};


// Obtain the trace id given a Sage node.
TraceId GetUniqueTraceIdForNode(SgNode * node){
    return (*nodeToIdMap.find(node)).second;
}

// This is the core instrumentation visitor.
// It visits all nodes and collects each block (or even statements that have a single statemnt expression body), conditional expressions, label statements, and short-circuit expressions.
// The above list should be exhaustive list of places where control can diverge.
// Then it inserts instrumentation statements or expressions in each of these places as appropriate. The id for the instrumentation would be precomputed and used here.

class NodesToInstrumentVisitor: public ROSE_VisitorPatternDefaultBase {
private:
    
    // vector of basic blocks to insert instrumentation **into**
    vector< pair<SgBasicBlock*, TraceId > >  m_basicBlockToInstrument;
    // vector of label statements to insert instrumentation **after**
    vector< pair<SgLabelStatement*, TraceId > >  m_labelStatementToInstrument;
    // vector of expressions  to insert instrumentation **before**
    vector< pair<SgExpression*, TraceId > >  m_expressionsToInstrument;
public:
    virtual ~NodesToInstrumentVisitor() {
        m_basicBlockToInstrument.clear();
        m_labelStatementToInstrument.clear();
        m_expressionsToInstrument.clear();
    }
    
    void Instrument() {
        // Insert instrumentation into each basic block
        for (vector< pair<SgBasicBlock*, TraceId > >::iterator it = m_basicBlockToInstrument.begin(); it != m_basicBlockToInstrument.end(); it++) {
            SgBasicBlock* block = (*it).first;
            uint64_t uniqueId = ((*it).second).GetSerializableId();
            block->prepend_statement(GetInstumentationStatement(block, uniqueId));
            cout<<"\n Instrumented :" << block->class_name();
        }
        // Insert instrumentation after each label statement
        for (vector< pair<SgLabelStatement*, TraceId > >::iterator it = m_labelStatementToInstrument.begin(); it != m_labelStatementToInstrument.end(); it++) {
            SgLabelStatement* label = (*it).first;
            uint64_t uniqueId = ((*it).second).GetSerializableId();
            insertStatement (label, GetInstumentationStatement(label, uniqueId), /*insertBefore=*/false, /*bool autoMovePreprocessingInfo=*/true);
            cout<<"\n Instrumented :" << label->class_name();
        }
        // Insert instrumentation before expressions
        for (vector< pair<SgExpression*, TraceId > >::iterator it = m_expressionsToInstrument.begin(); it != m_expressionsToInstrument.end(); it++) {
            SgExpression * expr = (*it).first;
            uint64_t uniqueId = ((*it).second).GetSerializableId();
            // if RHS is a NULL expression
            if(isSgNullExpression(expr)) {
                SageInterface::replaceExpression (/* old = */expr, /*new =*/GetInstumentationExpression(expr, uniqueId), /*bool keepOldExp=*/false);
            } else {
                SageInterface::insertBeforeUsingCommaOp (GetInstumentationExpression(expr, uniqueId), expr);
            }
            cout<<"\n Instrumented :" << expr->class_name();
        }
    }
    
protected:
    virtual void visit(SgNode * node) {
    }
    void AddExpressionToInstrumentationList(SgExpression * node){
        TraceId id =  GetUniqueTraceIdForNode(node);
        cout<<"\n"<<node->class_name() << ": File id = " <<  std::hex << id.fileId << " : node id = " << std::hex << id.nodeId << " : serialized id = " <<  std::hex << id.GetSerializableId();
        m_expressionsToInstrument.push_back(make_pair<SgExpression*, TraceId>(node, id));
    }
    // collect basic blocks needed to be instrumented
    virtual void visit(SgBasicBlock * node){
        TraceId id =  GetUniqueTraceIdForNode(node);
        cout<<"\n"<<node->class_name() << ": File id = " <<  std::hex << id.fileId << " : node id = " << std::hex << id.nodeId << " : serialized id = " <<  std::hex << id.GetSerializableId();
        m_basicBlockToInstrument.push_back(make_pair<SgBasicBlock*, TraceId>(node, id));
    }
    // collect label statements needed to be instrumented
    virtual void visit(SgLabelStatement * node){
        TraceId id =  GetUniqueTraceIdForNode(node);
        cout<<"\n"<<node->class_name() << ": File id = " <<  std::hex << id.fileId << " : node id = " << std::hex << id.nodeId << " : serialized id = " <<  std::hex << id.GetSerializableId();
        m_labelStatementToInstrument.push_back(make_pair<SgLabelStatement*, TraceId>(node, id));
    }
    // Collect true false arms of ?: expression to be instrumented
    virtual void visit(SgConditionalExp * node){
        AddExpressionToInstrumentationList(node->get_true_exp());
        AddExpressionToInstrumentationList(node->get_false_exp());
    }
    // collect single statement bodies to be instrumented.
    virtual void visit( SgExprStatement * node){
        // Any Expression statement that is a body needs to be instrumented.
        if(SageInterface::isBodyStatement (node)) {
            AddExpressionToInstrumentationList(node->get_expression ());
        }
    }
    // Collect both sides of && expression for instrumentation
    virtual void visit(SgAndOp * node){
        AddExpressionToInstrumentationList(node->get_lhs_operand());
        AddExpressionToInstrumentationList(node->get_rhs_operand());
    }
    // Collect both sides of || expression for instrumentation
    virtual void visit(SgOrOp * node){
        AddExpressionToInstrumentationList(node->get_lhs_operand());
        AddExpressionToInstrumentationList(node->get_rhs_operand());
    }
};



// A simple preorder AST traverser to call a visitor on each node to perform instrumentation.
class RoseTraceInstrumentor: public AstSimpleProcessing {
private:
    NodesToInstrumentVisitor * nodesToInstrumentVisitor;
public:
    RoseTraceInstrumentor(SgFile * fileRoot, const string & dbFile){
        // Walk over all nodes of this file in preoder and gather their node ids needed for inserting instrumentation.
        NodeToIdMapper mapper(fileRoot, dbFile);
        nodesToInstrumentVisitor = new NodesToInstrumentVisitor();
    }
    
    virtual ~RoseTraceInstrumentor(){
        delete nodesToInstrumentVisitor;
    }
    
    void Instrument(SgFile * node) {
        // Collect all nodes required for instrumenting. Follow preorder traversal
        traverse(node, preorder);
        ROSE_ASSERT(nodesToInstrumentVisitor);
        // Insert instrumantation
        nodesToInstrumentVisitor->Instrument();
    }
    
protected:
    void visit(SgNode * node) {
        node->accept(*nodesToInstrumentVisitor);
    }
    
};


int main( int argc, char * argv[] ) {
    // Generate the ROSE AST.
    SgProject* project = frontend(argc,argv);
    
    // AST consistency tests (optional for users, but this enforces more of our tests)
    AstTests::runAllTests(project);
    for(int i = 0 ; i < project->numberOfFiles(); i++) {
        SgFile & file = project->get_file(i);
        // Call the instrumentor to insert instrumentation
        RoseTraceInstrumentor roseTraceInstrumentor(&file, project->get_projectSpecificDatabaseFile());
        roseTraceInstrumentor.Instrument(&file);
        
        // If the file has main, insert the definitions by inclusing "roceTraceLib.c" file
        BuildInstrumentationFunctionDefinitionIfMainIsDetected(argc,argv, &file);
    }
    
    //generateDOT ( *project );
    //const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
    //generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
    
    
    
    // regenerate the source code and call the vendor
    // compiler, only backend error code is reported.
    return backend(project);
}
