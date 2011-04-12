#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR

template <class T>
static bool
remove_last(std::vector<T*> &vec, T *cb)
{
    if (!cb)
        return false;

    for (size_t i=vec.size(); i>0; --i) {
        if (vec[i-1]==cb) {
            while (i<vec.size())
                vec[i-1] = vec[i];
            vec.resize(vec.size()-1);
            return true;
        }
    }

    return false;
}

template <class T>
static void
copy_callback_vector(std::vector<T*> &dst, const std::vector<T*> &src)
{
    dst.clear();
    for (typename std::vector<T*>::const_iterator ci=src.begin(); ci!=src.end(); ci++) {
        T *cb = dynamic_cast<T*>((*ci)->clone());
        assert(cb!=NULL);
        dst.push_back(cb);
    }
}

void
RSIM_Callbacks::init(const RSIM_Callbacks &other)
{
    /* FIXME: Possible deadlock when doing "a=b" and "b=a" concurrently. [RPM 2011-04-01] */
    RTS_MUTEX(mutex) {
        RTS_MUTEX(other.mutex) {
            copy_callback_vector<InsnCallback>(pre_insn, other.pre_insn);
            copy_callback_vector<InsnCallback>(post_insn, other.post_insn);
            copy_callback_vector<SyscallCallback>(pre_syscall, other.pre_syscall);
            copy_callback_vector<SyscallCallback>(post_syscall, other.post_syscall);
            copy_callback_vector<ThreadCallback>(pre_thread, other.pre_thread);
            copy_callback_vector<ThreadCallback>(post_thread, other.post_thread);
        } RTS_MUTEX_END;
    } RTS_MUTEX_END;
}

/******************************************************************************************************************************
 *                                      Instruction callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_insn_callback(When when, InsnCallback *cb)
{
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                pre_insn.push_back(cb);
            } else {
                post_insn.push_back(cb);
            }
        } RTS_MUTEX_END;
    }
}


bool
RSIM_Callbacks::remove_insn_callback(When when, InsnCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                retval = remove_last<InsnCallback>(pre_insn, cb);
            } else {
                retval = remove_last<InsnCallback>(post_insn, cb);
            }
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_insn_callbacks(When when)
{
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            pre_insn.clear();
        } else {
            post_insn.clear();
        }
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_insn_callbacks(When when, RSIM_Thread *thread, SgAsmInstruction *insn, bool prev)
{
    std::vector<InsnCallback*> to_call;
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            to_call = pre_insn;
        } else {
            to_call = post_insn;
        }
    } RTS_MUTEX_END;

    for (std::vector<InsnCallback*>::iterator ci=to_call.begin(); ci!=to_call.end(); ++ci) {
        assert(NULL!=*ci);
        prev = (**ci)(thread, insn, prev);
    }

    return prev;
}

/******************************************************************************************************************************
 *                                      System call callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_syscall_callback(When when, SyscallCallback *cb)
{
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                pre_syscall.push_back(cb);
            } else {
                post_syscall.push_back(cb);
            }
        } RTS_MUTEX_END;
    }
}


bool
RSIM_Callbacks::remove_syscall_callback(When when, SyscallCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                retval = remove_last<SyscallCallback>(pre_syscall, cb);
            } else {
                retval = remove_last<SyscallCallback>(post_syscall, cb);
            }
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_syscall_callbacks(When when)
{
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            pre_syscall.clear();
        } else {
            post_syscall.clear();
        }
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_syscall_callbacks(When when, RSIM_Thread *thread, int callno, bool prev)
{
    std::vector<SyscallCallback*> to_call;
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            to_call = pre_syscall;
        } else {
            to_call = post_syscall;
        }
    } RTS_MUTEX_END;

    for (std::vector<SyscallCallback*>::iterator ci=to_call.begin(); ci!=to_call.end(); ++ci) {
        assert(NULL!=*ci);
        prev = (**ci)(thread, callno, prev);
    }

    return prev;
}

/******************************************************************************************************************************
 *                                      Thread callbacks
 ******************************************************************************************************************************/

void
RSIM_Callbacks::add_thread_callback(When when, ThreadCallback *cb)
{
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                pre_thread.push_back(cb);
            } else {
                post_thread.push_back(cb);
            }
        } RTS_MUTEX_END;
    }
}


bool
RSIM_Callbacks::remove_thread_callback(When when, ThreadCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            if (when==BEFORE) {
                retval = remove_last<ThreadCallback>(pre_thread, cb);
            } else {
                retval = remove_last<ThreadCallback>(post_thread, cb);
            }
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_thread_callbacks(When when)
{
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            pre_thread.clear();
        } else {
            post_thread.clear();
        }
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_thread_callbacks(When when, RSIM_Thread *thread, bool prev)
{
    std::vector<ThreadCallback*> to_call;
    RTS_MUTEX(mutex) {
        if (when==BEFORE) {
            to_call = pre_thread;
        } else {
            to_call = post_thread;
        }
    } RTS_MUTEX_END;

    for (std::vector<ThreadCallback*>::iterator ci=to_call.begin(); ci!=to_call.end(); ++ci) {
        assert(NULL!=*ci);
        prev = (**ci)(thread, prev);
    }

    return prev;
}

#endif /* ROSE_ENABLE_SIMULATOR */
