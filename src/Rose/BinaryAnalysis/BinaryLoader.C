#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>                                 // for SgBinaryComposite
#include <Rose/BinaryAnalysis/BinaryLoader.h>

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElf.h>
#include <Rose/BinaryAnalysis/BinaryLoaderElfObj.h>
#include <Rose/BinaryAnalysis/BinaryLoaderPe.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Dwarf/BasicTypes.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/Diagnostics.h>

#include <SgAsmGenericDLL.h>
#include <SgAsmGenericFile.h>
#include <SgAsmGenericHeaderList.h>
#include <SgAsmGenericSectionList.h>
#include <SgAsmGenericString.h>
#include <SgAsmInterpretation.h>

using namespace Rose::Diagnostics;

namespace Rose {
namespace BinaryAnalysis {

Sawyer::Message::Facility BinaryLoader::mlog;
std::vector<BinaryLoader::Ptr> BinaryLoader::loaders_;

std::ostream&
operator<<(std::ostream &o, const BinaryLoader::Exception &e) {
    e.print(o);
    return o;
}

void
BinaryLoader::Exception::print(std::ostream &o) const {
    o <<what();
}

/* We put some initializations here in a *.C file so we don't need to recompile so much if we need to change how a
 * BinaryLoader is constructed and we're just making the change for debugging purposes. */
void
BinaryLoader::init() {
    //performingDynamicLinking(true);
    //appendDirectory("/lib32");
}

// class method
void BinaryLoader::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::BinaryLoader");
        mlog.comment("mapping files into virtual memory");
    }
}

/* class method */
void
BinaryLoader::initclass() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        initDiagnostics();
        /* Registered in order from most generic to most specific */
        registerSubclass(BinaryLoader::instance());       // generically handles all formats, albeit with limited functionality
        registerSubclass(BinaryLoaderElf::instance());    // POSIX Executable and Linkable Format (ELF)
        registerSubclass(BinaryLoaderElfObj::instance()); // ELF loader for object files
        registerSubclass(BinaryLoaderPe::instance());     // Windows Portable Executable (PE)
    }
}

/* class method */
void
BinaryLoader::registerSubclass(const Ptr &loader) {
    initclass();
    ASSERT_not_null(loader);
    loaders_.push_back(loader);
}

/* class method */
BinaryLoader::Ptr
BinaryLoader::lookup(SgAsmInterpretation *interp) {
    Ptr retval;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        Ptr candidate = lookup(headers[i]);
        if (retval && retval!=candidate)
            throw Exception("interpretation has multiple loaders");
        retval = candidate;
    }
    if (!retval)
        throw Exception("no loader for interpretation");
    return retval;
}

/* class method */
BinaryLoader::Ptr
BinaryLoader::lookup(SgAsmGenericHeader *header) {
    initclass();
    for (size_t i=loaders_.size(); i>0; --i) {
        ASSERT_not_null(loaders_[i-1]);
        if (loaders_[i-1]->canLoad(header))
            return loaders_[i-1];
    }
    throw Exception("no loader for architecture");
}

/* class method */
void
BinaryLoader::load(SgBinaryComposite *composite, bool read_executable_file_format_only) {
    /* Parse the initial binary file to create an AST and the initial SgAsmInterpretation(s). */
    ASSERT_require(composite->get_genericFileList()->get_files().empty());
    SgAsmGenericFile *file = createAsmAST(composite, composite->get_sourceFileNameWithPath());
    ASSERT_always_not_null(file);

    /* Find an appropriate loader for each interpretation and parse, map, link, and/or relocate each interpretation as
     * specified by the loader properties. */
    const SgAsmInterpretationPtrList &interps = composite->get_interpretations()->get_interpretations();
    for (size_t i=0; i<interps.size(); i++) {
        Ptr loader = lookup(interps[i])->clone(); /* clone so we can change properties locally */
        if (read_executable_file_format_only) {
            loader->performingDynamicLinking(false);
            loader->performingRemap(false);
            loader->performingRelocations(false);
        } else {
            loader->performingDynamicLinking(false);
            loader->performingRemap(true);
            loader->performingRelocations(false);
        }

        loader->load(interps[i]);
    }
}

void
BinaryLoader::load(SgAsmInterpretation *interp) {
    if (performingDynamicLinking())
        link(interp);
    if (performingRemap())
        remap(interp);
    if (performingRelocations())
        fixup(interp);
}

