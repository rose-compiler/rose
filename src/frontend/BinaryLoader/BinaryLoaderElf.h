#ifndef ROSE_BINARYLOADERELF_H
#define ROSE_BINARYLOADERELF_H

#include <stdint.h>
#include <vector>
#include <map>
#include <set>



class BinaryLoaderElf : public BinaryLoaderGeneric
{
  /**
   */
  virtual bool relocateAllLibraries(SgBinaryComposite* binaryFile);

 protected:
  virtual void addSectionsForLayout(SgAsmGenericHeader* header, 
				    SgAsmGenericSectionPtrList &allSections);

  virtual Rose_STL_Container<std::string> getDLLs(SgAsmGenericHeader* header,
						  const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded);
    
  //virtual void handleSectionMapped(SgAsmGenericSection* section);
};

#endif // BINARYLOADERELF_H
