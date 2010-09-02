#include "sage3basic.h"

#include "BinaryLoader.h"
#include "BinaryLoaderElf.h"
#include "BinaryLoaderPe.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

std::vector<BinaryLoader*> BinaryLoader::loaders;

/* We put some initializations here in a *.C file so we don't need to recompile so much if we need to change how a
 * BinaryLoader is constructed and we're just making the change for debugging purposes. */
void
BinaryLoader::init()
{
#if 0
    set_perform_dynamic_linking(true);
    set_debug(stderr);
    add_directory("/lib32");
#endif
}

/* class method */
void
BinaryLoader::initclass()
{
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        /* Registered in order from most generic to most specific */
        register_subclass(new BinaryLoader);            /* generically handles all formats, albeit with limited functionality */
        register_subclass(new BinaryLoaderElf);         /* POSIX Executable and Linkable Format (ELF) */
        register_subclass(new BinaryLoaderPe);          /* Windows Portable Executable (PE) */
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
    if (get_perform_remap())
        remapSections(interp);
    if (get_perform_relocations())
        fixupSections(interp);
}

std::string
BinaryLoader::find_so_file(const std::string &libname) const
{
    if (debug) fprintf(debug, "BinaryLoader: find library=%s...\n", libname.c_str());
    for (std::vector<std::string>::const_iterator di=directories.begin(); di!=directories.end(); ++di) {
        if (debug) fprintf(debug, "BinaryLoader:   looking in %s...\n", di->c_str());
        std::string libpath = *di + "/" + libname;
        struct stat sb;
        if (stat(libpath.c_str(), &sb)>=0 && S_ISREG(sb.st_mode) && access(libpath.c_str(), R_OK)>=0) {
            if (debug) fprintf(debug, "BinaryLoader:   found.\n");
            return libpath;
        }
    }
    if (debug) {
        if (directories.empty()) fprintf(debug, "BinaryLoader:   no search directories\n");
        fprintf(debug, "BinaryLoader:   not found; throwing exception.\n");
    }
    throw Exception("cannot find file for library: " + libname);
}

bool
BinaryLoader::is_linked(SgBinaryComposite *composite, const std::string &filename)
{
    const SgAsmGenericFilePtrList &files = composite->get_genericFileList()->get_files();
    for (SgAsmGenericFilePtrList::const_iterator fi=files.begin(); fi!=files.end(); ++fi) {
        if ((*fi)->get_name()==filename)
            return true;
    }
    return false;
}

bool
BinaryLoader::is_linked(SgAsmInterpretation *interp, const std::string &filename)
{
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        SgAsmGenericFile *file = (*hi)->get_file();
        ROSE_ASSERT(file!=NULL);
        if (file->get_name()==filename)
            return true;
    }
    return false;
}


/* once called loadInterpLibraries */
void
BinaryLoader::linkDependencies(SgAsmInterpretation* interp)
{
    ROSE_ASSERT(interp != NULL);
    SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    ROSE_ASSERT(composite != NULL);

    /* Make sure the pre-load objects are parsed and linked into the AST. */
    for (std::vector<std::string>::const_iterator pi=preloads.begin(); pi!=preloads.end(); ++pi) {
        if (debug) fprintf(debug, "BinaryLoader: preload object %s\n", pi->c_str());
        std::string filename = find_so_file(*pi);
        if (is_linked(composite, filename)) {
            if (debug) fprintf(stderr, "BinaryLoader: %s is already parsed.\n", filename.c_str());
        } else {
            if (debug) fprintf(debug, "BinaryLoader: parsing %s...\n", filename.c_str());
            createAsmAST(composite, filename);
            if (debug) fprintf(debug, "BinaryLoader: done parsing %s\n", filename.c_str());
        }
    }

    /* Bootstrap */
    std::list<SgAsmGenericHeader*> unresolved_hdrs;
    unresolved_hdrs.insert(unresolved_hdrs.end(),
                           interp->get_headers()->get_headers().begin(),
                           interp->get_headers()->get_headers().end());

    /* Process unresolved headers from the beginning of the queue and push new ones onto the end. */
    while (!unresolved_hdrs.empty()) {
        SgAsmGenericHeader *header = unresolved_hdrs.front();
        unresolved_hdrs.pop_front();
        std::string header_name = header->get_file()->get_name();
        std::vector<std::string> deps = dependencies(header);
        for (std::vector<std::string>::iterator di=deps.begin(); di!=deps.end(); ++di) {
            if (debug) fprintf(debug, "BinaryLoader: library %s needed by %s\n", di->c_str(), header_name.c_str());
            std::string filename = find_so_file(*di);
            if (is_linked(composite, filename)) {
                if (debug) fprintf(debug, "BinaryLoader: %s is already parsed.\n", filename.c_str());
            } else {
                if (debug) fprintf(debug, "BinaryLoader: parsing %s...\n", filename.c_str());
                SgAsmGenericFile *new_file = createAsmAST(composite, filename);
                if (debug) fprintf(debug, "BinaryLoader: done parsing %s\n", filename.c_str());
                ROSE_ASSERT(new_file!=NULL); /*FIXME: more user-friendly failure [RPM 2010-09-01]*/
                SgAsmGenericHeaderPtrList new_hdrs = findSimilarHeaders(header, new_file->get_headers()->get_headers());
                unresolved_hdrs.insert(unresolved_hdrs.end(), new_hdrs.begin(), new_hdrs.end());
            }
        }
    }
}

