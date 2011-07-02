#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

#include "RSIM_Adapter.h"

/******************************************************************************************************************************
 *                                      Adapter Base Class
 ******************************************************************************************************************************/

bool
RSIM_Adapter::AdapterBase::NotImplemented::operator()(bool b, const Args &args)
{
    args.thread->tracing(TRACE_MISC)->mesg("%ssyscall %d is not implemented for I/O tracing; dumping core",
                                           adapter->prefix().c_str(), args.callno);
    args.thread->get_process()->dump_core(SIGILL);
    throw RSIM_Process::Exit(SIGILL & 0x7f, true);
}



/******************************************************************************************************************************
 *                                      Syscall Disabler Adapter (SyscallDisabler)
 ******************************************************************************************************************************/

void
RSIM_Adapter::SyscallDisabler::enable_syscall(int callno, bool state/*=true*/) 
{
    RTS_MUTEX(mutex) {
        syscall_state[callno] = state;
    } RTS_MUTEX_END;
}

void
RSIM_Adapter::SyscallDisabler::set_default(bool state)
{
    RTS_MUTEX(mutex) {
        dflt_state = state;
    } RTS_MUTEX_END;
}

bool
RSIM_Adapter::SyscallDisabler::is_enabled(int callno) const
{
    bool retval = dflt_state;
    RTS_MUTEX(mutex) {
        std::map<int, bool>::const_iterator found = syscall_state.find(callno);
        if (found!=syscall_state.end())
            retval = found->second;
    } RTS_MUTEX_END;
    return retval;
}

bool
RSIM_Adapter::SyscallDisabler::SyscallCB::operator()(bool state, const Args &args)
{
    if (state && !adapter->is_enabled(args.callno)) {
        RTS_Message *strace = args.thread->tracing(TRACE_SYSCALL);
        RTS_Message *mtrace = args.thread->tracing(TRACE_MISC);
        RSIM_Simulator *sim = args.thread->get_process()->get_simulator();
        RSIM_Simulator::SystemCall *sc = sim->syscall_is_implemented(args.callno) ?
                                         sim->syscall_implementation(args.callno) : NULL;
        
        /* Run the system call's "enter" callbacks in order to generate syscall trace, and add our own note. */
        if (sc)
            sc->enter.apply(true, RSIM_Simulator::SystemCall::Callback::Args(args.thread, args.callno));
        mtrace->mesg("%ssyscall %d is disabled", adapter->prefix().c_str(), args.callno);

        /* Set the syscall return value to "Function not implemented". The following callbacks may change the value. */
        uint32_t old_eax = args.thread->syscall_arg(-1);
        args.thread->syscall_return(-ENOSYS);

        /* Run the user-supplied body callbacks for disabled system calls. If those body callbacks changed the Boolean from
         * false to true, then make the system call enabled now. */
        if ((state=adapter->cblist.apply(false, RSIM_Simulator::SystemCall::Callback::Args(args.thread, args.callno)))) {
            adapter->enable_syscall(args.callno);
            strace->multipart_end(); /* because we won't be invoking the "leave" callbacks here */
            mtrace->mesg("%ssyscall %d is now enabled", adapter->prefix().c_str(), args.callno);
            args.thread->syscall_return(old_eax); /* restore the system call number */
        } else if (sc) {
            /* Run the system call "leave" callbacks to show the return value due to skipping. */
            sc->leave.apply(true, RSIM_Simulator::SystemCall::Callback::Args(args.thread, args.callno));
        }
    }
    return state;
}

void
RSIM_Adapter::SyscallDisabler::attach(RSIM_Simulator *sim)
{
    if (!syscall_cb)
        syscall_cb = new SyscallCB(this);

    sim->get_callbacks().add_syscall_callback(RSIM_Callbacks::BEFORE, syscall_cb);
}

void
RSIM_Adapter::SyscallDisabler::detach(RSIM_Simulator *sim)
{
    sim->get_callbacks().remove_syscall_callback(RSIM_Callbacks::BEFORE, syscall_cb);
}

