#ifndef ROSE_BINARYLOADERELF_H
#define ROSE_BINARYLOADERELF_H

class BinaryLoaderElf : public BinaryLoaderGeneric {
    virtual bool relocateAllLibraries(SgBinaryComposite* binaryFile);

protected:
    virtual void addSectionsForLayout(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections);
    virtual Rose_STL_Container<std::string> getDLLs(SgAsmGenericHeader* header,
                                                    const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded);
};

#endif /*ROSE_BINARYLOADERELF_H*/
