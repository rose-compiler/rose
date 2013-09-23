#include "rose.h"
#include "intervals.h"
#include "SingleStatementToBlockNormalization.h"
#include <iostream>
#include <sstream>
#include <functional>
#include <numeric>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost::algorithm;
using namespace SageBuilder;
using namespace SageInterface;




/***  The .gcov files contain the ‘:’ separated fields along with program source code. The format is:
 
 execution_count:line_number:source line text
 Additional block information may succeed each line, when requested by command line option. The execution_count is ‘-’ for lines containing no code.
 Unexecuted lines are marked ‘#####’ or ‘====’, depending on whether they are reachable by non-exceptional paths or only exceptional paths such as C++ exception handlers, respectively.
 **/

void GetExecutionInfoForFile(const string & fileName, vector< pair<string,string> > & executionInfo){
    string gcovCmd = "gcov " + fileName;
    int retVal = system(gcovCmd.c_str());
    
    if (retVal) {
        // failed in gcov. Move on assuming all lines got executed
        cerr<<endl<<"System cmd failed for :"<< gcovCmd;
        return;
    }
    
    string gcovOutputFile = fileName + ".gcov";
    ifstream infile(gcovOutputFile.c_str());
    if(infile.is_open()){
        for( std::string line; getline( infile, line ); ) {
            typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
            boost::char_separator<char> sep(":");
            tokenizer tokens(line, sep);
            tokenizer::iterator tokIter = tokens.begin();
            string one(*tokIter);
            string two(*(++tokIter));
            trim(one);
            trim(two);
            executionInfo.push_back(pair<string,string>(one, two));
        }
        infile.close();
    } else {
        cerr<<endl<<"Failed to open "<<gcovOutputFile;
    }
}

intervals unexecutedInterval, unExecutableInterval;

map<int, bool> executedMap;
void ClearIntervals(){
    unexecutedInterval.clear();
    unExecutableInterval.clear();
    executedMap.clear();
}

// Walks over the vector of gcov info and adds them to either unexecuted/unexecutable/executed buckets
void BuildIntervalTreeForFile(const string & fileName, const vector< pair<string,string> > & executionInfo){
    static string unexecuted = "#####";
    static string unexecutable = "-";
    for( vector< pair<string,string> >::const_iterator it = executionInfo.begin(); it != executionInfo.end(); it++){
        if ( (*it).first == unexecuted) {
            int start;
            istringstream ( (*it).second) >> start;
            unexecutedInterval.insert((void*)start, (void*)(start +1));
#ifdef VERBOSE
            cout<<endl<<"Adding unexecuted interval:"<<(*it).first<<":"<<(*it).second;
#endif
        } else if ( (*it).first == unexecutable) {
            int start;
            istringstream ( (*it).second) >> start;
            unExecutableInterval.insert((void*)start, (void*)(start +1));
#ifdef VERBOSE
            cout<<endl<<"Adding unexecutable interval:"<<(*it).first<<":"<<(*it).second;
#endif
        } else {
            // add to executed hash table
            executedMap[atoi(((*it).second).c_str())] = 1;
#ifdef VERBOSE
            cout<<endl<<"Adding executed interval:"<<(*it).first<<":"<<(*it).second;
#endif
        }
        
    }
}

// Given a SgNode, check if it is inside a function or if it is outside (i.e. Global)
bool IsInGlobalScope(SgNode * node){
    if (!node)
        return true;
    
    if(isSgFunctionDefinition(node))
        return false;
    
    return IsInGlobalScope(node->get_parent());
}

// Given a SgNode, check if it is part of a declaration statement.

bool IsInDeclarationStmt(SgNode * node){
    if (!node)
        return false;
    
    if(isSgDeclarationStatement(node))
        return true;
    
    if(isSgFunctionDefinition(node))
        return false;
    
    return IsInDeclarationStmt(node->get_parent());
}

#if 0

