#include "rose.h"
#include "BinQGui.h"
#include "BinDynamicInfo.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string DynamicInfo::name() {
  return "Binary Dynamic Info";
}

std::string DynamicInfo::getDescription() {
  return "Test to determine more information about dynamic libraries.";
}


void
DynamicInfo::addCommentsForIDAFiles(SgAsmBlock* fileA) {
  if (fileA==NULL)
    return;
  this->traverse(fileA,preorder);
}

void
DynamicInfo::visit(SgNode* node) {
  if (!project) {
    // IDA FILE
    SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
    if (inst && inst->get_kind() == x86_call ) {
      SgAsmOperandList * ops = inst->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      SgAsmExpressionPtrList::const_iterator itOP = opsList.begin();
      string comment="";
#if 1
      for (;itOP!=opsList.end();++itOP) {
	SgAsmExpression* exp = *itOP;
	if (exp->get_replacement()!="")
	  comment += "BD:"+exp->get_replacement();
      }

      if (comment!="") {
	if (comment[0]=='_')
	  comment = comment.substr(1,comment.size());
      }
      //      inst->set_comment(comment);
#endif
    }
  }
  else {
  if (firstIteration) 
    //    if (isSgAsmElfRelaEntryList(node)) {
    if (isSgAsmElfRelocEntryList(node)) {
      // try to find symbols
      SgAsmElfRelocEntryList* entry = isSgAsmElfRelocEntryList(node);
      Rose_STL_Container<SgAsmElfRelocEntry*> entries = entry->get_entries();
      if (debug)
	cerr << ">>>>>>>>>>>>>>>> Found SgAsmElfRelocEntryList: " << entries.size() << endl;
      if (instance) {
	QString res = QString(">>>>>>>>>>>>>>>> Found SgAsmElfRelocEntryList: %1")
	  .arg(entries.size());
	instance->analysisResult->append(res);  
      }
      Rose_STL_Container<SgAsmElfRelocEntry*>::const_iterator it = entries.begin();
      for (;it!=entries.end();++it) {
	SgAsmElfRelocEntry* en = *it;
	rose_addr_t offset = en->get_r_offset();
	unsigned long symbol = en->get_sym();

	string sym_name="unknown";
	SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
	for (unsigned int i=0;i<sectionsList.size();++i) {
	  SgAsmGenericSection* h = sectionsList[i];
	  SgAsmElfSymbolSection* symSec = isSgAsmElfSymbolSection(h);
	  if (symSec) {
	    string name = "unknown";
	    if (symSec->get_name()!=NULL)
	      name=symSec->get_name()->get_string();
	    if (name==".dynsym") {
	      Rose_STL_Container<SgAsmElfSymbol*> list = symSec->get_symbols()->get_symbols();
	      SgAsmElfSymbol* r_symbol = list[symbol];
	      if (r_symbol) {
		symbolMap[offset]=r_symbol;
		sym_name = r_symbol->get_name()->get_string();	  
	      }
	    }
	  }
	}

	string output =  "Entry : ";
	output+= RoseBin_support::HexToString(offset);
	output+="   sym: ";
	output+=symbol;
	output+=": ";
	output+= sym_name;
	if (debug)
	  cerr << output << endl;
	if (instance) {
	  QString res = QString("%1")
	    .arg(output.c_str());
	  instance->analysisResult->append(res);  
	}

      }
    }

  if (!firstIteration)
    // try to resolve call destinations with symbols
    if (isSgAsmx86Instruction(node) && isSgAsmx86Instruction(node)->get_kind() == x86_call) {
      SgAsmx86Instruction* inst = isSgAsmx86Instruction(node);
      SgAsmOperandList * ops = inst->get_operandList();
      SgAsmExpressionPtrList& opsList = ops->get_operands();
      rose_addr_t addrDest=0;
      SgAsmExpressionPtrList::iterator itOP = opsList.begin();
      for (;itOP!=opsList.end();++itOP) {
	SgAsmExpression* exp = *itOP;
	string address = unparseExpression(exp);
	addrDest = RoseBin_support::HexToDec(address);
      }
      // get the target
      if (addrDest!=0) {
	if (debug)
	  cerr << "Found call to : " << RoseBin_support::HexToString(addrDest) << endl;
	// for a given address get the instruction

	SgAsmx86Instruction* destInst = isSgAsmx86Instruction(info->getInstructionAtAddress(addrDest));
	if (destInst && isSgAsmx86Instruction(destInst)->get_kind() == x86_jmp) {
	  // check the operands and resolve the destination
	  SgAsmOperandList * ops2 = destInst->get_operandList();
	  SgAsmExpressionPtrList& opsList2 = ops2->get_operands();
	  SgAsmExpressionPtrList::iterator it = opsList2.begin();
	  SgAsmMemoryReferenceExpression* mem = isSgAsmMemoryReferenceExpression(*it);
	  rose_addr_t resolveAddr=0;
	  if (mem) {
	    resolveAddr=BinQSupport::evaluateMemoryExpression(destInst,mem);
	  }
	  if (debug)
	    cerr << "  Destination : " << unparseInstruction(destInst) <<endl;
	  // determine if we found a symbol
	  std::map<rose_addr_t, SgAsmElfSymbol*>::const_iterator si = symbolMap.find(resolveAddr);
	  SgAsmElfSymbol* symbol = NULL;
	  if (si!=symbolMap.end())
	    symbol=si->second;
	  string sym_name="unknown";
	  if (symbol && symbol->get_name()) {
	    sym_name = symbol->get_name()->get_string();	  
	  }
	  if (instance) {
	    QString res = QString("Found Call from : %1  to  %2  --  resolved addr : %3 -- name : %4")
	      .arg(RoseBin_support::HexToString(inst->get_address()).c_str())
	      .arg(RoseBin_support::HexToString(addrDest).c_str())
	      .arg( RoseBin_support::HexToString(resolveAddr).c_str())
	      .arg(sym_name.c_str());
	    instance->analysisResult->append(res);  
	  }
	  
	  // apply resolved name to GUI (comment field)
	  if(instance) {
	    int row =0;
	    std::map<SgNode*,int>::const_iterator rowIt = instance->itemsNodeA.find(inst);
	    if (rowIt!=instance->itemsNodeA.end())
	      row=rowIt->second;
	    instance->codeTableWidget->setText(sym_name, 4, row);
	  }
	  //cerr << ">>> set comment : " << sym_name << endl;
	  if (inst->get_comment()=="")
	    inst->set_comment(sym_name);
	}
      }

    }
  }
}

