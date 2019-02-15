#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#include "BinaryLoader.h"

using namespace Rose::BinaryAnalysis;

int
main(int argc, char *argv[])
{
    /* First argument is an address, from which 48 is subtracted, indicating where we start reading. We will read for 96 bytes. */
    assert(argc>1);
    rose_addr_t va = strtoull(argv[1], 0, 0) - 48;
    argv[1] = strdup("-rose:read_executable_file_format_only");
    SgProject *project = frontend(argc, argv);

    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project);
    for (std::vector<SgAsmInterpretation*>::iterator ii=interps.begin(); ii!=interps.end(); ++ii) {
        BinaryLoader::Ptr loader = BinaryLoader::lookup(*ii);
        ASSERT_not_null(loader);
        //loader->set_debug(stderr);
        loader->remap(*ii);
        MemoryMap::Ptr mm = (*ii)->get_map();
        if (mm) {
            mm->dump(stdout, "    ");

            unsigned char buf[96];
            size_t nread;

            SgAsmGenericHeader *hdr = (*ii)->get_headers()->get_headers()[0];
            SgAsmGenericFile *file = SageInterface::getEnclosingNode<SgAsmGenericFile>(hdr);
            memset(buf, 0xaa, sizeof buf);
            nread = file->read_content(mm, va, buf, sizeof buf, false);
            SgAsmExecutableFileFormat::hexdump(stdout, va, buf, sizeof buf, HexdumpFormat());
            printf("\n%zu bytes\n\n", nread);

            memset(buf, 0xaa, sizeof buf);
            nread = mm->readQuick(buf, va, sizeof buf);
            SgAsmExecutableFileFormat::hexdump(stdout, va, buf, sizeof buf, HexdumpFormat());
            printf("\n%zu bytes\n\n", nread);

        } else {
            printf("no map\n");
        }
    }
    
    return 0;
}

#endif
