#ifdef TEMPLATE_IMPLEMENTATIONS

#ifndef ASTDOTGENERATION_TEMPLATES_C
#define ASTDOTGENERATION_TEMPLATES_C

// if we included rose.h, then we need these files
#ifdef ROSE_H
// otherwise, we just include rose.h
#	define __STDC_FORMAT_MACROS
#	include <inttypes.h>
#else
#	include "sage3basic.h"
//	#include "AstConsistencyTests.h"
//#	ifndef __STDC_FORMAT_MACROS
#		define __STDC_FORMAT_MACROS
//#		define PRIx64 "I64x"
//#	endif
#	include <inttypes.h>
#	include "AsmUnparser_compat.h"
#	include "wholeAST_API.h"
#	include "sageInterface.h"
#endif

template <typename ExtraNodeInfo_t, typename ExtraNodeOptions_t, typename ExtraEdgeInfo_t, typename ExtraEdgeOptions_t>
DOTSynthesizedAttribute
AstDOTGenerationExtended<ExtraNodeInfo_t, ExtraNodeOptions_t, ExtraEdgeInfo_t, ExtraEdgeOptions_t>::evaluateSynthesizedAttribute(SgNode* node, DOTInheritedAttribute ia, SubTreeSynthesizedAttributes l)
{
	using namespace std;

	SubTreeSynthesizedAttributes::iterator iter;
	ROSE_ASSERT(node);

	// printf ("AstDOTGeneration::evaluateSynthesizedAttribute(): node = %s \n",node->class_name().c_str());

	// DQ (5/3/2006): Skip this IR node if it is specified as such in the inherited attribute
	if (ia.skipSubTree == true)
	{
	// I am unclear if I should return NULL or node as a parameter to DOTSynthesizedAttribute
	// Figured this out: if we return a valid pointer then we get a node in the DOT graph 
	// (with just the pointer value as a label), where as if we return a DOTSynthesizedAttribute 
	// with a NUL pointer then the node will NOT appear in the DOT graph.
	// return DOTSynthesizedAttribute(node);
	  return DOTSynthesizedAttribute(NULL);
	}

	string nodeoption;
//	if(AstTests::isProblematic(node))
//	{
//	// cout << "problematic node found." << endl;
//	  nodeoption="color=\"orange\" ";
//	}
	string nodelabel=string("\\n")+node->class_name();

	// DQ (1/19/2009): Added support for output of what specific instrcution this is in the dot graph.
	SgAsmInstruction* genericInstruction = isSgAsmInstruction(node);
	if (genericInstruction != NULL)
	{
	// At the moment the mnemonic name is stored, but it could be computed in the 
	// future from the kind and the tostring() function.
#if 1
	  string unparsedInstruction = unparseInstruction(genericInstruction);
	  string addressString       = StringUtility::numberToString( (void*) genericInstruction->get_address() );
	// string name = genericInstruction->get_mnemonic();
	  string name = unparsedInstruction + "\\n address: " + addressString;
#else
	  string name = unparsedInstruction + "\\n" + addressString;
#endif
	  ROSE_ASSERT(name.empty() == false);

	  nodelabel += string("\\n") + name;
	}

	SgAsmExpression* genericExpression = isSgAsmExpression(node);
	if (genericExpression != NULL)
	{
	  string name = unparseExpression(genericExpression);
	  ROSE_ASSERT(name.empty() == false);
	  nodelabel += string("\\n") + name;
	}

	// DQ (10/29/2008): Added some support for additional output of internal names for specific IR nodes.
	// In generall there are long list of these IR nodes in the binary and this helps make some sense of 
	// the lists (sections, symbols, etc.).
	SgAsmExecutableFileFormat* binaryFileFormatNode = isSgAsmExecutableFileFormat(node);
	if (binaryFileFormatNode != NULL)
	{
	// The case of binary file format IR nodes can be especially confusing so we want the 
	// default to output some more specific information for some IR nodes (e.g. sections).
	  string name;

	  SgAsmGenericSection* genericSection = isSgAsmGenericSection(node);
	  if (genericSection != NULL)
		 {
		   SgAsmGenericString* genericString = genericSection->get_name();
		   ROSE_ASSERT(genericString != NULL);

		   name = genericString->get_string();
		 }

	  SgAsmGenericSymbol* genericSymbol = isSgAsmGenericSymbol(node);
	  if (genericSymbol != NULL)
		 {
		   SgAsmGenericString* genericString = genericSymbol->get_name();
		   ROSE_ASSERT(genericString != NULL);

		   name = genericString->get_string();

		   if (name.empty() == true)
				name = "no_name_for_symbol";
		 }

	  SgAsmGenericDLL* genericDLL = isSgAsmGenericDLL(node);
	  if (genericDLL != NULL)
		 {
		   SgAsmGenericString* genericString = genericDLL->get_name();
		   ROSE_ASSERT(genericString != NULL);

		   name = genericString->get_string();
		 }

	  SgAsmPEImportHNTEntry* peImportHNTEntry = isSgAsmPEImportHNTEntry(node);
	  if (peImportHNTEntry != NULL)
		 {
		   SgAsmGenericString* genericString = peImportHNTEntry->get_name();
		   ROSE_ASSERT(genericString != NULL);

		   name = genericString->get_string();
		 }

	  SgAsmDwarfLine* asmDwarfLine = isSgAsmDwarfLine(node);
	  if (asmDwarfLine != NULL)
		 {
		   char buffer[100];

		// It does not work to embed the "\n" into the single sprintf parameter.
		// sprintf(buffer," Addr: 0x%08"PRIx64" \n line: %d col: %d ",asmDwarfLine->get_address(),asmDwarfLine->get_line(),asmDwarfLine->get_column());

		   sprintf(buffer,"Addr: 0x%08"PRIx64,asmDwarfLine->get_address());
		   name = buffer;
		   sprintf(buffer,"line: %d col: %d",asmDwarfLine->get_line(),asmDwarfLine->get_column());
		   name += string("\\n") + buffer;
		 }

	  SgAsmDwarfConstruct* asmDwarfConstruct = isSgAsmDwarfConstruct(node);
	  if (asmDwarfConstruct != NULL)
		 {
		   name = asmDwarfConstruct->get_name();
		 }

#if 0
	// This might not be the best way to implement this, since we want to detect common base classes of IR nodes.
	  switch (node->variantT())
		 {
		   case V_SgAsmElfSection:
			  {
				SgAsmElfSection* n = isSgAsmElfSection(node);
				name = n->get_name();
				break;
			  }

		   default:
			  {
			 // No additional information is suggested for the default case!
			  }
		 }
#endif

	  if (name.empty() == false)
		   nodelabel += string("\\n") + name;
	}

	// DQ (11/29/2008): Output the directives in the label of the IR node.
	SgC_PreprocessorDirectiveStatement* preprocessorDirective = isSgC_PreprocessorDirectiveStatement(node);
	if (preprocessorDirective != NULL)
	{
	  string s = preprocessorDirective->get_directiveString();

	// Change any double quotes to single quotes so that DOT will not misunderstand the generated lables.
	  while (s.find("\"") != string::npos)
		 {
		   s.replace(s.find("\""),1,"\'");
		 }

	  if (s.empty() == false)
		   nodelabel += string("\\n") + s;
	}


	nodelabel += eni(node);//additionalNodeInfo(node);
	string additionalOptions = eno(node);//additionalNodeOptions(node);

	string x;
	string y;
	x += additionalOptions;
	nodeoption += additionalOptions;

	DOTSynthesizedAttribute d(0);

	// DQ (7/27/2008): Added mechanism to support pruning of AST
	bool commentoutNode = commentOutNodeInGraph(node);
	if (commentoutNode == true)
	{
	// DQ (11/10/2008): Fixed to only output message when (verbose_level > 0); command-line option.
	// DQ (7/27/2008): For now just return to test this mechanism, then we want to add comment "//" propoerly to generated DOT file.
	  if (SgProject::get_verbose() > 0)
		 {
		   printf ("Skipping the use of this IR node in the DOT Graph \n");
		 }
	}
	else
	{

	// **************************

	switch(traversal)
	{
	  case TOPDOWNBOTTOMUP:
		   dotrep.addNode(node,dotrep.traceFormat(ia.tdbuTracePos,tdbuTrace)+nodelabel,nodeoption);
		   break;
	  case PREORDER:
	  case TOPDOWN:
		   dotrep.addNode(node,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
		   break;
	  case POSTORDER:
	  case BOTTOMUP:
		   dotrep.addNode(node,dotrep.traceFormat(buTrace)+nodelabel,nodeoption);
		   break;
	  default:
		   assert(false);
	}

	++tdbuTrace;
	++buTrace;

	// add edges or null values
	int testnum=0;
	for (iter = l.begin(); iter != l.end(); iter++)
	{
	  string edgelabel = string(node->get_traversalSuccessorNamesContainer()[testnum]);
	  string toErasePrefix = "p_";

//	  if (AstTests::isPrefix(toErasePrefix,edgelabel))
//		 {
//		   edgelabel.erase(0, toErasePrefix.size());
//		 }

	  if ( iter->node == NULL)
		 {
		// SgNode* snode=node->get_traversalSuccessorContainer()[testnum];
		   AstSuccessorsSelectors::SuccessorsContainer c;
		   AstSuccessorsSelectors::selectDefaultSuccessors(node,c);
		   SgNode* snode=c[testnum];

		// isDefault shows that the default constructor for synth attribute was used
		   if (l[testnum].isDefault() && snode && (visitedNodes.find(snode) != visitedNodes.end()) )
			  {
			 // handle bugs in SAGE
				dotrep.addEdge(node,edgelabel,snode,"dir=forward arrowhead=\"odot\" color=red ");
			  }
			 else
			  {
				if (snode == NULL)
				   {
					 dotrep.addNullValue(node,"",edgelabel,"");
				   }
			  }
		 }
		else
		 {
		// DQ (3/5/2007) added mechanism to add additional options (to add color, etc.)
		   string edgeoption = additionalEdgeOptions(node,iter->node,edgelabel);

		   switch(traversal)
			  {
				case TOPDOWNBOTTOMUP:
					 dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=both");
					 break;
				case PREORDER:
				case TOPDOWN:
					 dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=forward");
					 break;
				case POSTORDER:
				case BOTTOMUP:
					 dotrep.addEdge(node,edgelabel,(*iter).node,edgeoption + "dir=back");
					 break;
				default:
					 assert(false);
			  }
		 }

	  testnum++;
	}

	// **************************
	}



	// DQ (7/4/2008): Support for edges specified in AST attributes
	AstAttributeMechanism* astAttributeContainer = node->get_attributeMechanism();
	if (astAttributeContainer != NULL)
	{
	// Loop over all the attributes at this IR node
	  for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
		 {
		// std::string name = i->first;
		   AstAttribute* attribute = i->second;
		   ROSE_ASSERT(attribute != NULL);

		// This can return a non-empty list in user-defined attributes (derived from AstAttribute).
		// printf ("Calling attribute->additionalNodeInfo() \n");
		   std::vector<AstAttribute::AttributeNodeInfo> nodeList = attribute->additionalNodeInfo();
		// printf ("nodeList.size() = %lu \n",nodeList.size());

		   for (std::vector<AstAttribute::AttributeNodeInfo>::iterator i_node = nodeList.begin(); i_node != nodeList.end(); i_node++)
			  {
				SgNode* nodePtr   = i_node->nodePtr;
				string nodelabel  = i_node->label;
				string nodeoption = i_node->options;
			 // printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding a node nodelabel = %s nodeoption = %s \n",nodelabel.c_str(),nodeoption.c_str());
			 // dotrep.addNode(NULL,dotrep.traceFormat(ia.tdTracePos)+nodelabel,nodeoption);
				dotrep.addNode( nodePtr, dotrep.traceFormat(ia.tdTracePos) + nodelabel, nodeoption );
			  }

		// printf ("Calling attribute->additionalEdgeInfo() \n");
		   std::vector<AstAttribute::AttributeEdgeInfo> edgeList = attribute->additionalEdgeInfo();
		// printf ("edgeList.size() = %lu \n",edgeList.size());
		   for (std::vector<AstAttribute::AttributeEdgeInfo>::iterator i_edge = edgeList.begin(); i_edge != edgeList.end(); i_edge++)
			  {
				string edgelabel  = i_edge->label;
				string edgeoption = i_edge->options;
			 // printf ("In AstDOTGeneration::evaluateSynthesizedAttribute(): Adding an edge from i_edge->fromNode = %p to i_edge->toNode = %p edgelabel = %s edgeoption = %s \n",i_edge->fromNode,i_edge->toNode,edgelabel.c_str(),edgeoption.c_str());
				dotrep.addEdge(i_edge->fromNode,edgelabel,i_edge->toNode,edgeoption + "dir=forward");
			  }
		 }
	}



	switch(node->variantT())
	{
	// DQ (9/1/2008): Added case for output of SgProject rooted DOT file.
	// This allows source code and binary files to be combined into the same DOT file.
	  case V_SgProject: 
		 {
		   SgProject* project = dynamic_cast<SgProject*>(node);
		   ROSE_ASSERT(project != NULL);

		   string generatedProjectName = SageInterface::generateProjectName( project );
		// printf ("generatedProjectName (from SgProject) = %s \n",generatedProjectName.c_str());
		   if (generatedProjectName.length() > 40)
			  {
			 // printf ("Warning: generatedProjectName (from SgProject) = %s \n",generatedProjectName.c_str());
				generatedProjectName = "aggregatedFileNameTooLong";
				printf ("Proposed (generated) filename is too long, shortened to: %s \n",generatedProjectName.c_str());
			  }

		   string filename = string("./") + generatedProjectName + ".dot";

		// printf ("generated filename for dot file (from SgProject) = %s \n",filename.c_str());
		   if ( SgProject::get_verbose() >= 1 )
				printf ("Output the DOT graph from the SgProject IR node (filename = %s) \n",filename.c_str());

		   dotrep.writeToFileAsGraph(filename);
		   break;
		 }

	// case V_SgFile: 
	  case V_SgSourceFile: 
	  case V_SgBinaryComposite: 
		 {
		   SgFile* file = dynamic_cast<SgFile*>(node);
		   ROSE_ASSERT(file != NULL);

		   string original_filename = file->getFileName();

		// DQ (7/4/2008): Fix filenamePostfix to go before the "."
		// string filename = string("./") + ROSE::stripPathFromFileName(original_filename) + "."+filenamePostfix+"dot";
//		   string filename = string("./") + ROSE::stripPathFromFileName(original_filename) + filenamePostfix + ".dot";
		   string filename = string("./") + StringUtility::stripPathFromFileName(original_filename) + filenamePostfix + ".dot";

		// printf ("generated filename for dot file (from SgSourceFile or SgBinaryComposite) = %s file->get_parent() = %p \n",filename.c_str(),file->get_parent());

		// printf ("file->get_parent() = %p \n",file->get_parent());
		// cout << "generating DOT file (from SgSourceFile or SgBinaryComposite): " << filename2 << " ... ";

		// DQ (9/1/2008): this effects the output of DOT files when multiple files are specified 
		// on the command line.  A SgProject is still built even when a single file is specificed 
		// on the command line, however there are cases where a SgFile can be built without a 
		// SgProject and this case allows those SgFile rooted subtrees to be output as DOT files.
		// If there is a SgProject then output the dot file from there, else output as a SgFile.
		   if (file->get_parent() == NULL)
			  {
			 // If there is no SgProject then output the file now!
				if ( SgProject::get_verbose() >= 1 )
					 printf ("Output the DOT graph from the SgFile IR node (no SgProject available) \n");

				dotrep.writeToFileAsGraph(filename);
			  }
			 else
			  {
			 // There is a SgProject IR node, but if we will be traversing it we want to output the 
			 // graph then (so that the graph will include the SgProject IR nodes and connect multiple 
			 // files (SgSourceFile or SgBinaryComposite IR nodes).
				if ( visitedNodes.find(file->get_parent()) == visitedNodes.end() )
				   {
				  // This SgProject node was not input as part of the traversal, 
				  // so we will not be traversing the SgProject IR nodes and we 
				  // have to output the graph now!

					 if ( SgProject::get_verbose() >= 1 )
						  printf ("Output the DOT graph from the SgFile IR node (SgProject was not traversed) \n");

					 dotrep.writeToFileAsGraph(filename);
				   }
				  else
				   {
					 if ( SgProject::get_verbose() >= 1 )
						  printf ("Skip the output of the DOT graph from the SgFile IR node (SgProject will be traversed) \n");
				   }
			  }
		   
		// cout << "done." << endl;
		   break;
		 }

	// DQ (7/23/2005): Implemented default case to avoid g++ warnings 
	// about enum values not handled by this switch
	  default: 
		 {
		// nothing to do here
		   break;
		 }
	}

	d.node = node;
	return d;
}

#endif // ASTDOTGENERATION_TEMPLATES_C

#else
#error Must include template implementation files after all header files.  If you have, make sure you define TEMPLATE_IMPLEMENTATIONS
#endif // TEMPLATE_IMPLEMENTATIONS