void
DynamicInfo::printOutRelaEntries(SgNode* project) {
  this->traverse(project,preorder);
}

void
DynamicInfo::run(SgNode* fileA, SgNode* fileB) {
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();

  if (isSgProject(fileA)==NULL && isSgAsmBlock(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    if (!testFlag) {
      QString res = QString("This is not a valid file for this analysis");
      instance->analysisResult->append(res);  
    }
    return;
  }

  project=false;
  SgAsmGenericFile* file = NULL;
  if (isSgProject(fileA)) {
    project=true;
    SgBinaryComposite* binary = isSgBinaryComposite(isSgProject(fileA)->get_fileList()[0]);
    file = binary != NULL ? binary->get_binaryFile() : NULL;
    ROSE_ASSERT(file);
    info = new VirtualBinCFG::AuxiliaryInformation(file);
  }
  else
    info = new VirtualBinCFG::AuxiliaryInformation(fileA);

  // call graph analysis  *******************************************************
  if (debug)
    cerr << " Running dynamic info ... " << endl;
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    if (isSgProject(fileA)) {
      QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
      instance->analysisResult->append(res);  
    }
  }

  if (!isSgProject(fileA)) {
    // IDA FILE
    // run here and add comment for call instructions
    addCommentsForIDAFiles(isSgAsmBlock(fileA));
    if (instance) {
      QString res = QString("\n>>>>>>>>>>>>>>>> Resolving call addresses to names ...");
      instance->analysisResult->append(res);  
    }
    return;
  }

  genericF = file;
  symbolMap.clear();
  firstIteration=true;
  printOutRelaEntries(isSgProject(fileA));
  firstIteration=false;
  

  SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
  for (unsigned int i=0;i<sectionsList.size();++i) {
    SgAsmGenericSection* h = sectionsList[i];
    SgAsmElfSymbolSection* symSec = isSgAsmElfSymbolSection(h);
    if (symSec) {
      string name = "unknown";
      if (symSec->get_name()!=NULL)
	name=symSec->get_name()->get_string();
      if (!testFlag)
	instance->analysisResult->append( QString("\nFound Symbol Section: %1 ---------------------------")
					  .arg(name.c_str()));
      Rose_STL_Container<SgAsmElfSymbol*> list = symSec->get_symbols()->get_symbols();
      Rose_STL_Container<SgAsmElfSymbol*>::const_iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgAsmElfSymbol* entry = *it;
	//	rose_addr_t  addr = entry->get_st_name();
	rose_addr_t  addr = 0;
	string name = entry->get_name()->get_string();
	string type = RoseBin_support::ToString(entry->get_elf_type());	
	if (entry->get_elf_type()==2)
	  type="function";
	string addrS =  RoseBin_support::ToString(addr);
	if (!testFlag)
	  instance->analysisResult->append(QString(" Addr: %1 Found Symbol : %2       Type : %3")
                                           .arg(addrS.c_str())
					   .arg(name.c_str())
					   .arg(type.c_str()));
      }
    }

    SgAsmElfDynamicSection* elfSec = isSgAsmElfDynamicSection(h);
    if (elfSec) {
      if (!testFlag)
	instance->analysisResult->append( QString("\nFound Dynamic section. ---------------------------"));
      Rose_STL_Container<SgAsmElfDynamicEntry*> list = elfSec->get_entries()->get_entries();
      Rose_STL_Container<SgAsmElfDynamicEntry*>::const_iterator it = list.begin();
      SgAsmElfDynamicEntry* entry=NULL;
      for (;it!=list.end();++it) {
	entry = *it;
	ROSE_ASSERT(entry);
	string name="unknown";
	if (entry->get_name()!=NULL)
	  name = entry->get_name()->get_string();
	int nr = entry->get_d_tag();	
	rose_rva_t address = entry->get_d_val();
	if (nr==1) {
	  Rose_STL_Container<unsigned char> chars = entry->get_extra();
	  if (!testFlag)
	    instance->analysisResult->append(QString("  Found DL_NEEDED: %1    -- Contains %2 chars.   loc: %3")
                                             .arg(QString(name.c_str()))
					     .arg(chars.size())
					     .arg(RoseBin_support::ToString(address).c_str()));


	}
      }

    } else {
      //instance->analysisResult->append( QString("%1").arg(h->class_name().c_str()));
    }
  }

  if (instance) {
    QString res = QString("\n>>>>>>>>>>>>>>>> Resolving call addresses to names ...");
    instance->analysisResult->append(res);  
  }
  // here we go through the call instructions and resolve the calls
  printOutRelaEntries(isSgProject(fileA));
}



void
DynamicInfo::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
