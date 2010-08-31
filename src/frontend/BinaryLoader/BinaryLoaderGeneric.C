// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#include "Disassembler.h"
#include "binaryLoader.h"

#include "BinaryLoaderGeneric.h" // TODO remove

bool
BinaryLoaderGeneric::load(SgBinaryComposite* binaryFile, SgAsmGenericFile* executableAsmFile)
{
    ROSE_ASSERT(binaryFile->get_genericFileList()->get_files().empty());
   
    if (executableAsmFile == NULL) {
        executableAsmFile = createAsmAST(binaryFile, binaryFile->get_sourceFileNameWithPath());
    }
    ROSE_ASSERT(executableAsmFile != NULL);
  
    if (get_perform_dynamic_linking())
        loadAllLibraries(binaryFile);
    if (get_perform_layout())
        layoutAllLibraries(binaryFile);
    if (get_perform_relocations())
        relocateAllLibraries(binaryFile);
    if (get_perform_disassembly())
        disassembleAllLibraries(binaryFile);
    return true;
}

bool
BinaryLoaderGeneric::loadAllLibraries(SgBinaryComposite* binaryFile)
{
    ROSE_ASSERT(binaryFile != NULL);
    SgAsmInterpretationPtrList &interps = binaryFile->get_interpretations()->get_interpretations();
    for (size_t i=0; i < interps.size();++i) {
        loadInterpLibraries(binaryFile, interps[i]);
    }
    return true;
}  

bool
BinaryLoaderGeneric::isHeaderSimilar(const SgAsmGenericHeader* matchHeader, const SgAsmGenericHeader* candidateHeader)
{
    return (matchHeader->variantT() == candidateHeader->variantT());
}

SgAsmGenericHeaderPtrList
BinaryLoaderGeneric::findSimilarHeaders(const SgAsmGenericHeader* matchHeader, const SgAsmGenericHeaderPtrList& candidateHeaders)
{
    SgAsmGenericHeaderPtrList matches;
    for (size_t i=0; i<candidateHeaders.size(); ++i) {
        SgAsmGenericHeader* candidate = candidateHeaders[i]; 
        if (isHeaderSimilar(matchHeader, candidate))
            matches.push_back(candidate);
    }
    return matches;
}

SgAsmGenericFile* 
BinaryLoaderGeneric::createAsmAST(SgBinaryComposite* binaryFile, std::string filePath)
{
    if (get_verbose(binaryFile) > 0)
        printf("Loading Binary %s \n", filePath.c_str());

    ROSE_ASSERT(!filePath.empty());
  
    SgAsmGenericFile* file = SgAsmExecutableFileFormat::parseBinaryFormat(filePath.c_str());
    ROSE_ASSERT(file != NULL);
  
    // TODO do I need to attach here - or can I do after return
    binaryFile->get_genericFileList()->get_files().push_back(file);
    file->set_parent(binaryFile);

    /* Add a new disassembly interpretation to the SgBinaryComposite object for each header of the newly parsed
     * SgAsmGenericFile for which a suitable interpretation does not already exist. */
    const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
    SgAsmInterpretationPtrList &interps = binaryFile->get_interpretations()->get_interpretations();
    for (size_t i = 0; i < headers.size(); ++i) {
        SgAsmGenericHeader* header = headers[i];
        SgAsmInterpretation* interp = NULL;
        for (size_t j = 0; j < interps.size(); ++j) {
            ROSE_ASSERT(!interps[j]->get_headers()->get_headers().empty());
            SgAsmGenericHeader* interpHeader = interps[j]->get_headers()->get_headers().front();
            if (isHeaderSimilar(header,interpHeader)) {
                interp = interps[j];
                break;
            }
        }
        if (!interp) {
            interp = new SgAsmInterpretation();
            interps.push_back(interp);
            interp->set_parent(binaryFile);
        }
        interp->get_headers()->get_headers().push_back(header);
    }

#if USE_ROSE_DWARF_SUPPORT
    /* Parse Dwarf info and add it to the SgAsmGenericFile. */
    readDwarf(file); 
#endif
  
  return file;
}

