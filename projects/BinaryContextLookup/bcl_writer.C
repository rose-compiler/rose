#include "rose.h"
#include <CallGraph.h>
#include <iostream>
#include <string>
#include <map>
#include <libxml/tree.h>

using std::string;
using namespace std;

int main (int argc, char **argv){
	string	outFileName;
	// Build the AST used by ROSE
	SgProject * project = frontend (argc, argv);
	ROSE_ASSERT (project != NULL);
	char stringBuffer[1024];
	// !get all Function declarations
	list<SgNode*> functionDefList = NodeQuery::querySubTree(project,V_SgFunctionDefinition);
	// map for matching file name to xml-tree nodes
	map<string,xmlNodePtr> fileNameNodeMap;
	
	//check libxml library
	LIBXML_TEST_VERSION
	// xmlTree Variables
	xmlDocPtr doc;
	xmlNodePtr rootNode=NULL;
	xmlNodePtr fileListNode=NULL,
						 currentFile=NULL,
						 currentFunction=NULL,
						 lineStartNode=NULL,
						 callerListNode=NULL,
						 callingNode=NULL,
						 calledNode=NULL,
						 lineEndNode=NULL,
						 newNode=NULL;
	// counter 
	int differentFileCounter=0;
	// Create the root-node and set basic xml info
	doc = xmlNewDoc(BAD_CAST "1.0");
	xmlNewDocComment(doc, BAD_CAST "ROSE-Function-Lookup-Export");// xml-version 1.0
	// create a root node for data
	rootNode= xmlNewNode(NULL,BAD_CAST "RoseFLE");
	xmlDocSetRootElement(doc,rootNode);
	// create and add global processing information (PI)
	newNode=xmlNewComment(BAD_CAST"creating some style-information for browsers, to prvent nasty-looking browserdisplay");
	xmlAddPrevSibling(rootNode,newNode);

	// create the file list-tag
	fileListNode=xmlNewChild(rootNode,NULL,BAD_CAST"FILE_LIST",NULL);
	
	// iterate over all function-definitions and associate them with their file-name node
	for (list<SgNode*>::iterator i = functionDefList.begin();i!=functionDefList.end();i++)
	{
		SgFunctionDefinition * def = isSgFunctionDefinition(*i);

		// get the filename for the current function		
	  string fileName=def->get_startOfConstruct()->get_filenameString();
		// see if the filename has a xmlNode associated with it
		if (fileNameNodeMap.find(fileName)==fileNameNodeMap.end())
		{
		  // there is not yet a xml-node with the filename, create one
			currentFile=xmlNewChild(fileListNode,NULL,BAD_CAST"FILE",NULL);
			newNode=xmlNewChild(currentFile,NULL,BAD_CAST"NAME",BAD_CAST fileName.c_str());
			fileNameNodeMap[fileName]=currentFile;			
			differentFileCounter++;
			snprintf(stringBuffer,1020,"%i",differentFileCounter);
		  xmlNewProp(currentFile,BAD_CAST"fileNumber",BAD_CAST stringBuffer);			
		}
		else
		{
			// associat the function-definition with the file-name
			currentFile=fileNameNodeMap[fileName];
		}
		// create a new function node
		currentFunction=xmlNewChild(currentFile,NULL,BAD_CAST "FUNCTION_DEFINITION",NULL);
		snprintf(stringBuffer,1020,"%s",def->get_declaration()->get_qualified_name().str());		
		xmlNewProp(currentFunction,BAD_CAST "functionName",BAD_CAST stringBuffer);
		xmlNewChild(currentFunction,NULL,BAD_CAST "NAME",BAD_CAST stringBuffer);
	
		//store line-information for the function
		Sg_File_Info * fi=def->get_startOfConstruct();
		Sg_File_Info * fe=def->get_body()->get_endOfConstruct();
		snprintf(stringBuffer,1020,"%i",fi->get_line());
		lineStartNode=xmlNewChild(currentFunction,NULL,BAD_CAST "LINE_START",NULL);		
		xmlNewProp(lineStartNode,BAD_CAST "lineNumber",BAD_CAST stringBuffer);
		lineEndNode=xmlNewChild(currentFunction,NULL,BAD_CAST "LINE_END",NULL);
		snprintf(stringBuffer,1020,"%i",fe->get_line());
		xmlNewProp(lineEndNode,BAD_CAST "lineNumber",BAD_CAST stringBuffer);
	}
	// store meta-information (currently not really used)
	snprintf(stringBuffer,1020,"%i",differentFileCounter);
	xmlNewProp(fileListNode,BAD_CAST"fileCount",BAD_CAST stringBuffer);

	// create base XML-node for callgraph-information
	callerListNode=xmlNewChild(rootNode,NULL,BAD_CAST"CALLER_LIST",NULL);

	// now get the callgraph ....
	CallGraphBuilder    cgb (project);
	cgb.buildCallGraph ();
	CallGraphCreate *cg=cgb.getGraph ();
	// and get the called_by information
	// go through all nodes an print those, who are called by another function
	CallGraphCreate::NodeIterator itNode = cg->GetNodeIterator();
	while(!itNode.ReachEnd())
	{
		CallGraphCreate::Node * cgcFN = * itNode;
		CallGraphCreate::Node * callingFunctionNode;
		// get all edges pointing towards the current node
		CallGraphCreate::EdgeIterator itEdge = cg->GetNodeEdgeIterator(cgcFN,GraphAccess::EdgeIn);
		// if there are no edges towards this node, skipp the node and continue
		if (!itEdge.ReachEnd())
		{
			// function actually is called -> add to list
			calledNode=xmlNewChild(callerListNode,NULL,BAD_CAST"FUNCTION",NULL);
			// get the full qualified name form the node
			newNode=xmlNewChild(calledNode,NULL,BAD_CAST"NAME",BAD_CAST cgcFN->functionDeclaration->get_qualified_name().getString().c_str());
			// for alle calling-functions...
			while(!itEdge.ReachEnd())
			{
				CallGraphCreate::Edge * cgcEN = *itEdge;
				callingFunctionNode=cg->GetEdgeEndPoint(cgcEN,GraphAccess::EdgeOut);
				// create a called by-entry
				callingNode=xmlNewChild(calledNode,NULL,BAD_CAST"CALLED_BY",NULL);
				newNode=xmlNewChild(callingNode,NULL,BAD_CAST"NAME",BAD_CAST callingFunctionNode->label.c_str());
				itEdge++;
			}
		}
		itNode++;
	}
	// use the outputfilename-parameter of rose to determin the outfile
	outFileName=project->get_outputFileName();
	// save the xml-tree to a file:
	xmlSaveFormatFile(outFileName.c_str(),doc,1);
/* compression could be done the following way	
  xmlSetDocCompressMode(doc,8);
	xmlSaveFile("test-compressed.xml",doc);*/

  // the document is not needed anymore, free it
	xmlFreeDoc(doc);
	// xml-cleanup
  xmlCleanupParser();
}
