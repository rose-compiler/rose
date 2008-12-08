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
DynamicInfo::visit(SgNode* node) {
  if (isSgAsmElfRelaEntryList(node)) {
    SgAsmElfRelaEntryList* entry = isSgAsmElfRelaEntryList(node);
    Rose_STL_Container<SgAsmElfRelaEntry*> entries = entry->get_entries();
    cerr << ">>>>>>>>>>>>>>>> Found SgAsmElfRelaEntryList: " << entries.size() << endl;
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
	    sym_name = r_symbol->get_name()->get_string();	  
	  }
	}
      }

      cerr << "Entry : " << RoseBin_support::HexToString(offset) << "   sym: " << symbol << "   name: " << sym_name<<endl;

    }
  }
}

void
DynamicInfo::printOutRelaEntries(SgNode* project) {
  this->traverse(project,preorder);
}

void
DynamicInfo::run(SgNode* fileA, SgNode* fileB) {
  BinQGUI *instance =NULL;
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

  //  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  cerr << " Running dynamic info ... " << endl;
  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    QString res = QString("Looking at dynamic information : %1").arg(file->get_name().c_str());
    instance->analysisResult->append(res);  
  }


  genericF = file->get_genericFile() ;
  printOutRelaEntries(isSgProject(fileA));

  

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

}



void
DynamicInfo::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
