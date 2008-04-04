#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <string.h>
#include <libxml/encoding.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

using std::string;
using namespace std;

map<string,int > filenameIDMap;

vector<vector<string> > functionnameListList;
vector<vector<int> > functionstartListList;
vector<vector<int> > functionendListList;

map<string,int>  functionIDMap;

map<string, vector < string> > functionCallerListMap;

int readFunctionDefinitionSubTree(xmlNodePtr funcDefNode,vector<string> * functionnameList,vector<int> * functionstartList,vector<int> * functionendList,map<string, int> * functionIDMap);
void readFileList(xmlNodePtr fileListRootNode);
void readCallerList(xmlNodePtr callerList);
xmlNodePtr filterTreeForBlankNodes(xmlNodePtr  node);


int main (int argc, char **argv)
{
	bool fileListRead=false,callerListRead=false;
	// open XML file
	LIBXML_TEST_VERSION
	// xmlTree Variables
	xmlDocPtr doc= NULL;
	xmlNode *rootElement = NULL,
				  *current,
					*roseFLENode=NULL;
	// check program parameters, VERY SIMPLE
	if (argc <2) 
	{
		cerr << "lm_reader PROGRAM_DB"<<endl;
		return -1;
	}
	// read the XML-File
	// since in version 2.5.x, which is currently installed in the lab does not have a filtered xmlReadFile function parseFile is used an the resulting xmlTree is filterd for "blank"-nodes
#if LIBXML_VERSION> 20600
	doc = xmlReadFile(argv[1],NULL,XML_PARSE_NOBLANKS);
#else
	doc = xmlParseFile(argv[1]);
#endif
	if (doc == NULL)
	{
		cerr << "critical error while reading XML-File \""<<argv[1]<<"\""<<endl;
	}
  // get the root node
	current=rootElement = xmlDocGetRootElement(doc);
#if LIBXML_VERSION >20600
#else
	current=rootElement=filterTreeForBlankNodes(rootElement);
	//make shure that the doc is correct
	xmlDocSetRootElement(doc,rootElement);
#endif
	// search for the roseFLE-Node
	while(current)
	{
	  if (strncmp((char*)current->name,"RoseFLE",10)==0)
		{
			roseFLENode=current;
			break;
		}
		current=current->next;
	}
	// check if roseFLE was found
	if (roseFLENode==NULL)
	{
		cerr<< argv[1] << " is not a Rose-FLE database\n";
		return -1;
	}
	// go through the children of the FLE
	current=roseFLENode->children;
	while(current)
	{
		switch (current->type)
		{
			case XML_ELEMENT_NODE:				
				// child level of the root node: only two possible
				if (xmlStrcmp(current->name,BAD_CAST "FILE_LIST")==0)
				{
					if (!fileListRead)
					{
						readFileList(current);					
						fileListRead=true;
					}
					else
					{
						cerr <<"Mutliple FILE_LIST statements found. This is forbidden in the current verion"<<endl;
						return -1;
					}
				}
				else if (xmlStrcmp(current->name,BAD_CAST "CALLER_LIST")==0)
				{					
					if (!callerListRead)
					{
						readCallerList(current);
						callerListRead=true;
					}
					else
					{
						cerr << "Multiple CALLER_LIST statements found. This is forbidden in the current version" << endl;
					}
				}
				else 
				{
					cerr << "Unkown Tag: "<< current->name <<endl<<"Continuing with file, but this should not happen!!"<<endl;					
				}
				break;
				default:
					//CI (01/12/2007): This should not happen, but might for user modified or biuld XML-files
					break;
		}
		current=current->next;
	}
	// Database read, start with querys
	// this output is just for convenience
	cout << "Searchparamter: filename lineNo" << endl;
	string searchFileName;
	int searchLineNumber;
	bool continueInputLoop=true;
	while (continueInputLoop)
	{
		continueInputLoop=true;
		cin >> searchFileName;
		// check if the line-number has already been specified in the initial "file-name-striung"
		if (cin.eof()) continueInputLoop=false;
		{
			// has a colon ...
			if (searchFileName.rfind(":")!=std::string::npos)
			{
				// get the first colon from the back and split into filename:lineNumber
				sscanf(searchFileName.substr(searchFileName.rfind(":")+1,searchFileName.length()).c_str(),"%i",&searchLineNumber);
			  searchFileName=searchFileName.substr(0,searchFileName.find(":"));
			}
			else
			{
				// read line-number as a separate input
				cin >> searchLineNumber;
			}
		}
		if (cin.eof()) continueInputLoop=false;
		else 
		{
			// good input, work with it
			int id;
			// use the filename Map to identify the file with the searchID
			if (!(filenameIDMap.find(searchFileName) == filenameIDMap.end()))
			{
				id=filenameIDMap[searchFileName];
				bool found=false;
				// go through the functionlist and find the match to the given line
				for (unsigned int i=0;i<functionstartListList[id].size();i++)
				{
					// is the given line between a function-start and function-end line
					if (functionstartListList[id][i]<=searchLineNumber &&
							functionendListList[id][i]>=searchLineNumber)
						{
							// unsued ... int ID=functionIDMap[functionnameListList[id][i]];
							vector<string> localCallerList=functionCallerListMap[functionnameListList[id][i]];
							// output all functions calling ...
							cout << functionnameListList[id][i] << " called by " << localCallerList.size() << " function(s)";
							for (unsigned int j=0;j<localCallerList.size();j++)
							{
							  if (j==0) cout <<": ";
								else cout <<" ";
								cout <<localCallerList[j];
							}
							cout << endl;
							found=true;
						}
				}
				// if there was no line-number-match, output this
				if (!found)
				{
					cout << "File and line-number outside a function."<<endl;
				}
			}
			else
			{
				// if there was not a match in filename ... output this
				cout << "Filename is not in the database." << endl;
				continue;
			}
		}
	}// loop while continueInputLoop==true
	return 0;
}


