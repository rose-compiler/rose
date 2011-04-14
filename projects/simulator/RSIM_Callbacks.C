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
        typename T::CallbackType *cb = dynamic_cast<typename T::CallbackType*>((*ci)->clone());
        assert(cb!=NULL);
        dst.append(cb);
    }
}

void
RSIM_Callbacks::init(const RSIM_Callbacks &other)
{
    copy_callback_vector(insn_pre,  other.insn_pre);
    copy_callback_vector(insn_post, other.insn_post);

    copy_callback_vector(syscall_pre,  other.syscall_pre);
    copy_callback_vector(syscall_post, other.syscall_post);

    copy_callback_vector(thread_pre,  other.thread_pre);
    copy_callback_vector(thread_post, other.thread_post);
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
        return insn_pre.erase(cb, ROSE_Callbacks::BACKWARD);
    return insn_post.erase(cb, ROSE_Callbacks::BACKWARD);
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
        return insn_pre.apply (prev, RSIM_Callbacks::InsnCallback::Args(thread, insn), ROSE_Callbacks::FORWARD);
    return     insn_post.apply(prev, RSIM_Callbacks::InsnCallback::Args(thread, insn), ROSE_Callbacks::FORWARD);
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
        return syscall_pre.erase(cb, ROSE_Callbacks::BACKWARD);
    return syscall_post.erase(cb, ROSE_Callbacks::BACKWARD);
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
        return syscall_pre.apply (prev, RSIM_Callbacks::SyscallCallback::Args(thread, callno), ROSE_Callbacks::FORWARD);
    return     syscall_post.apply(prev, RSIM_Callbacks::SyscallCallback::Args(thread, callno), ROSE_Callbacks::FORWARD);
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
        return thread_pre.erase(cb, ROSE_Callbacks::BACKWARD);
    return thread_post.erase(cb, ROSE_Callbacks::BACKWARD);
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
        return thread_pre.apply (prev, RSIM_Callbacks::ThreadCallback::Args(thread), ROSE_Callbacks::FORWARD);
    return     thread_post.apply(prev, RSIM_Callbacks::ThreadCallback::Args(thread), ROSE_Callbacks::FORWARD);
}

#endif /* ROSE_ENABLE_SIMULATOR */