std::string
BinaryLoader::findSoFile(const std::string &libname) const {
    mlog[TRACE] <<"find library=" <<libname <<"\n";
    if (!libname.empty() && '/'==libname[0])
        return libname;
    for (std::vector<std::string>::const_iterator di=directories_.begin(); di!=directories_.end(); ++di) {
        mlog[TRACE] <<"  looking in " <<*di <<"\n";
        std::string libpath = *di + "/" + libname;
        struct stat sb;
#ifndef _MSC_VER
        if (stat(libpath.c_str(), &sb)>=0 && S_ISREG(sb.st_mode) && access(libpath.c_str(), R_OK)>=0) {
            mlog[TRACE] <<"    found.\n";
            return libpath;
        }
#endif
    }
    if (mlog[TRACE]) {
        if (directories_.empty())
            mlog[TRACE] <<"no search directories\n";
        mlog[TRACE] <<"  not found; throwing exception.\n";
    }
    throw Exception("cannot find file for library: " + libname);
}

bool
BinaryLoader::isLinked(SgBinaryComposite *composite, const std::string &filename) {
    const SgAsmGenericFilePtrList &files = composite->get_genericFileList()->get_files();
    for (SgAsmGenericFilePtrList::const_iterator fi=files.begin(); fi!=files.end(); ++fi) {
        if ((*fi)->get_name()==filename)
            return true;
    }
    return false;
}

bool
BinaryLoader::isLinked(SgAsmInterpretation *interp, const std::string &filename) {
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        SgAsmGenericFile *file = (*hi)->get_file();
        ASSERT_not_null(file);
        if (file->get_name()==filename)
            return true;
    }
    return false;
}


/* once called loadInterpLibraries */
void
BinaryLoader::link(SgAsmInterpretation* interp) {
    ASSERT_not_null(interp);
    SgBinaryComposite *composite = SageInterface::getEnclosingNode<SgBinaryComposite>(interp);
    ASSERT_not_null(composite);

    /* Make sure the pre-load objects are parsed and linked into the AST. */
    for (std::vector<std::string>::const_iterator pi=preloads_.begin(); pi!=preloads_.end(); ++pi) {
        mlog[TRACE] <<"preload object " <<*pi <<"\n";
        std::string filename = findSoFile(*pi);
        if (isLinked(composite, filename)) {
            mlog[TRACE] <<filename <<" is already parsed.\n";
        } else {
            Stream m1(mlog[TRACE] <<"parsing " <<filename);
            createAsmAST(composite, filename);
            m1 <<"... done.\n";
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
            mlog[TRACE] <<"library " <<*di <<" needed by " <<header_name <<"\n";
            std::string filename = findSoFile(*di);
            if (isLinked(composite, filename)) {
                mlog[TRACE] <<filename <<" is already parsed.\n";
            } else {
                Stream m1(mlog[TRACE] <<"parsing " <<filename);
                SgAsmGenericFile *new_file = createAsmAST(composite, filename);
                m1 <<"... done.\n";
                ASSERT_not_null2(new_file, "createAsmAST failed");
                SgAsmGenericHeaderPtrList new_hdrs = findSimilarHeaders(header, new_file->get_headers()->get_headers());
                unresolved_hdrs.insert(unresolved_hdrs.end(), new_hdrs.begin(), new_hdrs.end());
            }
        }
    }
}

/* class method */
SgAsmGenericHeaderPtrList
BinaryLoader::findSimilarHeaders(SgAsmGenericHeader *match, SgAsmGenericHeaderPtrList &candidates) {
    ASSERT_not_null(match);

    SgAsmGenericHeaderPtrList retval;
    auto arch1 = Architecture::findByHeader(match).orThrow();

    for (auto candidate: candidates) {
        ASSERT_not_null(candidate);
        if (match->class_name() == candidate->class_name()) {
            auto arch2 = Architecture::findByHeader(candidate).orThrow();
            if (arch1 == arch2)
                retval.push_back(candidate);
        }
    }
    return retval;
}

/* class method */
bool
BinaryLoader::isHeaderSimilar(SgAsmGenericHeader *h1, SgAsmGenericHeader *h2) {
    /* This is implemented in terms of findSimilarHeaders() rather than vice versa so that findSimilarHeaders() does not need
     * to call Disassembler::lookup() so often. */
    SgAsmGenericHeaderPtrList h2list;
    h2list.push_back(h2);
    return !findSimilarHeaders(h1, h2list).empty();
}