//! filter the xml-tree for blank nodes
xmlNodePtr filterTreeForBlankNodes(xmlNodePtr  node)
{
	xmlNodePtr next,current=node, retNode=NULL;
	// only text nodes can be blank and text nodes do not have children
	if (current && current->type ==XML_TEXT_NODE && current->children!=NULL)
	{
		cerr << "TEXTNODE HAS NO CHILDREN-ASSUMPTION FAILED" << endl;
		exit(-1);
	}
	// scan for the first non blank node and mark it as return-node
	while(current!=NULL)
	{
		// store the next node, because in the end of this loop the current node might already have been freed
		next=current->next;		
		// if the current node is not blank
		if (!xmlIsBlankNode(current))
		{
			// and no return node has been specified
			if(retNode==NULL)
			{
				retNode=current;
			}
			// proceed with this nodes children recursivly
			current->children=filterTreeForBlankNodes(current->children);
		}
		else // a blank node
		{	
			// if the node has a previouse node, the next pointer on that node needs to be changed
			if (current->prev)
				current->prev->next=current->next;
			// if there is a next node to the current node, the prev-pointer of that node needs to be changed
			if (current->next)
				current->next->prev=current->prev;
			// cleanup this node
			xmlFreeNode(current);
		}
		// go to the next node
		current=next;
	}
	// return the first not empty node in the list
	return retNode;
}

