#include "rose.h"
#include "SingleStatementToBlockNormalization.h"
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

using namespace std;
using namespace boost::algorithm;
using namespace SageBuilder;
using namespace SageInterface;



struct TraceId {
    uint32_t fileId;
    uint32_t nodeId;
    TraceId(uint32_t fid, uint32_t nid): fileId(fid), nodeId(nid){}
    TraceId(uint64_t uniqueId): fileId(uniqueId>>32), nodeId(uniqueId & ffffffff){}
    uint64_t GetSerializableId() { return (uint64_t(fid) << 32) | nodeId}
};

class NodeToIdMapper: public AstSimpleProcessing {
    boost::unordered_map<SgNode, TraceId> nodeToIdMap;
    string fileUnderCompilation;
    string fileIdUnderCompilation;
    uint32_t counter;
    string roseDBFile;
protected:
    void virtual visit(SgNode * node) {
        TraceId tid(fileIdUnderCompilation, counter++);
        nodeToIdMap[node] = tid;
    }
public:
    NodeToIdMapper(const string &compilationFile, const string & dbFile):counter(0), fileUnderCompilation(compilationFile), roseDBFile(dbFile){
        fileIdUnderCompilation = ROSE_GetProjectWideUniqueIdForPhysicalFile(dbFile, compilationFile);
    }
    
    TraceId GetIdForNode(SgNode * node) {
        return nodeToIdMap[node];
    }
};




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


SgStatement * GetInstumentationStatement(SgNode * node, uint64_t id){
    BuildInstrumentationFunctionDeclaration(node);
    uint64_t uniqueId = id; //GetUniqueTraceIdForNode(node);
    SgExprStatement  * functionCallStmt = buildFunctionCallStmt (SgName("ROSE_Tracing_Instrumentor"), /* SgName name */
                                                                 buildSignedIntType() /* return type */,
                                                                 SageBuilder::buildExprListExp(SageBuilder::buildUnsignedLongVal (uniqueId)), /* SgExprListExp *parameters=NULL*/
                                                                 getScope(node) /* scope */);
    return  SageBuilder::buildBasicBlock (isSgStatement(functionCallStmt));
}

SgExpression *  GetInstumentationExpression(SgNode * node, uint64_t id){
    BuildInstrumentationFunctionDeclaration(node);
    uint64_t uniqueId = id; //GetUniqueTraceIdForNode(node);
    SgFunctionCallExp * functionCallExp = SageBuilder::buildFunctionCallExp (SgName("ROSE_Tracing_Instrumentor"),
                                                                             buildSignedIntType() /* return type */,
                                                                             SageBuilder::buildExprListExp(SageBuilder::buildUnsignedLongVal (uniqueId)), /* SgExprListExp *parameters=NULL*/
                                                                             getScope(node) /* scope */);
    return isSgExpression(functionCallExp);
}


bool ValidateAccurateFileLineAndColumn(SgNode *node){
    int fileId = node->get_file_info()->get_physical_file_id();
    if(fileId ==  Sg_File_Info::COPY_FILE_ID ||
       fileId ==  Sg_File_Info::NULL_FILE_ID ||
       fileId ==  Sg_File_Info::TRANSFORMATION_FILE_ID ||
       fileId ==  Sg_File_Info::COMPILER_GENERATED_FILE_ID ||
       fileId ==  Sg_File_Info::COMPILER_GENERATED_MARKED_FOR_OUTPUT_FILE_ID ||
       fileId ==  Sg_File_Info::BAD_FILE_ID ){
        cout<<"\n Failed to find file for an SgNode:"<<node->class_name();
        return false;
    }
    
    string physicalFile = node->get_file_info()->get_physical_filename();
    uint32_t line = node->get_file_info()->get_line();
    uint32_t col = node->get_file_info()->get_col();
    
    if(physicalFile.empty()) {
        cout<<"\n Failed to find valid file for an SgNode:"<<node->class_name();
        return false;
    }
    
    
    if(line == 0) {
        cout<<"\n Failed to find line number for an SgNode:"<<node->class_name();
        return false;
    }
    
    if(col == 0) {
        cout<<"\n Failed to find column number for an SgNode:"<<node->class_name();
        return false;
    }
    
    return true;
    
}


