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
    //set_debug(stderr);
    //set_perform_dynamic_linking(true);
    //add_directory("/lib32");
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
BinaryLoader::load(SgBinaryComposite *composite, bool read_executable_file_format_only)
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
        if (read_executable_file_format_only) {
            loader->set_perform_dynamic_linking(false);
            loader->set_perform_remap(false);
            loader->set_perform_relocations(false);
        } else {
            loader->set_perform_dynamic_linking(false);
            loader->set_perform_remap(true);
            loader->set_perform_relocations(false);
        }

        try {
            loader->load(interps[i]);
            delete loader;
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
        link(interp);
    if (get_perform_remap())
        remap(interp);
    if (get_perform_relocations())
        fixup(interp);
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
BinaryLoader::link(SgAsmInterpretation* interp)
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
BinaryLoader::remap(SgAsmInterpretation* interp)
{
    /* Make sure we have a valid memory map. It is permissible for the caller to have reserved some space already. */
    MemoryMap *map = interp->get_map();
    if (!map) interp->set_map(map = new MemoryMap);

    /* Process each file header in the order it appears in the AST. This is also the order that the link() method parsed
     * dependencies (usually by a breadth-first search). */
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi)
        remap(map, *hi);
}

/* Maps the sections of a single header. */
void
BinaryLoader::remap(MemoryMap *map, SgAsmGenericHeader *header)
{
    SgAsmGenericFile *file = header->get_file();
    ROSE_ASSERT(file!=NULL);

    if (debug) fprintf(debug, "BinaryLoader::remap: remapping sections of %s...\n", header->get_file()->get_name().c_str());
    SgAsmGenericSectionPtrList sections = get_remap_sections(header);

    rose_addr_t old_base_va = header->get_base_va();
    rose_addr_t new_base_va = rebase(map, header, sections);
    if (new_base_va != old_base_va) {
        if (debug) fprintf(debug, "  temporarily rebasing header from 0x%08"PRIx64" to 0x%08"PRIx64"\n", old_base_va, new_base_va);
        header->set_base_va(new_base_va);
    }

    try {
        for (SgAsmGenericSectionPtrList::iterator si=sections.begin(); si!=sections.end(); ++si) {
            SgAsmGenericSection *section = *si;
            section->set_mapped_actual_va(0); /*reset in case previously mapped*/

            if (debug) {
                fprintf(debug, "  mapping section [%d] \"%s\"", section->get_id(), section->get_name()->c_str());
                if (section->get_header()->get_base_va()!=0)
                    fprintf(debug, " with base va 0x%08"PRIx64, section->get_header()->get_base_va());
                fprintf(debug, "\n");
                fprintf(debug, "    Specified RVA:       0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                        section->get_mapped_preferred_rva(), section->get_mapped_size(),
                        section->get_mapped_preferred_rva()+section->get_mapped_size());
                if (section->get_header()->get_base_va()!=0) {
                    fprintf(debug, "    Specified  VA:       0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                            section->get_header()->get_base_va() + section->get_mapped_preferred_rva(),
                            section->get_mapped_size(),
                            (section->get_header()->get_base_va() + section->get_mapped_preferred_rva() +
                             section->get_mapped_size()));
                }
                fprintf(debug, "    Specified offset:    0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                        section->get_offset(), section->get_size(), section->get_offset()+section->get_size());
                fprintf(debug, "    Specified alignment: memory=[%"PRIu64",%"PRIu64"], file=[%"PRIu64",%"PRIu64"]\n",
                        section->get_mapped_alignment(), section->get_mapped_alignment(),
                        section->get_file_alignment(), section->get_file_alignment());
            }

            /* Figure out alignment, etc. */
            rose_addr_t malign_lo=1, malign_hi=1, va=0, mem_size=0, offset=0, file_size=0, va_offset=0;
            bool anon_lo=true, anon_hi=true;
            ConflictResolution resolve = RESOLVE_THROW;
            MappingContribution contrib = align_values(section, map,                      /* inputs */
                                                       &malign_lo, &malign_hi,            /* alignment outputs */
                                                       &va, &mem_size,                    /* memory location outputs */
                                                       &offset, &file_size,               /* file location outputs */
                                                       &va_offset, &anon_lo, &anon_hi,    /* internal location outputs */
                                                       &resolve);                         /* conflict resolution output */
            rose_addr_t falign_lo = std::max(section->get_file_alignment(), (rose_addr_t)1);
            rose_addr_t falign_hi = falign_lo;

            if (debug) {
                if (CONTRIBUTE_NONE==contrib || 0==mem_size) {
                    fprintf(debug, "    Does not contribute to map\n");
                } else {
                    fprintf(debug, "    Adjusted alignment:  memory=[%"PRIu64",%"PRIu64"], file=[%"PRIu64",%"PRIu64"]\n",
                            malign_lo, malign_hi, falign_lo, falign_hi);
                    fprintf(debug, "    Aligned VA:          0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64,
                            va, mem_size, va+mem_size);
                    if (section->get_header()->get_base_va()+section->get_mapped_preferred_rva()==va &&
                        section->get_mapped_size()==mem_size) {
                        fputs(" (no change)\n", debug);
                    } else {
                        fputc('\n', debug);
                    }
                    if (va < new_base_va) {
                        fprintf(debug, "    WARNING: aligned va 0x%08"PRIx64" is less than %sbase va 0x%08"PRIx64"\n",
                                va, new_base_va==old_base_va?"":"temporary ", new_base_va);
                    }
                    if (CONTRIBUTE_ADD==contrib) {
                        fprintf(debug, "    Aligned offset:      0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"%s\n",
                                offset, file_size, offset+file_size,
                                section->get_offset()==offset && section->get_size()==file_size ? " (no change)" : "");
                        fprintf(debug, "    Permissions:         %c%c%c\n", 
                                section->get_mapped_rperm()?'r':'-',
                                section->get_mapped_wperm()?'w':'-',
                                section->get_mapped_xperm()?'x':'-');
                        fprintf(debug, "    Internal offset:     0x%08"PRIx64" (va 0x%08"PRIx64")\n",
                                va_offset, va+va_offset);
                    }
                }
            }

            /* Sanity checks */
            if (CONTRIBUTE_NONE==contrib || 0==mem_size)
                continue;
            ROSE_ASSERT(va_offset<mem_size);
            if (file_size>mem_size) file_size = mem_size;
            ROSE_ASSERT(va + va_offset >= header->get_base_va());

            /* Erase part of the mapping? */
            if (CONTRIBUTE_SUB==contrib) {
                map->erase(MemoryMap::MapElement(va, mem_size));
                continue;
            }

            /* Resolve mapping conflicts.  The new mapping may have multiple parts, so we test whether all those parts can be
             * mapped by first mapping an anonymous region and then removing it.  In this way we can perform the test atomically
             * rather than trying to undo the parts that had been successful. Allocating a large anonymous region does not
             * actually allocate any memory. */
            try {
                MemoryMap::MapElement test(va, mem_size);
                map->insert(test);
                map->erase(test);
            } catch (const MemoryMap::Exception&) {
                switch (resolve) {
                    case RESOLVE_THROW:
                        throw;
                    case RESOLVE_OVERMAP:
                        map->erase(MemoryMap::MapElement(va, mem_size));
                        break;
                    case RESOLVE_REMAP:
                    case RESOLVE_REMAP_ABOVE: {
                        if (debug) fprintf(debug, "    Unable to map entire desired region.\n");
                        rose_addr_t above = RESOLVE_REMAP_ABOVE==resolve ? va : 0;
                        rose_addr_t new_va = map->find_free(above, mem_size, malign_lo); /*may throw MemoryMap::NoFreeSpace*/
                        ROSE_ASSERT(0 == (new_va+mem_size) % malign_hi); /*FIXME: not handled yet [RPM 2010-09-03]*/
                        va = new_va;
                        if (debug) {
                            fprintf(debug, "    Relocated to VA:     0x%08"PRIx64" + 0x%08"PRIx64" bytes = 0x%08"PRIx64"\n",
                                    va, mem_size, va+mem_size);
                        }
                        break;
                    }
                }
            }

            /* Save the virtual address where this section is (will be) mapped.  When a section is mapped more than once
             * (perfectly legal to do so) only the last mapping is saved. */
            section->set_mapped_actual_va(va + va_offset);

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

            /* Anonymously map the part of memory beyond the physical end of the file */
            SgAsmGenericFile *file = section->get_file();
            rose_addr_t total = file->get_data().size(); /*total size of file*/
            if (offset+mem_size > total) {
                rose_addr_t n = (offset + mem_size) - total;
                rose_addr_t a = va + total - offset;
                MemoryMap::MapElement me(a, n, mapperms);
                me.set_name(melmt_name);
                map->insert(me);
                mem_size -= n;
                file_size = std::min(file_size, mem_size);
            }

            /* Anonymously map the part of memory beyond the part of file */
            if (anon_hi && mem_size>file_size) {
                rose_addr_t n = mem_size - file_size;
                rose_addr_t a = va + file_size;
                MemoryMap::MapElement me(a, n, mapperms);
                me.set_name(melmt_name);
                map->insert(me);
                mem_size -= n;
            }

            /* Anonymously map the part of memory before the section */
            if (anon_lo && va_offset>0) {
                rose_addr_t n = va_offset - va;
                rose_addr_t a = va;
                MemoryMap::MapElement me(a, n, mapperms);
                me.set_name(melmt_name);
                map->insert(me);
                mem_size -= n;
                file_size -= n;
                va += n;
                offset += n;
            }

            /* Map the section. We use the file content as the underlying storage of the map because we might be mapping parts of
             * the file left and right of the actual section. */
            if (mem_size>0) {
                MemoryMap::MapElement me(va, mem_size, &(file->get_data()[0]), offset, mapperms);
                me.set_name(melmt_name);
                map->insert(me);
            }
        }
        header->set_base_va(old_base_va);
    } catch(...) {
        header->set_base_va(old_base_va);
        throw;
    }

    if (debug) {
        fprintf(debug, "Loader: created map:\n");
        map->dump(debug, "    ");
    }
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

#if 0
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
#endif

/* class method */
int64_t
BinaryLoader::gcd(int64_t a, int64_t b, int64_t *xout/*=NULL*/, int64_t *yout/*=NULL*/)
{
    uint64_t x=0, xprev=1, y=1, yprev=0;
    if (b>a) std::swap(a, b);
    while (b!=0) {
        uint64_t quotient = a / b;
        uint64_t temp;
        
        temp = b;
        b = a % b;
        a = temp;
        
        if (xout) {
            temp = x;
            x = xprev - quotient * x;
            xprev = temp;
        }

        if (yout) {
            temp = y;
            y = yprev - quotient * y;
            yprev = temp;
        }
    }
    
    if (xout) *xout = xprev;
    if (yout) *yout = yprev;
    return a;
}

rose_addr_t
BinaryLoader::bialign(rose_addr_t val1, rose_addr_t align1, rose_addr_t val2, rose_addr_t align2)
{
    if (0==val1 % align1 && 0==val2 % align2)
        return 0;

    if (debug) fprintf(debug, "    Aligning %"PRIu64" to %"PRIu64" and %"PRIu64" to %"PRIu64"\n", val1, align1, val2, align2);

    /* Minimum amount by which the addresses must be adjusted downward to independently meet their alignment constraint. */
    int64_t Ma = val1 - ALIGN_DN(val1, align1);
    int64_t Mb = val2 - ALIGN_DN(val2, align2);
    if (debug) fprintf(debug, "      Misalignment: Ma=%"PRId64", Mb=%"PRId64"\n", Ma, Mb);

    /* Alignment constraints that must both be satisfied. */
    int64_t a = align1;
    int64_t b = align2;
    int64_t c = Mb - Ma;
    if (debug) fprintf(debug, "      Constraints:  a=%"PRId64", b=%"PRId64", c=%"PRId64"\n", a, b, c);

    /* Alignment is satsfied when:
     *
     *   x a + Ma = y b + Mb                       (1)
     *
     * Rearranging, we get the linear Biophantine equation:
     *
     *   x a + y (-b) = Mb - Ma = c                (2)
     *
     * This has solutions if and only if c is a multiple of the greatest common divisor of a and b. */
    int64_t t, u;
    int64_t g = gcd(a, b, &t, &u);
    if (debug) fprintf(debug, "      Bezout coef:  t=%"PRId64", u=%"PRId64", gcd(a,b)=%"PRId64"\n", t, u, g);
    if (c % g) {
        if (debug) fprintf(debug, "      No solutions (Mb-Ma not a multiple of gcd(a,b))\n");
        throw Exception("no solutions to alignment constraints");
    }
    ROSE_ASSERT(t*a + u*b == g);

    /* The t and u are Bezout coefficients that satisfy
     *
     *   t a + u b = g                             (3)
     *
     * By multiplying both sides by c/g we get a solution for (2) above:
     *
     *    m = c/g
     *    (m t) (a) + (m u) (-b) = c
     *
     * Substituting:
     *    x = m t
     *    y = m u
     *    c = Mb - Ma
     *
     * and rearranging gives us (1) above.  Therefore, both alignment constraints can be satisfied simultaneously
     * by subtracting a common amount, namely:
     *
     *    x a + Ma  or
     *    y b + Mb
     */
    ROSE_ASSERT(c % g == 0);
    int64_t m = c/g;
        
    /* Calculate adjustment */
    int64_t Aa = m * t * a + Ma;
    int64_t Ab = m * u * b + Mb;
    if (debug) fprintf(debug, "      Adjustment:   %"PRId64"\n", Aa);
    ROSE_ASSERT(Aa==Ab);
    ROSE_ASSERT(Aa>0); /*add multiples of lcm(a,b) to make this positive; not implemented yet [RPM 2010-09-07]*/
    return (rose_addr_t)Aa;
}

BinaryLoader::MappingContribution
BinaryLoader::align_values(SgAsmGenericSection *section, MemoryMap *map,
                           rose_addr_t *malign_lo_p, rose_addr_t *malign_hi_p,
                           rose_addr_t *va_p, rose_addr_t *mem_size_p,
                           rose_addr_t *offset_p, rose_addr_t *file_size_p,
                           rose_addr_t *va_offset_p, bool *anon_lo_p, bool *anon_hi_p, 
                           ConflictResolution *resolve_p)
{
    ROSE_ASSERT(section!=NULL);
    ROSE_ASSERT(section->is_mapped());
    SgAsmGenericHeader *header = section->get_header();
    ROSE_ASSERT(header!=NULL);
    
    /* Initial guesses */
    rose_addr_t malign_lo = std::max(section->get_mapped_alignment(), (rose_addr_t)1);
    rose_addr_t malign_hi = std::min(std::max(section->get_mapped_alignment(), (rose_addr_t)1), (rose_addr_t)4096);
    rose_addr_t va        = header->get_base_va() + section->get_mapped_preferred_rva();
    rose_addr_t mem_size  = section->get_mapped_size();
    rose_addr_t falign_lo = std::max(section->get_file_alignment(), (rose_addr_t)1);
    rose_addr_t offset    = section->get_offset();
    rose_addr_t file_size = section->get_size();

    /* Align lower end of mapped region to satisfy both memory and file alignment constraints. */
    rose_addr_t va_offset = bialign(va, malign_lo, offset, falign_lo);
    if (va_offset>va || va_offset>offset) {
        if (debug) {
            fprintf(debug, "      Adjustment %"PRIu64" exceeds va or offset (va=%"PRIu64", offset=%"PRIu64")\n",
                    va_offset, va, offset);
        }
        throw Exception("no solutions to memory/file alignment constraints");
    }
    ROSE_ASSERT((va - va_offset) % malign_lo == 0);
    ROSE_ASSERT((offset - va_offset) % falign_lo == 0);
    va -= va_offset;
    mem_size += va_offset;
    offset -= va_offset;
    file_size += va_offset;

    /* Extend mapped region to satisfy high alignment; but do not extend the file region */
    if ((va + mem_size) % malign_hi != 0) {
        uint64_t extend_by = ALIGN_UP(va+mem_size, malign_hi) - (va+mem_size);
        mem_size += extend_by;
    }

    /* Return values */
    *malign_lo_p = malign_lo;
    *malign_hi_p = malign_hi;
    *va_p = va;
    *mem_size_p = mem_size;
    *offset_p = offset;
    *file_size_p = file_size;
    *va_offset_p = va_offset;
    *anon_lo_p = true;
    *anon_hi_p = true;
    *resolve_p = RESOLVE_THROW;
    return CONTRIBUTE_ADD;
}

/* Used to be called relocateAllLibraries */
void
BinaryLoader::fixup(SgAsmInterpretation *interp)
{
    // 1. Get section map (name -> list<section*>)
    // 2. Create Symbol map from relevant sections (.dynsym)
    // 3. Create Extent sorted list of sections
    // 4. Collect Relocation Entries.
    // 5.  for each relocation entry, perform relocation
}
