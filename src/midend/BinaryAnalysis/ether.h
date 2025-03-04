#ifndef ROSE_ETHER_H
#define ROSE_ETHER_H

#ifdef ROSE_USE_ETHER

extern "C" {
#include <sys/mman.h>
#include <xenctrl.h>
#include <xen/domctl.h>
#include <xen/hvm/ether.h>
}

#include <sys/mman.h>

class Ether {
private:
    Ether() {}
    
public:
    struct Exception {
        Exception(const std::string &mesg)
            : mesg(mesg) {}
        std::string mesg;
    };

    /** This constructor contacts the hypervisor via xenctrl and attaches to the domainU specified by @p dom. */
    Ether(domid_t dom)
        : xc_iface(-1), xc_event_iface(-1), dom(0), shared_page_mfn(0), shared_page(NULL), event_port(-1) {
        
        if ((xc_iface = xc_interface_open())<0)
            throw Exception("cannot contact Xen hypervisor");
        if ((xc_event_iface = xc_evtchn_open())<0)
            throw Exception("cannot open event channel");
        
        CmdInit cmd;
        domctl(cmd, dom);
        this->dom = dom;
        shared_page_mfn = cmd.u.ether.comm.shared_page_mfn;

        /* The hypervisor allocated a page of memory that we now map into our own address space. We should call munmap()
         * during destruction. The hypervisor will initialize the page to all zeros. */
        shared_page = (volatile unsigned char*)xc_map_foreign_range(xc_iface, DOMID_XEN, getpagesize(),
                                                                    PROT_READ|PROT_WRITE, shared_page_mfn);
        if (!shared_page)
            throw("cannot map shared page");

        /* The Xen hypervisor allocated an unbound port whose number was returned. This event channel will be used to notify
         * us when Ether-related things happen in the domU.  So now we create the local end of the channel and bind it with
         * the remote end. */
        event_port = xc_evtchn_bind_interdomain(xc_event_iface, DOMID_SELF, cmd.u.ether.comm.event_channel_port);
        if (event_port<0)
            throw("cannot bind ether event port");

        /* DEBUG */
        printf("After init:\n");
        printf("    domain:             %d\n", dom);
        printf("    xc_iface:           %d\n", xc_iface);
        printf("    xc_event_iface:     %d\n", xc_event_iface);
        printf("    shared_page:        %p\n", shared_page);
        printf("    shared_page_mfn:    0x%lx\n", shared_page_mfn);
        printf("    event_channel_port: %d\n", cmd.u.ether.comm.event_channel_port);
        printf("    event_port:         %u\n", event_port);
    }

    /** Terminates the connection with the hypervisor. */
    ~Ether() {
        if (shared_page!=NULL) {
            domctl(CmdTerminate());
            xc_domain_unpause(xc_iface, dom);
        }
        if (event_port>=0) {
            xc_evtchn_unbind(xc_event_iface, event_port);
            event_port = -1;
        }
        if (shared_page!=NULL) {
            if (is_lock_held())
                release_lock();
            munmap((void*)shared_page, getpagesize());
            shared_page = NULL;
            shared_page_mfn = 0;
        }
        if (xc_event_iface>=0) {
            xc_evtchn_close(xc_event_iface);
            xc_event_iface = -1;
        }
        if (xc_iface) {
            xc_interface_close(xc_iface);
            xc_iface = -1;
        }
    }

    /** Reads @p size bytes of memory from virtual address @p va in the connected domainU into a local buffer. */
    void readguest(Rose::BinaryAnalysis::Address va, unsigned char *buffer, size_t size) {
        domctl(CmdReadGuest(va, buffer, size));
    }
    
    /** Turn single-stepping on or off. This controls whether the hypervisor will cause a process to single step, but does
     *  not enable reporting of the single stepping events to this client. */
    void set_single_step(bool status) {
        domctl(CmdSingleStep(status));
    }
    
    /** Turns single-step notification on or off. This determines whether single stepping in the hypervisor will be reported
     *  to this client. You must also enable single stepping with set_single_step(). */
    void set_single_step_notify(bool status) {
        domctl(CmdSingleStepNotify(status));
    }