bool IsTemplateInstantiation(SgNode * node){
    
    if(!node)
        return false;
    
    if(SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(node)){
        if(isSgTemplateInstantiationFunctionDecl(node))
            return true;
        return IsTemplateInstantiation(funcDecl->get_scope());
    } else if(SgMemberFunctionDeclaration * memFunc = isSgMemberFunctionDeclaration(node)) {
        if(isSgTemplateInstantiationMemberFunctionDecl(memFunc))
            return true;
        return IsTemplateInstantiation(memFunc->get_scope());
        
    } else if (SgClassDeclaration * classDecl = isSgClassDeclaration(node)){
        if(isSgTemplateInstantiationDecl(classDecl))
            return true;
        return IsTemplateInstantiation(classDecl->get_scope());
    }
    
    SgStatement * stmt = isSgStatement(node);
    
    if(stmt && stmt->hasExplicitScope())
        return IsTemplateInstantiation(stmt->get_scope());
    else
        return IsTemplateInstantiation(node->get_parent());
    
    
}

SgExpression * GetNearestNodeWithSourceInfoAfterStrippingImplicitCasts(SgExpression * node){
    if(ValidateAccurateFileLineAndColumn(node))
        return node;
    else {
        SgCastExp * castExp = isSgCastExp(node);
        if(!castExp) {
            ROSE_ASSERT(0 && "Can't handle a node without source information");
            return NULL;
        }
        return GetNearestNodeWithSourceInfoAfterStrippingImplicitCasts(castExp->get_operand_i());
    }
}

SgStatement * GetNearestStatementWithSourceInfo(SgStatement * node){
    // if the statement has source info return it.
    if(ValidateAccurateFileLineAndColumn(node))
        return node;
    
    if(SgBasicBlock * block = isSgBasicBlock(node)){
        // Don't instrument empty compiler inserted SgBasicBlocks
        if (block->get_statements().begin() == block->get_statements().end()){
            return NULL;
        } else {
            // compiler inserted basic blocks
            // e.g., if () else /* compiler inserted  */if () {}
            // Get the nearest Statement that is not compiler generated.
            return GetNearestStatementWithSourceInfo(*(block->get_statements().begin()));
        }
        
    } else {
        ROSE_ASSERT(0 && " can't handle this case ...! you have found a bug to fix!");
        return NULL;
    }
    
}


uint32_t ROSE_GetProjectWideUniqueIdForPhysicalFile(const string & dbFile, const string & physicalFile) {
    int fd = open(dbFile.c_str(), O_RDWR | O_CREAT , O_SYNC | S_IRUSR| S_IWUSR | S_IRGRP | S_IWGRP );
    if (fd == -1) {
        std::cout<<"\n Failed to open the file:" << dbFile;
        exit(-1);
    }
    if(flock(fd,LOCK_EX) == -1) {
        cout<<"\n Failed to acquire lock on the file:" << dbFile;
        close(fd);
        exit(-1);
    }
    
    FILE * fptr = fdopen(fd, "r+");
    if(fptr == NULL){
        std::cout<<"\n Failed to fdopen file id:" << fd;
        flock(fd, LOCK_UN);
        close(fd);
        exit(-1);
    }
    
    char line [PATH_MAX];
    uint32_t index= 0;
    while(fgets(line,PATH_MAX,fptr) != NULL){
        // chop the trailing \n
        line[strlen(line) - 1] = '\0';
        if(physicalFile == string(line)) {
            flock(fd, LOCK_UN);
            fclose(fptr);
            return index;
        }
        index++;
    }
    // Add the new line
    if(fputs((physicalFile + "\n").c_str(),fptr) < 0 ){
        std::cout<<"\n Failed to fputs file:" << dbFile;
        flock(fd, LOCK_UN);
        close(fd);
        exit(-1);
    }
    
    flock(fd, LOCK_UN);
    fclose(fptr);
    return index;
    
}