bool IsExecuted(SgNode * node){
    Sg_File_Info * startInfo = node->get_startOfConstruct();
    Sg_File_Info * endInfo = node->get_endOfConstruct();
    if(startInfo && endInfo) {
        int startLine = startInfo->get_line();
        int endline = endInfo->get_line();
        // if the entire range is in the interval then we can drop it
        pair<void *const, void *> * startNode = unexecutedInterval.contains((void*)startLine);
        pair<void *const, void *> * endNode = unexecutedInterval.contains((void*)endline);
        if ((startNode)  && (endNode)  && (endNode == startNode)) {
            return false;
        }
        
        startNode = unExecutableInterval.contains((void*)startLine);
        endNode = unExecutableInterval.contains((void*)endline);
        if ((startNode)  && (endNode)  && (endNode == startNode)) {
            return false;
        }
        
    }
    return true;
}

#else

// Checks if a given SgNode is was executed.
// If atlease of the lines in the range of lines spanned by this SgNode was executed then we consider this SgNode to be executed.
bool IsExecuted(SgNode * node){
    Sg_File_Info * startInfo = node->get_startOfConstruct();
    Sg_File_Info * endInfo = node->get_endOfConstruct();
    if(startInfo && endInfo) {
        int startLine = startInfo->get_line();
        int endline = endInfo->get_line();
        // if at least one line is referenced then we can't delete it
        for(int i = startLine; i <= endline ; i++) {
            if (executedMap.find(i) != executedMap.end())
                return true;
        }
    }
    return false;
}


#endif

// Checks if a given SgNode is unexecutable.
// If the range of lines spanned by this SgNode are all in unExecutableInterval tree then it is unexecutable.
bool IsUnExecutableNode(SgNode * node){
    Sg_File_Info * startInfo = node->get_startOfConstruct();
    Sg_File_Info * endInfo = node->get_endOfConstruct();
    if(startInfo && endInfo) {
        int startLine = startInfo->get_line();
        int endline = endInfo->get_line();
        pair<void *const, void *> * startNode = unExecutableInterval.contains((void*)startLine);
        pair<void *const, void *> * endNode = unExecutableInterval.contains((void*)endline);
        if ((startNode)  && (endNode)  && (endNode == startNode)) {
            return true;
        }
    }
    return false;
}


// IsExecutable is a routine to eliminate nodes marked as compiler generated.
bool IsExecutable(SgNode * node){
    Sg_File_Info * startInfo = node->get_startOfConstruct();
    Sg_File_Info * endInfo = node->get_endOfConstruct();
    if(startInfo && endInfo) {
        // Nodes  with hasPositionInSource ()== 0 and isCompilerGenerated () == 1 are also considered as not executed
        if (startInfo->hasPositionInSource() == false &&
            startInfo->isCompilerGenerated() == true &&
            endInfo->hasPositionInSource() == false &&
            endInfo->isCompilerGenerated() == true)
            return false;
    }
    return true;
}


/* For now, we consider an SgNode undeletable if it has one of the following attributes:
 isSgStatement
 IsInDeclarationStmt
 isSgFunctionDefinition
 Function body {}
 */

bool IsDeletableNode(SgNode * node){
    SgStatement * stmt = isSgStatement(node);
    // Statement node
    if (!stmt) {
#ifdef VERBOSE
        cout<<endl<<"--- !! NOT SgStatement ----";
#endif
        return false;
    }
    
    // Not a declaration node
    if(IsInDeclarationStmt(stmt)){
#ifdef VERBOSE
        cout<<endl<<"--- !! IsInDeclarationStmt ----";
#endif
        return false;
    }
    
    
    // Not a definition node
    if(isSgFunctionDefinition(stmt)){
#ifdef VERBOSE
        cout<<endl<<"--- !! isSgFunctionDefinition ----";
#endif
        return false;
    }
    
    // Function body {}
    if(stmt->get_parent() && isSgFunctionDefinition(stmt->get_parent())){
#ifdef VERBOSE
        cout<<endl<<"--- !! Function Body {} ----";
#endif
        return false;
    }
    
    
    // body statements
    if(isBodyStatement(stmt)) {
#ifdef VERBOSE
        cout<<endl<<"--- !! isBodyStatement ----";
#endif
        return false;
    }
    
    
    // Not too confident here
#if 1
    // Unexecutable statements (case label)
    if(IsUnExecutableNode(node)) {
        // GCOV fakes unexecuted IF/While/For as nor executavle i.e.g if(0) ... Let us ignore them
        
        switch(node->variantT()){
                // executable statements, control flow
            case V_SgIfStmt:
                return true;
                
            default:
#ifdef VERBOSE
                cout<<endl<<"--- !! IsUnExecutableNode ----";
#endif
                return false;
        }
        
    }
#endif
    
    return true;
}

