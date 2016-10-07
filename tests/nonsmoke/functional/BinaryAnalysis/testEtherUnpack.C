/* This demonstrates a combination of static and dynamic analysis for self-unpacking Windows XP executable specimen.  The
 * specimen is parsed statically in ROSE. Then this test program connects (through Xen) to the same specimen running under
 * Windows XP. We keep track of which pages the specimen writes to, and if it then tries to execute a page that was previously
 * modified we generate a new SgAsmGenericSection and add it to the AST.
 *
 * Run this program with three arguments:
 *    1. a local path to the executable on which we can perform static analysis,
 *    2. the ID of the Windows XP domain (see output from "xm list"),
 *    3. the name of the process to trace as it exists in the Windows XP environment (e.g., "sol.exe")
 *
 * The specimen should be started in Windows XP manually after this test so indicates.
 *
 */

#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* "ether.h" should probably include <xenctrl.h> since it depends on it. */
extern "C" {
#  include <xenctrl.h>
#  include <xen/hvm/ether.h>
}

#  include "ether.h" /*not same as <xen/hvm/ether.h>, but rather ether support functions from GA Tech*/

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
    int domU=1;
    volatile unsigned char *shared_page=NULL;
    volatile uint32_t *shared_page_lock=NULL;
    static const evtchn_port_t UNDEFINED_EVENT_PORT = (evtchn_port_t)(-1);
    evtchn_port_t event_port = UNDEFINED_EVENT_PORT;
    int status, xc_event_iface=-1;
    struct ether_communication comm;
    char filter_name[16], *rest;
    memset(&comm, 0, sizeof comm);

    /* Parse command-line arguments */
    if (argc!=4) {
        fprintf(stderr, "usage: %s DOMID EXENAME\n", argv[0]);
        exit(1);
    }
    
    char *rose_args[3];
    rose_args[0] = argv[0];
    rose_args[1] = argv[1];
    rose_args[2] = NULL;
    SgProject *project = frontend(3, rose_args);

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

    /* Open handle to the hypervisor interface. Eventually call xc_interface_close(). */
    int xc_iface = xc_interface_open();
    if (xc_iface<0) {
        fprintf(stderr, "%s: failed to contact Xen hypervisor\n", argv[0]);
        status = 1;
        goto cleanup;
    }

    /* Get a handle to the event channel driver, or -1 */
    xc_event_iface = xc_evtchn_open();
    if (xc_event_iface<0) {
        fprintf(stderr, "%s: failed to open main event channel\n", argv[0]);
        status = 1;
        goto cleanup;
    }
    
    /* Send a XEN_DOMCTL_ETHER_INIT message to the hypervisor to initialize 'comm' */
    if (ether_initialize_communication(xc_iface, domU, &comm)<0) {
        fprintf(stderr, "%s: cannot initialize Ether component of Xen hypervisor\n", argv[0]);
        status = 1;
        goto cleanup;
    }
    printf("After init:\n");
    printf("    shared_page_ptr:    %p\n",    comm.shared_page_ptr);
    printf("    shared_page_mfn:    0x%lx\n", comm.shared_page_mfn);
    printf("    domid_source:       %d\n",    comm.domid_source);
    printf("    event_channel_port: %d\n",    comm.event_channel_port);

    /* The XEN_DOMCTL_ETHER_INIT caused the hypervisor to allocate a page of memory. We now map that page into our own address
     * space. We should call munmap() when done with it. The hypervisor will initialize the page to all zeros and place a
     * NUL-terminated ASCII string at the beginning for debugging (to make sure we got the right page).  The string is
     * "Page-Sharing is A-OK!\n". */
    shared_page = (volatile unsigned char*)xc_map_foreign_range(xc_iface, DOMID_XEN, getpagesize(),
                                                                PROT_READ|PROT_WRITE, comm.shared_page_mfn);
    if (!shared_page) {
        fprintf(stderr, "%s: cannot share memory with hypervisor\n", argv[0]);
        status = 1;
        goto cleanup;
    }
    shared_page_lock = (volatile uint32_t*)shared_page;
    printf("Shared Page va: %p\n", shared_page);
    printf("Shared Page test:\n\t%s\n", (char*)shared_page);

    /* The Xen hypervisor allocated an unbound port whose number was returned in by ether_initialize_communication(). This
     * event channel will be used to notify us when Ether-related things happen in the domU.  So now we create the local
     * end of the channel and bind it with the remote end.
     *
     * Q: How does the hypervisor know which domain DOMID_SELF refers to? We didn't specify a domain when we created the
     *    xc_event_iface. */
    event_port = xc_evtchn_bind_interdomain(xc_event_iface, DOMID_SELF, comm.event_channel_port);
    if (event_port<0) {
        fprintf(stderr, "%s: cannot bind to Ether event channel\n", argv[0]);
        status = 1;
        goto cleanup;
    }

    /* The lock was created in a set state due to the debug string "Page-Sharing is A-OK\n", so we reset it now.
     *
     * Q: Is it possible for the hypervisor to try to store something in the shared page before we get to this point? See
     *    ether_comm_write() where there's a printk("ETHER: problem: should never be here\n"). If so, then we may be setting
     *    the lock back to zero and discarding the first event. Perhaps that's what the next paragraph is trying to work
     *    around? */
    *shared_page_lock = 0;

    /* Q: Why are we discarding the first event? */
    if (xc_evtchn_pending(xc_event_iface) == event_port) {
        fprintf(stderr, "%s: taking off spurious pending notification\n", argv[0]);
        status = xc_evtchn_unmask(xc_event_iface, event_port);
        if (status<0) {
            fprintf(stderr, "%s: event unmask failed for spurious event flushing\n", argv[0]);
            status = 1;
            goto cleanup;
        }
    }
    
    /* Enable single step */
    if (ether_ss(xc_iface, domU, 1)<0) {                /* apparently enables single stepping in the hypervisor? */
        fprintf(stderr, "%s: could not enable single stepping in hypervisor\n", argv[0]);
        status = 1;
        goto cleanup;
    }
    if (ether_ss_notify(xc_iface, domU, 1)<0) {         /* apparently causes hypervisor to report ETHER_NOTIFY_INSTRUCTION? */
        fprintf(stderr, "%s: could not tell hypervisor to report single stepping events\n", argv[0]);
        status = 1;
        goto cleanup;
    }

    /* Set filter name (we don't want to trace everything!) */
    if (ether_name(xc_iface, domU, XEN_DOMCTL_ETHER_ADD_NAME, filter_name)<0) {
        fprintf(stderr, "%s: could not filter tracing by process name\n", argv[0]);
        status = 1;
        goto cleanup;
    }

    /* Set termination handlers here, or we'll leave hypervisor in a bad state! */
    if (setjmp(unwind_to_main)>0) {
        status = 1;
        goto cleanup;
    }
    struct sigaction sa;
    sa.sa_handler = termination_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    /* Event loop */
    while (!terminating) {
        /* Find a pending Ether event; ignore (just unpause) events that are not from Ether */
        if (xc_evtchn_pending(xc_event_iface)!=event_port) {
            *shared_page_lock = 0;  /*how can it even be anything other than zero?*/
            xc_domain_unpause(xc_iface, domU); /*does it pause automatically?*/
            continue;
        }
        
        /* Clear event mask to enable next event */
        if (xc_evtchn_unmask(xc_event_iface, event_port)<0) {
            fprintf(stderr, "%s: could not unmask ether event; bailing...\n", argv[0]);
            status = 1;
            goto cleanup;
        }

        /* The Ether module of the hypervisor should have placed data in the shared page already. */
        if (!*shared_page_lock) {
            fprintf(stderr, "%s: warning: ether event notification but shared page lock is clear\n", argv[0]);
            xc_domain_unpause(xc_iface, domU);
            continue;
        }

        if (((volatile uint32_t*)shared_page)[1]==ETHER_NOTIFY_INSTRUCTION) {
            const struct instruction_info *insn_info = (const struct instruction_info*)(shared_page+8);
            rose_addr_t va = insn_info->registers.eip; /*or perhaps insn_info->guest_rip?*/
            SgAsmInstruction *insn = NULL;
            try {
                insn = analysis.disassembler->disassembleOne(insn_info->instruction, va, sizeof(insn_info->instruction), va);
                printf("0x%08"PRIx64": %s\n", va, unparseInstruction(insn).c_str());
            } catch(const Disassembler::Exception &e) {
                printf("0x%08"PRIx64": cannot disassemble: %s\n", va, e.mesg.c_str());
            }
        }

        /* Clear lock and continue */
        *shared_page_lock = 0;
        xc_domain_unpause(xc_iface, domU); /*not actually necessary for ETHER_NOTIFY_INSTRUCTION, but possibly for others*/
    }
    status = 0;

cleanup:
    if (comm.shared_page_ptr) {
        ether_terminate(xc_iface, domU);
        xc_domain_unpause(xc_iface, domU);
    }
    if (event_port>=0) {
        xc_evtchn_unbind(xc_event_iface, event_port);
    }
    if (shared_page) {
        *shared_page_lock = 0;
        munmap((void*)shared_page, getpagesize());
    }
    if (xc_event_iface>=0) {
        xc_evtchn_close(xc_event_iface);
    }
    if (xc_iface) {
        xc_interface_close(xc_iface);
    }
    return status;
}
