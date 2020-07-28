#include "AnalysisDebuggingUtils.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>

#include "genericDataflowCommon.h"
#include "VirtualCFGIterator.h"
#include "cfgUtils.h"
#include "CallGraphTraverse.h"
#include "analysisCommon.h"
#include "dataflow.h"
#include "latticeFull.h"
#include "printAnalysisStates.h"
#include "liveDeadVarAnalysis.h"


using namespace std;
using namespace Rose;

  //------------ function level dot graph output ----------------
  // this is purposely put outside of the namespace scope for simplicity 
  class analysisStatesToDOT : virtual public UnstructuredPassIntraAnalysis
  {
    private:
      //LiveDeadVarsAnalysis* lda; // reference to the source analysis
      Analysis* lda; // reference to the source analysis
      void printEdge(const DataflowEdge& e); // print data flow edge
      void printNode(const DataflowNode& n, std::string state_string); // print date flow node
      void visit(const Function& func, const DataflowNode& n, NodeState& state); // visitor function
    public:
      std::ostream* ostr; 
      // must transfer the custom filter from l, or the default filter will kick in!
      analysisStatesToDOT (Analysis* l): Analysis(l->filter), lda(l){ }; 
  };
  
  void analysisStatesToDOT::printEdge (const DataflowEdge& e)
  {
    (*ostr) << e.source().id() << " -> " << e.target().id() << " [label=\"" << escapeString(e.toString ()) <<
      "\", style=\"" << "solid" << "\"];\n";
  }
  
  void analysisStatesToDOT::printNode(const DataflowNode& n, std::string state_string)
  {
    std::string id = n.id();  // node id
    std::string nodeColor = "black";
  
    if (isSgStatement(n.getNode()))
      nodeColor = "blue";
    else if (isSgExpression(n.getNode()))
      nodeColor = "green";
    else if (isSgInitializedName(n.getNode()))
      nodeColor = "red";
  
    // node_id [label="", color="", style=""]
    (*ostr) << id << " [label=\""  << escapeString(n.toString()) <<"\\n" << escapeString (state_string) << "\", color=\"" << nodeColor <<
      "\", style=\"" << (n.isInteresting()? "solid" : "dotted") << "\"];\n";
  }
  
  // This will be visited only once? Not sure, better check
    void
  analysisStatesToDOT::visit(const Function& func, const DataflowNode& n, NodeState& state)
  { 
    std::string state_str = state.str( lda, " "); // convert lattice into a string
    printNode(n, state_str);
    std::vector < DataflowEdge> outEdges = n.outEdges();
    for (unsigned int i = 0; i < outEdges.size(); ++i)
    {
      printEdge(outEdges[i]);
    }
  }
  //------------ call graph level  driver ----------------
  class IntraAnalysisResultsToDotFiles: public UnstructuredPassInterAnalysis
  {
    public:
      // internal function level dot generation, a name for the output file
      IntraAnalysisResultsToDotFiles(IntraProceduralAnalysis & funcToDot)
        : InterProceduralAnalysis(&funcToDot), UnstructuredPassInterAnalysis(funcToDot){ }
      // For each function, call function level Dot file generator to generate separate dot file
      void runAnalysis() ;
  };
  
  void IntraAnalysisResultsToDotFiles::runAnalysis()
  {
    set<FunctionState*> allFuncs = FunctionState::getAllDefinedFuncs();
    // Go through functions one by one, call an intra-procedural analysis on each of them
    // iterate over all functions with bodies
    for(set<FunctionState*>::iterator it=allFuncs.begin(); it!=allFuncs.end(); it++)
    {
  
      FunctionState* fState = *it;
      // compose the output file name as filename_mangled_function_name.dot
      Function *func = & (fState->getFunc());
      assert(func != NULL);
      SgFunctionDefinition* proc = func->get_definition();
      assert(proc != NULL);
      string file_name = StringUtility::stripPathFromFileName(proc->get_file_info()->get_filename());
      string file_func_name= file_name+ "_"+proc->get_mangled_name().getString();
  
      string full_output = file_func_name +"_cfg.dot";
      std::ofstream ostr(full_output.c_str());
  
      ostr << "digraph " << "mygraph" << " {\n";
      analysisStatesToDOT* dot_analysis = dynamic_cast <analysisStatesToDOT*> ( intraAnalysis);
      assert (dot_analysis != NULL);
      dot_analysis->ostr = &ostr;
      dot_analysis->runAnalysis(fState->func, &(fState->state));
      ostr << "}\n";
    }
  
  }
  

