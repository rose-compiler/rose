#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

template <class T>
static void
copy_callback_vector(T &dst, const T &src)
{
    dst.clear();
    typename T::CBList callbacks = src.callbacks();
    for (typename T::CBList::iterator ci=callbacks.begin(); ci!=callbacks.end(); ci++) {
        typename T::CallbackType *old_cb = *ci;
        assert(old_cb!=NULL);
        typename T::CallbackType *new_cb = dynamic_cast<typename T::CallbackType*>(old_cb->clone());
        assert(old_cb!=NULL);
        dst.append(new_cb);
    }
}

void
RSIM_Callbacks::init(const RSIM_Callbacks &other)
{
    copy_callback_vector(insn_pre,  other.insn_pre);
    copy_callback_vector(insn_post, other.insn_post);

    copy_callback_vector(memory_pre, other.memory_pre);
    copy_callback_vector(memory_post, other.memory_post);

    copy_callback_vector(syscall_pre,  other.syscall_pre);
    copy_callback_vector(syscall_post, other.syscall_post);

    copy_callback_vector(signal_pre,  other.signal_pre);
    copy_callback_vector(signal_post, other.signal_post);

    copy_callback_vector(thread_pre,  other.thread_pre);
    copy_callback_vector(thread_post, other.thread_post);

    copy_callback_vector(process_pre,  other.process_pre);
    copy_callback_vector(process_post, other.process_post);
}

/******************************************************************************************************************************
 *                                      Instruction callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_insn_callback(When when, InsnCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            insn_pre.append(cb);
        } else {
            insn_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_insn_callback(When when, InsnCallback *cb)
{
    if (when==BEFORE)
        return insn_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return insn_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_insn_callbacks(When when)
{
    if (when==BEFORE) {
        insn_pre.clear();
    } else {
        insn_post.clear();
    }
}

bool
RSIM_Callbacks::call_insn_callbacks(When when, RSIM_Thread *thread, SgAsmInstruction *insn, bool prev)
{
    if (when==BEFORE)
        return insn_pre.apply (prev, InsnCallback::Args(thread, insn), Rose::Callbacks::FORWARD);
    return     insn_post.apply(prev, InsnCallback::Args(thread, insn), Rose::Callbacks::FORWARD);
}

/******************************************************************************************************************************
 *                                      Memory callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_memory_callback(When when, MemoryCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            memory_pre.append(cb);
        } else {
            memory_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_memory_callback(When when, MemoryCallback *cb)
{
    if (when==BEFORE)
        return memory_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return memory_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_memory_callbacks(When when)
{
    if (when==BEFORE) {
        memory_pre.clear();
    } else {
        memory_post.clear();
    }
}

bool
RSIM_Callbacks::call_memory_callbacks(When when,
                                      RSIM_Process *process, unsigned how, unsigned req_perms,
                                      rose_addr_t va, size_t nbytes, void *buffer, size_t &nbytes_xfer,
                                      bool prev)
{
    if (when==BEFORE)
        return memory_pre.apply (prev,
                                 MemoryCallback::Args(process, how, req_perms, va, nbytes, buffer, &nbytes_xfer),
                                 Rose::Callbacks::FORWARD);
    return     memory_post.apply(prev,
                                 MemoryCallback::Args(process, how, req_perms, va, nbytes, buffer, &nbytes_xfer),
                                 Rose::Callbacks::FORWARD);
}

/******************************************************************************************************************************
 *                                      System call callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_syscall_callback(When when, SyscallCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            syscall_pre.append(cb);
        } else {
            syscall_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_syscall_callback(When when, SyscallCallback *cb)
{
    if (when==BEFORE)
        return syscall_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return syscall_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_syscall_callbacks(When when)
{
    if (when==BEFORE) {
        syscall_pre.clear();
    } else {
        syscall_post.clear();
    }
}

bool
RSIM_Callbacks::call_syscall_callbacks(When when, RSIM_Thread *thread, int callno, bool prev)
{
    if (when==BEFORE)
        return syscall_pre.apply (prev, SyscallCallback::Args(thread, callno), Rose::Callbacks::FORWARD);
    return     syscall_post.apply(prev, SyscallCallback::Args(thread, callno), Rose::Callbacks::FORWARD);
}

/******************************************************************************************************************************
 *                                      Signal callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_signal_callback(When when, SignalCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            signal_pre.append(cb);
        } else {
            signal_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_signal_callback(When when, SignalCallback *cb)
{
    if (when==BEFORE)
        return signal_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return signal_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_signal_callbacks(When when)
{
    if (when==BEFORE) {
        signal_pre.clear();
    } else {
        signal_post.clear();
    }
}

bool
RSIM_Callbacks::call_signal_callbacks(When when, RSIM_Thread *thread, int signo, RSIM_SignalHandling::SigInfo *info,
                                      SignalCallback::Reason reason, bool prev)
{
    if (when==BEFORE)
        return signal_pre.apply (prev, SignalCallback::Args(thread, signo, info, reason), Rose::Callbacks::FORWARD);
    return     signal_post.apply(prev, SignalCallback::Args(thread, signo, info, reason), Rose::Callbacks::FORWARD);
}

/******************************************************************************************************************************
 *                                      Thread callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_thread_callback(When when, ThreadCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            thread_pre.append(cb);
        } else {
            thread_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_thread_callback(When when, ThreadCallback *cb)
{
    if (when==BEFORE)
        return thread_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return thread_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_thread_callbacks(When when)
{
    if (when==BEFORE) {
        thread_pre.clear();
    } else {
        thread_post.clear();
    }
}

bool
RSIM_Callbacks::call_thread_callbacks(When when, RSIM_Thread *thread, bool prev)
{
    if (when==BEFORE)
        return thread_pre.apply (prev, ThreadCallback::Args(thread), Rose::Callbacks::FORWARD);
    return     thread_post.apply(prev, ThreadCallback::Args(thread), Rose::Callbacks::FORWARD);
}

/******************************************************************************************************************************
 *                                      Process callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_process_callback(When when, ProcessCallback *cb)
{
    if (cb) {
        if (when==BEFORE) {
            process_pre.append(cb);
        } else {
            process_post.append(cb);
        }
    }
}

bool
RSIM_Callbacks::remove_process_callback(When when, ProcessCallback *cb)
{
    if (when==BEFORE)
        return process_pre.erase(cb, Rose::Callbacks::BACKWARD);
    return process_post.erase(cb, Rose::Callbacks::BACKWARD);
}

void
RSIM_Callbacks::clear_process_callbacks(When when)
{
    if (when==BEFORE) {
        process_pre.clear();
    } else {
        process_post.clear();
    }
}

bool
RSIM_Callbacks::call_process_callbacks(When when, RSIM_Process *process, RSIM_Callbacks::ProcessCallback::Reason reason, bool prev)
{
    if (when==BEFORE)
        return process_pre.apply (prev, ProcessCallback::Args(process, reason), Rose::Callbacks::FORWARD);
    return     process_post.apply(prev, ProcessCallback::Args(process, reason), Rose::Callbacks::FORWARD);
}

#endif /* ROSE_ENABLE_SIMULATOR */
