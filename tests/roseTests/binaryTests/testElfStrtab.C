/* Tests string reallocation functions by changing some strings */

#include "rose.h"

class MyTraversal : public SgSimpleProcessing {
  public:
    void visit(SgNode *astNode);
};

        /* Some tests for string allocation functions. Some of these assume that the string table complies with the ELF
         * specification, which guarantees that the first byte of the string table is NUL. The parser can handle non-compliant
         * string tables. */
void
MyTraversal::visit(SgNode* astNode)
{
    SgAsmElfSymbolSection *symsec = isSgAsmElfSymbolSection(astNode);
    if (symsec && symsec->get_name()->get_string()==".dynsym") {
        fputs("*** Test uses these sections:\n", stdout);
        printf("    symsec = [%d] \"%s\"\n", symsec->get_id(), symsec->get_name()->get_string(true).c_str());
        SgAsmElfStringSection *strsec = dynamic_cast<SgAsmElfStringSection*>(symsec->get_linked_section());
        ROSE_ASSERT(strsec!=NULL);
        printf("    strsec = [%d] \"%s\"\n", strsec->get_id(), strsec->get_name()->get_string(true).c_str());

        /* Find a particular symbol in the symbol table */
        const SgAsmElfSymbolPtrList &symbols = symsec->get_symbols()->get_symbols();
        SgAsmStoredString *test = NULL;
        for (size_t i=0; i<symbols.size(); i++) {
            if (symbols[i]->get_name()->get_string()=="memset") {
                test = new SgAsmStoredString(strsec->get_strtab(), symbols[i]->get_name()->get_offset());
            }
        }
        ROSE_ASSERT(test!=NULL);

        /* Test 1: Create another reference to the empty string and then try to modify it. This should create a new string rather
         *         than modifying the empty string. The ELF specification reserves offset zero to hold a NUL to represent the
         *         empty string and we must leave the NUL there even if nothing references it. */
        SgAsmStoredString *s1 = new SgAsmStoredString(strsec->get_strtab(), 0);
        ROSE_ASSERT(s1->get_string()==""); /*must comply with spec!*/
        s1->set_string("fprintf");
        ROSE_ASSERT(s1->get_offset()!=SgAsmStoredString::unallocated);
        ROSE_ASSERT(s1->get_offset()!=0);

        /* Test 2: Create a new string that happens to have the same initial value as something already in the string table. When
         *         allocated the new string will share the same space as the existing string. */
        s1 = new SgAsmStoredString(strsec->get_strtab(), test->get_offset());
        ROSE_ASSERT(s1->get_offset()!=SgAsmStoredString::unallocated);
        ROSE_ASSERT(s1->get_offset()==test->get_offset());

        /* Test 3: Change one of two strings that share the same offset by removing the first character. The new offset should be
         *         incremented by one. */
        s1->set_string(&(s1->get_string()[1]));
        ROSE_ASSERT(s1->get_offset()!=SgAsmStoredString::unallocated);
        ROSE_ASSERT(s1->get_offset()==test->get_offset()+1);

        /* Test 4: Change one of two strings that share memory and see if it gets allocated elsewhere. */
        s1->set_string("intf"); /*probably to the end of "fprintf"*/
        ROSE_ASSERT(s1->get_offset()!=SgAsmStoredString::unallocated);
        ROSE_ASSERT(s1->get_offset()+s1->get_string().size()+1 <= test->get_offset() ||  /*left of test*/
                    s1->get_offset() >= test->get_offset()+test->get_string().size()+1);       /*right of test*/

        /* Test 5: Copy a string and change the copy. Both strings should change. */
        SgAsmStoredString copy = *s1;
        copy.set_string(test->get_string());
        ROSE_ASSERT(s1->get_offset()!=SgAsmStoredString::unallocated);
        ROSE_ASSERT(s1->get_offset()==copy.get_offset());
        ROSE_ASSERT(s1->get_string()==copy.get_string());

        /* Test 6: Effectively delete a string by setting it to the empty string. Its offset should revert to zero along with all
         *         copies of the string. */
        s1->set_string("");
        ROSE_ASSERT(s1->get_offset()==0);
        ROSE_ASSERT(copy.get_offset()==0);
        ROSE_ASSERT(copy.get_string()=="");

        /* Test 7: Deleting one copy should not cause problems for the other copy. */
        delete s1;
        s1 = NULL;
        ROSE_ASSERT(copy.get_offset()==0);
        ROSE_ASSERT(copy.get_string()=="");

        /* Test 8: Reallocate the entire string table, leaving holes (unparsed areas) at their original offsets. Ideally the table
         *         size should not increase even though the compiler/linker may have agressively shared string storage. */
        rose_addr_t orig_size = strsec->get_size();
        strsec->get_strtab()->free_all_strings();
        strsec->get_strtab()->reallocate(false);
        ROSE_ASSERT(orig_size==strsec->get_size());

        /* Test 9: Reallocate the *entire* string table, blowing away holes in the process. */
        strsec->get_strtab()->free_all_strings(true);
        strsec->get_strtab()->reallocate(false);
        ROSE_ASSERT(orig_size==strsec->get_size());

        /* Test 10: After reallocating an entire table, the empty string should remain at offset zero. */
        SgAsmStoredString *s2 = new SgAsmStoredString(strsec->get_strtab(), 0);
        ROSE_ASSERT(s2->get_offset()==0);
        ROSE_ASSERT(s2->get_string()=="");

        /* Test 11: What happens if the string table needs to grow?  We'll just allocate something big with no guarantee that
         * it's even large enough to extend the table. */  
        test->set_string("ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ"
                         "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ");
        //test->get_offset(); /*force reallocation*/

#if 0
        /* Test 12: It's not legal to (re)parse a new region of the string table after we've made modifications. */
        fprintf(stderr, "TESTING: an error message and abort should follow this line.\n");
        s2 = new SgAsmStoredString(strsec->get_strtab(), test->get_offset());
#endif

        fputs("*** End of test\n", stdout);
    }
}

int
main(int argc, char *argv[])
{
    SgProject *project= frontend(argc,argv);
    MyTraversal myTraversal;
    myTraversal.traverseInputFiles(project, postorder);
    return backend(project);
}