// Mark the given node to be not unparsed by the backend, effectively deleting the unexecuted code.

void MarkNotToUnparse(SgNode *node){
    cout<<endl<<"Deleting:" << node->get_startOfConstruct()->get_line() << "-" << node->get_endOfConstruct()->get_line();
    node->get_file_info()->setCompilerGenerated();
    node->get_startOfConstruct()->setCompilerGenerated();
    node->get_endOfConstruct()->setCompilerGenerated();
}

// Check if the given node is marked as not to be unparsed.
bool IsMarkedNotToUnparse(SgNode *node){
    return node->get_file_info()->isCompilerGenerated() && node->get_startOfConstruct()->isCompilerGenerated() && node->get_endOfConstruct()->isCompilerGenerated();
}


#if 0

class LineRangeDeleterTraversal:public AstTopDownProcessing<bool> {
public:
    LineRangeDeleterTraversal(){}
protected:
    bool evaluateInheritedAttribute(SgNode * node, bool parentValue);
};

bool LineRangeDeleterTraversal::evaluateInheritedAttribute(SgNode * node, bool parentExecuted) {
    
    // if my parent was not executed, I wouldn't either
    if (!parentExecuted) {
        MarkNotToUnparse(node);
        return false;
    }
    
    // if all lines in my range were not executed, then I will not execute
    bool executed = IsExecuted(node);
    if (!executed) {
        MarkNotToUnparse(node);
    }
    
    return executed;
}
#endif

// Bottom up traversal, synthesize nodes that are not be executed.
// If all my children in the AST are unexecuted and I am either unexecutaed or unexecutable then I am also unexecuted.

class LineDeleterTraversal:public AstBottomUpProcessing <bool> {
public:
    LineDeleterTraversal(){}
protected:
    bool evaluateSynthesizedAttribute(SgNode * node, SynthesizedAttributesList atts);
    bool defaultSynthesizedAttribute(bool waste);
    
    
    void PrintAll(SynthesizedAttributesList atts){
        cout<<endl;
        for(SynthesizedAttributesList::iterator it = atts.begin(); it != atts.end(); it++)
            cout<<" "<<*it;
        cout<<endl;
    }
    
};

bool LineDeleterTraversal::defaultSynthesizedAttribute(bool waste) {
    // if the node does not exist then we did not execute it.
    return false;
}

void PrintNodeInfo(SgNode * node){
    Sg_File_Info * startInfo = node->get_startOfConstruct();
    Sg_File_Info * endInfo = node->get_endOfConstruct();
    cout<<node->class_name()<<" Lines: ";
    if(startInfo && endInfo) {
        int startLine = startInfo->get_line();
        int endline = endInfo->get_line();
        cout<<startLine <<":" << endline;
    }
    
    cout<<":hasPositionInSource ()="<<startInfo->hasPositionInSource ()<<":isCompilerGenerated () ="<<startInfo->isCompilerGenerated () <<":isSourcePositionUnavailableInFrontend ()="<<startInfo->isSourcePositionUnavailableInFrontend ();
}

