#include "sage3basic.h"
#include "Registers.h"

int
main()
{
    const RegisterDictionary *dict = RegisterDictionary::powerpc();
    ROSE_ASSERT(dict!=NULL);
    // std::cout <<*dict;

    const RegisterDescriptor *desc = dict->lookup("spr8");
    ROSE_ASSERT(desc!=NULL);
    // std::cout <<"found r8 = " <<*desc <<"\n";

    /* Register "lr" is the preferred name for "spr8", so that's what we'll get back for the reverse lookup */
    std::string alias = dict->lookup(*desc);
    ROSE_ASSERT(alias=="lr");

    return 0;
}
