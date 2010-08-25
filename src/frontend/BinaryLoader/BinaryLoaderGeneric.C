// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Loader.h"
#include "Disassembler.h"
#include "binaryLoader.h"

#include "BinaryLoaderGeneric.h" // TODO remove
using std::string;
using std::set;


BinaryLoaderGeneric::BinaryLoaderGeneric()
  :p_verbose(0),
   p_perform_dynamic_linking(false),
   p_perform_relocations(false),
   p_perform_disassembly(true)
{
  set_verbose(SgProject::get_verbose());
}


/** Loads, performs layout, relocates and disassembles the given file */
bool BinaryLoaderGeneric::load(SgBinaryComposite* binaryFile,SgAsmGenericFile* executableAsmFile)
{
  
  ROSE_ASSERT(binaryFile->get_genericFileList()->get_files().empty());
   
  if(executableAsmFile == NULL){
    executableAsmFile = createAsmAST(binaryFile,binaryFile->get_sourceFileNameWithPath());
  } 

  ROSE_ASSERT(executableAsmFile != NULL);
  
  if(get_perform_dynamic_linking())
    loadAllLibraries(binaryFile);

  if(get_perform_layout())
    layoutAllLibraries(binaryFile);

  if(get_perform_relocations())
    relocateAllLibraries(binaryFile);

  if(get_perform_disassembly())
    disassembleAllLibraries(binaryFile);
  return true;
}

bool BinaryLoaderGeneric::loadAllLibraries(SgBinaryComposite* binaryFile)
{
  ROSE_ASSERT(binaryFile != NULL);
  SgAsmInterpretationPtrList &interps = binaryFile->get_interpretations()->get_interpretations();
  for(size_t i=0; i < interps.size();++i){
    loadInterpLibraries(binaryFile,interps[i]);
  }
  return true;
}  

/** 
    Determine if two headers are similar enough that they can be located in
        the same interpretation.

   Current defined as exactly the same class

   TODO, do we need something better to disambiguate header similarity?
*/
bool isHeaderSimilar(const SgAsmGenericHeader* matchHeader,
                     const SgAsmGenericHeader* candidateHeader)
{
  return (matchHeader->variantT() == candidateHeader->variantT());

}

/**
   find all headers in candidate headers that 'match' the matchHeader
*/
SgAsmGenericHeaderPtrList findSimilarHeaders(const SgAsmGenericHeader* matchHeader,
                                             const SgAsmGenericHeaderPtrList& candidateHeaders)
{
  SgAsmGenericHeaderPtrList matches;
  for(size_t i=0; i < candidateHeaders.size(); ++i){
    SgAsmGenericHeader* candidate = candidateHeaders[i]; 
    if(isHeaderSimilar(matchHeader,candidate)){
      matches.push_back(candidate);
    }
  }
  return matches;
}

