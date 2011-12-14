#include "rose.h"
#include "BinQAbstract.h"
#include "BinQSupport.h"

#include "boost/filesystem/operations.hpp" 
#include <boost/lexical_cast.hpp>
#include <iostream>

#include "icons.h"
#include "folder.xpm"

#include "../DistributedMemoryAnalysisCompass/LoadSaveAST.h"

using namespace qrs;
using namespace boost::filesystem;
using namespace std;
using namespace boost;
using namespace __gnu_cxx;

// Initialization of all analyzes and parsing of all files
void BinQAbstract::init(){


  //cerr << "Disassemble File A ... " << fileNameA << endl;
  std::string sourceFileS;
  fileA = binqsupport->disassembleFile(fileNameA, sourceFileS);
  ROSE_ASSERT(fileA);

  if (isSgProject(fileA)) {
    SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
    SgAsmGenericFile* file = binary != NULL ? binary->get_binaryFile() : NULL;
    ROSE_ASSERT(file);
    info = new VirtualBinCFG::AuxiliaryInformation(file);
    algo = new GraphAlgorithms(info);
  }

  if (saveAST!="") {
    //std::cerr << "ROSE saving FILE.... " << saveAST << std::endl;
    LoadSaveAST::saveAST(saveAST, isSgProject(fileA)); 
    exit(0);
  }

  fileB=NULL;
  if (fileNameB!="") {
    //cerr << "\nDisassemble File B ... " << fileNameB << endl;
    fileB = binqsupport->disassembleFile(fileNameB, sourceFileS);
    ROSE_ASSERT(fileB);
    if (sourceFileS=="true")
      sourceFile=true;
  }

  if (dllA.size()>0) {
    std::vector<std::string>::const_iterator nameIt = dllA.begin();
    for (;nameIt!=dllA.end();++nameIt) {
      string name = *nameIt;
      SgNode* node = binqsupport->disassembleFile(name, sourceFileS);
      dllFilesA.push_back(node);
    }
  }

  if (dllB.size()>0) {
    std::vector<std::string>::const_iterator nameIt = dllB.begin();
    for (;nameIt!=dllB.end();++nameIt) {
      string name = *nameIt;
      SgNode* node = binqsupport->disassembleFile(name, sourceFileS);
      dllFilesB.push_back(node);
    }
  }

  if(isSgProject(fileA))  backend(isSgProject(fileA));

  string filename="binary_tree.dot";
  AST_BIN_Traversal* trav = new AST_BIN_Traversal();
  trav->run(fileA, filename);

  // ------------------------------------------------------------
  // can be used with  -rose:read_executable_file_format_only
  //  generateDOTforMultipleFile ( *isSgProject(fileA) );
  // ------------------------------------------------------------
  
  // -------------------------------------------------------------------------------------
  if (fileA)
    itemsFileA.clear();
  if (fileB)
    itemsFileB.clear();
  

  createFunction(fileA, funcsFileA, false);
  createFunction(fileB, funcsFileB, false);

#if 0
  if (fileA)
    cerr << " File A has " << RoseBin_support::ToString(funcsFileA.size()) << " funcs." << endl;
  if (fileB)
    cerr << " File B has " << RoseBin_support::ToString(funcsFileB.size()) << " funcs." << endl;
#endif 

  createItem(fileA,itemsFileA, funcsFileA, false);
  createItem(fileB,itemsFileB, funcsFileB, false);
}



