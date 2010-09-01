#ifndef ROSE_BINARYLOADERELF_H
#define ROSE_BINARYLOADERELF_H

#include "BinaryLoader.h"

class BinaryLoaderElf: public BinaryLoader {
public:
    BinaryLoaderElf() {}

    BinaryLoaderElf(const BinaryLoaderElf &other)
        : BinaryLoader(other)
        {}

    virtual ~BinaryLoaderElf() {}
    
    virtual BinaryLoaderElf *clone() const {
        return new BinaryLoaderElf(*this);
    }
    
    virtual bool can_load(SgAsmGenericHeader*) const;

    virtual void fixupSections(SgAsmInterpretation *interp);

protected:
    virtual void addSectionsForLayout(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections);

#if 0
    /* FIXME: replaced by dependencies() */
    virtual Rose_STL_Container<std::string> getDLLs(SgAsmGenericHeader* header,
                                                    const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded);
#endif

};

#endif /*ROSE_BINARYLOADERELF_H*/