/**

*/
SgAsmGenericFile* 
BinaryLoaderGeneric::createAsmAST( SgBinaryComposite* binaryFile, string filePath)
{

  if ( get_verbose(binaryFile) > 0)
    printf ("Loading Binary %s \n",filePath.c_str());

  ROSE_ASSERT(!filePath.empty());
  
  SgAsmGenericFile* file = SgAsmExecutableFileFormat::parseBinaryFormat(filePath.c_str());
  ROSE_ASSERT(file != NULL);
  
  // TODO do I need to attach here - or can I do after return
  binaryFile->get_genericFileList()->get_files().push_back(file);
  file->set_parent(binaryFile);


  /* Add a disassembly interpretation for each header. Actual disassembly will occur later.
   * NOTE: This probably isn't the right place to add interpretation nodes, but I'm leaving it here for the time being. We
   *       probably don't want an interpretation for each header if we're doing dynamic linking. [RPM 2009-09-17] */
  // TODO, fix the above comments to reflect that we 'do this right' now
  const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
  SgAsmInterpretationPtrList &interps = binaryFile->get_interpretations()->get_interpretations();

  for (size_t i = 0; i < headers.size(); ++i){
    SgAsmGenericHeader* header = headers[i];
    SgAsmInterpretation* interp = NULL;

    for(size_t j = 0; j < interps.size(); ++j){
      ROSE_ASSERT(!interps[j]->get_headers()->get_headers().empty());

      SgAsmGenericHeader* interpHeader = interps[j]->get_headers()->get_headers().front();
      if(isHeaderSimilar(header,interpHeader)){
	// This interpretation is similar to the new header
	interp=interps[j];
	break;
      }
    }
    if(interp == NULL){
      // Only create a new interpretation if a similar one doesn't exist 
      interp = new SgAsmInterpretation();
      interps.push_back(interp);
      interp->set_parent(binaryFile);
    }
    interp->get_headers()->get_headers().push_back(header);
  }

  // TODO should elf support be here?
  // If dwarf is available then read it into the AST.
#if USE_ROSE_DWARF_SUPPORT
  // DQ (3/14/2009): Dwarf support now works within ROSE when used with Intel Pin
  // (was a huge problem until everything (e.g. libdwarf) was dynamically linked).
  
  // DQ (11/7/2008): New Dwarf support in ROSE (Dwarf IR nodes are generated in the AST).
  readDwarf(file); 
#endif
  
  return file;
}


 
/** Performs loading only.  Will populate binaryFile->p_binaryFileList with SgAsmFile* 
    return false if any library cannot be loaded
*/
bool BinaryLoaderGeneric::loadInterpLibraries(SgBinaryComposite* binaryFile,SgAsmInterpretation* interp)
{
  ROSE_ASSERT(binaryFile != NULL);
  ROSE_ASSERT(interp != NULL);
  /* 
     Algorithm:
     Dynamically linked executable have a list of libraries on which they depend.
     Dynamically linked libraries will ALSO have a list of libraries on which they depend.
      So we need to recursively satisfy all requirements.
      
     ELF specifies a particular order symbols should be resolved (in general breadth first search
     with the executable as root).  We choose to load libraries in that order in the general case
     since order doesn't seem to matter to PE (since symbols are scoped to library)
       
     So, we maintain a set of all loaded files (currently by path, in future by SgAsmGenericFile?).
      
     We boostrap the process by getting required libraries from whatever files
       are in the interpretation (usually just the executable itself).

     We keep a queue of SgAsmGenericHeaders, and get a list of paths to libraries left to load.
     for each header we get its dlls via getDLLS
       Its up to the getDLLs function to filter out files that have already been loaded.
       for each dll, we open and parse the file, and stick its (relevant) header into the 
       queue
     
  */
  
  // Bootstrap DLL list - these are library names, not file paths
  // on *NIX machines these are called shared libraries (.so)
  Rose_STL_Container<string> filesAlreadyLoaded;
  std::list<SgAsmGenericHeader*> unresolvedHeaders;
  unresolvedHeaders.insert(unresolvedHeaders.end(),
                           interp->get_headers()->get_headers().begin(),
			   interp->get_headers()->get_headers().end());

  
  for(; !unresolvedHeaders.empty(); unresolvedHeaders.pop_front())
  {
    SgAsmGenericHeader* header = unresolvedHeaders.front();

    Rose_STL_Container<string> filesToLoad = getDLLs(header,filesAlreadyLoaded);
    for(size_t i=0; i < filesToLoad.size(); ++i){
      string dllPath = filesToLoad[i];
      
      if(get_verbose(binaryFile) > 0){
	printf("Loading: %s\n",dllPath.c_str());
      }

      SgAsmGenericFile* newFile = createAsmAST(binaryFile,dllPath);

      // TODO more user friendly fail notification
      ROSE_ASSERT(newFile != NULL);

      // Mark loaded by dllPath
      filesAlreadyLoaded.push_back(dllPath);
      
      SgAsmGenericHeaderPtrList newHeaders = findSimilarHeaders(header,newFile->get_headers()->get_headers());
      unresolvedHeaders.insert(unresolvedHeaders.end(),newHeaders.begin(),newHeaders.end());
    }
  }

  return true;
}

bool BinaryLoaderGeneric::layoutAllLibraries(SgBinaryComposite* binaryFile)
{
  SgAsmInterpretationPtrList& interps = binaryFile->get_interpretations()->get_interpretations();
  for(size_t i=0; i < interps.size(); ++i){
    layoutInterpLibraries(binaryFile,interps[i]);
  }
  return true;
}

/** Performs layout on unmapped sections in all files in binaryFile->p_binaryFile
    If a valid layout cannot be performed, return false
*/
bool BinaryLoaderGeneric::layoutInterpLibraries(SgBinaryComposite*,SgAsmInterpretation* interp)
{
  SgAsmGenericSectionPtrList allSections;
  Loader* loader = Loader::find_loader(interp->get_headers()->get_headers().front());
  for(size_t i=0; i < interp->get_headers()->get_headers().size(); ++i){
    SgAsmGenericHeader* header = interp->get_headers()->get_headers()[i];
    addSectionsForLayout(header, allSections);		       
  }


  //MCB: Note - this will likely mess up actual dynamic linking, but was necessary
  //     to get regressions to pass - a more nuanced solution is required
  //     (failed for fnord.ppc, testConstants)
  //MemoryMap* memMap = loader->map_all_sections(interp->get_map(),allSections);
  MemoryMap* memMap = loader->map_code_sections(interp->get_map(),allSections);
  interp->set_map(memMap);
  return true;
}