bool LineDeleterTraversal::evaluateSynthesizedAttribute(SgNode * node, SynthesizedAttributesList atts) {
    
#ifdef VERBOSE
    cout<<endl<<"evaluateSynthesizedAttribute:"; PrintNodeInfo(node);
#endif
    
    // don't delete anything in the global scope
    if (IsInGlobalScope(node)){
#ifdef VERBOSE
        cout<<endl<<"   GBLNode:"<<node->class_name();
#endif
        return true;
    }
    
    
    //bool executed = (!node->get_file_info()->isCompilerGenerated()) || (node->get_file_info()->isCompilerGeneratedNodeToBeUnparsed());
    //if I am a leaf node then rely fully on the execution data
    if(atts.size() == 0) {
#ifdef VERBOSE
        cout<<endl<<"   LeafNode:"<<node->class_name();
#endif
        bool executed = IsExecuted(node);
        if (!executed)  {
            // if it is a SgBasicBlock, then we will not delete it, instead the parent will decide what to do.
            if (IsDeletableNode(node)){
                MarkNotToUnparse(node);
            } else {
#ifdef VERBOSE
                cout<<endl<<"   InteriorNode:"<<node->class_name()<<" Is NOT IsDeletableNode";
#endif
            }
            return false;
        }
        
        // Nodes  with hasPositionInSource ()== 0 and isCompilerGenerated () == 1 are also considered as not executed
        //if (!IsExecutable(node))
        //   return false;
        
        return true;
    }
    
    
    // if I am an interior node and none of my children executed, then return false
    bool someChildExecuted  = std::accumulate(atts.begin(), atts.end(), false, std::logical_or<bool>());
    bool executed = IsExecuted(node);
#ifdef VERBOSE
    cout<<endl<<"   InteriorNode:"<<node->class_name()<<":kids="<<atts.size()<<":Synth value:"<<someChildExecuted<<":self exec:"<<executed;
#endif
    executed = executed || someChildExecuted;
    
    // statements like "return;" get NOT executed as the default value from synthesized attribute because
    // there is "return 0;" form as well. This is a trouble. Hence I need to make sure that Nothing in
    // this constructs range was ever executed before declaring it to be deleted.
    
    
    if (!executed) {
        // if it is a SgBasicBlock, then we will not delete it, instead the parent will decide what to do.
        if (IsDeletableNode(node)){
            MarkNotToUnparse(node);
        } else {
#ifdef VERBOSE
            cout<<endl<<"   InteriorNode:"<<node->class_name()<<" Is NOT IsDeletableNode";
#endif
        }
    }
    return executed;
}


// Traverse AST, deleting label statements whose successor statements are marked as deleted.
// This is needed otherwise we will have a dangling label that backend compilers don't like.
class LabelDeleter : public AstSimpleProcessing {
public:
    LabelDeleter(){
    }
    
protected:
    void visit(SgNode * node) {
        if (IsMarkedNotToUnparse(node))
            return;
        
        if(SgLabelStatement * labelStmt = isSgLabelStatement(node)){
            if(SgStatement * stmt = labelStmt->get_statement()){
                if (IsMarkedNotToUnparse(stmt)) {
                    MarkNotToUnparse(node);
                }
            }
        }
    }
};


// Traverse AST, deleting goto statements whose targets are marked as deleted.
// This is needed otherwise we will habe a dangling goto w/o a target label.
class GotoDeleter : public AstSimpleProcessing {
public:
    GotoDeleter(){
    }
    
protected:
    void visit(SgNode * node) {
        if (IsMarkedNotToUnparse(node))
            return;
        
        if(SgGotoStatement * gotoStmt = isSgGotoStatement(node)){
            if(SgStatement * labelStmt = gotoStmt->get_label()){
                if (IsMarkedNotToUnparse(labelStmt)) {
                    MarkNotToUnparse(node);
                }
            }
        }
    }
    
};


int main( int argc, char * argv[] ) {
    // Generate the ROSE AST.
    SgProject* project = frontend(argc,argv);
    
    AstTests::runAllTests(project);
    // Get line deletion information for each specified input file
    for(int i = 0 ; i < project->numberOfFiles(); i++) {
        SgFile & file = project->get_file(i);
        const string & inputFileName =  file.getFileName();
#ifdef VERBOSE
        cout<<endl<<"Now processing:"<<inputFileName;
#endif
        vector<pair<string,string> > executionInfo;
        GetExecutionInfoForFile(inputFileName, executionInfo);
        BuildIntervalTreeForFile(inputFileName, executionInfo);
        
        // Traverse tree and normalize  Statement nodes that need to become Blocks
        SingleStatementToBlockNormalizer singleStatementToBlockNormalizer;
        singleStatementToBlockNormalizer.Normalize(&file);
        
        // delete nodes
        LineDeleterTraversal lineDeleterTraversal;
        lineDeleterTraversal.traverse(&file);
        
        // Delete dangling labels
        LabelDeleter labelDeleter;
        labelDeleter.traverse(&file, preorder);
        
        // delete dead gotos
        GotoDeleter gotoDeleter;
        gotoDeleter.traverse(&file, preorder);
        
        ClearIntervals();
    }
    
    // regenerate the source code and call the vendor
    // compiler, only backend error code is reported.
    return backend(project);
}