    /** Specifies whether the hypervisor will notify us of all write operations. */
    void set_memwrite_notify(bool status) {
        domctl(CmdMemwriteNotify(status));
    }

    /** Specifies whether the hypervisor will notify us when a process attempts to execute memory that was earlier modified. */
    void set_unpack_notify(bool status) {
        domctl(CmdUnpackNotify(status));
    }

    /** Specifies whether the hypervisor will notify us when a syscall is executed.  The @p new_eip is the virtual address
     *  used by the hypervisor to detect system calls. Specifying a @p new_eip of zero will cause the hypervisor to use a
     *  built-in default. */
    void set_sysenter(bool new_eip) {
        domctl(CmdSysenter(new_eip));
    }

    /** Add a process name filter. The hypervisor will only report events for a process with this name. */
    void add_name_filter(const std::string &name) {
        domctl(CmdAddName(name));
    }


    void add_cr3_filter(Rose::BinaryAnalysis::Address cr3) {
        domctl(CmdAddCR3(cr3));
    }

    /** Returns lock status. True means the lock is held by us; false means the lock is clear and the hypervisor is free to
     *  modify the shared page. */
    bool is_lock_held() {
        if (shared_page==NULL)
            throw Exception("no shared page");
        return *(volatile uint32_t*)shared_page ? true : false;
    }
    
    /** Clear the shared page lock. This should be done whenever the hypervisor has notified us that there is an Ether
     *  event pending. The domainU will block in the hypervisor until the lock is cleared. (The hypervisor uses a timer to
     *  detect if the client has died, so the lock must be cleared in a reasonable amount of time.) */
    void release_lock() {
        if (shared_page==NULL)
            throw Exception("no shared page");
        if (!is_lock_held())
            throw Exception("shared page lock is not set\n");
        *(volatile uint32_t*)shared_page = 0;
    }

    /** Returns the next event type. This function will block until an event becomes ready.  It then returns the event type
     *  and additional information can be read from the shared memory page.  If a non-Ether event is received it will be
     *  ignored and the domainU will be resumed. */
    int next_event() {
        if (shared_page==NULL)
            throw Exception("no shared page");
        while (1) {
            if (xc_evtchn_pending(xc_event_iface)!=event_port) {
                xc_domain_unpause(xc_iface, dom);
            } else if (xc_evtchn_unmask(xc_event_iface, event_port)<0) {
                throw Exception("cannot unmask ether event");
            } else if (!is_lock_held()) {
                xc_domain_unpause(xc_iface, dom);
                throw Exception("ether event but shared page lock is clear");
            }

            return ((volatile uint32_t*)shared_page)[1];
        }
    }

    /** Returns a pointer into the shared page. The caller typically casts that pointer to some other data structure. */
    const void *event_data() {
        if (!is_lock_held())
            throw Exception("shared page lock is not set");
        if (!shared_page)
            throw Exception("no shared page");
        return (const void*)(shared_page+8);
    }
    
    /** Cause domainU to resume execution. This should be called after the client is done processing an event. It releases the
     *  shared page lock and resumes execution of the domainU. */
    void resume() {
        if (is_lock_held())  /*perhaps this should be an exception instead!*/
            release_lock();
        xc_domain_unpause(xc_iface, dom); /*not always necessary, but doesn't hurt*/
    }

private:
    struct CmdInit: public xen_domctl {
        CmdInit() {
            u.ether.command_code = XEN_DOMCTL_ETHER_INIT;
            memset(&u.ether.comm, 0, sizeof(u.ether.comm));
        }
    };

    struct CmdTerminate: public xen_domctl {
        CmdTerminate() {
            u.ether.command_code = XEN_DOMCTL_ETHER_TERMINATE;
        }
    };