/* class method */
SgAsmGenericFile*
BinaryLoader::createAsmAST(SgBinaryComposite* binaryFile, std::string filePath) {
    ASSERT_forbid(filePath.empty());

    SgAsmGenericFile* file = SgAsmExecutableFileFormat::parseBinaryFormat(filePath.c_str());
    ASSERT_not_null(file);

    // TODO do I need to attach here - or can I do after return
    binaryFile->get_genericFileList()->get_files().push_back(file);
    file->set_parent(binaryFile->get_genericFileList());

    /* Add a new interpretation to the SgBinaryComposite object for each header of the newly parsed
     * SgAsmGenericFile for which a suitable interpretation does not already exist. */
    const SgAsmGenericHeaderPtrList &headers = file->get_headers()->get_headers();
    SgAsmInterpretationPtrList &interps = binaryFile->get_interpretations()->get_interpretations();
    for (size_t i = 0; i < headers.size(); ++i) {
        SgAsmGenericHeader* header = headers[i];
        SgAsmInterpretation* interp = NULL;
        for (size_t j = 0; j < interps.size(); ++j) {
            ASSERT_forbid(interps[j]->get_headers()->get_headers().empty());
            SgAsmGenericHeader* interpHeader = interps[j]->get_headers()->get_headers().front();
            if (isHeaderSimilar(header, interpHeader)) {
                interp = interps[j];
                break;
            }
        }
        if (!interp) {
            interp = new SgAsmInterpretation();
            interps.push_back(interp);
            interp->set_parent(binaryFile->get_interpretations());
        }
        interp->get_headers()->get_headers().push_back(header);
    }

    /* Parse Dwarf info and add it to the SgAsmGenericFile. */
    Dwarf::parse(file);

    return file;
}

/* Used to be called layoutInterpLibraries */
void
BinaryLoader::remap(SgAsmInterpretation* interp) {
    /* Make sure we have a valid memory map. It is permissible for the caller to have reserved some space already. */
    MemoryMap::Ptr map = interp->get_map();
    if (!map)
        interp->set_map(map = MemoryMap::instance());

    /* Process each file header in the order it appears in the AST. This is also the order that the link() method parsed
     * dependencies (usually by a breadth-first search). */
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (auto header: headers)
        remap(map, header);
}

