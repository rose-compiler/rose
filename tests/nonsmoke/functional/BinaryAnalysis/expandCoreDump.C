/* Core dumps on Linux don't include the contents of read-only memory. The ELF Segment Table contains an entry for the
 * read-only memory, but the contents of the memory is not saved in the core file.  Instead, the core file has a PT_NOTE
 * segment that has a type 3 "CORE" note containing the base name of the corresponding executable and GDB and related tools
 * use that information to locate the contents of read-only segments.
 *
 * The purpose of this program is to read the core file and corresponding executable file and generate a new core file that
 * includes the contents of read-only sections.  Data from the executable file's segments are copied into the core file. The
 * core file is also given an ELF Section Table and sections whose data was copied from the executable file are also defined
 * in the core file.  This new core file can then be processed by ROSE.
 *
 * Usage:
 *     expandCoreDump core a.out
 *
 * Generates a new core file called "core.new" */
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <inttypes.h>

static const char *arg0;

/* Returns true if the specified section belongs to a core dump. */
static bool is_core_dump(SgAsmGenericSection *section) {
    if (!section)
        return false;
    SgAsmGenericHeader *header = isSgAsmGenericHeader(section);
    if (!header) {
        header = section->get_header();
        ROSE_ASSERT(header!=NULL);
    }
    return header->get_exec_format()->get_purpose() == SgAsmGenericFormat::PURPOSE_CORE_DUMP;
}

/* Returns true if any node in the tree has the isModified bit set. */
static bool is_modified(SgNode *node) {
    class T1 : public SgSimpleProcessing {
    public:
        bool modified;
        T1(SgNode *node) 
            : modified(false) {
            traverse(node, preorder);
        }
        void visit(SgNode *node) {
            if (node->get_isModified())
                modified = true;
        }
    };
    T1 t1(node);
    return t1.modified;
};

/* Returns the number of bytes that two section overlap in virtual address space. */
static rose_addr_t overlaps(const SgAsmElfSection *s1, const SgAsmElfSection *s2) {
    if (!s1->is_mapped() || !s2->is_mapped())
        return 0;
    if (s1->get_mapped_preferred_rva()>s2->get_mapped_preferred_rva())
        std::swap(s1, s2); /*s1 always starts before s2*/

    rose_addr_t s1_begin = s1->get_mapped_preferred_rva();
    rose_addr_t s2_begin = s2->get_mapped_preferred_rva();
    rose_addr_t s1_end = s1_begin + s1->get_mapped_size();
    rose_addr_t s2_end = s2_begin + s2->get_mapped_size();

    if (s1_end <= s2_begin)
        return 0;
    if (s1_end <= s2_end)
        return s1_end - s2_begin;
    return s2->get_mapped_size();
}

/* Print generic information about a section. */
static void show_section(FILE *f, const char *prefix, SgAsmElfSection *section) {
    /* File addresses */
    fprintf(f, "%s", prefix);
    fprintf(f, "%c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
            section->get_file_alignment()==0 || section->get_offset()%section->get_file_alignment()==0?' ':'!',
            section->get_offset(), section->get_size(), section->get_offset()+section->get_size());

    /* Mapped addresses */
    if (section->is_mapped()) {
        fprintf(f, " %c0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64 " 0x%08" PRIx64,
                (section->get_mapped_alignment()==0 ||
                 section->get_mapped_preferred_rva()%section->get_mapped_alignment()==0
                 ? ' ' : '!'),
                section->get_base_va(), section->get_mapped_preferred_rva(), section->get_mapped_size(),
                section->get_mapped_preferred_rva()+section->get_mapped_size());
    } else {
        fprintf(f, " %*s", 4*11, "");
    }

    /* Permissions */
    if (section->is_mapped()) {
        fprintf(f, " %c%c%c ",
                section->get_mapped_rperm()?'r':'-',
                section->get_mapped_wperm()?'w':'-', 
                section->get_mapped_xperm()?'x':'-');
    } else {
        fputs("     ", f);
    }

    /* Section ID, name */
    if (section->get_id()>=0) {
        fprintf(f, " %3d", section->get_id());
    } else {
        fputs("    ", f);
    }
    fprintf(f, " %s\n", section->get_name()->get_string(true).c_str());
}
    
