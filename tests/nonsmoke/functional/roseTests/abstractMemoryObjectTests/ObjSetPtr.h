#ifndef _OBJSETPTR_H
#define _OBJSETPTR_H

using namespace AbstractMemoryObject;

// Pointer Wrapper recommended to be used in STL containers
class ObjSetPtr
{
    ObjSet* ptr;

public:
    // default ctor
    ObjSetPtr()
    {
        ROSE_ASSERT(false);
        ptr = NULL;
    }
    // ctor
    ObjSetPtr(ObjSet* _ptr)
    {
        ROSE_ASSERT(_ptr != NULL);
        this->ptr = _ptr;        
    }

    void setObjSetPtr(ObjSet* _ptr)
    {
        this->ptr = _ptr;
    }

    ObjSet* getObjSetPtr()
    {
        return ptr;
    }

    // Should we return ObjSet* or ObjSet
    ObjSet operator*() const 
    {
        return *ptr;
    }

    ObjSet* operator->() const
    {
        return ptr;
    }

    // copy ctor
    ObjSetPtr(const ObjSetPtr& that)
    {
        // to avoid dangling pointers
        safecopy(const_cast<ObjSetPtr&> (that));
    }

    ObjSetPtr& operator=(const ObjSetPtr& that)
    {
        // to avoid dangling pointers
        safecopy(const_cast<ObjSetPtr&> (that));
        return *this;
    }

    // sets that.ptr to NULL to avoid dangling refs
    void safecopy(ObjSetPtr &that)
    {
        ROSE_ASSERT(that.ptr != NULL);
        this->ptr = that.ptr;
        //ROSE_ASSERT(that.ptr != NULL);
        //if(this != &that) {
        //    this->ptr = that.ptr;
        //    that.ptr = NULL;
        //}
    }

    bool operator<(const ObjSetPtr& that) const
    {
        return (*ptr < *(that.ptr));
    }

    bool operator==(const ObjSetPtr& that) const
    {
        return (*ptr == *(that.ptr));
    }

    ~ObjSetPtr()
    {
        // TODO: Should we memory manage here ?
        // if(this->ptr) {
        //     std::cerr << "destroyed : " << ptr->toString() << std::endl;
        //     delete ptr;
        // }
    }
};
#endif