/******************************************************************************************************************************
 *                                      I/O Tracing Adapter (TraceIO)
 ******************************************************************************************************************************/

bool
RSIM_Adapter::TraceIO::ReadWriteSyscall::operator()(bool b, const Args &args)
{
    int32_t     nbytes = args.thread->syscall_arg(-1);
    int32_t     fd     = args.thread->syscall_arg(0);
    rose_addr_t buf_va = args.thread->syscall_arg(1);
    if (nbytes>0 && tracer->is_tracing_fd(fd)) {
        RTS_Message *mesg = args.thread->tracing(TRACE_MISC);
        mesg->mesg("%s%s fd=%"PRId32", nbytes=%"PRIu32":\n", tracer->prefix().c_str(),
                   label.c_str(), fd, (uint32_t)nbytes);
        unsigned char *buf = new unsigned char[nbytes];
        if ((size_t)nbytes!=args.thread->get_process()->mem_read(buf, buf_va, nbytes)) {
            mesg->mesg("    short read");
        } else {
            RTS_MESSAGE(*mesg) {
                SgAsmExecutableFileFormat::hexdump(mesg->get_file(), (rose_addr_t)0, buf, nbytes, tracer->hd_format);
            } RTS_MESSAGE_END(true);
        }
        delete[] buf;
    }
    return b;
}

bool
RSIM_Adapter::TraceIO::ReadWriteVectorSyscall::operator()(bool b, const Args &args)
{
    int32_t nbytes = args.thread->syscall_arg(-1);
    int32_t fd = args.thread->syscall_arg(0);
    rose_addr_t iov_va = args.thread->syscall_arg(1);
    RTS_Message *mesg = args.thread->tracing(TRACE_MISC);
    if (nbytes>0 && tracer->is_tracing_fd(fd)) {
        mesg->mesg("%s%s fd=%"PRId32", nbytes=%"PRIu32":\n", tracer->prefix().c_str(),
                   label.c_str(), fd, (uint32_t)nbytes);
    }
    while (nbytes>0) {
        uint32_t vasz[2];
        if (8!=args.thread->get_process()->mem_read(vasz, iov_va, 8)) {
            mesg->mesg("    short read");
            nbytes = 0;
        } else {
            uint32_t sz = std::min(vasz[1], (uint32_t)nbytes);
            unsigned char *buf = new unsigned char[sz];
            if (sz!=args.thread->get_process()->mem_read(buf, vasz[0], sz)) {
                mesg->mesg("    short read");
            } else {
                RTS_MESSAGE(*mesg) {
                    SgAsmExecutableFileFormat::hexdump(mesg->get_file(), (rose_addr_t)0, buf, sz, tracer->hd_format);
                } RTS_MESSAGE_END(true);
            }
            delete[] buf;
            iov_va += 8;
            nbytes -= sz;
        }
    }
    return b;
}

bool
RSIM_Adapter::TraceIO::MmapSyscall::operator()(bool b, const Args &args)
{
    uint32_t result = args.thread->syscall_arg(-1);
    int32_t fd = args.thread->syscall_arg(4);
    if (result!=0 && tracer->is_tracing_fd(fd))
        args.thread->tracing(TRACE_MISC)->mesg("%snot tracing mmap'd fd=%"PRId32, tracer->prefix().c_str(), fd);
    return b;
}

bool
RSIM_Adapter::TraceIO::FtruncateSyscall::operator()(bool b, const Args &args)
{
    int32_t result = args.thread->syscall_arg(-1);
    int32_t fd = args.thread->syscall_arg(0);
    if (result>=0 && tracer->is_tracing_fd(fd)) {
        off_t len = args.thread->syscall_arg(1);
        args.thread->tracing(TRACE_MISC)->mesg("%struncating fd=%"PRId32", offset=%ld", tracer->prefix().c_str(),
                                               fd, (long)len);
    }
    return b;
}

