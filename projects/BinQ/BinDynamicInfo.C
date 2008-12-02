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
    if (h->get_name()->get_string()!=".dynamic")
      continue;
    SgAsmElfSection* elfSec = isSgAsmElfSection(h);
    if (elfSec) {
      SgAsmElfSectionTableEntry* entry= elfSec->get_section_entry();
      if (entry) {
	rose_addr_t addr = entry->get_sh_addr();
	rose_addr_t size = entry->get_sh_size();
	rose_addr_t offset = entry->get_sh_offset();
	string addrS = RoseBin_support::HexToString(addr);
	string sizeS = RoseBin_support::HexToString(size);
	string offsetS = RoseBin_support::HexToString(offset);
	instance->analysisResult->append( QString("%1           type:     %2 Addr: %3  Size: %4   Offset: %5")
			  .arg(QString(h->get_name()->get_string().c_str()))
			  .arg(h->class_name().c_str())
			  .arg(addrS.c_str())
			  .arg(sizeS.c_str())
			  .arg(offsetS.c_str()));	 
      }
    } else {
      instance->analysisResult->append( QString("%1")
			.arg(h->class_name().c_str()));
    }
  }

}