void readFileList(xmlNodePtr fileListRootNode)
{
  int fileCounter=0;
	
	int fileNameID=-1;
	xmlNodePtr currentFile;
	
	vector<xmlNodePtr> functionList;
	vector<xmlNodePtr> fileNameList;
	
	// get the attribute of files ..
	xmlAttrPtr fileCountAttr=xmlHasProp(fileListRootNode,BAD_CAST "fileCount");
	if (fileCountAttr!=NULL)
	{
#ifdef DEBUG
		cerr << "WARNING: expecting " << fileCountAttr->children->content <<endl;
#endif
	}
	else
	{
		//no helper has been identified, ignore this
		cerr << "MSG: malformed db-file,but not fatal"<<endl;
	}
	
	//! iterate on all children of the FILE_LIST-Tag
	for (xmlNodePtr currentFileCandidate=fileListRootNode->children;currentFileCandidate!=NULL;currentFileCandidate=currentFileCandidate->next)
	{
		//!cleanup the functionlists
	  functionList.clear();
		fileNameList.clear();

		switch (currentFileCandidate->type)
		{
			// if it is a "normal"-tag
			case XML_ELEMENT_NODE:
				//! check for node if it is a FILE-Tag
				if (xmlStrcmp(currentFileCandidate->name,BAD_CAST "FILE")==0)
				{
					//! one more file-entry found
					fileCounter++;
					
					// get meta information
					xmlAttrPtr fileNumberAttr=xmlHasProp(currentFileCandidate,BAD_CAST "fileNumber");
					if (fileNumberAttr!=NULL)
					{
#ifdef DEBUG
						cerr << "MSG: according to prop this should be file number >" << fileNumberAttr->children->content << "< and is >" << fileCounter << "<\n";
#endif
					}
					else
					{
						cerr << "WARNING: Missing node, malformed db-file,but not fatal!\tline"<<__LINE__<<endl;	
					}
					//! enummerate files
					currentFile=currentFileCandidate;
					for (xmlNodePtr functionDefCand=currentFile->children;functionDefCand!=NULL;functionDefCand=functionDefCand->next)
					{
						// check if the child-node is a tag-element
						if (functionDefCand->type == XML_ELEMENT_NODE)
						{
							if (xmlStrcmp(functionDefCand->name,BAD_CAST "FUNCTION_DEFINITION")==0)
							{
								// mark the functiondefinition for later processing
								functionList.push_back(functionDefCand);
							}
							else if (xmlStrcmp(functionDefCand->name,BAD_CAST "NAME")==0)
							{
								// marl the file-name for later processing
								fileNameList.push_back(functionDefCand);
								//cout << "FileName: " << functionDefCand->children->content << endl;
							}
							else
							{
								cerr << "Unkown Node >" << functionDefCand->name<<"<"<<endl;
							}
						}
					}				
				}
			break;
#ifdef DEBUG
			case XML_TEXT_NODE:
				cout<< currentFileCandidate->content << endl;			
			default:
			cerr << "WARNING: Unintercepted elementType: " << currentFileCandidate->type<<endl;
			cerr << "\t\t"<<currentFileCandidate->name << endl;
			break;
#else
			default:
			break;
#endif
		}

		// now all sub-nodes of the current file have bee read, check for conformity
		if (fileNameList.size()!=1)
		{
			cerr << "There must be only 1 file-name defined, found " << fileNameList.size()<<endl;
			exit(-1);
		}
		// get the fileName and register it in the ID-map;
		if (filenameIDMap.find((char*)(fileNameList[0]->children->content))==filenameIDMap.end())
		{
			// element not existant: OK
			fileNameID=filenameIDMap.size();
			filenameIDMap[(char*)(fileNameList[0]->children->content)]=fileNameID;
		}
		else
		{
			cerr << "Mutliple files with the same name "<<fileNameList[0]->children->content<<" found. Not allowed. Exiting..."<<endl;
			exit(-1);
		}
		// associate functions with the file
		for (unsigned int i=0;i<functionList.size();i++)
		{
			vector<string> tmp;
			functionnameListList.push_back(tmp);
			functionstartListList.push_back(vector<int>::vector());
			functionendListList.push_back(vector<int>::vector());
			readFunctionDefinitionSubTree(
				functionList[i],
				&(functionnameListList[fileNameID]),
				&(functionstartListList[fileNameID]),
				&(functionendListList[fileNameID]),
				&functionIDMap);
		}
	}
}


/* this functions converts the <FUNCTION_DEFINITION functionName="foo3">
        <NAME>foo3</NAME>
				        <LINE_START lineNumber="3"/>
								        <LINE_END lineNumber="3"/>
												      </FUNCTION_DEFINITION> suptree into machine-date */