class SimpleGlobalScopeCollector: public AstSimpleProcessing {
    SgGlobal * globalDeclarations;
protected:
    void virtual visit(SgNode * node) {
        if(SgGlobal * g = isSgGlobal(node)) {
            globalDeclarations = g;
        }
    }
    
public:
    SgGlobal * GetSgGlobal(){
        return globalDeclarations;
    }
};


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





//#define ROSE_BIN_DIR_INSTRUMENTATION_UTIL "/export/tmp.chabbi1/development/rose/tracing/build_tracing/projects/RoseBlockLevelTracing/src/me.c"

#define ROSE_BIN_DIR_INSTRUMENTATION_UTIL  ROSE_INSTALLATION_PATH + string("/lib/roseTracingLib.c")

class TransformationMarker: public AstSimpleProcessing {
protected:
    virtual void visit(SgNode * node) {
        if(node->get_file_info()) {
            if(node->get_file_info()->isFrontendSpecific () ||
               (node->get_file_info()->isCompilerGenerated() && (!node->get_file_info()->isCompilerGeneratedNodeToBeUnparsed()))
               ) {
                // Nop
                
            } else {
                // if the node belongs to the file trace library, so that we can eliminate the contents of header files.
                if(node->get_file_info()->get_physical_filename() == ROSE_BIN_DIR_INSTRUMENTATION_UTIL){
                    node->get_file_info()->setTransformation();
                    node->get_file_info()->setOutputInCodeGeneration ();
                }
            }
        }
    }
};


void BuildInstrumentationFunctionDefinitionIfMainIsDetected(int argc, char ** argv, SgNode * node){

    MainDetector mainDetector;
    mainDetector.traverse(node, preorder);
    if(!mainDetector.GetMain())
        return;

#if 0
    // file has main
    
    
    int fakeArgc = 3;
    char *fakeArgv[] = {argv[0], "-c", ROSE_BIN_DIR_INSTRUMENTATION_UTIL};
    SgProject* project = frontend(fakeArgc,fakeArgv);
    
    // Mark all nodes as Transofrmation
    TransformationMarker transformationMarker;
    transformationMarker.traverse(project, preorder);
    
    
    // Insert the AST into current project's global scope
    SgGlobal * globalScope = TransformationSupport::getGlobalScope(mainDetector.GetMain());
    
    
    SimpleGlobalScopeCollector simpleGlobalScopeCollector;
    simpleGlobalScopeCollector.traverse(project, preorder);
    
    
    SgDeclarationStatementPtrList & decls = simpleGlobalScopeCollector.GetSgGlobal()->get_declarations();
    for (SgDeclarationStatementPtrList::const_iterator it = decls.begin(); it != decls.end() ; ++it){
        
#if 1
        // Get the Symbol
        if( (*it)->hasAssociatedSymbol ()) {
            SgSymbol* mysymbol = isSgSymbol((*it)->search_for_symbol_from_symbol_table ());
            
            // If this symbol exits in the local symbol table, then skip .. e.g.,  contents of same #include files.
            
            if (mainDetector.GetMain()->get_scope()->get_symbol_table()->exists(mysymbol->get_name())) {
                //nop
            } else {
                // Insert the symbol in the hash table
                mainDetector.GetMain()->get_scope()->get_symbol_table()->insert (mysymbol->get_name(), mysymbol);
            }
        }
#endif
        
        
        // If the stmt has an explicit scope, set it to the global scope
        if((*it)->hasExplicitScope()) {
            (*it)->set_scope(mainDetector.GetMain()->get_scope());
        }
        
        // If it is a declaration and its nondefining delcaration is in a different scope, set it to this scope
        
        if(SgDeclarationStatement * declarationStatement = isSgDeclarationStatement(*it)) {
            SgDeclarationStatement * firstNonDefiningDeclaration = declarationStatement->get_firstNondefiningDeclaration();
            if(firstNonDefiningDeclaration != declarationStatement) {
                if(firstNonDefiningDeclaration->hasExplicitScope()) {
                    firstNonDefiningDeclaration->set_scope(mainDetector.GetMain()->get_scope());
                    firstNonDefiningDeclaration->set_parent(mainDetector.GetMain()->get_parent());
                }
            }
        }
        
        //globalScope->insertStatementInScope(*it,  false);
        SageInterface::insertStatementBefore(mainDetector.GetMain(), *it);
        
        ROSE_ASSERT((*it)->get_parent() == mainDetector.GetMain()->get_parent());
        ROSE_ASSERT((*it)->get_scope() == mainDetector.GetMain()->get_scope());

    }
/*
    for(vector<SgFunctionDefinition*>::iterator it = decls.begin(); it != decls.end(); it++) {
        // Insert function
        globalScope->insertStatementInScope(*it ,  false);
    }
*/
#else
    string includeString = "#include \"" + string(ROSE_BIN_DIR_INSTRUMENTATION_UTIL) + "\"\n";
    PreprocessingInfo * preprocessingInfo = new  PreprocessingInfo (PreprocessingInfo::CpreprocessorIncludeDeclaration, includeString, string("rose-tracing-lib"), 0, 0, 0, PreprocessingInfo::before );
    mainDetector.GetMain()->addToAttachedPreprocessingInfo(preprocessingInfo);
    
    
#endif
    
 }