    struct CmdReadGuest: public xen_domctl {
        CmdReadGuest(Rose::BinaryAnalysis::Address va, void *buffer, size_t size) {
            u.ether.command_code = XEN_DOMCTL_ETHER_READ_GUEST;
            u.ether.guest_va = va;
            u.ether.guest_buffer = (unsigned char*)buffer;
            u.ether.data_length = size;
            /* This is actually necessary to get this memory paged in so the hypervisor can write to it. */
            memset(buffer, 0, size);
        }
    };

    struct CmdSingleStep: public xen_domctl {
        CmdSingleStep(bool status) {
            u.ether.command_code = XEN_DOMCTL_ETHER_SINGLE_STEP;
            u.ether.on_or_off = status;
        }
    };

    struct CmdSingleStepNotify: public xen_domctl {
        CmdSingleStepNotify(bool status) {
            u.ether.command_code = XEN_DOMCTL_ETHER_SS_DETECT;
            u.ether.on_or_off = status;
        }
    };
    
    struct CmdMemwriteNotify: public xen_domctl {
        CmdMemwriteNotify(bool status) {
            u.ether.command_code = XEN_DOMCTL_ETHER_MEMWRITE;
            u.ether.on_or_off = status;
        }
    };
    
    struct CmdUnpackNotify: public xen_domctl {
        CmdUnpackNotify(bool status) {
            u.ether.command_code = XEN_DOMCTL_ETHER_UNPACK;
            u.ether.on_or_off = status;
        }
    };

    struct CmdSysenter: public xen_domctl {
        CmdSysenter(Rose::BinaryAnalysis::Address eip) {
            u.ether.command_code = XEN_DOMCTL_ETHER_SET_SYSENTER;
            u.ether.sysenter_cs = 0;
            u.ether.sysenter_eip = eip;
        }
    };

    struct CmdAddCR3: public xen_domctl {
        CmdAddCR3(Rose::BinaryAnalysis::Address cr3) {
            u.ether.command_code = XEN_DOMCTL_ETHER_ADD_CR3;
            u.ether.cr3_value = cr3;
        }
    };

    struct CmdAddName: public xen_domctl {
        CmdAddName(const std::string &name) {
            u.ether.command_code = XEN_DOMCTL_ETHER_ADD_NAME;
            u.ether.data_length = name.size() + 1;
            u.ether.guest_buffer = (unsigned char*)(name.c_str());
        }
    };
    
    bool connected() {
        return xc_iface>=0 && dom>0;
    }

    /* Send an ether command to the hypervisor.  The command should have been constructed with one of the Cmd* constructors in
     * order to initialize it properly.  The @p dom parameter is normally only specified by the constructor in order
     * to send a message to the hypervisor before this object is connected. */
    void domctl(const xen_domctl &cmd, domid_t dom=0) {
        domctl(const_cast<xen_domctl*>(&cmd), dom);
    }
    void domctl(xen_domctl &cmd, domid_t dom=0) {
        domctl(&cmd, dom);
    }
    void domctl(xen_domctl *cmd, domid_t dom=0) {
        if (connected()) {
            if (dom>0 && dom!=this->dom)
                throw Exception("contradicting domain IDs");
            dom = this->dom;
        } else if (dom<=0) {
            throw Exception("not connected");
        }

        cmd->domain = dom;
        cmd->cmd = XEN_DOMCTL_ether;
        int result = xc_domctl(xc_iface, cmd);
        if (result<0) {
            char buf[256];
            snprintf(buf, sizeof(buf), "xc_domctl(request=%u) failed", cmd->u.ether.command_code);
            throw Exception(buf);
        }
    }
    
private:
    int xc_iface;                               /**< Main interface to the Xen controller */
    int xc_event_iface;                         /**< Main Xen controller event channel */
    domid_t dom;                                /**< ID number of the DomainU; See "xm list" shell command. */
    
    Rose::BinaryAnalysis::Address shared_page_mfn;      /**< Machine frame number of shared page */
    volatile unsigned char *shared_page;        /**< Shared page mapped to our address space */
    
    evtchn_port_t event_port;                   /**< Our end of the event channel that was created by the constructor. */
};

#endif /*ROSE_USE_ETHER*/
#endif /*ROSE_ETHER_H*/