/* Maps the sections of a single header. */
void
BinaryLoader::remap(MemoryMap::Ptr &map, SgAsmGenericHeader *header) {
    SgAsmGenericFile *file = header->get_file();
    ASSERT_not_null(file);

    Stream trace(mlog[TRACE]);

    trace <<"remapping sections of " <<header->get_file()->get_name() <<"\n";
    SgAsmGenericSectionPtrList sections = getRemapSections(header);

    Address old_base_va = header->get_baseVa();
    Address new_base_va = rebase(map, header, sections);
    if (new_base_va != old_base_va) {
        trace <<"  temporarily rebasing header from " <<StringUtility::addrToString(old_base_va)
              <<" to " <<StringUtility::addrToString(new_base_va) <<"\n";
        header->set_baseVa(new_base_va);
    }

    try {
        for (auto section: sections) {
            section->set_mappedActualVa(0); /*reset in case previously mapped*/
            unsigned mapperms = mappingPermissions(section);

            if (trace) {
                trace <<"  mapping section [" <<section->get_id() <<"] \"" <<section->get_name()->get_string(true) <<"\"";
                if (section->get_baseVa()!=0)
                    trace <<" with base va " <<StringUtility::addrToString(section->get_baseVa());
                trace <<"\n";
                trace <<"    specified RVA:       " <<StringUtility::addrToString(section->get_mappedPreferredRva())
                      <<" + " <<StringUtility::addrToString(section->get_mappedSize()) <<" bytes"
                      <<" = " <<StringUtility::addrToString(section->get_mappedPreferredRva()+section->get_mappedSize())
                      <<"\n";
                if (section->get_baseVa()!=0) {
                    trace <<"    specified  VA:       "
                          <<StringUtility::addrToString(section->get_baseVa() + section->get_mappedPreferredRva()) <<" + "
                          <<StringUtility::addrToString(section->get_mappedSize()) <<" bytes = "
                          <<StringUtility::addrToString(section->get_baseVa() + section->get_mappedPreferredRva() +
                                                        section->get_mappedSize()) <<"\n";
                }
                trace <<"    specified offset:    "
                      <<StringUtility::addrToString(section->get_offset()) <<" + "
                      <<StringUtility::addrToString(section->get_size()) <<" bytes = "
                      <<StringUtility::addrToString(section->get_offset()+section->get_size()) <<"\n";
                trace <<"    specified alignment: memory="
                      <<StringUtility::addrToString(section->get_mappedAlignment()) <<", file="
                      <<StringUtility::addrToString(section->get_fileAlignment()) <<"\n";
            }

            // Initial guesses
            Address malign_lo = std::max(section->get_mappedAlignment(), (Address)1);
            Address malign_hi = std::min(std::max(section->get_mappedAlignment(), (Address)1), (Address)4096);
            Address va        = header->get_baseVa() + section->get_mappedPreferredRva();
            Address mem_size  = section->get_mappedSize();
            Address offset    = section->get_offset();
            Address file_size = section->get_size();

            /* Figure out alignment, etc. */
            Address va_offset=0;
            bool anon_lo=true, anon_hi=true, map_private=false;
            ConflictResolution resolve = RESOLVE_THROW;
            MappingContribution contrib = alignValues(section, map,                      /* inputs */
                                                      &malign_lo, &malign_hi,            /* alignment outputs */
                                                      &va, &mem_size,                    /* memory location outputs */
                                                      &offset, &file_size, &map_private, /* file location outputs */
                                                      &va_offset, &anon_lo, &anon_hi,    /* internal location outputs */
                                                      &resolve);                         /* conflict resolution output */
            Address falign_lo = std::max(section->get_fileAlignment(), (Address)1);
            Address falign_hi = falign_lo;

            if (trace) {
                if (CONTRIBUTE_NONE==contrib || 0==mem_size) {
                    trace <<"    does not contribute to map\n";
                } else {
                    trace <<"    adjusted alignment:  memory=["
                          <<StringUtility::addrToString(malign_lo) <<".."
                          <<StringUtility::addrToString(malign_hi) <<"], file=["
                          <<StringUtility::addrToString(falign_lo) <<".."
                          <<StringUtility::addrToString(falign_hi) <<"]\n";
                    trace <<"    aligned VA:          "
                          <<StringUtility::addrToString(va) <<" + "
                          <<StringUtility::addrToString(mem_size) <<" bytes = "
                          <<StringUtility::addrToString(va+mem_size);
                    if (section->get_baseVa()+section->get_mappedPreferredRva()==va &&
                        section->get_mappedSize()==mem_size) {
                        trace <<" (no change)\n";
                    } else {
                        trace <<"\n";
                    }
                    if (va < new_base_va) {
                        trace <<"    WARNING: aligned va " <<StringUtility::addrToString(va) <<" is less than "
                              <<(new_base_va==old_base_va?"":"temporary ") <<"base va "
                              <<StringUtility::addrToString(new_base_va) <<"\n";
                    }
                    if (CONTRIBUTE_ADD==contrib) {
                        trace <<"    aligned offset:      " <<StringUtility::addrToString(offset) <<" + "
                              <<StringUtility::addrToString(file_size) <<" bytes = "
                              <<StringUtility::addrToString(offset+file_size)
                              <<(section->get_offset()==offset && section->get_size()==file_size ? " (no change)\n" : "\n");
                        trace <<"    permissions:         "
                              <<((mapperms & MemoryMap::READABLE)   ? "r" : "-")
                              <<((mapperms & MemoryMap::WRITABLE)   ? "w" : "-")
                              <<((mapperms & MemoryMap::EXECUTABLE) ? "x" : "-");
                        if ((mapperms & ~MemoryMap::READ_WRITE_EXECUTE) != 0)
                            mfprintf(trace)("0x%x", (mapperms & ~MemoryMap::READ_WRITE_EXECUTE));
                        trace <<"\n";
                        trace <<"    internal offset:     " <<StringUtility::addrToString(va_offset)
                              <<" (va " <<StringUtility::addrToString(va+va_offset) <<")\n";
                    }
                }
            }

            /* Sanity checks */
            if (CONTRIBUTE_NONE==contrib || 0==mem_size)
                continue;
            ASSERT_require(va_offset<mem_size);
            if (file_size>mem_size) file_size = mem_size;
            ASSERT_require(va + va_offset >= header->get_baseVa());
            if (trace) {
                trace <<"    current memory map (before we map this section)\n";
                map->dump(trace, "        ");
            }

            /* Erase part of the mapping? */
            if (CONTRIBUTE_SUB==contrib) {
                trace <<"    subtracting contribution\n";
                map->erase(AddressInterval::baseSize(va, mem_size));
                continue;
            }

            // Resole mapping conflicts
            if (map->within(AddressInterval::baseSize(va, mem_size)).exists()) {
                switch (resolve) {
                    case RESOLVE_THROW:
                        throw MemoryMap::NotMapped("cannot map segment", map, va);
                    case RESOLVE_OVERMAP:
                        trace <<"    conflict: resolved by making a hole\n";
                        map->erase(AddressInterval::baseSize(va, mem_size));
                        break;
                    case RESOLVE_REMAP:
                    case RESOLVE_REMAP_ABOVE: {
                        trace <<"    unable to map entire desired region.\n";
                        AddressInterval where = AddressInterval::hull(RESOLVE_REMAP_ABOVE==resolve ? va : 0,
                                                                      AddressInterval::whole().greatest());
                        Address new_va = 0;
                        if (!map->findFreeSpace(mem_size, malign_lo, where).assignTo(new_va)) {
                            throw MemoryMap::NoFreeSpace("unable to allocate space in specimen memory map",
                                                         map, mem_size);
                        }
                        ASSERT_require2(0 == (new_va+mem_size) % malign_hi, "FIXME: not handled yet [RPM 2010-09-03]");
                        va = new_va;
                        trace <<"    relocated to VA:     " <<StringUtility::addrToString(va) <<" + "
                              <<StringUtility::addrToString(mem_size) <<" bytes = "
                              <<StringUtility::addrToString(va + mem_size) <<"\n";
                        break;
                    }
                }
            }

            /* Save the virtual address where this section is (will be) mapped.  When a section is mapped more than once
             * (perfectly legal to do so) only the last mapping is saved. */
            section->set_mappedActualVa(va + va_offset);

            /* Segment name for debugging. This is the file base name and section name concatenated. */
            std::string::size_type file_basename_pos = file->get_name().find_last_of("/");
            file_basename_pos = file_basename_pos==file->get_name().npos ? 0 : file_basename_pos+1;
            std::string melmt_name = file->get_name().substr(file_basename_pos) + "(" + section->get_name()->get_string() + ")";
            trace <<"    map element name: " <<escapeString(melmt_name) <<"\n";

            /* Anonymously map the part of memory beyond the physical end of the file */
            SgAsmGenericFile *file = section->get_file();
            Address total = file->get_data().size(); /*total size of file*/
            if (offset+mem_size > total) {
                Address n, a;
                if (offset >= total) {
                    /* starts beyond EOF */
                    n = mem_size;
                    a = va;
                } else {
                    /* overlaps EOF */
                    n = (offset + mem_size) - total;
                    a = va + total - offset;
                }
                trace <<"    mapping part beyond EOF(" <<StringUtility::addrToString(total) <<"):      "
                      <<"va=" <<StringUtility::addrToString(a) <<" + " <<StringUtility::addrToString(n) <<" = "
                      <<StringUtility::addrToString(a+n) <<"\n";
                map->insert(AddressInterval::baseSize(a, n),
                            MemoryMap::Segment::anonymousInstance(n, mapperms|MemoryMap::PRIVATE, melmt_name));
                mem_size -= n;
                file_size = std::min(file_size, mem_size);
            }

            /* Anonymously map the part of memory beyond the part of file */
            if (anon_hi && mem_size>file_size) {
                Address n = mem_size - file_size;
                Address a = va + file_size;
                trace <<"    mapping part beyond end of section:        va="
                      <<StringUtility::addrToString(a) <<" + " <<StringUtility::addrToString(n) <<" = "
                      <<StringUtility::addrToString(a+n) <<"\n";
                map->insert(AddressInterval::baseSize(a, n),
                            MemoryMap::Segment::anonymousInstance(n, mapperms|MemoryMap::PRIVATE, melmt_name));
                mem_size -= n;
            }

            /* Anonymously map the part of memory before the section */
            if (anon_lo && va_offset>0 && mem_size>0) {
                Address n = va_offset - va;
                Address a = va;
                trace <<"    mapping part before beginning of section: va="
                      <<StringUtility::addrToString(a) <<" + " <<StringUtility::addrToString(n) <<" = "
                      <<StringUtility::addrToString(a+n) <<"\n";
                map->insert(AddressInterval::baseSize(a, n),
                            MemoryMap::Segment::anonymousInstance(n, mapperms|MemoryMap::PRIVATE, melmt_name));
                mem_size -= n;
                file_size -= n;
                va += n;
                offset += n;
            }

            /* Map the section. We use the file content as the underlying storage of the map because we might be mapping parts of
             * the file left and right of the actual section. */
            if (mem_size>0) {
                trace <<"    mapping section:                          va="
                      <<StringUtility::addrToString(va) <<" + " <<StringUtility::addrToString(mem_size) <<" = "
                      <<StringUtility::addrToString(va+mem_size) <<" "
                      <<(map_private?"private":"shared") <<"\n";
                if (map_private) {
                    map->insert(AddressInterval::baseSize(va, mem_size),
                                MemoryMap::Segment::anonymousInstance(mem_size, mapperms|MemoryMap::PRIVATE,
                                                                      melmt_name));
                    map->at(va).limit(mem_size).write(&file->get_data()[offset]);
                } else {
                    // Create the buffer, but the buffer should not take ownership of data from the file.
                    map->insert(AddressInterval::baseSize(va, mem_size),
                                MemoryMap::Segment(MemoryMap::StaticBuffer::instance(&file->get_data()[0],
                                                                                     file->get_data().size()),
                                                   offset, mapperms, melmt_name));
                }
            }

            if (trace) {
                trace <<"    after mapping this section:\n";
                map->dump(trace, "      ");
            }
        }
        header->set_baseVa(old_base_va);
    } catch(...) {
        header->set_baseVa(old_base_va);
        throw;
    }
}