class NodesToInstrumentVisitor: public ROSE_VisitorPatternDefaultBase {
private:
    vector< pair<SgBasicBlock*, uint64_t > >  basicBlockToInstrument;
    vector< pair<SgLabelStatement*, uint64_t > >  labelStatementToInstrument;
    vector< pair<SgExpression*, uint64_t > >  expressionsToInstrument;
    
    boost::unordered_map<SgNode, TraceId> nodeToIdMap;
    string fileUnderCompilation;
    string fileIdUnderCompilation;
    uint32_t counter;
    string roseDBFile;
   
    
    
public:
    NodesToInstrumentVisitor(SgNode * fileRoot){
    }
    virtual ~NodesToInstrumentVisitor() {
        basicBlockToInstrument.clear();
        labelStatementToInstrument.clear();
        expressionsToInstrument.clear();
    }
    
    void Normalize(const NodeToIdMapper & nodeToIdMapper) {

        // Now transform
        
        for (vector< pair<SgBasicBlock*, uint64_t > >::iterator it = basicBlockToInstrument.begin(); it != basicBlockToInstrument.end(); it++) {
            ((*it).first)->prepend_statement(GetInstumentationStatement((*it).first, ((*it).second)));
            cout<<"\n Instrumented :" << ((*it).first)->class_name();
        }
        for (vector< pair<SgLabelStatement*, uint64_t > >::iterator it = labelStatementToInstrument.begin(); it != labelStatementToInstrument.end(); it++) {
            insertStatement ((*it).first, GetInstumentationStatement((*it).first, (*it).second), /*insertBefore=*/false, /*bool autoMovePreprocessingInfo=*/true);
            cout<<"\n Instrumented :" << ((*it).first)->class_name();
        }
        for (vector< pair<SgExpression*, uint64_t > >::iterator it = expressionsToInstrument.begin(); it != expressionsToInstrument.end(); it++) {
            insertBeforeUsingCommaOp (GetInstumentationExpression((*it).first, (*it).second), (*it).first);
            cout<<"\n Instrumented :" << ((*it).first)->class_name();
        }
    }
    
protected:
    