int readFunctionDefinitionSubTree(xmlNodePtr funcDefNode,
	vector<string> * functionnameList,
	vector<int> * functionstartList,
	vector<int> * functionendList,
	map<string, int> * functionIDMap)
{
	string functionNameStr;
	int functionStart,functionEnd;
	xmlNodePtr   functionNameNode=NULL,
							 functionStartNode=NULL,
							 functionEndNode=NULL;
	// iterate over all children of the function 
	for (xmlNodePtr current=funcDefNode->children;current!=NULL;current=current->next)
	{
		// ignore non-tag nodes like text
		if (current->type != XML_ELEMENT_NODE)
		{
			cerr << "ignoring >" << current->name <<"<"<<endl;
			continue;
		}
		// name found,mark for later processing
		if (xmlStrcmp(current->name,BAD_CAST "NAME")==0)
		{
			if (functionNameNode==NULL) functionNameNode=current;
			else {cerr << "double-tag >"<<functionNameNode->name<<"< failure"<<endl;exit(-1);}
		}
		// line start found, mark for later processing
		else if (xmlStrcmp(current->name,BAD_CAST "LINE_START")==0)
		{
			functionStartNode=current;
		}
		// line end found, mark for later processing
		else if (xmlStrcmp(current->name,BAD_CAST "LINE_END")==0)
		{
			functionEndNode=current;
		}
		// non-expected tag found, IGNORE
		else 
		{
			cerr << "ignoring >" << current->name << "<" << endl;
			continue;
		}
	}
	// check if everything is available
	if (functionNameNode==NULL || !functionStartNode || !functionEndNode)
	{
		cerr << "Missing an element" << endl;
	}
	// get data form (TEXT)
	functionNameStr=(char*)(functionNameNode->children->content);
	xmlAttrPtr fileNumberAttr=xmlHasProp(functionStartNode,BAD_CAST "lineNumber");
	functionStart=atoi((char*)fileNumberAttr->children->content);
	fileNumberAttr=xmlHasProp(functionEndNode,BAD_CAST "lineNumber");
	functionEnd=atoi((char*)fileNumberAttr->children->content);
	functionnameList->push_back(functionNameStr);
	functionstartList->push_back(functionStart);
	functionendList->push_back(functionEnd);
	if (functionIDMap->find(functionNameStr)==functionIDMap->end())
	(*functionIDMap)[functionNameStr]=(*functionIDMap).size();
	
	return 1;
}


void readCallerList(xmlNodePtr callerList)
{
  string functionName;
	string callerName;
	int functionID;
	vector<xmlNodePtr> functionNameList;
	vector<xmlNodePtr> calledByList;
	// iterate over all children of a function which states its callers
	for (xmlNodePtr current=callerList->children;current!=NULL;current=current->next)
	{
		// is the correct tag available
		if (xmlStrcmp(current->name,BAD_CAST "FUNCTION")==0)
		{
		  // clear internal work-lists
			calledByList.clear();
			functionNameList.clear();
			// iterate over the subtree containing the caller of the current"s node
			for(xmlNodePtr cFC=current->children;cFC!=NULL;cFC=cFC->next)
			{
				// NAME found, mark for later processing
				if (xmlStrcmp(cFC->name,BAD_CAST "NAME")==0)
				{
					functionNameList.push_back(cFC);
				}
				// called by found, mark for later processing
				else if (xmlStrcmp(cFC->name,BAD_CAST "CALLED_BY")==0)
				{
					calledByList.push_back(cFC);
				}
				// unspecified tag found, IGNORE
				else
				{
					cerr <<"Unkown TAG within CALLER_LIST->FUNCTION: " << cFC->name << endl;
				}				
			}
			// check for only 1 functionname
			if (functionNameList.size()!=1)
			{
				cerr << "Muliple Functions-Names found in CALLER_LIST->FUNCTION. Must not happen. Failing!"<<endl;
				exit(-1);
			}
			else
			{
				// get the functionname form the TEXT tag within the NAME tag (hack)
				functionName=(char*)(functionNameList[0]->children->content);			
				if (functionIDMap.find(functionName)==functionIDMap.end())
				{
					cout << "Function >" << functionName << "< has not been defined in any of the souce-files. This function is probably a librarycall or a sourcefile was missing when building the database!"<<endl;
				}
				else 
				{
					
					functionID=functionIDMap[functionName];
					//get the called by list
					functionCallerListMap[functionName]=vector<string>();
					// iterate over all called-by nodes to extract their name
					for (unsigned int i=0;i<calledByList.size();i++)
					{
						// evaluate the CALLED_BY tag
						if (xmlStrcmp(calledByList[i]->children->name,BAD_CAST "NAME")==0)
						{
							// get the caller name from within th NAME-tag (hack)
							callerName=(char*)(calledByList[i]->children->children->content);
							//cout << functionName << " called by "<< callerName << endl;
							functionCallerListMap[functionName].push_back(callerName);
						}
					}
				}
			}
		}	
	}
}