void
BinaryLoader::addSectionsForRemap(SgAsmGenericHeader* header, SgAsmGenericSectionPtrList &allSections) {
    allSections.insert(allSections.end(),
                       header->get_sections()->get_sections().begin(),
                       header->get_sections()->get_sections().end());
}

std::vector<std::string>
BinaryLoader::dependencies(SgAsmGenericHeader *header) {
    ASSERT_not_null(header);
    std::vector<std::string> retval;
    const SgAsmGenericDLLPtrList &dlls = header->get_dlls();
    for (auto dll: dlls)
        retval.push_back(dll->get_name()->get_string());
    return retval;
}

/* class method */
int64_t
BinaryLoader::gcd(int64_t a, int64_t b, int64_t *xout/*=NULL*/, int64_t *yout/*=NULL*/) {
    uint64_t x=0, xprev=1, y=1, yprev=0;
    bool swapped = false;
    if (b>a) {
        std::swap(a, b);
        swapped = true;
    }

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

    if (swapped)
        std::swap(xprev, yprev);

    if (xout) *xout = xprev;
    if (yout) *yout = yprev;
    return a;
}

Address
BinaryLoader::bialign(Address val1, Address align1, Address val2, Address align2) {
    using namespace StringUtility;

    if (0==val1 % align1 && 0==val2 % align2)
        return 0;
    Stream trace(mlog[TRACE]);

    /* Minimum amount by which the addresses must be adjusted downward to independently meet their alignment constraint. */
    int64_t Ma = val1 - alignDown(val1, align1);
    int64_t Mb = val2 - alignDown(val2, align2);
    if (Ma > Mb) {
        std::swap(val1, val2);
        std::swap(align1, align2);
        std::swap(Ma, Mb);
    }

    SAWYER_MESG(trace) <<"    aligning " <<addrToString(val1) <<" to " <<addrToString(align1)
                       <<" and " <<addrToString(val2) <<" to " <<addrToString(align2) <<"\n"
                       <<"      misalignment: Ma=" <<addrToString(Ma) <<", Mb=" <<addrToString(Mb) <<"\n";

    /* Alignment constraints that must both be satisfied. */
    int64_t a = align1;
    int64_t b = align2;
    int64_t c = Mb - Ma;
    SAWYER_MESG(trace) <<"      constraints:  a=" <<addrToString(a) <<", b=" <<addrToString(b) <<", c=" <<addrToString(c) <<"\n";

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
    SAWYER_MESG(trace) <<"      bezout coef:  t=" <<addrToString(t) <<", u=" <<addrToString(u)
                       <<", gcd(a,b)=" <<addrToString(g) <<"\n";
    if (c % g) {
        trace <<"      no solutions (Mb-Ma not a multiple of gcd(a,b))\n";
        throw Exception("no solutions to alignment constraints");
    }
    ASSERT_require(t*a + u*b == g);

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
    ASSERT_require(c % g == 0);
    int64_t m = c/g;

    /* Calculate adjustment */
    int64_t Aa = m * t * a + Ma;
    int64_t Ab = m * u * b + Mb;
    SAWYER_MESG(trace) <<"      adjustment:   " <<addrToString(Aa) <<"\n";
    ASSERT_always_require(Aa==Ab);
    ASSERT_always_require2(Aa>0, "FIXME[Robb Matzke 2010-09-07]: add multiples of lcm(a,b) to make this positive");
    return (Address)Aa;
}