/* class method */
SgAsmGenericHeaderPtrList
BinaryLoader::findSimilarHeaders(SgAsmGenericHeader *match, SgAsmGenericHeaderPtrList &candidates)
{
    SgAsmGenericHeaderPtrList retval;
    Disassembler *d1 = Disassembler::lookup(match);

    for (SgAsmGenericHeaderPtrList::iterator ci=candidates.begin(); ci!=candidates.end(); ++ci) {
        Disassembler *d2 = d1 ? Disassembler::lookup(*ci) : NULL;
        if (!d1 && !d2) {
            if (match->variantT() == (*ci)->variantT())
                retval.push_back(*ci);
        } else if (d1==d2) {
            retval.push_back(*ci);
        }
    }
    return retval;
}

/* class method */
bool
BinaryLoader::isHeaderSimilar(SgAsmGenericHeader *h1, SgAsmGenericHeader *h2)
{
    /* This is implemented in terms of findSimilarHeaders() rather than vice versa so that findSimilarHeaders() does not need
     * to call Disassembler::lookup() so often. */
    SgAsmGenericHeaderPtrList h2list;
    h2list.push_back(h2);
    return !findSimilarHeaders(h1, h2list).empty();
}


/* class method */
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
            if (isHeaderSimilar(header, interpHeader)) {
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

/* Used to be called layoutInterpLibraries */
void
BinaryLoader::remapSections(SgAsmInterpretation* interp)
{
    SgAsmGenericSectionPtrList allSections;
    for(size_t i=0; i < interp->get_headers()->get_headers().size(); ++i){
        SgAsmGenericHeader* header = interp->get_headers()->get_headers()[i];
        addSectionsForRemap(header, allSections);		       
    }

    MemoryMap* memMap = map_all_sections(interp->get_map(), allSections);
    interp->set_map(memMap);
}

void
BinaryLoader::addSectionsForRemap(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections)
{
    allSections.insert(allSections.end(),
                       header->get_sections()->get_sections().begin(),
		       header->get_sections()->get_sections().end());
}

std::vector<std::string>
BinaryLoader::dependencies(SgAsmGenericHeader *header)
{
    ROSE_ASSERT(header!=NULL);
    std::vector<std::string> retval;
    const SgAsmGenericDLLPtrList &dlls = header->get_dlls();
    for (SgAsmGenericDLLPtrList::const_iterator di=dlls.begin(); di!=dlls.end(); ++di)
        retval.push_back((*di)->get_name()->get_string());
    return retval;
}

/* Returns a map containing the specified sections (if they are mapped). */
MemoryMap *
BinaryLoader::map_all_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
    return NULL;
#else
    // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            return CONTRIBUTE_ADD; /*alignment might weed out non-mapped sections*/
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of code-containing sections. A code-containing section is any section that is mapped with execute permission or
 * any section explicitly marked as containing code. Any section that is not code-containing is subtracted from the mapping. */
MemoryMap *
BinaryLoader::map_code_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
    return NULL;
#else
    // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (section->get_contains_code()) {
                return CONTRIBUTE_ADD; /*even if it has no preferred mapping*/
            } else if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_xperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of executable sections. Any mapped section that isn't executable is subtracted from the mapping.
 *
 * Note that another way to do this is to use call map_all_sections() and then prune away, using MemoryMap::prune(), those
 * parts of the map that are not executable. */
MemoryMap *
BinaryLoader::map_executable_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
    return NULL;
#else
    // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_xperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Returns a map of executable sections. Any section that is not writable is removed from the mapping.
 *
 * Note that another way to do this is to call map_all_sections() and then prune away, using MemoryMap::prune(), those parts
 * of the map that are not writable. */