/* Finds the ELF header of the core file and the mapped sections/segments of the non-core files. */
class SectionDictionary : public SgSimpleProcessing {
public:
    SgAsmElfFileHeader *core_header;                    /* ELF header for the core file */
    SgAsmElfSectionTable *section_table;                /* Core file section table (probably not present) */
    SgAsmElfStringSection *shstrtab;                    /* Core file string table for section names (".shstrtab" section) */
    std::vector<SgAsmElfSection*> sections;             /* Mapped sections/segments for the non-core files */

    SectionDictionary(SgProject *project)
        : core_header(NULL), section_table(NULL), shstrtab(NULL) {
        traverse(project, preorder);
    }
private:
    void visit(SgNode *node) {
        SgAsmElfFileHeader *header = isSgAsmElfFileHeader(node);
        SgAsmElfSection *section = isSgAsmElfSection(node);

        if (header && header->get_exec_format()->get_purpose() == SgAsmGenericFormat::PURPOSE_CORE_DUMP) {
            if (core_header) {
                fprintf(stderr, "%s: only one core file should be specified on the command line\n", arg0);
                exit(1);
            }
            core_header = header;
        }

        if (is_core_dump(section)) {
            if (isSgAsmElfStringSection(section) && section->get_name()->get_string()==".shstrtab")
                shstrtab = isSgAsmElfStringSection(section);
            if (isSgAsmElfSectionTable(section))
                section_table = isSgAsmElfSectionTable(section);
        } else if (section && section->is_mapped()) {
            sections.push_back(section);
        }
    }
};

/* Modifies the core file AST to include data for the read-only segments. */
class ExpandCoreDump : public SgSimpleProcessing {
public:
    ExpandCoreDump(SectionDictionary *sd)
        : sd(sd) {
        ctor();
    }
private:
    typedef std::map<SgAsmElfSection*, rose_addr_t> CopiedSections; /*addr is section's mapped virtual address in core file*/
    CopiedSections copied;                      /* The sections that have been copied into the core file. */
    SectionDictionary *sd;                      /* Dictionary provided to the constructor. */

    void ctor() {
        traverse(sd->core_header, preorder);    /* Adds data to the core segments that have no data. */
        update_header();
        update_section_table();
    }

    void update_header() {
        /* Fix the entry address of the core file to match that of the executable. Core files generally have a null entry
         * address. */
        rose_addr_t entry_rva = sd->sections.front()->get_elf_header()->get_entry_rva();
        sd->core_header->get_entry_rvas()[0] = entry_rva;
    }

    void visit(SgNode *node) {
        SgAsmElfSection *core_section = isSgAsmElfSection(node);
        if (core_section && core_section->is_mapped() && core_section->get_size()==0 && core_section->get_mapped_size()>0) {
            show_section(stdout, "core section: ", core_section);

            /* Find all other sections (candidates) that overlap with the core_section under consideration. Also find the
             * maximum rva (high_water) where core_section overlaps with the other sections (actually, the first rva past the
             * overlapping region). The high_water value will be used to calculate the size of the vector that stores the new
             * contents of the section. */
            std::vector<SgAsmElfSection*> candidates;
            rose_addr_t high_water = 0;
            for (size_t i=0; i<sd->sections.size(); i++) {
                SgAsmElfSection *other_section=sd->sections[i];
                if (overlaps(other_section, core_section)) {
                    candidates.push_back(other_section);
                    rose_addr_t hi_overlap_addr = std::min((other_section->get_mapped_preferred_rva()+
                                                            other_section->get_mapped_size()), 
                                                           (core_section->get_mapped_preferred_rva()+
                                                            core_section->get_mapped_size()));
                    high_water = std::max(high_water, hi_overlap_addr);
                }
            }

            /* Modify the core section by creating file storage */
            if (candidates.size()>0) {
                assert(high_water > core_section->get_mapped_preferred_rva());
                core_section->set_offset(core_section->get_file()->get_current_size());
                core_section->set_size(high_water - core_section->get_mapped_preferred_rva());
                show_section(stdout, "     resized: ", core_section);

                /* Copy sections of data from executable file into core file.  Remember (in "copied" map) the sections that we
                 * copied into the core file in their entirety--we'll also add them to the core file's ELF Section Table later. */
                unsigned char *dst_content = core_section->writable_content(high_water);
                for (size_t i=0; i<candidates.size(); i++) {
                    SgAsmElfSection *src = candidates[i];
                    show_section(stdout, "         src: ", src);

                    rose_addr_t src_offset, dst_offset;
                    rose_addr_t nbytes = overlaps(core_section, src);
                    if (src->get_mapped_preferred_rva() <= core_section->get_mapped_preferred_rva()) {
                        src_offset = core_section->get_mapped_preferred_rva() - src->get_mapped_preferred_rva();
                        dst_offset = 0;
                    } else {
                        src_offset = 0;
                        dst_offset = src->get_mapped_preferred_rva() - core_section->get_mapped_preferred_rva();
                    }
                    printf("               copying from src offset 0x%" PRIx64
                           " to dst offset 0x%" PRIx64 " for 0x%" PRIx64 " bytes\n",
                           src_offset, dst_offset, nbytes);
                    src->read_content_local(src_offset, dst_content+dst_offset, nbytes, false);
                    if (src->get_section_entry()!=NULL && src_offset==0 && src->get_size()==nbytes)
                        copied.insert(CopiedSections::value_type(src, core_section->get_mapped_preferred_rva()+dst_offset));
                }
            }
        }
    }
    
