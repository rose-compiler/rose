#include "BinQGui.h"
#include "DwarfFileInfo.h"

#include <iostream>
#include "BinQSupport.h"
#include "slide.h"
#include <qtabwidget.h>


using namespace qrs;
using namespace std;
using namespace __gnu_cxx;


std::string DwarfFileInfo::name() {
  return "Dwarf File Info";
}

std::string DwarfFileInfo::getDescription() {
  return "Detects the Files compiled for this binary";
}



void
DwarfFileInfo::run(SgNode* fileA, SgNode* fileB) {
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

  if (!testFlag) {
    ROSE_ASSERT(instance);
    ROSE_ASSERT(instance->analysisTab);
    instance->analysisTab->setCurrentIndex(1);
    QString res = QString("Start : %1").arg(file->get_name().c_str());
    instance->analysisResult->append(res);  
  }
  genericF = file->get_genericFile() ;

#if 0
  std::set <std::string> files;
#if USE_ROSE_DWARF_SUPPORT
  // The input file is the binary file...
  int binary_file_id = isSgProject(fileA)->get_fileList()[0]->get_file_info()->get_file_id();

  std::string binaryFilename = Sg_File_Info::getFilenameFromID(binary_file_id);
  printf ("file_id = %d binaryFilename = %s \n",binary_file_id,binaryFilename.c_str());
  // Compute the binary executable instruction address range
  std::pair<uint64_t,uint64_t> addressRange = SgAsmDwarfLineList::instructionRange();
  printf ("\nBinary instruction address range = (0x%lx, 0x%lx) \n",addressRange.first,addressRange.second);

  uint64_t minInstructionAddress = addressRange.first;
  // Iterate over the addresses of the binary and compute the source code line numbers (limit the range so the output will be short).
  //printf ("\nSource lines computed from address range (truncated to keep output short): \n");
  for (uint64_t address = minInstructionAddress - 1; address < addressRange.second; address++)
    {
      FileIdLineColumnFilePosition s_map = SgAsmDwarfLineList::addressToSourceCode(address);
      //printf ("   addressToSourceCode: address 0x%lx = (%d,%d,%d) \n",address,s_map.first,s_map.second.first,s_map.second.second);
      if (s_map.first>=0) {
	std::string filename = Sg_File_Info::getFilenameFromID(s_map.first);
	files.insert(filename);
      }
    }
#else
  printf ("\n\nROSE must be configured with --with-dwarf=<path to libdwarf> to use Dwarf support. \n\n");
#endif

  set<string>::const_iterator it= files.begin();
  for (;it!=files.end();++it) {
    string filename = *it;
    cerr << "  >>> Filename " << filename << endl;
  }
  //  runTraversal(isSgProject(fileA));

#endif
  if (instance) {
    QString res = QString("\n>>>>>>>>>>>>>>>> Resolving call addresses to names ...");
    instance->analysisResult->append(res);  
  }
}



void
DwarfFileInfo::test(SgNode* fileA, SgNode* fileB) {
  testFlag=true;
  run(fileA,fileB);
  testFlag=false;
}