/******************
 ***** dbgBuf *****
 ******************/

namespace Dbg {

dbgBuf::dbgBuf()
{
        init(NULL);
}

dbgBuf::dbgBuf(std::streambuf* baseBuf)
{
        init(baseBuf);
}

void dbgBuf::init(std::streambuf* baseBuf)
{
        this->baseBuf = baseBuf;
        synched = true;
        ownerAccess = true;
        numOpenAngles = 0;
        //numDivs = 0;
        parentDivs.empty();
        parentDivs.push_back(0);
        //cout << "Initially parentDivs (len="<<parentDivs.size()<<")\n";
}

// This dbgBuf has no buffer. So every character "overflows"
// and can be put directly into the teed buffers.
int dbgBuf::overflow(int c)
{
        //cout << "overflow\n";
        if (c == EOF)
        {
                return !EOF;
        }
        else
        {
                int const r1 = baseBuf->sputc(c);
                return r1 == EOF ? EOF : c;
        }
}

// Prints the indent to the stream buffer, returns 0 on success non-0 on failure
/*int dbgBuf::printIndent()
{
        string indent="";
        for(list<string>::iterator i=indents.begin(); i!=indents.end(); i++) indent+=*i;
        int r = baseBuf->sputn(indent.c_str(), indent.length());
        if(r!=indent.length()) return -1;
        return 0;
}*/

// Prints the given string to the stream buffer
int dbgBuf::printString(string s)
{
        int r = baseBuf->sputn(s.c_str(), s.length());
        if(((size_t)r)!=s.length()) return -1;
        return 0;
}

streamsize dbgBuf::xsputn(const char * s, streamsize n)
{
//      cout << "xputn() ownerAccess="<<ownerAccess<<" n="<<n<<" s=\""<<string(s)<<"\"\n";
        /*if(synched && !ownerAccess) {
                int ret2 = printIndent();
                if(ret2 != 0) return 0;
        }*/
        // If the owner is printing, output their text exactly
        if(ownerAccess) {
                return baseBuf->sputn(s, n);
        // Otherwise, replace all line-breaks with <BR>'s
        } else {
                int ret;
                int i=0;
                char br[]="<BR>\n";
                char space[]=" ";
                char spaceHTML[]="&nbsp;";
                char tab[]="\t";
                char tabHTML[]="&#09;";
                //char lt[]="&lt;";
                //char gt[]="&gt;";
                while(i<n) {
                        int j;
                        for(j=i; j<n && s[j]!='\n' && s[j]!=' ' && s[j]!='\t'/* && s[j]!='<' && s[j]!='>'*/; j++) {
                                if(s[j]=='<') numOpenAngles++;
                                else if(s[j]=='>') numOpenAngles--;
                        }
//                      cout << "char=\""<<s[j]<<"\" numOpenAngles="<<numOpenAngles<<"\n";
                        // Send out all the bytes from the start of the string or the 
                        // last line-break until this line-break
                        if(j-i>0) {
                                ret = baseBuf->sputn(&(s[i]), j-i);
                                if(ret != (j-i)) return 0;
                                //cout << "   printing char "<<i<<" - "<<j<<"\n";
                        }
                        
                        if(j<n) {
                                // If we're at at line-break Send out the line-break
                                if(s[j]=='\n') {
                                        ret = baseBuf->sputn(br, sizeof(br)-1);
                                        if(ret != sizeof(br)-1) return 0;
                                } else if(s[j]==' ') {
                                        // If we're at a space and not inside an HTML tag, replace it with an HTML space escape code
                                        if(numOpenAngles==0) {
                                                ret = baseBuf->sputn(spaceHTML, sizeof(spaceHTML)-1);
                                                if(ret != sizeof(spaceHTML)-1) return 0;
                                        // If we're inside an HTML tag, emit a regular space character
                                        } else {
                                                ret = baseBuf->sputn(space, sizeof(space)-1);
                                                if(ret != sizeof(space)-1) return 0;
                                        }
                                } else if(s[j]=='\t') {
                                        // If we're at a tab and not inside an HTML tag, replace it with an HTML tab escape code
                                        if(numOpenAngles==0) {
                                                ret = baseBuf->sputn(tabHTML, sizeof(tabHTML)-1);
                                                if(ret != sizeof(tabHTML)-1) return 0;
                                        // If we're inside an HTML tag, emit a regular tab character
                                        } else {
                                                ret = baseBuf->sputn(tab, sizeof(tab)-1);
                                                if(ret != sizeof(tab)-1) return 0;
                                        }
                                }/* else if(s[j]=='<') {
                                        ret = baseBuf->sputn(lt, sizeof(lt)-1);
                                        if(ret != sizeof(lt)-1) return 0;
                                } else if(s[j]=='>') {
                                        ret = baseBuf->sputn(gt, sizeof(gt)-1);
                                        if(ret != sizeof(gt)-1) return 0;
                                }*/
                                //cout << "   printing <BR>\n";
                        }
                                
                        // Point i to immediately after the line-break
                        i = j+1;
                }
                
                return n;
        }
}


// Sync buffer.
int dbgBuf::sync()
{
        int r = baseBuf->pubsync();
        if(r!=0) return -1;

        //cout << "Synch synched="<<synched<<" ownerAccess="<<ownerAccess<<"\n";
        if(synched && !ownerAccess) {
                int ret2 = printString("<br>\n");
                if(ret2 != 0) return 0;
                synched = false;
        }
        synched = true;
        
        return 0;
}

// Switch between the owner class and user code writing text
void dbgBuf::userAccessing() { ownerAccess = false; synched = true; }
void dbgBuf::ownerAccessing() { ownerAccess = true; synched = true; }

void dbgBuf::enterFunc(string funcName/*, string indent*/)
{
        funcs.push_back(funcName);
        //indents.push_back(indent);
        // Increment the index of this function within its parent
        //cout << "Incrementing parentDivs (len="<<parentDivs.size()<<") from "<<*(parentDivs.rbegin())<<" to ";
        (*(parentDivs.rbegin()))++;
        /*cout << *(parentDivs.rbegin())<<": div=";
        ostringstream divName;
        for(list<int>::iterator d=parentDivs.begin(); d!=parentDivs.end(); d++) {
                if(d!=parentDivs.end())
                        divName << *d << "_";
        }
        cout << divName.str()<<"\n";*/
        // Add a new level to the parentDivs list, starting the index at 0
        parentDivs.push_back(0);
        //numDivs++;
}

void dbgBuf::exitFunc(string funcName)
{
        if(funcName != funcs.back()) { 
                cout << "dbgStream::exitFunc() ERROR: exiting from function "<<funcName<<" which is not the most recent function entered!\n";
                cout << "funcs=\n";
                for(list<string>::iterator f=funcs.begin(); f!=funcs.end(); f++)
                        cout << "    "<<*f<<"\n";
                cout.flush();
                baseBuf->pubsync();
                exit(-1);
        }
        funcs.pop_back();
        //indents.pop_back();
        parentDivs.pop_back();
}

/*********************
 ***** dbgStream *****
 *********************/

// Returns a string that contains n tabs
string tabs(int n)
{
        string s;
        for(int i=0; i<n; i++)
                s+="\t";
        return s;
}

dbgStream::dbgStream() : std::ostream(&buf), initialized(false)
{}

dbgStream::dbgStream(string title, string dbgFileName, string workDir, string imgPath)
        : std::ostream(&buf)
{
        init(title, dbgFileName, workDir, imgPath);
}

void dbgStream::init(string title, string dbgFileName, string workDir, string imgPath)
{
        this->workDir = workDir;
        
        ofstream indexFile;
        try {                                                                                           
        indexFile.open(dbgFileName.c_str());
        }                                                                                               
        catch (ofstream::failure e)                                                                     
        { cout << "dbgStream::init() ERROR opening file \""<<dbgFileName<<"\" for writing!"; exit(-1); }

        indexFile << "<frameset cols=\"20%,80%\">\n";
        indexFile << "\t<frame src=\"summary.html\" name=\"summary\"/>\n";
        indexFile << "\t<frame src=\"detail.html\" name=\"detail\"/>\n";
        indexFile << "</frameset>\n";
        indexFile.close();
        
        
        try {
                dbgFile.open((workDir+"/detail.html").c_str());
        }
        catch (ofstream::failure e)
        { cout << "dbgStream::init() ERROR opening file \""<<workDir+"/summary.html\" for writing!"; exit(-1); }

        
        //std::ostream::rdbuf(&buf);
        buf.init(dbgFile.rdbuf());
        this->imgPath = imgPath;
        this->dbgFileName = dbgFileName;

        try {
                summaryF.open((workDir+"/summary.html").c_str());
        }
        catch (ofstream::failure e)
        { cout << "dbgStream::init() ERROR opening file \""<<workDir+"/summary.html\" for writing!"; exit(-1); }
        summaryF << "<html>\n";
        summaryF << "\t<head>\n";
        summaryF << "\t<title>"<<title<<"</title>\n";
        summaryF << "\t</head>\n";
        summaryF << "\t<body>\n";
        summaryF << "\t<h1>Summary</h1>\n";
        summaryF << "\t\t<table width=\"100%\">\n";
        summaryF << "\t\t\t<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        
        buf.ownerAccessing();
        printDetailFileHeader(title);
        buf.userAccessing();
        
        // Initialize colors with a list of light pastel colors 
        colors.push_back("FF97E8");
        colors.push_back("75D6FF");
        colors.push_back("72FE95");
        colors.push_back("8C8CFF");
        colors.push_back("57BCD9");
        colors.push_back("99FD77");
        colors.push_back("EDEF85");
        colors.push_back("B4D1B6");
        colors.push_back("FF86FF");
        colors.push_back("4985D6");
        colors.push_back("D0BCFE");
        colors.push_back("FFA8A8");
        colors.push_back("A4F0B7");
        colors.push_back("F9FDFF");
        colors.push_back("FFFFC8");
        colors.push_back("5757FF");
        colors.push_back("6FFF44");
        
        numImages++;
        initialized = true;
}

dbgStream::~dbgStream()
{
       if (!initialized)
         return;

        buf.ownerAccessing();
        printDetailFileTrailer();
        buf.userAccessing();
        
        summaryF << "\t\t\t</td></tr>\n";
        summaryF << "\t\t</table>\n";
        summaryF << "\t</body>\n";
        summaryF << "</html>\n\n";
        summaryF.close();
        
        // Run CPP on the output file to incorporate the summary into the main file
        //{
        //      ostringstream cmd;
        //      cmd << "mv "<<dbgFileName<<" "<<dbgFileName<<".tmp";
        //      system(cmd.str().c_str());
        //}
        //
        //{
        //      ostringstream cmd;
        //      cmd << "cpp -E -x c -P -C "<<dbgFileName<<".tmp > "<<dbgFileName;
        //      //cout << cmd.str()<<"\n";
        //      system(cmd.str().c_str());
        //}
}

void dbgStream::printDetailFileHeader(string title)
{
        *(this) << "<html>\n";
        *(this) << "\t<head>\n";
        *(this) << "\t<title>"<<title<<"</title>\n";
        *(this) << "\t<STYLE TYPE=\"text/css\">\n";
        *(this) << "\tBODY\n";
        *(this) << "\t\t{\n";
        *(this) << "\t\tfont-family:courier;\n";
        *(this) << "\t\t}\n";
        *(this) << "\t.hidden { display: none; }\n";
        *(this) << "\t.unhidden { display: block; }\n";
        *(this) << "\t</style>\n";
        *(this) << "\t<script type=\"text/javascript\">\n";
        *(this) << "\tfunction unhide(divID) {\n";
        *(this) << "\t  var parentDiv = document.getElementById(\"div\"+divID);\n";
        *(this) << "\t\tif (parentDiv) {\n";
        *(this) << "\t\t\t// Hide the parent div\n";
        *(this) << "\t\t\tparentDiv.className=(parentDiv.className=='hidden')?'unhidden':'hidden';\n";
        *(this) << "\t\t\t// Get all the tables\n";
        *(this) << "\t\t\tvar childTbls = document.getElementsByTagName(\"table\");\n";
        *(this) << "\t\t\tcondition = new RegExp(\"table\"+divID+\"*\");\n";
        *(this) << "\t\t\tfor (var i=0; i<childTbls.length; i++){ \n";
        *(this) << "\t\t\t\tvar child = childTbls[i];\n";
        *(this) << "\t\t\t\t// Set the visibility status of each child table to be the same as its parent div\n";
        *(this) << "\t\t\t\tif (\"table\"+divID!=child.id && child.nodeType==1 && child.id!=undefined && child.id.match(condition)) {\n";
        *(this) << "\t\t\t\t    child.className=parentDiv.className;\n";
        *(this) << "\t\t\t\t}\n";
        *(this) << "\t\t\t}\n";
        *(this) << "\t\t}\n";
        *(this) << "\t}\n";
        *(this) << "\tfunction highlightLink(divID, newcolor) {\n";
        *(this) << "\t\tvar sumLink = top.summary.document.getElementById(\"link\"+divID);\n";
        *(this) << "\t\tsumLink.style.backgroundColor= newcolor;\n";
        *(this) << "\t}\n";
        *(this) << "\tfunction focusLink(divID, e) {\n";
        *(this) << "\t\te = e || window.event;\n";
        *(this) << "\t\tif('cancelBubble' in e) {\n";
        *(this) << "\t\t\te.cancelBubble = true;\n";
        *(this) << "\t\t\ttop.summary.location = \"summary.html#anchor\"+divID;\n";
        *(this) << "\t\t}\n";
        *(this) << "\t}\n";
        *(this) << "\tfunction showNodes(imgNum, str) {\n";
        *(this) << "\t\tvar f = document.getElementById('imgFrame_'+imgNum);\n";
        *(this) << "\t\tf.width = \"100%\"\n";
        *(this) << "\t\tf.height = 100;\n";
        *(this) << "\t\tf = (f.contentWindow ? f.contentWindow : (f.contentDocument.document ? f.contentDocument.document : f.contentDocument));\n";
        *(this) << "\t\tf.document.open();\n";
        *(this) << "\t\tf.document.write(\"~/Compilers/focusOnNodes.pl "<<imgPath<<"/image_\"+imgNum+\".dot \"+str+\" > "<<imgPath<<"/image_\"+imgNum+\".focus.dot ; dot -Tsvg -o"<<imgPath<<"/image_\"+imgNum+\".focus.svg "<<imgPath<<"/image_\"+imgNum+\".focus.dot\");\n";
        *(this) << "\t\tf.document.close();\n";
        *(this) << "\t}\n";
        *(this) << "\t</script>\n";
        *(this) << "\t</head>\n";
        *(this) << "\t<body>\n";
        *(this) << "\t<h1>"<<title<<"</h1>\n";
        *(this) << "\t\t<table width=\"100%\">\n";
        *(this) << "\t\t\t<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
}

void dbgStream::printDetailFileTrailer()
{
        *(this) << "\t\t\t</td></tr>\n";
        *(this) << "\t\t</table>\n";
        *(this) << "\t</body>\n";
        *(this) << "</html>\n\n";
}

void dbgStream::enterFunc(string funcName/*, string indent*/)
{
        buf.ownerAccessing();
        
        buf.enterFunc(funcName/*, indent*/);
        
        ostringstream divName;
        for(list<int>::iterator d=buf.parentDivs.begin(); d!=buf.parentDivs.end(); ) {
                divName << *d;
                d++;
                if(d!=buf.parentDivs.end()) {
                        divName << "_";
                }
        }

        *(this) << "\t\t\t"<<tabs(buf.funcs.size())<<"</td></tr>\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<table bgcolor=\"#"<<colors[(buf.funcs.size()-1)%colors.size()]<<"\" width=\"100%\" id=\"table"<<divName.str()<<"\" style=\"border:1px solid white\" onmouseover=\"this.style.border='1px solid black'; highlightLink('"<<divName.str()<<"', '#F4FBAA');\" onmouseout=\"this.style.border='1px solid white'; highlightLink('"<<divName.str()<<"', '#FFFFFF');\" onclick=\"focusLink('"<<divName.str()<<"', event);\">\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\"><h2><a name=\""<<divName.str()<<"\" href=\"javascript:unhide('"<<divName.str()<<"');\">";
        buf.userAccessing();
        *(this) << funcName;
        buf.ownerAccessing();
        *(this) << "</a></h2></td></tr>\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\"><div id=\"div"<<divName.str()<<"\" class=\"unhidden\">\n";
        this->flush();
        
        //summaryF << "<li><a target=\"detail\" href=\"detail.html#"<<divName.str()<<"\">"<<funcName<<"</a><br>\n";
        //summaryF << "<ul>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size())<<"</td></tr>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<table width=\"100%\" style=\"border:0px\">\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<tr width=\"100%\"><td width=50></td><td id=\"link"<<divName.str()<<"\" width=\"100%\"><a name=\"anchor"<<divName.str()<<"\" target=\"detail\" href=\"detail.html#"<<divName.str()<<"\">"<<funcName<<"</a> ("<<divName.str()<<")</td></tr>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        summaryF.flush();
        
        buf.userAccessing();
}

void dbgStream::exitFunc(string funcName)
{
        buf.ownerAccessing();
        buf.exitFunc(funcName);
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"</td></tr>\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"</table>\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size())<<"</td></tr>\n";
        *(this) << "\t\t\t"<<tabs(buf.funcs.size())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        this->flush();
        buf.userAccessing();
        
