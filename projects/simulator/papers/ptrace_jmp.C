/* Runs a specimen to a specified address, then branches to another address and single steps until the specimen dies.  Reports
 * how many instructions were executed.
 *
 * Usage:
 *      ptrace_jmp TRIGGER TARGET SPECIMEN SPECIMEN_ARGS...
 *
 * The output will look something like this:
 *    $ i386 -LRB3 ./ptrace_jmp 0x08049940 0x40070652 ~/32bit/grep
 *    lt-ptrace_jmp: setting break point at 0x08049940
 *    lt-ptrace_jmp: n=2 at eip=0x08049941
 *    lt-ptrace_jmp: break point reached; removing it
 *    lt-ptrace_jmp: branching to 0x40070652
 *    lt-ptrace_jmp: n=3 at eip=0x4007065a
 *    lt-ptrace_jmp: n=4 at eip=0x4007065e
 *    lt-ptrace_jmp: specimen received Segmentation fault
 *    lt-ptrace_jmp: killing specimen
 *    lt-ptrace_jmp: processed 3 instructions
 */

#include "rose.h"
#include "RSIM_Private.h"

#if defined(ROSE_ENABLE_SIMULATOR) && __WORDSIZE==64 /* protects this whole file */

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/types.h>
#include <sys/wait.h>

static const char *arg0 = "";

static bool die(const char *mesg) __attribute__((noreturn));
static bool die(const char *mesg)
{
    perror((std::string(arg0)+": "+mesg).c_str());
    abort();
}

int
main(int argc, char *argv[])
{
    static const size_t limit = 200; // arbitrary limit

    arg0 = strrchr(argv[0], '/');
    arg0 = arg0 ? arg0+1 : argv[0];

    assert(argc>=4);
    rose_addr_t trigger_va = strtoull(argv[1], NULL, 0);
    rose_addr_t target_va = strtoull(argv[2], NULL, 0);
    assert(trigger_va>=0 && target_va>=0);

    pid_t pid = fork();
    if (-1==pid) {
        die("fork");
    } else if (0==pid) {
        ptrace(PTRACE_TRACEME)==-1 && die("ptrace_traceme");
        execv(argv[3], argv+3);
        die("execv");
    }

    int status;
    size_t n=0, at_bp=0;
    unsigned long bp_word=0;
    while (++n<=limit && -1!=waitpid(pid, &status, WUNTRACED) && !WIFEXITED(status)) {
        assert(WIFSTOPPED(status));
        if (1==n) {
            std::cerr <<arg0 <<": setting break point at " <<StringUtility::addrToString(trigger_va) <<"\n";
            (bp_word = ptrace(PTRACE_PEEKTEXT, pid, (void*)trigger_va))==(unsigned long)(-1) && die("ptrace_peektext");
            unsigned long word_with_int3 = (bp_word & 0xffffff00) | 0xcc;
            ptrace(PTRACE_POKETEXT, pid, (void*)trigger_va, (void*)word_with_int3)==-1 && die("ptrace_poketext");
            ptrace(PTRACE_CONT, pid, NULL, NULL)==-1 && die("ptrace_cont");
        } else if (WSTOPSIG(status)==SIGTRAP) {
            unsigned long eip = ptrace(PTRACE_PEEKUSER, pid, (void*)(8*RIP));
            eip==(unsigned long)(-1) && die("ptrace_peekuser");
            std::cerr <<arg0 <<": n=" <<n <<" at eip=" <<StringUtility::addrToString(eip) <<"\n";
            if (eip==trigger_va+1 && 0==at_bp) { // +1 because INT3 is one byte
                at_bp = n;
                std::cerr <<arg0 <<": break point reached; removing it\n";
                ptrace(PTRACE_POKETEXT, pid, (void*)trigger_va, (void*)bp_word)==-1 && die("ptrace_poketext");
                std::cerr <<arg0 <<": branching to " <<StringUtility::addrToString(target_va) <<"\n";
                ptrace(PTRACE_POKEUSER, pid, (void*)(8*RIP), (void*)target_va)==-1 && die("ptrace_pokeuser");
            }
            ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL)==-1 && die("patrace_singlestep");
        } else {
            std::cerr <<arg0 <<": specimen received " <<strsignal(WSTOPSIG(status)) <<"\n";
            std::cerr <<arg0 <<": killing specimen\n";
            ptrace(PTRACE_KILL, pid)==-1 && die("ptrace kill");
            break;
        }
    }

    if (WIFEXITED(status)) {
        std::cerr <<arg0 <<": specimen did exit\n";
    } else if (n>limit) {
        std::cerr <<arg0 <<": limit (" <<limit <<") reached, stopping specimen\n";
        ptrace(PTRACE_KILL, pid)==-1 && die("ptrace_kill");
    }

    if (n<at_bp) {
        std::cerr <<arg0 <<": the trigger address was never reached\n";
    } else {
        std::cerr <<arg0 <<": processed " <<(n-at_bp) <<" instruction" <<(1==n-at_bp?"":"s") <<"\n";
    }

    return 0;
}

#else
int main(int, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif /* ROSE_ENABLE_SIMULATOR */