    uint64_t GetUniqueTraceIdForNode(SgNode * node){
        ROSE_ASSERT(node);
        static boost::unordered_map<std::string, uint32_t> fileToIdMap;

#if 0
        // if the file info has NULL_FILE_ID, then go up the parent till we get a valid file id.
        
        SgNode * tmpNode = node;
        int fileId = tmpNode->get_file_info()->get_physical_file_id();
        
        
        while(tmpNode && (fileId ==  Sg_File_Info::COPY_FILE_ID ||
                          fileId ==  Sg_File_Info::NULL_FILE_ID ||
              fileId ==  Sg_File_Info::TRANSFORMATION_FILE_ID ||
              fileId ==  Sg_File_Info::COMPILER_GENERATED_FILE_ID ||
              fileId ==  Sg_File_Info::COMPILER_GENERATED_MARKED_FOR_OUTPUT_FILE_ID ||
              fileId ==  Sg_File_Info::BAD_FILE_ID )){
            tmpNode = tmpNode->get_parent ();
            if(!tmpNode) {
                cout<<"\n Failed to find file for an SgNode:"<<node->class_name();
                exit(-1);
            }
            fileId = tmpNode->get_file_info()->get_physical_file_id();
        }
        string physicalFile = tmpNode->get_file_info()->get_physical_filename();
        
        uint32_t line = tmpNode->get_file_info()->get_line();
        uint32_t col = tmpNode->get_file_info()->get_col();
#else
        string physicalFile = node->get_file_info()->get_physical_filename();
        uint32_t line = node->get_file_info()->get_line();
        uint32_t col = node->get_file_info()->get_col();
        
#endif
        
        // line number should fit in 20 bits
        ROSE_ASSERT(line < (1<<20));
        // column number should fit in 12 bits
        ROSE_ASSERT(col < (1<<12));
        
        // check if we have the file in local database.
        
        if (fileToIdMap.find(physicalFile) == fileToIdMap.end()){
            // Get it from the project-wide repository
            string databaseFile = TransformationSupport::getProject(node)->get_projectSpecificDatabaseFile ();
            ROSE_ASSERT(databaseFile != "");
            fileToIdMap[physicalFile] = ROSE_GetProjectWideUniqueIdForPhysicalFile(databaseFile, physicalFile);
        }
        
        
        std::cout<<"\n File is = " << fileToIdMap[physicalFile];
        std::cout<<"\n line is = " << line;
        std::cout<<"\n col is = " << col;

        uint64_t uniqueId = (((uint64_t)fileToIdMap[physicalFile]) << 32) | (line << 12) | (col);
        std::cout<<"\n Id is = " << uniqueId;
        
        return uniqueId;
        
    }
    
    

    
    virtual void visit(SgNode * node) {
    }
    
    
    
    void AddExpressionToInstrumentationList(SgExpression * node){
        // if we can't find a valid source line due to compiler generated casting. Let us fetch the line from the real place i.e. argument of the cast
        if( ValidateAccurateFileLineAndColumn(node) == false){
            
            // Skip instrumenting template instantiations
            if(IsTemplateInstantiation(node)){
                return;
            }
            
            node = GetNearestNodeWithSourceInfoAfterStrippingImplicitCasts(node);
        }
        expressionsToInstrument.push_back(make_pair<SgExpression*, uint64_t>(node, GetUniqueTraceIdForNode(node)));
    }

    virtual void visit(SgBasicBlock * node){
        
        if (ValidateAccurateFileLineAndColumn(node)){
            basicBlockToInstrument.push_back(make_pair<SgBasicBlock*, uint64_t>(node, GetUniqueTraceIdForNode(node)));
        } else {
            
            // Skip instrumenting template instantiations
            if(IsTemplateInstantiation(node)){
                return;
            }

            
            if(SgStatement * stmt = GetNearestStatementWithSourceInfo(node)){
                basicBlockToInstrument.push_back(make_pair<SgBasicBlock*, uint64_t>(node, GetUniqueTraceIdForNode(stmt)));
            } else {
                cout<<"\n Skipping over node: " <<node->class_name();
            }
            
        }
    }
    virtual void visit(SgLabelStatement * node){

        ROSE_ASSERT(ValidateAccurateFileLineAndColumn(node));
        labelStatementToInstrument.push_back(make_pair<SgLabelStatement*, uint64_t>(node, GetUniqueTraceIdForNode(node)));
    }
    virtual void visit(SgConditionalExp * node){
        
        AddExpressionToInstrumentationList(node->get_true_exp());
        AddExpressionToInstrumentationList(node->get_false_exp());
    }

    virtual void visit( SgExprStatement * node){
        
        // Any Expression statement that is a body needs to be instrumented.
        
        if(SageInterface::isBodyStatement (node)) {
            AddExpressionToInstrumentationList(node->get_expression ());
        }
    }
    