bool
RSIM_Adapter::TraceIO::is_tracing_fd(int fd)
{
    bool retval = false;
    RTS_MUTEX(mutex) {
        retval = tracefd.find(fd)!=tracefd.end();
    } RTS_MUTEX_END;
    return retval;
}

void
RSIM_Adapter::TraceIO::trace_fd(int fd, bool how/*=true*/)
{
    RTS_MUTEX(mutex) {
        if (how) {
            tracefd.insert(fd);
        } else {
            tracefd.erase(fd);
        }
    } RTS_MUTEX_END;
}

void
RSIM_Adapter::TraceIO::attach(RSIM_Simulator *sim)
{
    /* Data transfer callbacks */
    sim->syscall_implementation(3/*read*/)          ->body.append(&read_cb);
    sim->syscall_implementation(4/*write*/)         ->body.append(&write_cb);
    sim->syscall_implementation(145/*readv*/)       ->body.append(&readv_cb);
    sim->syscall_implementation(146/*writev*/)      ->body.append(&writev_cb);
    sim->syscall_implementation(180/*pread64*/)     ->body.append(&NOT_IMPLEMENTED);
    sim->syscall_implementation(181/*pwrite64*/)    ->body.append(&NOT_IMPLEMENTED);
    sim->syscall_implementation(187/*sendfile*/)    ->body.append(&NOT_IMPLEMENTED);
    sim->syscall_implementation(239/*sendfile64*/)  ->body.append(&NOT_IMPLEMENTED);
    sim->syscall_implementation(315/*tee*/)         ->body.append(&NOT_IMPLEMENTED);
    sim->syscall_implementation(316/*vmsplice*/)    ->body.append(&NOT_IMPLEMENTED);

    /* Other file operations of interest */
    sim->syscall_implementation(93/*ftruncate*/)    ->body.append(&ftruncate_cb);
    sim->syscall_implementation(194/*ftruncate64*/) ->body.append(&NOT_IMPLEMENTED);

    /* Error callbacks */
    sim->syscall_implementation(90/*mmap*/)         ->body.append(&mmap_cb);
    sim->syscall_implementation(192/*mmap2*/)       ->body.append(&mmap_cb);
}

void
RSIM_Adapter::TraceIO::detach(RSIM_Simulator *sim)
{
    /* Data transfer callbacks */
    sim->syscall_implementation(3/*read*/)          ->body.erase(&read_cb);
    sim->syscall_implementation(4/*write*/)         ->body.erase(&write_cb);
    sim->syscall_implementation(145/*readv*/)       ->body.erase(&readv_cb);
    sim->syscall_implementation(146/*writev*/)      ->body.erase(&writev_cb);
    sim->syscall_implementation(180/*pread64*/)     ->body.erase(&NOT_IMPLEMENTED);
    sim->syscall_implementation(181/*pwrite64*/)    ->body.erase(&NOT_IMPLEMENTED);
    sim->syscall_implementation(187/*sendfile*/)    ->body.erase(&NOT_IMPLEMENTED);
    sim->syscall_implementation(239/*sendfile64*/)  ->body.erase(&NOT_IMPLEMENTED);
    sim->syscall_implementation(315/*tee*/)         ->body.erase(&NOT_IMPLEMENTED);
    sim->syscall_implementation(316/*vmsplice*/)    ->body.erase(&NOT_IMPLEMENTED);

    /* Other file operations of interest */
    sim->syscall_implementation(93/*ftruncate*/)    ->body.erase(&ftruncate_cb);
    sim->syscall_implementation(194/*ftruncate64*/) ->body.erase(&NOT_IMPLEMENTED);

    /* Error callbacks */
    sim->syscall_implementation(90/*mmap*/)         ->body.erase(&mmap_cb);
    sim->syscall_implementation(192/*mmap2*/)       ->body.erase(&mmap_cb);
}



/******************************************************************************************************************************
 *                                      File I/O Tracing Adapter (TraceFileIO)
 ******************************************************************************************************************************/

