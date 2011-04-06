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
            copy_callback_vector<ThreadCallback>(pre_thread, other.pre_thread);
            copy_callback_vector<ThreadCallback>(post_thread, other.post_thread);
        } RTS_MUTEX_END;
    } RTS_MUTEX_END;
}

void
RSIM_Callbacks::add_pre_insn(InsnCallback *cb)
{
    if (cb) {
        RTS_MUTEX(mutex) {
            pre_insn.push_back(cb);
        } RTS_MUTEX_END;
    }
}


bool
RSIM_Callbacks::remove_pre_insn(InsnCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            retval = remove_last<InsnCallback>(pre_insn, cb);
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_pre_insn()
{
    RTS_MUTEX(mutex) {
        pre_insn.clear();
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_pre_insn(RSIM_Thread *thread, SgAsmInstruction *insn, bool prev) const
{
    std::vector<InsnCallback*> to_call;
    RTS_MUTEX(mutex) {
        to_call = pre_insn;
    } RTS_MUTEX_END;

    for (size_t i=to_call.size(); i>0; --i)
        prev = (*to_call[i-1])(thread, insn, prev);

    return prev;
}

void
RSIM_Callbacks::add_post_insn(InsnCallback *cb)
{
    
    if (cb) {
        RTS_MUTEX(mutex) {
            post_insn.push_back(cb);
        } RTS_MUTEX_END;
    }
}

bool
RSIM_Callbacks::remove_post_insn(InsnCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            retval = remove_last<InsnCallback>(pre_insn, cb);
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_post_insn()
{
    RTS_MUTEX(mutex) {
        post_insn.clear();
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_post_insn(RSIM_Thread *thread, SgAsmInstruction *insn, bool prev) const
{
    std::vector<InsnCallback*> to_call;
    RTS_MUTEX(mutex) {
        to_call = post_insn;
    } RTS_MUTEX_END;

    for (std::vector<InsnCallback*>::iterator ci=to_call.begin(); ci!=to_call.end(); ci++)
        prev = (**ci)(thread, insn, prev);
    return prev;
}

void
RSIM_Callbacks::add_pre_thread(ThreadCallback *cb)
{
    if (cb) {
        RTS_MUTEX(mutex) {
            pre_thread.push_back(cb);
        } RTS_MUTEX_END;
    }
}


bool
RSIM_Callbacks::remove_pre_thread(ThreadCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            retval = remove_last<ThreadCallback>(pre_thread, cb);
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_pre_thread()
{
    RTS_MUTEX(mutex) {
        pre_thread.clear();
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_pre_thread(RSIM_Thread *thread, bool prev) const
{
    std::vector<ThreadCallback*> to_call;
    RTS_MUTEX(mutex) {
        to_call = pre_thread;
    } RTS_MUTEX_END;

    for (size_t i=to_call.size(); i>0; --i)
        prev = (*to_call[i-1])(thread, prev);

    return prev;
}

void
RSIM_Callbacks::add_post_thread(ThreadCallback *cb)
{
    
    if (cb) {
        RTS_MUTEX(mutex) {
            post_thread.push_back(cb);
        } RTS_MUTEX_END;
    }
}

bool
RSIM_Callbacks::remove_post_thread(ThreadCallback *cb)
{
    bool retval = false;
    if (cb) {
        RTS_MUTEX(mutex) {
            retval = remove_last<ThreadCallback>(post_thread, cb);
        } RTS_MUTEX_END;
    }
    return retval;
}

void
RSIM_Callbacks::clear_post_thread()
{
    RTS_MUTEX(mutex) {
        post_thread.clear();
    } RTS_MUTEX_END;
}

bool
RSIM_Callbacks::call_post_thread(RSIM_Thread *thread, bool prev) const
{
    std::vector<ThreadCallback*> to_call;
    RTS_MUTEX(mutex) {
        to_call = post_thread;
    } RTS_MUTEX_END;

    for (std::vector<ThreadCallback*>::iterator ci=to_call.begin(); ci!=to_call.end(); ci++)
        prev = (**ci)(thread, prev);
    return prev;
}

#endif /* ROSE_ENABLE_SIMULATOR */