    virtual void visit(SgAndOp * node){
        AddExpressionToInstrumentationList(node->get_lhs_operand());
        AddExpressionToInstrumentationList(node->get_rhs_operand());
    }
    virtual void visit(SgOrOp * node){
        AddExpressionToInstrumentationList(node->get_lhs_operand());
        AddExpressionToInstrumentationList(node->get_rhs_operand());
    }
};



class RoseTraceInstrumentor: public AstSimpleProcessing {
private:
    NodesToInstrumentVisitor * nodesToInstrumentVisitor;
public:
    RoseTraceInstrumentor(SgNode * fileRoot){
        nodesToInstrumentVisitor = new NodesToInstrumentVisitor(fileRoot);
    }
    ~RoseTraceInstrumentor(){
        delete nodesToInstrumentVisitor;
    }
    
    /*
     void Collect(SgNode * node) {
     traverse(node, preorder);
     }
     void CollectWithinFile(SgNode * node) {
     traverseWithinFile(node, preorder);
     }
     void CollectInputFiles(SgProject * project) {
     traverse(project, preorder);
     }
     */
    
    void Normalize(SgNode * node, const NodeToIdMapper & nodeToIdMapper) {
        // NodesToInstrumentVisitor * visitor = dynamic_cast<NodesToInstrumentVisitor*>(nodesToInstrumentVisitor);
        traverse(node, preorder);
        ROSE_ASSERT(nodesToInstrumentVisitor);
        nodesToInstrumentVisitor->Normalize(nodeToIdMapper);
    }
    void NormalizeWithinFile(SgNode * node, const NodeToIdMapper & nodeToIdMapper) {
        //NodesToInstrumentVisitor * visitor = dynamic_cast<NodesToInstrumentVisitor*>(nodesToInstrumentVisitor);
        traverseWithinFile(node, preorder);
        ROSE_ASSERT(nodesToInstrumentVisitor);
        nodesToInstrumentVisitor->Normalize(nodeToIdMapper);
    }
    void NormalizeInputFiles(SgProject * project, const NodeToIdMapper & nodeToIdMapper) {
        //NodesToInstrumentVisitor * visitor = dynamic_cast<NodesToInstrumentVisitor*>(nodesToInstrumentVisitor);
        traverse(project, preorder);
        ROSE_ASSERT(nodesToInstrumentVisitor);
        nodesToInstrumentVisitor->Normalize(nodeToIdMapper);
    }
    
    virtual ~RoseTraceInstrumentor();
protected:
    void visit(SgNode * node) {
        node->accept(*nodesToInstrumentVisitor);
    }
    
};



//SageInterface::makeSingleStatementBodyToBlock

int main( int argc, char * argv[] ) {
    // Generate the ROSE AST.
    SgProject* project = frontend(argc,argv);
    
    // AST consistency tests (optional for users, but this enforces more of our tests)
    AstTests::runAllTests(project);
    for(int i = 0 ; i < project->numberOfFiles(); i++) {
        SgFile & file = project->get_file(i);
        
       /*
        NodeToIdMapper nodeToIdMapper(file.get_physical_filename(), project->get_projectSpecificDatabaseFile ());
        nodeToIdMapper.Traverse(&file, preorder);
        */
        
        RoseTraceInstrumentor roseTraceInstrumentor(&file);
        roseTraceInstrumentor.Normalize(&file, nodeToIdMapper);

        //roseTraceInstrumentor.Collect(&file);
        

        // Traverse tree and normalize  Statement nodes that need to become Blocks
        //SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
        //singleStatementToBlockNormalizer.Normalize(&file);
        
        //roseTraceInstrumentor.Normalize(&file, nodeToIdMapper);
        
        BuildInstrumentationFunctionDefinitionIfMainIsDetected(argc,argv, &file);
        
    }
    
    generateDOT ( *project );

    const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
    generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
    

    
    // regenerate the source code and call the vendor
    // compiler, only backend error code is reported.
    return backend(project);
}