BinaryLoader::MappingContribution
BinaryLoader::alignValues(SgAsmGenericSection *section, const MemoryMap::Ptr&,
                          Address *malign_lo_p, Address *malign_hi_p,
                          Address *va_p, Address *mem_size_p,
                          Address *offset_p, Address *file_size_p, bool *map_private_p,
                          Address *va_offset_p, bool *anon_lo_p, bool *anon_hi_p,
                          ConflictResolution *resolve_p) {
    ASSERT_not_null(section);
    ASSERT_require2(section->isMapped(), "section must be mapped to virtual memory");
    ASSERT_not_null(malign_lo_p);
    ASSERT_not_null(malign_hi_p);
    ASSERT_not_null(va_p);
    ASSERT_not_null(mem_size_p);
    ASSERT_not_null(offset_p);
    ASSERT_not_null(file_size_p);
    ASSERT_not_null(anon_lo_p);
    ASSERT_not_null(anon_hi_p);
    ASSERT_not_null(resolve_p);

    SgAsmGenericHeader *header = isSgAsmGenericHeader(section);
    if (!header) header = section->get_header();
    ASSERT_not_null(header);

    /* Initial guesses */
    Address malign_lo = *malign_lo_p;
    Address malign_hi = *malign_hi_p;
    Address va = *va_p;
    Address mem_size = *mem_size_p;
    Address offset = *offset_p;
    Address falign_lo = std::max(section->get_fileAlignment(), (Address)1);
    Address file_size = *file_size_p;

    /* Align lower end of mapped region to satisfy both memory and file alignment constraints. */
    Address va_offset = bialign(va, malign_lo, offset, falign_lo);
    if (va_offset>va || va_offset>offset) {
        mlog[TRACE] <<"      adjustment " <<va_offset <<" exceeds va or offset (va=" <<va
                    <<", offset=" <<offset <<")\n";
        throw Exception("no solutions to memory/file alignment constraints");
    }
    ASSERT_require((va - va_offset) % malign_lo == 0);
    ASSERT_require((offset - va_offset) % falign_lo == 0);
    va -= va_offset;
    mem_size += va_offset;
    offset -= va_offset;
    file_size += va_offset;

    /* Extend mapped region to satisfy high alignment; but do not extend the file region */
    if ((va + mem_size) % malign_hi != 0) {
        uint64_t extend_by = alignUp(va+mem_size, malign_hi) - (va+mem_size);
        mem_size += extend_by;
    }

    /* Return values */
    *malign_lo_p = malign_lo;
    *malign_hi_p = malign_hi;
    *va_p = va;
    *mem_size_p = mem_size;
    *offset_p = offset;
    *file_size_p = file_size;
    *map_private_p = false;
    *va_offset_p = va_offset;
    *anon_lo_p = true;
    *anon_hi_p = true;
    *resolve_p = RESOLVE_THROW;
    return CONTRIBUTE_ADD;
}

unsigned
BinaryLoader::mappingPermissions(SgAsmGenericSection *section) const {
    unsigned mapperms=0;
    if (section->get_mappedReadPermission())
        mapperms |= MemoryMap::READABLE;
    if (section->get_mappedWritePermission())
        mapperms |= MemoryMap::WRITABLE;
    if (section->get_mappedExecutePermission())
        mapperms |= MemoryMap::EXECUTABLE;
    return mapperms;
}

/* Used to be called relocateAllLibraries */
void
BinaryLoader::fixup(SgAsmInterpretation*, FixupErrors*) {
    // 1. Get section map (name -> list<section*>)
    // 2. Create Symbol map from relevant sections (.dynsym)
    // 3. Create Extent sorted list of sections
    // 4. Collect Relocation Entries.
    // 5. For each relocation entry, perform relocation
}

} // namespace
} // namespace

#endif