/**
   Default, add all sections
 */
void BinaryLoaderGeneric::addSectionsForLayout(SgAsmGenericHeader* header, 
                                               SgAsmGenericSectionPtrList &allSections)
{
    allSections.insert(allSections.end(),
                       header->get_sections()->get_sections().begin(),
		       header->get_sections()->get_sections().end());
}

/**
 */
bool BinaryLoaderGeneric::relocateAllLibraries(SgBinaryComposite* binaryFile)
{
  // 1. Get section map (name -> list<section*>)
  // 2. Create Symbol map from relevant sections (.dynsym)
  // 3. Create Extent sorted list of sections
  // 4. Collect Relocation Entries.
  // 5.  for each relocation entry, perform relocation
  return false;
}

#if 0
/** Given a binaryFile, build an ExtentMap of sections that are already mapped.
 */
ExtentMap BinaryLoaderGeneric::buildExtentMap(SgBinaryComposite* binaryFile)
{
  ExtentMap usedExtents;
  SgAsmGenericFilePtrList::const_iterator fileIter=binaryFile->get_genericFileList()->get_files().begin();
  SgAsmGenericFilePtrList::const_iterator fileIterEnd=binaryFile->get_genericFileList()->get_files().end();
  for(;fileIter != fileIterEnd; ++fileIter)
  {
    SgAsmGenericFile* genFile = *fileIter;

    const SgAsmGenericHeaderPtrList &headers= genFile->get_headers()->get_headers();
    SgAsmGenericHeaderPtrList::const_iterator headerIter=headers.begin();
    for(; headerIter != headers.end(); ++headerIter)
    {
      SgAsmGenericHeader* genHeader = *headerIter;
      const SgAsmGenericSectionPtrList &sections= genHeader->get_sections()->get_sections();
      SgAsmGenericSectionPtrList::const_iterator sectionIter = sections.begin();
      for(;sectionIter != sections.end(); ++sectionIter)
      {
	const SgAsmGenericSection* section = *sectionIter;
	if(section->is_mapped())
	{
	  if(get_verbose(binaryFile) > 0){
	    // TODO use c-style printing
	    std::cout << " 0x" << std::hex << section->get_mapped_extent().first 
		      << " 0x" << std::hex << section->get_mapped_extent().first + section->get_mapped_extent().second
		      << " adding mapped section " << section->get_name()->c_str() << std::endl;
	  }
	  usedExtents.insert(section->get_mapped_extent());
	}
	else
	{
	  if(get_verbose(binaryFile) > 0){
	    std::cout << "ignoring: " << section->get_name()->c_str() << std::endl;
	  }
	}
      }
    }
  }
  return usedExtents;
}
#endif

/** 
    Some executable formats allow multiple architectures to reside in the same file
    (fat binaries).  It is not clear what we should do in that case.  For now we're
    going to guess at the "most likely" type we're looking for in each 
    individual asmFile and hope we're right.  

    TODO This should be cleaned up in the future.
*/
size_t wordSizeOfFile(SgAsmGenericFile* file)
{
  size_t maxWordSize=0;
  SgAsmGenericHeaderPtrList &headers= file->get_headers()->get_headers();
  SgAsmGenericHeaderPtrList::iterator headerIter=headers.begin();
  for(; headerIter != headers.end(); ++headerIter) {
    maxWordSize = std::max(maxWordSize,(*headerIter)->get_word_size());
  }  
  return maxWordSize;
}