bool
BinaryLoaderGeneric::loadInterpLibraries(SgBinaryComposite* binaryFile, SgAsmInterpretation* interp)
{
    ROSE_ASSERT(binaryFile != NULL);
    ROSE_ASSERT(interp != NULL);
  
    /* Bootstrap DLL list - these are library names, not file paths. On *NIX machines these are called shared libraries (.so). */
    Rose_STL_Container<std::string> filesAlreadyLoaded;
    std::list<SgAsmGenericHeader*> unresolvedHeaders;
    unresolvedHeaders.insert(unresolvedHeaders.end(),
                             interp->get_headers()->get_headers().begin(),
                             interp->get_headers()->get_headers().end());

  
    for (/*void*/; !unresolvedHeaders.empty(); unresolvedHeaders.pop_front()) {
        SgAsmGenericHeader* header = unresolvedHeaders.front();
        Rose_STL_Container<std::string> filesToLoad = getDLLs(header, filesAlreadyLoaded);
        for(size_t i=0; i<filesToLoad.size(); ++i){
            std::string dllPath = filesToLoad[i];
            if (get_verbose(binaryFile) > 0) {
                printf("Loading: %s\n", dllPath.c_str());
            }
            SgAsmGenericFile* newFile = createAsmAST(binaryFile,dllPath);
            ROSE_ASSERT(newFile != NULL); // TODO more user friendly fail notification
            filesAlreadyLoaded.push_back(dllPath); /* Mark loaded by dllPath */
            
            SgAsmGenericHeaderPtrList newHeaders = findSimilarHeaders(header, newFile->get_headers()->get_headers());
            unresolvedHeaders.insert(unresolvedHeaders.end(), newHeaders.begin(), newHeaders.end());
        }
    }

    return true;
}

bool
BinaryLoaderGeneric::layoutAllLibraries(SgBinaryComposite* binaryFile)
{
    SgAsmInterpretationPtrList& interps = binaryFile->get_interpretations()->get_interpretations();
    for(size_t i=0; i < interps.size(); ++i){
        layoutInterpLibraries(binaryFile,interps[i]);
    }
    return true;
}

bool
BinaryLoaderGeneric::layoutInterpLibraries(SgBinaryComposite*, SgAsmInterpretation* interp)
{
    SgAsmGenericSectionPtrList allSections;
    Loader* loader = Loader::find_loader(interp->get_headers()->get_headers().front());
    for(size_t i=0; i < interp->get_headers()->get_headers().size(); ++i){
        SgAsmGenericHeader* header = interp->get_headers()->get_headers()[i];
        addSectionsForLayout(header, allSections);		       
    }

    //FIXME
    //MCB: Note - this will likely mess up actual dynamic linking, but was necessary
    //     to get regressions to pass - a more nuanced solution is required
    //     (failed for fnord.ppc, testConstants)
    //MemoryMap* memMap = loader->map_all_sections(interp->get_map(),allSections);
    MemoryMap* memMap = loader->map_code_sections(interp->get_map(), allSections);
    interp->set_map(memMap);
    return true;
}

void
BinaryLoaderGeneric::addSectionsForLayout(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections)
{
    allSections.insert(allSections.end(),
                       header->get_sections()->get_sections().begin(),
		       header->get_sections()->get_sections().end());
}

bool
BinaryLoaderGeneric::relocateAllLibraries(SgBinaryComposite* binaryFile)
{
    // 1. Get section map (name -> list<section*>)
    // 2. Create Symbol map from relevant sections (.dynsym)
    // 3. Create Extent sorted list of sections
    // 4. Collect Relocation Entries.
    // 5.  for each relocation entry, perform relocation
    return false;
}

size_t wordSizeOfFile(SgAsmGenericFile* file)
{
    size_t maxWordSize=0;
    SgAsmGenericHeaderPtrList &headers= file->get_headers()->get_headers();
    for(SgAsmGenericHeaderPtrList::iterator headerIter=headers.begin(); headerIter != headers.end(); ++headerIter)
        maxWordSize = std::max(maxWordSize, (*headerIter)->get_word_size());
    return maxWordSize;
}

Rose_STL_Container<std::string>
BinaryLoaderGeneric::getDLLs(SgAsmGenericHeader* header, const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded)
{
    Rose_STL_Container<std::string> files;
    ROSE_ASSERT(header != NULL);
    std::set<std::string> alreadyLoadedSet(dllFilesAlreadyLoaded.begin(), dllFilesAlreadyLoaded.end());

    const SgAsmGenericDLLPtrList &dlls = header->get_dlls();
    for (size_t j=0; j<dlls.size(); ++j) {
        std::string file = dlls[j]->get_name()->c_str();
        if (alreadyLoadedSet.find(file) == alreadyLoadedSet.end())
            files.push_back(file);
    }
    return files;
}

bool
BinaryLoaderGeneric::disassembleAllLibraries(SgBinaryComposite* binaryFile)
{
    SgAsmInterpretationPtrList& interps = binaryFile->get_interpretations()->get_interpretations();
    for (size_t i=0; i<interps.size(); ++i) {
        SgAsmInterpretation* interp = interps[i];
        Disassembler::disassembleInterpretation(interp);
    }
    return true;
}
