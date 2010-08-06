/* This program connects to a Windows XP process through Xen+Ether and obtains the instructions which the process is
 * executing. Those instruction bytes are run through the ROSE disassembler to print the results.  The test should be
 * run with three arguments:
 *    1. a local path to the executable on which we can perform static analysis,
 *    2. the ID of the Windows XP domain (see output from "xm list"),
 *    3. the name of the process to trace as it exists in the Windows XP environment (e.g., "sol.exe")
 *
 * The process should be started in Windows XP manually after this test is running.
 */

#include "rose.h"

#ifndef ROSE_USE_ETHER
#include <stdio.h>
int main(int argc, char *argv[]) {
    printf("%s: Ether is not available. Test is skipped.\n", argv[0]);
    return 0;
}
#else

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* "ether.h" should probably include <xenctrl.h> since it depends on it. */
extern "C" {
#  include <xenctrl.h>
#  include <xen/hvm/ether.h>
}

#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

static jmp_buf unwind_to_main;
static sig_atomic_t terminating = 0;
static void termination_handler(int signo)
{
    if (terminating) {
        /* This might not be safe; I'm not sure as to what extent the Ether and Xen functions are async-signal-safe. But it
         * seems to get us out of here cleanly most of the time if setting 'terminating' doesn't break the main event loop. */
        longjmp(unwind_to_main, 1);
    }
    
    terminating = 1;
}

int main(int argc, char *argv[])
{
    try {
        int domU=1;
        int status;
        char filter_name[16], *rest;

        /* Parse command-line arguments */
        if (argc!=4) {
            fprintf(stderr, "usage: %s DOMID EXENAME\n", argv[0]);
            exit(1);
        }

        char *rose_args[3];
        rose_args[0] = argv[0];
        rose_args[1] = argv[1];
        rose_args[2] = NULL;
        SgProject *project = frontend(2, rose_args);

        domU = strtol(argv[2], &rest, 0);
        if (domU<=0) {
            fprintf(stderr, "%s: invalid domain ID: %s\n", argv[0], argv[2]);
            exit(1);
        }
        if (strlen(argv[3])>=sizeof filter_name) {
            fprintf(stderr, "%s: filter name is too long: %s\n", argv[0], argv[3]);
            exit(1);
        }
        strncpy(filter_name, argv[3], sizeof filter_name);
        filter_name[sizeof(filter_name)-1] = '\0';

        /* Create the disassembler based on static analysis */
        struct T1: public SgSimpleProcessing {
            Disassembler *disassembler;
            T1(): disassembler(NULL) {}
            void visit(SgNode *node) {
                SgAsmGenericHeader *fhdr = isSgAsmGenericHeader(node);
                if (!disassembler && fhdr)
                    disassembler = Disassembler::lookup(fhdr)->clone();
            }
        };
        T1 analysis;
        analysis.traverse(project, preorder);
        assert(analysis.disassembler!=NULL);

        /* Set up communication with Xen/Ether */
        Ether ether(domU);
        ether.set_single_step(true);
        ether.set_single_step_notify(true);
        ether.add_name_filter(filter_name);

        /* Set termination handlers here, or we'll leave hypervisor in a bad state! */
        if (setjmp(unwind_to_main)>0) {
            status = 1;
            goto done;
        }
        struct sigaction sa;
        sa.sa_handler = termination_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        /* Event loop */
        while (!terminating) {
            int event=-1;
            try {
                event = ether.next_event();
            } catch(const Ether::Exception &e) {
                printf("Ether::Exception: %s (skipping event)\n", e.mesg.c_str());
                ether.resume();
                continue;
            }

            if (ETHER_NOTIFY_INSTRUCTION==event) {
                const struct instruction_info *insn_info = (const struct instruction_info*)ether.event_data();
                rose_addr_t va = insn_info->registers.eip; /*or perhaps insn_info->guest_rip?*/
                try {
                    SgAsmInstruction *insn = NULL;
                    insn = analysis.disassembler->disassembleOne(insn_info->instruction, va, sizeof(insn_info->instruction), va);
                    printf("0x%08"PRIx64": %s\n", va, unparseInstruction(insn).c_str());
                    SageInterface::deleteAST(insn);
                } catch(const Disassembler::Exception &e) {
                    std::cerr <<e <<"\n";
                }
            } else {
                printf("received event %d\n", event);
            }

            /* Unlock the shared page and cause the domainU to resume execution. */
            ether.resume();
        }

    done:
        return 0;
    } catch(const Ether::Exception &e) {
        fprintf(stderr, "Ether::Exception: %s\n", e.mesg.c_str());
        throw;
    }
}

#endif /*ROSE_USE_ETHER*/