bool
RSIM_Adapter::TraceFileIO::OpenSyscall::operator()(bool b, const Args &args)
{
    int32_t fd = args.thread->syscall_arg(-1);
    if (fd>=0) {
        args.thread->tracing(TRACE_MISC)->mesg("%sactivating fd=%"PRId32, tracer->prefix().c_str(), fd);
        tracer->trace_fd(fd, true);
    }
    return b;
}

bool
RSIM_Adapter::TraceFileIO::DupSyscall::operator()(bool b, const Args &args)
{
    int32_t new_fd = args.thread->syscall_arg(-1);
    int32_t old_fd = args.thread->syscall_arg(0);
    if (new_fd>0) {
        if (tracer->is_tracing_fd(old_fd)) {
            args.thread->tracing(TRACE_MISC)->mesg("%sactivating fd=%"PRId32, tracer->prefix().c_str(), new_fd);
            tracer->trace_fd(new_fd, true);
        } else if (tracer->is_tracing_fd(new_fd)) {
            args.thread->tracing(TRACE_MISC)->mesg("%deactivating fd=%"PRId32, tracer->prefix().c_str(), new_fd);
            tracer->trace_fd(new_fd, false);
        }
    }
    return b;
}

bool
RSIM_Adapter::TraceFileIO::PipeSyscall::operator()(bool b, const Args &args)
{
    int32_t result = args.thread->syscall_arg(-1);
    rose_addr_t fds_va = args.thread->syscall_arg(0);
    if (result>=0) {
        int32_t fds[2];
        if (8!=args.thread->get_process()->mem_read(fds, fds_va, 8)) {
            args.thread->tracing(TRACE_MISC)->mesg("    short read");
        } else {
            args.thread->tracing(TRACE_MISC)->mesg("%sactivating fd=%"PRId32",%"PRId32, tracer->prefix().c_str(),
                                                   fds[0], fds[1]);
            tracer->trace_fd(fds[0], true);
            tracer->trace_fd(fds[1], true);
        }
    }
    return b;
}

bool
RSIM_Adapter::TraceFileIO::CloseSyscall::operator()(bool b, const Args &args)
{
    int32_t result = args.thread->syscall_arg(-1);
    int32_t fd = args.thread->syscall_arg(0);
    if (result>=0 && tracer->is_tracing_fd(fd)) {
        args.thread->tracing(TRACE_MISC)->mesg("%sdeactivating fd=%"PRId32"\n", tracer->prefix().c_str(), fd);
        tracer->trace_fd(fd, false);
    }
    return b;
}

void
RSIM_Adapter::TraceFileIO::attach(RSIM_Simulator *sim)
{
    TraceIO::attach(sim);

    sim->syscall_implementation(5/*open*/)          ->body.append(&open_cb);
    sim->syscall_implementation(8/*creat*/)         ->body.append(&open_cb);
    sim->syscall_implementation(41/*dup*/)          ->body.append(&dup_cb);
    sim->syscall_implementation(63/*dup2*/)         ->body.append(&dup_cb);
    sim->syscall_implementation(42/*pipe*/)         ->body.append(&pipe_cb);
    sim->syscall_implementation(6/*close*/)         ->body.append(&close_cb);
}

void
RSIM_Adapter::TraceFileIO::detach(RSIM_Simulator *sim)
{
    sim->syscall_implementation(5/*open*/)          ->body.erase(&open_cb);
    sim->syscall_implementation(8/*creat*/)         ->body.erase(&open_cb);
    sim->syscall_implementation(41/*dup*/)          ->body.erase(&dup_cb);
    sim->syscall_implementation(63/*dup2*/)         ->body.erase(&dup_cb);
    sim->syscall_implementation(42/*pipe*/)         ->body.erase(&pipe_cb);
    sim->syscall_implementation(6/*close*/)         ->body.erase(&close_cb);

    TraceIO::detach(sim);
}




#endif /* ROSE_ENABLE_SIMULATOR */
