/* Tests ability to build a new executable from scratch */
#include "sage3basic.h"

#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

int
main(int argc, char *argv[]) 
{
    /* See testElfConstruct.C for additional commentary. */
    const char *outputName = argc > 1 ? argv[1] : "a.out";
    
    /* The SgAsmGenericFile is the root of a tree describing a binary file (executable, shared lib, object, core dump). */
    SgAsmGenericFile *ef = new SgAsmGenericFile;

    /***************************************************************************************************************************
     * The DOS part of the file.
     ***************************************************************************************************************************/

    /* The DOS File Header is the first thing in the file, always at offset zero. The constructors generally take arguments to
     * describe the new object's relationship with existing objects. In this case, the SgAsmDOSFileHeader is a child of the
     * SgAsmGenericFile in the AST. If we were parsing an existing binary file then we would construct the SgAsmDOSFileHeader
     * and then invoke parse(), which recursively constructs and parses everything reachable from the DOS File Header.
     *
     * A freshly constructed DOS File Header isn't too useful since nearly all its data members are initialized to zeros. But
     * all we're using it for is to describe the location of the PE File Header. */
    SgAsmDOSFileHeader *dos_fhdr = new SgAsmDOSFileHeader(ef);

    /* The Extended DOS Header immediately follows the DOS File Header and contains the address of the PE File Header. In this
     * example, we'll place the PE File Header immediately after the Extended DOS File Header. */
    SgAsmDOSExtendedHeader *dos2 = new SgAsmDOSExtendedHeader(dos_fhdr);

    /* The optional DOS real-mode text and data section. */
    if (true) {
        /* This isn't actually necessary if the program will only be run in Windows. Normally the DOS program will just print
         * an error message that the program must be run in Windows.  DOS files don't have a section table like many other
         * headers. Instead, the location and size of the text/data segment is stored directly in the DOS header. If the DOS
         * text/data section is created, it must appear directly after the DOS Extended Header in the file. */
        SgAsmGenericSection *s = new SgAsmGenericSection(ef, dos_fhdr);
        s->set_size(0x90);
        s->set_mapped_preferred_rva(0);
        s->set_mapped_size(s->get_size());
        s->set_mapped_rperm(true);
        s->set_mapped_wperm(true);
        s->set_mapped_xperm(true);
        dos_fhdr->set_rm_section(s);
        dos_fhdr->add_entry_rva(rose_rva_t(s->get_mapped_preferred_rva(), s));
    }

    /* Call reallocate() to give the sections we've defined so far a chance to adjust to the proper sizes since we'll need to
     * know the final offsets in the next step. */
    ef->reallocate();

    /***************************************************************************************************************************
     * PE File Header
     ***************************************************************************************************************************/

    /* The PE File Header follows the DOS Extended Header. We need to store the offset in the extended header. */
    SgAsmPEFileHeader *fhdr = new SgAsmPEFileHeader(ef);
    fhdr->set_base_va(0x01000000);
    dos2->set_e_lfanew(fhdr->get_offset());

    /***************************************************************************************************************************
     * PE Section Table
     ***************************************************************************************************************************/

    /* Most files have a PE Section Table that describes the various sections in the file (the code, data, dynamic linking,
     * symbol tables, string tables, etc). The PE Section Table is a child of the PE File Header. We give it an address that's
     * at the end of the file. Generally, newly-constructed objects will have a size of at least one byte so that when placing
     * new items at the EOF they'll have unique starting offsets (this is important for the functions that resize and/or
     * rearrange sections--they need to know the relative order of sections in the file). */
    SgAsmPESectionTable *sectab = new SgAsmPESectionTable(fhdr);

    /***************************************************************************************************************************
     * PE Text Section
     ***************************************************************************************************************************/

    /* The ".text" section holds instructions and some read-only data.  We'll fill our example text section with x86 NOP
     * instructions encoded as 0x90 bytes. We choose the preferred mapping address manually, but must make sure it doesn't
     * overlap with anything else. */
    SgAsmPESection *text = new SgAsmPESection(fhdr);
    text->get_name()->set_string(".text");      /* name is important; truncated if more than eight characters */
    text->set_size(0x800);                      /* must be a multiple of 0x200 (512) */
    text->set_file_alignment(0x200);            /* must be a multiple of 0x200 (512) */
    text->set_mapped_size(0x656);               /* may be smaller than section's file size */
    text->set_mapped_preferred_rva(0x1000);     /* must be a multiple of 0x1000 (4096) */
    text->set_mapped_alignment(0x1000);         /* must be a multiple of 0x1000 (4096) */
    text->set_mapped_rperm(true);
    text->set_mapped_wperm(false);
    text->set_mapped_xperm(true);
    text->align();                              /* necessary because we changed alignment values above */
    sectab->add_section(text);

    /* The entry address for the executable as a whole should be somewhere in the .text section. We'll just make it the first
     * byte of the text section. */
    fhdr->add_entry_rva(rose_rva_t(text->get_mapped_preferred_rva(), text));

    /***************************************************************************************************************************
     * PE Data Section
     ***************************************************************************************************************************/

    /* The ".data" section holds read-write initialized data. We choose the preferred mapping address manually, using an
     * address that's large enough that it doesn't collide with the ".text" section we created above. */
    SgAsmPESection *data = new SgAsmPESection(fhdr);
    data->get_name()->set_string(".data");      /* name is important; truncated if more than eight characters */
    data->set_size(0x200);                      /* must be a multiple of 0x200 (512) */
    data->set_file_alignment(0x200);            /* must be a multiple of 0x200 (512) */
    data->set_mapped_size(0xc);                 /* may be smaller than section's file size */
    data->set_mapped_preferred_rva(0x2000);     /* must be a multiple of 0x1000 (4096) */
    data->set_mapped_alignment(0x1000);         /* must be a multiple of 0x1000 (4096) */
    data->set_mapped_rperm(true);
    data->set_mapped_wperm(true);
    data->set_mapped_xperm(false);
    data->align();                              /* necessary because we changed alignment values above */
    sectab->add_section(data);

    /***************************************************************************************************************************
     * PE Resource Section
     ***************************************************************************************************************************/

    /* The ".rsrc" section holds read-only program resources. We choose the preferred mapping address manually, using an
     * address that's large enough that it doesn't collide with the ".data" section we created above. */
    SgAsmPESection *rsrc = new SgAsmPESection(fhdr);
    rsrc->get_name()->set_string(".rsrc");      /* name is important; truncated if more than eight characters. */
    rsrc->set_size(0x800);                      /* must be a multiple of 0x200 (512) */
    rsrc->set_file_alignment(0x200);            /* must be a multiple of 0x200 (512) */
    rsrc->set_mapped_size(0x780);               /* may be smaller than section's file size */
    rsrc->set_mapped_preferred_rva(0x3000);     /* must be a multiple of 0x1000 (4096) */
    rsrc->set_mapped_alignment(0x1000);         /* must be a multiple of 0x1000 (4096) */
    rsrc->set_mapped_rperm(true);
    rsrc->set_mapped_wperm(false);
    rsrc->set_mapped_xperm(false);
    rsrc->align();                              /* necessary because we changed alignment values above */
    sectab->add_section(rsrc);

    /* The resource section is also pointed to by RvaSizePair[2], although for some unknown reason, the sizes are not always
     * the same. Often the RvaSizePair points to a smaller area than the section table. */
    fhdr->set_rvasize_pair(SgAsmPEFileHeader::PAIR_RESOURCES, rsrc);

    /**************************************************************************************************************************
     * PE Import Section
     **************************************************************************************************************************/

    /* The import section as a whole. We choose the preferred mapping address manually, using an address that's large enough
     * that it doesn't collide with the ".data" section we created above. */
    SgAsmPEImportSection *imports = new SgAsmPEImportSection(fhdr);
    imports->get_name()->set_string(".idata");  /* name is important, indicates an import section */
    imports->set_file_alignment(0x200);         /* must be a multiple of 0x200 (512) */
    imports->set_mapped_size(imports->get_size()); /* must be same as its size in the file */
    imports->set_mapped_preferred_rva(0x4000);  /* must be a multiple of 0x1000 (4096) */
    imports->set_mapped_alignment(0x1000);      /* must be a multiple of 0x1000 (4096) */
    imports->set_mapped_rperm(true);
    imports->set_mapped_wperm(true);
    imports->set_mapped_xperm(false);
    imports->align();                           /* necessary because we changed alignment values above */
    sectab->add_section(imports);

    /* kernel32.dll imports by name and hint */
    SgAsmPEImportDirectory *kernel32 = new SgAsmPEImportDirectory(imports, "KERNEL32.dll");
    new SgAsmPEImportItem(kernel32, "GetTimeFormatW",              467);
    new SgAsmPEImportItem(kernel32, "GetDateFormatW",              320);
    new SgAsmPEImportItem(kernel32, "FileTimeToSystemTime",        195);
    new SgAsmPEImportItem(kernel32, "FileTimeToLocalFileTime",     194);
    new SgAsmPEImportItem(kernel32, "GetModuleHandleW",            376);
    new SgAsmPEImportItem(kernel32, "QueryPerformanceCounter",     657);
    new SgAsmPEImportItem(kernel32, "GetTickCount",                465);
    new SgAsmPEImportItem(kernel32, "GetCurrentThreadId",          318);
    new SgAsmPEImportItem(kernel32, "GetCurrentProcessId",         316);
    new SgAsmPEImportItem(kernel32, "GetSytemTimeAsFileTime",      445);
    new SgAsmPEImportItem(kernel32, "TerminateProcess",            838);
    new SgAsmPEImportItem(kernel32, "GetCurrentProcess",           315);
    new SgAsmPEImportItem(kernel32, "UnhandledExceptionFilter",    855);
    new SgAsmPEImportItem(kernel32, "SetUnhandledExceptionFilter", 818);

    /* user32.dll imports by ordinal */
    SgAsmPEImportDirectory *user32 = new SgAsmPEImportDirectory(imports, "USER32.dll");
    new SgAsmPEImportItem(user32, 457);

    /* shell32.dll imports by name but no hint */
    SgAsmPEImportDirectory *shell32 = new SgAsmPEImportDirectory(imports, "SHELL32.dll");
    new SgAsmPEImportItem(shell32, "ShellAboutW");

    /* Each Import Directory has it's own IAT, but the IATs are contiguous in memory to form a global IAT.  This global IAT is
     * usually allocated in the .rdata section and it's address normally needs to be known and fixed because it's reference by
     * indirect jumps in the .text section.  If we allow ROSE to allocate the IATs, ROSE will place them inside the import
     * section and probably not contiguously with each other.  There's nothing wrong with letting ROSE allocate the IATs inside
     * the import section and then asking the import directories for their IAT address and using those addresses when
     * generating code.  A better approach is to allocate space manually in a specific section, which is what we do here. */
    SgAsmPESection *rdata = new SgAsmPESection(fhdr);
    rdata->get_name()->set_string(".rdata");
    rdata->set_file_alignment(0x200);
    rdata->set_mapped_preferred_rva(0x5000);
    rdata->set_mapped_alignment(0x1000);
    rdata->set_mapped_rperm(true);
    rdata->set_mapped_wperm(false);
    rdata->set_mapped_xperm(false);
    rdata->align();                             /* necessary because we changed alignment values above */
    sectab->add_section(rdata);

    /* Allocate the individual IATs in contiguous memory beginning at the start of .rdata */
    rose_rva_t rdata_rva(rdata->get_mapped_preferred_rva(), rdata);
    size_t global_iat_size = imports->reallocate_iats(rdata_rva);
    rdata->set_mapped_size(global_iat_size);
    rdata->set_size(global_iat_size);

    /***************************************************************************************************************************
     * Generate the output.
     ***************************************************************************************************************************/

    /* Some of the sections we created above have default sizes of one byte because there's no way to determine their true
     * size until we're all done creating sections.  The SgAsmGenericFile::reallocate() traverses the AST and allocates the
     * actual file storage for each section, adjusting other sections to make room and updating various tables and
     * file-based data structures.
     *
     * We have chosen preferred mapping addresses for the above section manually. If we had wanted to automate that process, we
     * could have called reallocate() after each section was created and then based the preferred mapping address of the
     * following section on the mapped location and size of the previous section. */
    ef->reallocate();

    /* The .text section also contains (parts of) various other sections defined by the RVA/Size pairs. The way we do this is
     * to first declare the .text section to be big enough only for the program instructions, then create the other sections
     * immediately after it, allow all sections to allocate file space, and finally we extend the .text section to overlap with
     * the following sections. */
    // text->set_mapped_size(xxx);
    // text->set_size(xxx);

#if 0
    /* Show the results */
    ef->dump(stdout);
    SgAsmGenericSectionPtrList all = ef->get_sections(true);
    for (size_t i=0; i<all.size(); i++) {
        fprintf(stdout, "Section %zu:\n", i);
        all[i]->dump(stdout, "    ", -1);
    }
#endif

    /* Unparse the AST to generate an executable. */
    std::ofstream f(outputName);
    ef->unparse(f);

    return 0;
}
    
#endif
