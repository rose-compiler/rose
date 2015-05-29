/* This file contains Linux-64 system call emulation.  Most of these functions are callbacks and have names like:
 *
 *    RSIM_Linux64::syscall_FOO_enter           -- prints syscall tracing info when the call is entered
 *    RSIM_Linux64::syscall_FOO                 -- implements the system call
 *    RSIM_Linux64::syscall_FOO_leave           -- prints syscall tracing info when the call returns
 */
#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Linux64.h"
#include "Diagnostics.h"

using namespace rose::Diagnostics;

void
RSIM_Linux64::init() {}

bool
RSIM_Linux64::isSupportedArch(SgAsmGenericHeader *fhdr) {
    return isSgAsmElfFileHeader(fhdr) && fhdr->get_word_size()==8;
}

rose_addr_t
RSIM_Linux64::pushAuxVector(RSIM_Process *process, rose_addr_t sp, rose_addr_t execfn_va, SgAsmElfFileHeader *fhdr,
                            FILE *trace) {
    TODO("[Robb P. Matzke 2015-05-29]");
}

#endif