MemoryMap *
BinaryLoader::map_writable_sections(MemoryMap *map, const SgAsmGenericSectionPtrList &sections, bool allow_overmap)
{
#ifdef USE_ROSE
    return NULL;
#else
    // DQ (1/27/2010): This is a problem for ROSE.
    struct: public Selector {
        Contribution contributes(SgAsmGenericSection *section) {
            if (!section->is_mapped()) {
                return CONTRIBUTE_NONE;
            } else if (section->get_mapped_wperm()) {
                return CONTRIBUTE_ADD;
            } else {
                return CONTRIBUTE_SUB;
            }
        }
    } s1;
    return create_map(map, sections, &s1, allow_overmap);
#endif
}

/* Align section addresses and sizes */
rose_addr_t
BinaryLoader::align_values(SgAsmGenericSection *section, Contribution contrib,
                           rose_addr_t *va_p/*out*/, rose_addr_t *mem_size_p/*out*/,
                           rose_addr_t *offset_p/*out*/, rose_addr_t *file_size_p/*out*/, 
                           const MemoryMap*)
{
    if (!section->is_mapped()) {
        *va_p = *mem_size_p = *offset_p = *file_size_p = 0;
        return 0;
    }
    
    rose_addr_t va = section->get_header()->get_base_va() + section->get_mapped_preferred_rva();
    rose_addr_t mem_size = section->get_mapped_size();
    rose_addr_t offset = section->get_offset();
    rose_addr_t file_size = section->get_size();
    rose_addr_t nleading = 0; /*number of bytes prepended to the mapped part of the file for alignment*/

    /* Align the file offset downward, adjusting file size as necessary. Then align the file size upward so the mapped file
     * extent ends on a boundary. */
    rose_addr_t fa = section->get_file_alignment();
    if (fa>0) {
        rose_addr_t n = ALIGN_DN(offset, fa);
        ROSE_ASSERT(n<=offset);
        nleading = offset-n;
        file_size += nleading;
        offset = n;
        /* file_size = ALIGN_UP(file_size, fa); -- a partial ending page shall be zero filled */
    }

    /* Align memory address downward, adjusting memory size as necessary. Then align memory size upward so the mapped region
     * ends on a boundary. */
    rose_addr_t ma = section->get_mapped_alignment();
    if (ma>0) {
        rose_addr_t n = ALIGN_DN(va, ma);
        ROSE_ASSERT(n<=va);
        mem_size += va-n;
        va = n;
        mem_size = ALIGN_UP(mem_size+nleading, std::min(ma, (rose_addr_t)0x1000));
    }

    *va_p = va;
    *mem_size_p = mem_size;
    *offset_p = offset;
    *file_size_p = file_size;

    return va + nleading;
}

/* Returns sections in mapping order. */
SgAsmGenericSectionPtrList
BinaryLoader::order_sections(const SgAsmGenericSectionPtrList &sections)
{
    SgAsmGenericSectionPtrList retval;
    for (size_t i=0; i<sections.size(); i++) {
        if (!sections[i]->get_synthesized()) {
            retval.push_back(sections[i]);
        }
    }
    return sections;
}

