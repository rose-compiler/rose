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
DynamicInfo::run() {
  BinQGUI *instance = QROSE::cbData<BinQGUI *>();

  ROSE_ASSERT(isSgProject(instance->fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(instance->fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  //  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  cerr << " Running dynamic info ... " << endl;
  ROSE_ASSERT(instance);
  ROSE_ASSERT(instance->analysisTab);
  instance->analysisTab->setCurrentIndex(1);
  QString res = QString("Looking at dynamic information ");
  instance->analysisResult->append(res);  
  
  SgAsmGenericFile *genericF = file->get_genericFile() ;

  SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
  for (unsigned int i=0;i<sectionsList.size();++i) {
    SgAsmGenericSection* h = sectionsList[i];
    SgAsmElfSymbolSection* symSec = isSgAsmElfSymbolSection(h);
    if (symSec) {
      instance->analysisResult->append( QString("\nFound Symbol Section. ---------------------------"));
      Rose_STL_Container<SgAsmElfSymbol*> list = symSec->get_symbols()->get_symbols();
      Rose_STL_Container<SgAsmElfSymbol*>::const_iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgAsmElfSymbol* entry = *it;
	string name = entry->get_name()->get_string();
	instance->analysisResult->append(QString("  Found Symbol : %1").arg(name.c_str()));
      }
    }

    SgAsmElfDynamicSection* elfSec = isSgAsmElfDynamicSection(h);
    if (elfSec) {
      instance->analysisResult->append( QString("\nFound Dynamic section. ---------------------------"));
      Rose_STL_Container<SgAsmElfDynamicEntry*> list = elfSec->get_entries()->get_entries();
      Rose_STL_Container<SgAsmElfDynamicEntry*>::const_iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgAsmElfDynamicEntry* entry = *it;
	int nr = entry->get_d_tag();	
	rose_rva_t address = entry->get_d_val();
	//	SgAsmElfDynamicEntry::EntryType en = entry->get_d_tag();
	/*
	instance->analysisResult->append(QString("Found an entry %1: %2     loc: %3")
					 .arg(nr)
					 .arg(entry->stringify_tag(en))
					 .arg(RoseBin_support::ToString(address).c_str()));
	*/
	if (nr==1) {
	  Rose_STL_Container<unsigned char> chars = entry->get_extra();
	  instance->analysisResult->append(QString("  Found DL_NEEDED.  Contains %1 chars.   loc: %2").
					   arg(chars.size())
					   .arg(RoseBin_support::ToString(address).c_str()));

#if 0

	  Rose_STL_Container<unsigned char>::const_iterator itC = chars.begin();
	  for (;itC!=chars.end();++itC) {
	    unsigned char c = *itC;
	    cerr << "Found chars : " << c << endl;
	  }	  
#endif

	}
      }

    } else {
      //instance->analysisResult->append( QString("%1").arg(h->class_name().c_str()));
    }
  }

}






void
DynamicInfo::test(SgNode* fileA, SgNode* fileB) {
  ROSE_ASSERT(isSgProject(fileA));
  SgBinaryFile* binaryFile = isSgBinaryFile(isSgProject(fileA)->get_fileList()[0]);
  SgAsmFile* file = binaryFile != NULL ? binaryFile->get_binaryFile() : NULL;
  ROSE_ASSERT(file);

  //  VirtualBinCFG::AuxiliaryInformation* info = new VirtualBinCFG::AuxiliaryInformation(file);

  // call graph analysis  *******************************************************
  cerr << " Running dynamic info ... " << endl;
  SgAsmGenericFile *genericF = file->get_genericFile() ;

  SgAsmGenericSectionPtrList sectionsList = genericF->get_sections();
  for (unsigned int i=0;i<sectionsList.size();++i) {
    SgAsmGenericSection* h = sectionsList[i];
    SgAsmElfSymbolSection* symSec = isSgAsmElfSymbolSection(h);
    if (symSec) {

      Rose_STL_Container<SgAsmElfSymbol*> list = symSec->get_symbols()->get_symbols();
      Rose_STL_Container<SgAsmElfSymbol*>::const_iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgAsmElfSymbol* entry = *it;
	string name = entry->get_name()->get_string();
	
      }
    }

    SgAsmElfDynamicSection* elfSec = isSgAsmElfDynamicSection(h);
    if (elfSec) {
      
      Rose_STL_Container<SgAsmElfDynamicEntry*> list = elfSec->get_entries()->get_entries();
      Rose_STL_Container<SgAsmElfDynamicEntry*>::const_iterator it = list.begin();
      for (;it!=list.end();++it) {
	SgAsmElfDynamicEntry* entry = *it;
	int nr = entry->get_d_tag();	
	rose_rva_t address = entry->get_d_val();
	//	SgAsmElfDynamicEntry::EntryType en = entry->get_d_tag();
	/*
	instance->analysisResult->append(QString("Found an entry %1: %2     loc: %3")
					 .arg(nr)
					 .arg(entry->stringify_tag(en))
					 .arg(RoseBin_support::ToString(address).c_str()));
	*/
	if (nr==1) {
	  Rose_STL_Container<unsigned char> chars = entry->get_extra();
	  
#if 0

	  Rose_STL_Container<unsigned char>::const_iterator itC = chars.begin();
	  for (;itC!=chars.end();++itC) {
	    unsigned char c = *itC;
	    cerr << "Found chars : " << c << endl;
	  }	  
#endif

	}
      }

    } else {
      //instance->analysisResult->append( QString("%1").arg(h->class_name().c_str()));
    }
  }

}
