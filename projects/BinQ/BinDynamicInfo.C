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

std::string
DynamicInfo::resolveValue(SgAsmValueExpression* leftVal) {
  string valStr="";
  uint8_t byte_val=0xF;
  uint16_t word_val=0xFF;
  uint32_t double_word_val=0xFFFF;
  uint64_t quad_word_val=0xFFFFFFFFU;
  valStr = 
    RoseBin_support::resolveValue(leftVal, true,
				  byte_val,
				  word_val,
				  double_word_val,
				  quad_word_val);
  return valStr;
}

void
DynamicInfo::visit(SgNode* node) {
  if (firstIteration) 
    if (isSgAsmElfRelaEntryList(node)) {
      // try to find symbols
      SgAsmElfRelaEntryList* entry = isSgAsmElfRelaEntryList(node);
      Rose_STL_Container<SgAsmElfRelaEntry*> entries = entry->get_entries();
      if (debug)
	cerr << ">>>>>>>>>>>>>>>> Found SgAsmElfRelaEntryList: " << entries.size() << endl;
      if (instance) {
	QString res = QString(">>>>>>>>>>>>>>>> Found SgAsmElfRelaEntryList: %1")
	  .arg(entries.size());
	instance->analysisResult->append(res);  
      }
      Rose_STL_Container<SgAsmElfRelaEntry*>::const_iterator it = entries.begin();
      for (;it!=entries.end();++it) {
	SgAsmElfRelaEntry* en = *it;
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
	string address = unparseX86Expression(exp,false);
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
	  string adr="";
	  rose_addr_t resolveAddr=0;
	  if (mem) {
	    SgAsmExpression* exprOffset = mem->get_address();
	    SgAsmExpression* left =NULL;
	    SgAsmExpression* right =NULL;
	    SgAsmBinaryAdd* add = isSgAsmBinaryAdd(exprOffset);
	    SgAsmValueExpression* Val = isSgAsmValueExpression(exprOffset);
	    if (add) {
	      left = add->get_lhs();
	      right = add->get_rhs();
	    } else if (Val) {
	      left=Val;
	    }
	    if (left || right) {
	      SgAsmx86RegisterReferenceExpression* leftReg = isSgAsmx86RegisterReferenceExpression(left);
	      SgAsmx86RegisterReferenceExpression* rightReg = isSgAsmx86RegisterReferenceExpression(right);
	      SgAsmValueExpression* leftVal = isSgAsmValueExpression(left);
	      SgAsmValueExpression* rightVal = isSgAsmValueExpression(right);
	      X86RegisterClass regClass ;
	      if (leftReg) 
		regClass = leftReg->get_register_class();
	      if (rightReg) 
		regClass = rightReg->get_register_class();
	      //cerr << " print : " << regClass << endl;
	      string val = "NULL";
	      if (regClass>=0 && regClass <=10)
		val = regclassToString(regClass);
	      uint64_t next_addr = destInst->get_address() + destInst->get_raw_bytes().size();
	      if (val=="ip") 
		resolveAddr+=next_addr;
	      if (leftVal) {
		string valStr = resolveValue(leftVal);
		resolveAddr += RoseBin_support::HexToDec(valStr);
	      }
	      if (rightVal) {
		string valStr = resolveValue(rightVal);
		resolveAddr += RoseBin_support::HexToDec(valStr);
	      }
	      
	    }
	    //SgAsmType* type = mem->get_type();
	    adr+="resolved: " +RoseBin_support::HexToString(resolveAddr)+" - orig:";
	    adr += unparseX86Expression(exprOffset,false);
	  }
	  if (debug)
	    cerr << "  Destination : " << unparseInstruction(destInst) << "  ops: " <<adr <<endl;
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
	  inst->set_comment(sym_name);
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
  if (true)
    return;
  instance=NULL;
  if (!testFlag)
    instance = QROSE::cbData<BinQGUI *>();

  if (isSgProject(fileA)==NULL) {
    cerr << "This is not a valid file for this analysis!" << endl;
    QString res = QString("This is not a valid file for this analysis");
    instance->analysisResult->append(res);  
    return;
  }

  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);
  info = new VirtualBinCFG::AuxiliaryInformation(file);

  //  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  if (debug)
    cerr << " Running dynamic info ... " << endl;
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
    instance->analysisResult->append(res);  
  }


  genericF = file->get_genericFile() ;
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
	rose_addr_t  addr = entry->get_st_name();
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
      for (;it!=list.end();++it) {
	SgAsmElfDynamicEntry* entry = *it;
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
