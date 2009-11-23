/* Copyright 2008 Lawrence Livermore National Security, LLC */

#include "rose.h"
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <algorithm>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// functions
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/** Writes a new file from the IR node for a parsed executable file. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(const std::string &name, SgAsmGenericFile *ef)
{
    std::ofstream f(name.c_str(), std::ios_base::out|std::ios_base::binary|std::ios_base::trunc);
    ROSE_ASSERT(f.is_open());
    f.exceptions(std::ios::badbit | std::ios::failbit);
    unparseBinaryFormat(f, ef);
    f.close();
}

/** Unparses an executable file into the supplied output stream. Warning: This function might modify the AST by calling
 *  reallocate(), which makes sure all parts of the AST are consistent with respect to each other. */
void
SgAsmExecutableFileFormat::unparseBinaryFormat(std::ostream &f, SgAsmGenericFile *ef)
{
    ROSE_ASSERT(ef);

    if (checkIsModifiedFlag(ef))
        ef->reallocate();

    ef->unparse(f);

    /* Extend the file to the full size. The unparser will not write zero bytes at the end of a file because some files
     * actually use the fact that sections that extend past the EOF will be zero padded.  For the time being we'll extend the
     * file to its full size. */
    if (!ef->get_truncate_zeros())
        ef->extend_to_eof(f);
}

SgAsmGenericFile *
SgAsmExecutableFileFormat::parseBinaryFormat(const char *name)
{
    Loader::initclass();
    SgAsmGenericFile *ef = (new SgAsmGenericFile())->parse(name);
    ROSE_ASSERT(ef != NULL);

    if (SgAsmElfFileHeader::is_ELF(ef)) {
        (new SgAsmElfFileHeader(ef))->parse();
    } else if (SgAsmDOSFileHeader::is_DOS(ef)) {
        SgAsmDOSFileHeader *dos_hdr = new SgAsmDOSFileHeader(ef);
        dos_hdr->parse(false); /*delay parsing the DOS Real Mode Section*/

        /* DOS Files can be overloaded to also be PE, NE, LE, or LX. Such files have an Extended DOS Header immediately after
         * the DOS File Header (various forms of Extended DOS Header exist). The Extended DOS Header contains a file offset to
         * a PE, NE, LE, or LX File Header, the first bytes of which are a magic number. The is_* methods check for this magic
         * number. */
        SgAsmGenericHeader *big_hdr = NULL;
        if (SgAsmPEFileHeader::is_PE(ef)) {
            SgAsmDOSExtendedHeader *dos2_hdr = new SgAsmDOSExtendedHeader(dos_hdr);
            dos2_hdr->parse();
            SgAsmPEFileHeader *pe_hdr = new SgAsmPEFileHeader(ef);
            pe_hdr->set_offset(dos2_hdr->get_e_lfanew());
            pe_hdr->parse();
            big_hdr = pe_hdr;
        } else if (SgAsmNEFileHeader::is_NE(ef)) {
            SgAsmNEFileHeader::parse(dos_hdr);
        } else if (SgAsmLEFileHeader::is_LE(ef)) { /*or LX*/
            SgAsmLEFileHeader::parse(dos_hdr);
        }

#if 0 /*This iterferes with disassembling the DOS interpretation*/
        /* Now go back and add the DOS Real-Mode section but rather than using the size specified in the DOS header, constrain
         * it to not extend beyond the beginning of the PE, NE, LE, or LX file header. This makes detecting holes in the PE
         * format much easier. */
        dos_hdr->add_rm_section(big_hdr ? big_hdr->get_offset() : 0);
#else
        dos_hdr->add_rm_section(0);
#endif

    } else {
        delete ef; ef=NULL;
        /* Use file(1) to try to figure out the file type to report in the exception */
        int child_stdout[2];
        pipe(child_stdout);
        pid_t pid = fork();
        if (0==pid) {
            close(0);
            dup2(child_stdout[1], 1);
            close(child_stdout[0]);
            close(child_stdout[1]);
            execlp("file", "file", "-b", name, NULL);
            exit(1);
        } else if (pid>0) {
            char buf[4096];
            memset(buf, 0, sizeof buf);
            read(child_stdout[0], buf, sizeof buf);
            buf[sizeof(buf)-1] = '\0';
            if (char *nl = strchr(buf, '\n')) *nl = '\0'; /*keep only first line w/o LF*/
            waitpid(pid, NULL, 0);
            char mesg[64+sizeof buf];
            sprintf(mesg, "unrecognized file format: %s", buf);
            throw FormatError(mesg);
        } else {
            throw FormatError("unrecognized file format");
        }
    }

    ef->set_tracking_references(false); /*all done parsing*/

    /* Is the file large enough to hold all sections?  If any section extends past the EOF then set truncate_zeros, which will
     * cause the unparser to not write zero bytes to the end of the file. */
    ef->set_truncate_zeros(ef->get_current_size()>ef->get_orig_size());

    /* If any section is the target of a function symbol then mark that section as containing code even if that section is not
     * memory mapped with execute permission. */
    struct: public AstSimpleProcessing {
        void visit(SgNode *node) {
            SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node);
            if (symbol && symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC) {
               SgAsmGenericSection *section = symbol->get_bound();
               if (section)
                   section->set_contains_code(true);
            }
        }
    } t1;
    t1.traverse(ef, preorder);
    
    return ef;
}