        //summaryF << "</ul>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"</div></td></tr>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"</table>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size())<<"</td></tr>\n";
        summaryF << "\t\t\t"<<tabs(buf.funcs.size())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
        summaryF.flush();
}

// Adds an image to the output with the given extension and returns the path of this image
// so that the caller can write to it.
string dbgStream::addImage(string ext)
{
        ostringstream imgFName; imgFName << imgPath << "/image_" << numImages << "." << ext;
        buf.ownerAccessing();
        *(this) << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<img src="<<imgFName.str()<<">\n";
        buf.userAccessing();
        return imgFName.str();
}

// Given a reference to an object that can be represented as a dot graph, create an image from it and add it to the output.
// Return the path of the image.
string dbgStream::addDOT(dottable& obj)
{
        ostringstream imgFName; imgFName << "dbg_imgs/image_" << numImages << ".svg";
        ostringstream graphName; graphName << "graph_"<<numImages;
        ostringstream ret;
        addDOT(imgFName.str(), graphName.str(), obj.toDOT(graphName.str()), *this);
        
        return ret.str();
        
        return imgFName.str();
}

// Given a reference to an object that can be represented as a dot graph, create an image of it and return the string
// that must be added to the output to include this image.
std::string dbgStream::addDOTStr(dottable& obj)
{
        ostringstream imgFName; imgFName << "dbg_imgs/image_" << numImages << ".svg";
        ostringstream graphName; graphName << "graph_"<<numImages;
        ostringstream ret;
        addDOT(imgFName.str(), graphName.str(), obj.toDOT(graphName.str()), ret);
        
        return ret.str();
}

