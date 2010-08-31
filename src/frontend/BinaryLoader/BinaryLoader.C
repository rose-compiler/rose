#include "sage3basic.h"

#include "BinaryLoader.h"
#include "BinaryLoaderElf.h"
#include "Loader.h"

std::vector<BinaryLoader*> BinaryLoader::loaders;

/* class method */
void
BinaryLoader::initclass()
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        register_subclass(new BinaryLoader);            /* generically handles all formats, albeit with limited functionality */
        register_subclass(new BinaryLoaderElf);
    }
}

/* class method */
void
BinaryLoader::register_subclass(BinaryLoader *loader)
{
    initclass();
    ROSE_ASSERT(loader!=NULL);
    loaders.push_back(loader);
}

/* class method */
BinaryLoader *
BinaryLoader::lookup(SgAsmInterpretation *interp)
{
    BinaryLoader *retval = NULL;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        BinaryLoader *candidate = lookup(headers[i]);
        if (retval && retval!=candidate)
            throw Exception("interpretation has multiple loaders");
        retval = candidate;
    }
    return retval;
}

/* class method */
BinaryLoader *
BinaryLoader::lookup(SgAsmGenericHeader *header)
{
    initclass();
    for (size_t i=loaders.size(); i>0; --i) {
        if (loaders[i-1]->can_load(header))
            return loaders[i-1];
    }
    throw Exception("no loader for architecture");
}

/* class method */
void
BinaryLoader::load(SgBinaryComposite *composite)
{
    /* Parse the initial binary file to create an AST and the initial SgAsmInterpretation(s). */
    ROSE_ASSERT(composite->get_genericFileList()->get_files().empty());
    SgAsmGenericFile *file = createAsmAST(composite, composite->get_sourceFileNameWithPath());
    ROSE_ASSERT(file!=NULL);
    
    /* Find an appropriate loader for each interpretation and parse, map, link, and/or relocate each interpretation as
     * specified by the loader properties. */
    const SgAsmInterpretationPtrList &interps = composite->get_interpretations()->get_interpretations();
    for (size_t i=0; i<interps.size(); i++) {
        BinaryLoader *loader = lookup(interps[i])->clone(); /* clone so we can change properties locally */
        ROSE_ASSERT(loader!=NULL); /* lookup() should have thrown an exception already */
        try {
            loader->load(interps[i]);
        } catch (...) {
            delete loader;
            throw;
        }
    }
}

void
BinaryLoader::load(SgAsmInterpretation *interp)
{
    if (get_perform_dynamic_linking())
        linkDependencies(interp);
    if (get_perform_layout())
        remapSections(interp);
    if (get_perform_relocations())
        fixupSections(interp);
}

    













bool
BinaryLoader::isHeaderSimilar(const SgAsmGenericHeader* matchHeader, const SgAsmGenericHeader* candidateHeader)
{
    return (matchHeader->variantT() == candidateHeader->variantT());
}

SgAsmGenericHeaderPtrList
BinaryLoader::findSimilarHeaders(const SgAsmGenericHeader* matchHeader, const SgAsmGenericHeaderPtrList& candidateHeaders)
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
BinaryLoader::createAsmAST(SgBinaryComposite* binaryFile, std::string filePath)
{
    ROSE_ASSERT(!filePath.empty());
  
    SgAsmGenericFile* file = SgAsmExecutableFileFormat::parseBinaryFormat(filePath.c_str());
    ROSE_ASSERT(file != NULL);
  
    // TODO do I need to attach here - or can I do after return
    binaryFile->get_genericFileList()->get_files().push_back(file);
    file->set_parent(binaryFile);

    /* Add a new interpretation to the SgBinaryComposite object for each header of the newly parsed
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

/* used to be called loadInterpLibraries */
void
BinaryLoader::linkDependencies(SgAsmInterpretation* interp)
{
    ROSE_ASSERT(interp != NULL);
    SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    ROSE_ASSERT(composite != NULL);
  
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
            if (get_verbose(composite) > 0) {
                printf("Loading: %s\n", dllPath.c_str());
            }
            SgAsmGenericFile* newFile = createAsmAST(composite, dllPath);
            ROSE_ASSERT(newFile != NULL); // TODO more user friendly fail notification
            filesAlreadyLoaded.push_back(dllPath); /* Mark loaded by dllPath */
            
            SgAsmGenericHeaderPtrList newHeaders = findSimilarHeaders(header, newFile->get_headers()->get_headers());
            unresolvedHeaders.insert(unresolvedHeaders.end(), newHeaders.begin(), newHeaders.end());
        }
    }
}

/* Used to be called layoutInterpLibraries */
void
BinaryLoader::remapSections(SgAsmInterpretation* interp)
{
    SgAsmGenericSectionPtrList allSections;
    Loader* loader = Loader::find_loader(interp->get_headers()->get_headers().front());
    for(size_t i=0; i < interp->get_headers()->get_headers().size(); ++i){
        SgAsmGenericHeader* header = interp->get_headers()->get_headers()[i];
        addSectionsForLayout(header, allSections);		       
    }

    MemoryMap* memMap = loader->map_code_sections(interp->get_map(), allSections);
    interp->set_map(memMap);
}

void
BinaryLoader::addSectionsForLayout(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections)
{
    allSections.insert(allSections.end(),
                       header->get_sections()->get_sections().begin(),
		       header->get_sections()->get_sections().end());
}

/* Used to be called relocateAllLibraries */
void
BinaryLoader::fixupSections(SgAsmInterpretation *interp)
{
    // 1. Get section map (name -> list<section*>)
    // 2. Create Symbol map from relevant sections (.dynsym)
    // 3. Create Extent sorted list of sections
    // 4. Collect Relocation Entries.
    // 5.  for each relocation entry, perform relocation
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
BinaryLoader::getDLLs(SgAsmGenericHeader* header, const Rose_STL_Container<std::string> &dllFilesAlreadyLoaded)
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