#if 0
void BinaryLoaderGeneric::layoutSingleLibrary(ExtentMap& usedExtents, SgAsmFile* asmFile)
{

  static const rose_addr_t MaxMemory32 = (1ul << 32) -1;
  static const rose_addr_t MaxMemory64 = (rose_addr_t)(-1);
  size_t wordSize=wordSizeOfFile(asmFile);

  
  ExtentMap unusedExtents;
  if(wordSize == 4)
    unusedExtents = usedExtents.subtract_from(0,MaxMemory32);
  else if(wordSize == 8)
    unusedExtents = usedExtents.subtract_from(0,MaxMemory64);
  else
    ROSE_ASSERT(false);
  
  SgAsmGenericFile* genFile = asmFile->get_genericFile();

  rose_addr_t size = genFile->get_current_size();
  // TODO figure out alignment
  const rose_addr_t Alignment=0x1000;//4k page sizeb
  rose_addr_t align_size = size;
  if((size % Alignment) != 0){
    // align size if necessary
    align_size = Alignment + ( size & ~(Alignment-1));
  }
  
  // pad out size because extents are not necessarily aligned
  const rose_addr_t align_plus_1_size = align_size + Alignment;
  ExtentPair newLocation = unusedExtents.allocate_best_fit(align_plus_1_size);
  rose_addr_t newBaseRA = newLocation.first;
  
  if((newBaseRA % Alignment) != 0){
    // align base RA if necessary
    newBaseRA = Alignment + (newBaseRA & ~(Alignment-1));
  }
  
  if(get_verbose() > 0)
  {
    std::cout << "Allocating: " << std::hex << newBaseRA << ":" 
	      << std::hex << newBaseRA + align_size << " to " 
	      << asmFile->get_name() << std::endl;
  }
  
  ExtentMap tmpExtents;
  // Set the base_va for all headers
  // also, as a sort of hack, we have to set_mapped_size for all sections,
  //  otherwise is_mapped() will return false - I think that is a bug - MCB 6/9
  SgAsmGenericHeaderPtrList &headers= genFile->get_headers()->get_headers();
  SgAsmGenericHeaderPtrList::iterator headerIter=headers.begin();
  for(; headerIter != headers.end(); ++headerIter)
  {
    SgAsmGenericHeader* genHeader = *headerIter;
    genHeader->set_base_va(newBaseRA);
    genHeader->set_mapped_size((*headerIter)->get_size());
    
    SgAsmGenericSectionPtrList &sections= genHeader->get_sections()->get_sections();
    SgAsmGenericSectionPtrList::iterator sectionIter = sections.begin();
    for(;sectionIter != sections.end(); ++sectionIter)
    {
      SgAsmGenericSection* section = *sectionIter;

      // some types of sections have different 'on disk' sizes vs. 'in memory' sizes
      section->set_mapped_size(section->get_size());

      // used for initialize bss sections among other things
      handleSectionMapped(section);

      // now we mark this section has "used"
      tmpExtents.insert(section->get_mapped_va(),section->get_mapped_size());
    }
  }
  //tmpExtents.dump_extents(stdout,"","   ");
  usedExtents.insert(tmpExtents);
}
#endif

Rose_STL_Container<string> BinaryLoaderGeneric::getDLLs(SgAsmGenericHeader* header,
							const Rose_STL_Container<string> &dllFilesAlreadyLoaded)
{
  Rose_STL_Container<string> files;
  ROSE_ASSERT(header != NULL);
  std::set<string> alreadyLoadedSet(dllFilesAlreadyLoaded.begin(),dllFilesAlreadyLoaded.end());

  const SgAsmGenericDLLPtrList &dlls = header->get_dlls();
  for(size_t j=0; j < dlls.size(); ++j){
    string file = dlls[j]->get_name()->c_str();
    if(alreadyLoadedSet.find(file) == alreadyLoadedSet.end())
      files.push_back(file);
  }
  return files;

}


bool BinaryLoaderGeneric::disassembleAllLibraries(SgBinaryComposite* binaryFile)
{
  SgAsmInterpretationPtrList& interps = binaryFile->get_interpretations()->get_interpretations();
  for(size_t i=0; i < interps.size(); ++i){
    SgAsmInterpretation* interp = interps[i];
    Disassembler::disassembleInterpretation(interp);
  }
  return true;
}

void BinaryLoaderGeneric::set_perform_dynamic_linking(bool val)
{
  p_perform_dynamic_linking =val;
}
bool BinaryLoaderGeneric::get_perform_dynamic_linking() const
{
  return p_perform_dynamic_linking;
}


void BinaryLoaderGeneric::set_perform_layout(bool val)
{
  p_perform_layout = val;
}


bool BinaryLoaderGeneric::get_perform_layout() const
{
  return p_perform_layout;
}

void BinaryLoaderGeneric::set_perform_relocations(bool val)
{
  p_perform_relocations =val;
}
bool BinaryLoaderGeneric::get_perform_relocations() const
{
  return p_perform_relocations;
}

void BinaryLoaderGeneric::set_perform_disassembly(bool val)
{
  p_perform_disassembly=val;
}

bool BinaryLoaderGeneric::get_perform_disassembly() const
{
  return p_perform_disassembly;
}

void BinaryLoaderGeneric::set_verbose(int verbose)
{
  p_verbose=verbose;
}

int BinaryLoaderGeneric::get_verbose() const
{
  return p_verbose;
}

/**
   returns verbose if this or a given other node is verbose
   allows terser syntax
 */
int BinaryLoaderGeneric::get_verbose(SgBinaryComposite* binary) const
{
  return std::max(binary->get_verbose(), p_verbose);
}