/* Uses a selector to determine what sections should be mapped. */
MemoryMap *
BinaryLoader::create_map(MemoryMap *map, const SgAsmGenericSectionPtrList &unordered_sections, Selector *selector,
                         bool allow_overmap)
{
    if (!map)
        map = new MemoryMap();
    SgAsmGenericSectionPtrList sections = order_sections(unordered_sections);
    if (debug)
        fprintf(debug, "Loader: creating memory map...\n");
    for (size_t i=0; i<sections.size(); i++) {
        SgAsmGenericSection *section = sections[i];
        section->set_mapped_actual_rva(0); /*assigned below if mapped*/

        /* Does this section contribute anything to the mapping? */
        Contribution contrib = selector->contributes(section);
        if (CONTRIBUTE_NONE==contrib)
            continue;
        if (debug) {
            fprintf(debug, "  %smapping section [%d] \"%s\" with base va 0x%08"PRIx64"\n",
                    CONTRIBUTE_SUB==contrib ? "un" : "",
                    section->get_id(), section->get_name()->c_str(),
                    section->get_header()->get_base_va());
            fprintf(debug, "    Section specified: RVA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_mapped_preferred_rva(), section->get_mapped_size(),
                    section->get_mapped_preferred_rva()+section->get_mapped_size());
            fprintf(debug, "    Section specified:  VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_header()->get_base_va() + section->get_mapped_preferred_rva(),
                    section->get_mapped_size(),
                    section->get_header()->get_base_va() + section->get_mapped_preferred_rva() + section->get_mapped_size());
        }
        
        rose_addr_t va=0, mem_size=0, offset=0, file_size=0;
        rose_addr_t section_va = align_values(section, contrib, &va, &mem_size, &offset, &file_size, map);
        if (debug) {
            fprintf(debug, "    Aligned values:     VA 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64,
                    va, mem_size, va+mem_size);
            if (section->get_header()->get_base_va()+section->get_mapped_preferred_rva()==va &&
                section->get_mapped_size()==mem_size) {
                fputs(" (no change)\n", debug);
            } else {
                fprintf(debug, " (fa=%"PRIu64", ma=%"PRIu64")\n",
                        section->get_file_alignment(), section->get_mapped_alignment());
            }
            
        }

        if (0==mem_size) {
            if (debug) fprintf(debug, "    Nothing left to do.\n");
            continue;
        }

        if (debug && CONTRIBUTE_ADD==contrib) {
            fprintf(debug, "    Section begins at va   0x%08"PRIx64"\n", section_va);
            fprintf(debug, "    File location:         0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                    section->get_offset(), section->get_size(), section->get_offset()+section->get_size());
            fprintf(debug, "    File aligned:          0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"%s\n",
                    offset, file_size, offset+file_size,
                    section->get_offset()==offset && section->get_size()==file_size ? " (no change)" : "");
            fprintf(debug, "    Permissions: %c%c%c\n", 
                    section->get_mapped_rperm()?'r':'-', section->get_mapped_wperm()?'w':'-', section->get_mapped_xperm()?'x':'-');
        }
        
        if (allow_overmap || CONTRIBUTE_SUB==contrib) {
            if (debug) {
                fprintf(debug, "    Erasing   va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                        va, mem_size, va+mem_size);
            }
            map->erase(MemoryMap::MapElement(va, mem_size));
        }

        if (CONTRIBUTE_ADD==contrib) {
            /* Split into a left and right part where the right part, extending beyond EOF, is mapped anonymously. */
            SgAsmGenericFile *file = section->get_file();
            rose_addr_t total = file->get_data().size(); /*total size of file*/
            rose_addr_t ltsz = 0;
            if (offset>total) {
                if (debug)
                    fprintf(debug, "    Map starts beyond end-of-file.\n");
                ltsz = 0;
            } else if (offset+file_size>total) {
                if (debug)
                    fprintf(debug, "    Map crosses end-of-file at 0x%08"PRIx64"\n", total);
                ltsz = std::min(mem_size, total-offset);
            } else {
                /* Map falls entirely within the file, but mem size might be larger than file size */
                ltsz = std::min(mem_size, file_size);
            }
            rose_addr_t rtsz = mem_size - ltsz;

            /* Permissions */
            unsigned mapperms=MemoryMap::MM_PROT_NONE;
            if (section->get_mapped_rperm())
                mapperms |= MemoryMap::MM_PROT_READ;
            if (section->get_mapped_wperm())
                mapperms |= MemoryMap::MM_PROT_WRITE;
            if (section->get_mapped_xperm())
                mapperms |= MemoryMap::MM_PROT_EXEC;

            /* MapElement name for debugging. This is the file base name and section name concatenated. */
            std::string::size_type file_basename_pos = file->get_name().find_last_of("/");
            file_basename_pos = file_basename_pos==file->get_name().npos ? 0 : file_basename_pos+1;
            std::string melmt_name = file->get_name().substr(file_basename_pos) + "(" + section->get_name()->get_string() + ")";

            /* Map the left part to the file; right part is anonymous. */
            if (ltsz>0) {
                if (debug) {
                    fprintf(debug, "    Mapping   va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64
                            " at file offset 0x%08"PRIx64" for %s\n", va, ltsz, va+ltsz, offset, melmt_name.c_str());
                }
                MemoryMap::MapElement melmt(va, ltsz, &(file->get_data()[0]), offset, mapperms);
                melmt.set_name(melmt_name);
                map->insert(melmt);
            }
            if (rtsz>0) {
                if (debug) {
                    fprintf(debug, "    Anonymous va 0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64
                            " zero filled for %s\n", va+ltsz, rtsz, va+ltsz+rtsz, melmt_name.c_str());
                }
                MemoryMap::MapElement melmt(va+ltsz, rtsz, mapperms);
                melmt.set_name(melmt_name);
                map->insert(melmt);
            }

            /* Remember virtual address of first byte of section. */
            ROSE_ASSERT(section_va >= section->get_header()->get_base_va());
            section->set_mapped_actual_rva(section_va - section->get_header()->get_base_va());
        }
    }
    if (debug) {
        fprintf(debug, "Loader: created map:\n");
        map->dump(debug, "    ");
    }

    return map;
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