void BinQAbstract::createItem(SgNode* file, std::vector<Item*>& itemsFile,std::vector<SgNode*>& funcsFile, bool dll) {
  std::vector<SgNode*> stmts;
  vector<SgNode*>::iterator it= stmts.begin();
  int pos=0;
  int row=0;
  if (file) {
    if (sourceFile==true && file==fileB && dll==false) {
      FindNodeVisitor visStat2;
      AstQueryNamespace::querySubTree(file, std::bind2nd( visStat2, &stmts ));
    } else {
      FindAsmStatementsHeaderVisitor visStat2;
      AstQueryNamespace::querySubTree(file, std::bind2nd( visStat2, &stmts ));
    }
    it= stmts.begin();
    for (;it!=stmts.end();++it) {
      Item* item = NULL;
      int length=1;
      if (isSgAsmFunction(*it)){
	SgAsmFunction* func = isSgAsmFunction(*it);
	item = new Item(func->get_address(),func,2,row,length,0,pos,"",0);
      }    else if (isSgAsmBlock(*it)) {
	continue;
      } else if (isSgAsmInstruction(*it)) {
	SgAsmInstruction* inst = isSgAsmInstruction(*it);
	length = isSgAsmInstruction(*it)->get_raw_bytes().size();
	item = new Item(inst->get_address(),inst,0,row,length,length,pos,inst->get_comment(),0);
	item->bg=QColor(255,255,255);
      } else if (isSgFunctionDeclaration(*it)) {
	int color=4;
	SgFunctionDeclaration* func = isSgFunctionDeclaration(*it);
	SgFunctionDefinition* def = isSgFunctionDefinition(isSgFunctionDeclaration(*it)->get_definition());
	if (def)
	  color=2;
	if (isSgFunctionDeclaration(*it)->get_file_info()->isCompilerGenerated())
	  color=3;
	item = new Item(0,func,color,row,length,length,pos,"",0);
      } else if (isSgAsmElfSymbol(*it)) {
	SgAsmElfSymbol* sym = isSgAsmElfSymbol(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sym->get_st_size());
	int color=6;
	//	item = new Item(sym->get_st_name(),sym,color,row,length,sym->get_st_size(),pos,nam,0);
	item = new Item(0,sym,color,row,length,sym->get_st_size(),pos,nam,0);
      }  else if (isSgAsmElfSection(*it)) {
	SgAsmElfSection* sec = isSgAsmElfSection(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_size());
	item = new Item(sec->get_mapped_preferred_rva(),sec,2,row,length,sec->get_size(),pos,nam,0);
      } else if (isSgAsmElfSectionTableEntry(*it)) {
	SgAsmElfSectionTableEntry* sec = isSgAsmElfSectionTableEntry(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_sh_size());
	int color=0;
	item = new Item(sec->get_sh_addr(),sec,color,row,length,sec->get_sh_size(),pos,nam,0);
      } else if (isSgAsmElfSegmentTableEntry(*it)) {
	SgAsmElfSegmentTableEntry* sec = isSgAsmElfSegmentTableEntry(*it);
	std::string nam = "size: " + RoseBin_support::ToString(sec->get_offset());
	int color=0;
	item = new Item(sec->get_vaddr(),sec,color,row,length,sec->get_offset(),pos,nam,0);
      } else if (isSgLocatedNode(*it)) {
	Sg_File_Info* fi = isSgLocatedNode(*it)->get_file_info();
	int line = -1;
	if (fi) {
	  line = fi->get_line();
	  length = 1;
	}
	//      cerr << fi << " creating statement : " << isSgLocatedNode(*it)->class_name() << " ... comment " << line << endl;
	item = new Item(line,isSgLocatedNode(*it),0,row,length,length,pos,
			isSgLocatedNode(*it)->class_name(),line);
      } else {
	//      cerr << "unknown node " << endl;//*it->class_name() << endl;
	//      item = new Item(false,NULL,0,0,row,0,pos, " ",0);
      }
      //example -- color calls red
      if (isSgAsmx86Instruction(*it)) {
	length = isSgAsmInstruction(*it)->get_raw_bytes().size();
	if (isSgAsmx86Instruction(*it)->get_kind() == x86_call) {
	  SgAsmx86Instruction* inst = isSgAsmx86Instruction(*it);
	  SgAsmOperandList * ops = inst->get_operandList();
	  SgAsmExpressionPtrList& opsList = ops->get_operands();
	  std::string addrDest="";
	  SgAsmExpressionPtrList::iterator itOP = opsList.begin();
	  for (;itOP!=opsList.end();++itOP) {
	    addrDest += unparseExpression(*itOP) ;
	  }
	  string s="<...>";
	  s+=inst->get_comment();
	  for (unsigned int j=0; j<funcsFile.size();++j) {
	    SgAsmFunction* f = isSgAsmFunction(funcsFile[j]);
	    string addrF= RoseBin_support::HexToString(f->get_address());
	    addrF="0x"+addrF.substr(1,addrF.size());
	    if (addrF==addrDest) {
	      s="<"+f->get_name()+">";
	      break;
	    }
	  }
	  delete item;
	  item = new Item(inst->get_address(),inst,3,row,length,length,pos,s,0);
	  item->bg=QColor(255,255,255);
	}
      }
      if (item) {
	itemsFile.push_back(item);
	if (*it!=NULL)
	  itemsNodeA[*it]=row;
	pos+=length;
	row++;
      }
    }
  }
}

void 
BinQAbstract::createFunction(SgNode* file,std::vector<SgNode*>& funcsFile, bool dll) {
  // ---------------------- Create itemsFileA and itemsFileB , containing all statements
  FindAsmFunctionsVisitor funcVis;
  if (file) {
    if (sourceFile==true && file==fileB && dll==false) {
      FindSgFunctionsVisitor funcVisSource;
      AstQueryNamespace::querySubTree(file, std::bind2nd( funcVisSource, &funcsFile ));
    } else {
      AstQueryNamespace::querySubTree(file, std::bind2nd( funcVis, &funcsFile ));
    }
  }
}
