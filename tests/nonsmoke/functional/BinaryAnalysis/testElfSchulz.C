/* Code based on a task described by Martin Schulz <schulzm@llnl.gov>: Given a shared library, replace all occurrences of
 * PMPI_* with XMPI_* in all symbol tables.
 *
 * This method correctly handles allocation in the string tables when the name being changed is being shared with other
 * symbols. For instance, when three symbols share the same string storage:
 *     _PMPI_Send
 *      PMPI_Send
 *       MPI_Send
 * then changing PMPI_Send to XMPI_Send will not affect the other two--it will be reallocated to a new position in the
 * string table so that "_PMPI_Send" and "MPI_Send" continue to share storage.
 *
 * When a string is reallocated to a new position in the string table one of four things can happen:
 *   1. The new string can share storage with an existing string (if there's an "foo_XMPI_Send" then "XMPI_Send" will
 *      share its storage).
 *   2. The new string can share storage with a shorter string if the shorter string has enough free space in
 *      front of it.
 *   3. The new string gets its own storage if there's a free block large enough.
 *   3b. (see below)
 *   4. The string table is extended, possibly affecting the location and/or size of a host of other sections
 *      and/or segments.
 *
 * NOTE: Case 4 is not currently working. Although sections are correctly moved and/or extended, ROSE does not rewrite
 *       instructions by adjusting inter-section offsets that might have changed. (RPM 2008-10-15)
 *
 * If the string table has enough discontiguous free space but no free block is large enough for the new string then we
 * could add a case 3b, which is to repack the entire string table. This operation doesn't currently happen automatically. See
 * the commented-out source code below. (RPM 2008-10-15)
 */

#include "rose.h"

class MyTraversal : public SgSimpleProcessing {
  public:
    void visit(SgNode *astNode) {
        SgAsmElfSymbolSection *symsec = isSgAsmElfSymbolSection(astNode);
        if (symsec) {
            printf("Scanning symbols in [%d] \"%s\"\n", symsec->get_id(), symsec->get_name()->get_string(true).c_str());
            const SgAsmElfSymbolPtrList &symbols = symsec->get_symbols()->get_symbols();
            for (SgAsmElfSymbolPtrList::const_iterator i=symbols.begin(); i!=symbols.end(); ++i) {
                std::string name = (*i)->get_name()->get_string();
                if (name.substr(0, 5)=="PMPI_") {
                    printf("  %s\n", name.c_str());
                    name[0] = 'X';
                    (*i)->get_name()->set_string(name);
                }
            }

#if 0
            /* Case 3a: repack the string table to make free space contiguous in hope of being able to allocate space for the
             * new string without having to extend the string table. This works by removing storage for all the strings in the
             * table. When the string table is unparsed all its strings are reallocated in one coordinated operation. */
            SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(symsec->get_linked_section());
            ROSE_ASSERT(strsec!=NULL);
            strsec->get_strtab()->free_all_strings(true); /*true means free even the unreferenced parts of the table*/
#endif
        }
    }
};

int
main(int argc, char *argv[])
{
    SgProject *project= frontend(argc,argv);
    MyTraversal().traverseInputFiles(project, postorder);
    return backend(project);
}