    /* Creates the ELF Section Table and populates it with copies of the sections whose data were copied into the core file. */
    void update_section_table() {
        if (copied.size()==0)
            return;

        /* Create the string table for section names */
        if (NULL==sd->shstrtab) {
            sd->shstrtab = new SgAsmElfStringSection(sd->core_header);
            sd->shstrtab->get_name()->set_string(".shstrtab");
        }

        /* Create the section table */
        if (NULL==sd->section_table) {
            sd->section_table = new SgAsmElfSectionTable(sd->core_header);
            sd->section_table->add_section(sd->shstrtab);
        }
        
        /* Create new sections and initialize with values from the executable file(s). */
        for (CopiedSections::iterator i=copied.begin(); i!=copied.end(); i++) {
            SgAsmElfSection *src = i->first;
            rose_addr_t mapped_rva = i->second;
            SgAsmElfSection *dst = new SgAsmElfSection(sd->core_header);
            dst->set_offset(sd->core_header->get_file()->get_current_size());
            dst->set_size(src->get_size());
            dst->set_file_alignment(src->get_file_alignment());
            src->read_content_local(0, dst->writable_content(src->get_size()), src->get_size(), false);
            dst->set_purpose(src->get_purpose());
            dst->get_name()->set_string(src->get_name()->get_string());
            dst->set_mapped_preferred_rva(mapped_rva);
            dst->set_mapped_size(src->get_size());
            dst->set_mapped_alignment(src->get_mapped_alignment());
            dst->set_mapped_rperm(src->get_mapped_rperm());
            dst->set_mapped_wperm(src->get_mapped_wperm());
            dst->set_mapped_xperm(src->get_mapped_xperm());
            sd->section_table->add_section(dst);
        }
    }
};
        
int main(int argc, char *argv[]) {
    arg0 = argv[0];

    /* Fix up arguments before calling the front end. */
    char **tmp_argv = (char**)calloc(argc+2, sizeof(*argv));
    ASSERT_always_not_null(tmp_argv);
    memcpy(tmp_argv+1, argv, argc*sizeof(*argv));
    tmp_argv[0] = argv[0];
    tmp_argv[1] = strdup("-rose:read_executable_file_format_only");
    SgProject *project = frontend(argc+1, tmp_argv);

    /* Locate relevant parts of the files */
    SectionDictionary sd(project);
    if (!sd.core_header) {
        fprintf(stderr, "%s: no core file specified\n", arg0);
        exit(1);
    } else if (sd.sections.size()==0) {
        fprintf(stderr, "%s: no non-core files specified\n", arg0);
        exit(1);
    }

    /* Expand the core file by giving it data from the other file(s) */
    ExpandCoreDump expander(&sd);

    /* Write out a new core file */
    if (!is_modified(sd.core_header)) {
        fprintf(stderr, "%s: core file was not modified\n", arg0);
        exit(1);
    }
    std::ofstream f("core.new");
    sd.core_header->get_file()->reallocate();
    sd.core_header->get_file()->unparse(f);
}

#endif