// Given a representation of a graph in dot format, create an image from it and add it to the output.
// Return the path of the image.
std::string dbgStream::addDOT(string dot)
{
        ostringstream imgFName; imgFName << "dbg_imgs/image_" << numImages << ".svg";
        ostringstream graphName; graphName << "graph_"<<numImages;

        addDOT(imgFName.str(), graphName.str(), dot, *this);
        
        return imgFName.str();
}

// The common work code for all the addDOT methods
void dbgStream::addDOT(string imgFName, string graphName, string dot, ostream& ret)
{
        ostringstream dotFName; dotFName << imgPath << "/image_" << numImages << ".dot";
        ostringstream mapFName; mapFName << imgPath<<"/image_" << numImages << ".map";

        ofstream dotFile;
        dotFile.open(dotFName.str().c_str());
        dotFile << dot;
        dotFile.close();
        
        // Create the SVG file's picture of the dot file
        /*pid_t child = fork();
        // Child
        if(child==0) 
        {*/
                ostringstream cmd; cmd << "dot -Tsvg -o"<<imgPath<<"/image_" << numImages << ".svg "<<dotFName.str() << "&"; // -Tcmapx -o"<<mapFName.str()<<"
                cout << "Command \""<<cmd.str()<<"\"\n";
                system(cmd.str().c_str());
        /*      exit(0);
        // ERROR
        } else if(child<0) {
                cout << "dbgStream::addDOTStr() ERROR forking!";
                exit(-1);
        }*/
        
        //// Identify the names of the nodes in the dot file
        //ostringstream namesFName; namesFName << imgPath << "/image_" << numImages << ".names";
        //{
        //      ostringstream cmd; cmd << "~/Compilers/dot2Nodes.pl "<<dotFName.str()<<" > "<<namesFName.str(); // -Tcmapx -o"<<mapFName.str()<<"
        //      //cout << "Command \""<<cmd.str()<<"\"\n";
        //      system(cmd.str().c_str());
        //}
   //
        //// Read the names of the dot file's node
        //ifstream namesFile(namesFName.str().c_str());
        //string line;
        //set<string> nodes;
        //if(namesFile.is_open()) {
        //      while(namesFile.good()) {
        //              getline(namesFile, line);
        //              //*(this) << line << endl;
        //              if(line!="") nodes.insert(line);
        //      }
        //      namesFile.close();
        //}
        //else {
        //      cout << "dbgStream::addDOT() ERROR opening file \""<<namesFName.str()<<"\" for reading!"<<endl;
        //}
                
        ret << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"image_"<<numImages<<":<img src=\""<<imgFName<<"\" "; // <a href=\"image_" << numImages << ".dot\">
        //usemap=\""<<graphName.str()<<"\"
        ret << "><br>\n"; // </a>
        
        //ret << "<a href=\"javascript:showNodes("<<numImages<<", '";
        //for(set<string>::iterator n=nodes.begin(); n!=nodes.end(); ) {
        //      ret << "" << *n << "";
        //      n++;
        //      if(n!=nodes.end())
        //              ret << " ";
        //}
        //ret << "')\">Subset</a><iframe height=0 width=0 id=\"imgFrame_"<<numImages<<"\"></iframe><br>\n";
        
        //ret << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<embed height=\"100%\" width=\"100%\" pluginspage=\"http://www.adobe.com/svg/viewer/install\" src=\""<<imgFName<<"\">"<<endl;
        //ret << "\t\t\t"<<tabs(buf.funcs.size()+1)<<"<object height=\"100%\" width=\"100%\" data=\""<<imgFName<<"\" type=\"image/svg+xml\"></object>"<<endl;
        
        // Open the map file and copy its contents into the output stream
        /*ifstream mapFile(mapFName.str().c_str());
        string line;
        //cout << "opening file \""<<mapFName.str()<<"\" for reading!"<<endl;
        if(mapFile.is_open()) {
                while(mapFile.good()) {
                        getline(mapFile, line);
                        ret << line << endl;
                }
                mapFile.close();
        }
        else {
                cout << "dbgStream::addDOT() ERROR opening file \""<<mapFName.str()<<"\" for reading!"<<endl;
        }       */
        numImages++;
}

        bool initialized=false;
        dbgStream dbg;
        
        // Initializes the debug sub-system
        void init(string title, string workDir, string fName)
        {
                if(initialized) return;
                        
                { 
                        ostringstream cmd; cmd << "mkdir -p "<<workDir;
                        int ret = system(cmd.str().c_str());
                        if(ret == -1) { cout << "Dbg::init() ERROR creating directory \""<<workDir<<"\"!"; exit(-1); }
                }
                ostringstream imgPath; imgPath << workDir << "/dbg_imgs";
                
                {
                        ostringstream cmd; cmd << "mkdir -p "<<imgPath.str();
                        //cout << "Command \""<<cmd.str()<<"\"\n";
                        int ret = system(cmd.str().c_str());
                        if(ret == -1) { cout << "Dbg::init() ERROR creating directory \""<<imgPath.str()<<"\"!"; exit(-1); }
                }
                        
                ostringstream dbgFileName; dbgFileName << workDir << "/" << fName;
                dbg.init(title, dbgFileName.str(), workDir, imgPath.str());
                initialized = true;
  }

        
        // Indicates that the application has entered or exited a function
        void enterFunc(std::string funcName/*, std::string indent="    "*/)
        {
                if(!initialized) init("Debug Output", ".", "debug");
                dbg.enterFunc(funcName);
        }
        
        void exitFunc(std::string funcName)
        {
                if(!initialized) init("Debug Output", ".", "debug");
                dbg.exitFunc(funcName);
        }
                
        // Adds an image to the output with the given extension and returns the path of this image
        // so that the caller can write to it.
        std::string addImage(std::string ext)
        {
                if(!initialized) init("Debug Output", ".", "debug");
                return dbg.addImage(ext);
        }
        
        // Given a representation of a graph in dot format, create an image from it and add it to the output.
        // Return the path of the image.
        std::string addDOT(dottable& obj)
        {
                if(!initialized) init("Debug Output", ".", "debug");
                return dbg.addDOT(obj);
        }
        
        // Given a representation of a graph in dot format, create an image of it and return the string
        // that must be added to the output to include this image.
        std::string addDOTStr(dottable& obj)
        {
                if(!initialized) init("Debug Output", ".", "debug");
                return dbg.addDOTStr(obj);
        }
        
        // Given a representation of a graph in dot format, create an image from it and add it to the output.
        // Return the path of the image.
        std::string addDOT(std::string dot) {
                if(!initialized) init("Debug Output", ".", "debug");
                return dbg.addDOT(dot);
        }
        
        // Given a string, returns a version of the string with all the control characters that may appear in the 
        // string escaped to that the string can be written out to Dbg::dbg with no formatting issues.
        // This function can be called on text that has already been escaped with no harm.
        std::string escape(std::string s)
        {
                string out;
                for(size_t i=0; i<s.length(); i++) {
                        // Manage HTML tags
                             if(s[i] == '<') out += "&lt;";
                        else if(s[i] == '>') out += "&gt;";
                        // Manage hashes, since they confuse the C PreProcessor CPP
                        else if(s[i] == '#') out += "&#35;";
                        else                 out += s[i];
                }
                return out;
        }


  void dotGraphGenerator (::Analysis *a) 
  {
    ::analysisStatesToDOT eas( a);
    IntraAnalysisResultsToDotFiles upia_eas(eas);
    upia_eas.runAnalysis();
  }

} // namespace Dbg
