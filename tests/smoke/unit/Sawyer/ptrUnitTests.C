// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/SharedPointer.h>

#include <boost/algorithm/string/predicate.hpp>
#include <Sawyer/Assert.h>
#include <sstream>

using namespace Sawyer;

struct ObjectStats {
    bool deleted;
    ObjectStats(): deleted(false) {}
};

struct MonitoredObject: Sawyer::SharedObject {
    size_t nrefs_;                                      // try to confuse reference counting
    ObjectStats &stats;
    MonitoredObject(ObjectStats &stats): nrefs_(1000), stats(stats) {
        ASSERT_always_require(nrefs_=1000);
    }
    virtual ~MonitoredObject() {
        ASSERT_always_forbid(stats.deleted);
        ASSERT_always_require(nrefs_==1000);
        stats.deleted = true;
    }
};

struct Subclass: MonitoredObject {
    int dummy;
    Subclass(ObjectStats &stats, int dummy): MonitoredObject(stats), dummy(dummy) {}
};

// Tests that a default constructor creates the equivalent of a null pointer.
static void default_ctor() {
    SharedPointer<MonitoredObject> ptr;
    ASSERT_always_require(getRawPointer(ptr)==NULL);
    ASSERT_always_require(ownershipCount(ptr)==0);
}

// Tests that a shared pointer can be explicitly created from a null pointer.
static void null_ctor() {
    MonitoredObject *obj = NULL;
    SharedPointer<MonitoredObject> ptr3(obj);
    ASSERT_always_require(getRawPointer(ptr3)==NULL);
}

// Tests that a shared pointer can be created from a pointer to a raw object.
static void raw_ctor() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    {
        SharedPointer<MonitoredObject> ptr(obj);
        ASSERT_always_require(getRawPointer(ptr)==obj);
        ASSERT_always_require(ownershipCount(ptr)==1);
    }
    ASSERT_always_require(stats.deleted);
}

// Tests that a shared pointer can point to a const-qualified object.
static void raw_ctor_const() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    {
        SharedPointer<const MonitoredObject> ptr(obj);
        ASSERT_always_require(getRawPointer(ptr)==obj);
        ASSERT_always_require(ownershipCount(ptr)==1);
    }
    ASSERT_always_require(stats.deleted);
}

// Tests that a shared pointer can point to a derived class.
static void raw_ctor_upcast() {
    ObjectStats stats;
    Subclass *obj = new Subclass(stats, 123);
    {
        SharedPointer<MonitoredObject> ptr(obj);
        ASSERT_always_require(getRawPointer(ptr)==obj);
        ASSERT_always_require(ownershipCount(ptr)==1);
    }
    ASSERT_always_require(stats.deleted);
}

// Tests that construction fails when a shared pointer is constructed from an object that is already owned by another shared
// pointer.
static void double_ownership() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);

    SharedPointer<MonitoredObject> ptr1(obj);
#if 0 // FIXME[Robb Matzke 2014-06-09]: needs to throw so we can catch
    SharedPointer<MonitoredObject> ptr2(obj);           // should fail
#endif
}

// Tests that a shared pointer can be copy-constructed
static void copy_ctor() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    SharedPointer<MonitoredObject> ptr1(obj);
    {
        SharedPointer<MonitoredObject>ptr2(ptr1);
        ASSERT_always_require(getRawPointer(ptr1)==getRawPointer(ptr2));
        ASSERT_always_require(ownershipCount(ptr1)==2);
        ASSERT_always_require(ownershipCount(ptr2)==2);
    }
    ASSERT_always_require(ownershipCount(ptr1)==1);
    ASSERT_always_forbid(stats.deleted);
}

// Tests that a shared pointer can be copy constructed toa const-qualified type
static void copy_ctor_const() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    SharedPointer<MonitoredObject> ptr1(obj);
    {
        SharedPointer<const MonitoredObject> ptr2(ptr1);
        ASSERT_always_require(getRawPointer(ptr1)==getRawPointer(ptr2));
        ASSERT_always_require(ownershipCount(ptr1)==2);
        ASSERT_always_require(ownershipCount(ptr2)==2);
    }
    ASSERT_always_require(ownershipCount(ptr1)==1);
    ASSERT_always_forbid(stats.deleted);
}

// Tests that one SharedPointer<T> can be constructed from SharedPointer<U> if U is a subclass of T.
static void copy_ctor_upcast() {
    ObjectStats stats;
    Subclass *obj = new Subclass(stats, 123);
    SharedPointer<Subclass> ptr1(obj);
    {
        SharedPointer<MonitoredObject> ptr2(ptr1);
        ASSERT_always_require(getRawPointer(ptr1)==getRawPointer(ptr2));
        ASSERT_always_require(ownershipCount(ptr1)==2);
        ASSERT_always_require(ownershipCount(ptr2)==2);
    }
    ASSERT_always_require(ownershipCount(ptr1)==1);
    ASSERT_always_forbid(stats.deleted);
}

// Tests that assignment from SharedPointer<T> to SharedPointer<T> works when the destination is null
static void assignment_to_null() {
    {
        SharedPointer<MonitoredObject> src;
        SharedPointer<MonitoredObject> dst;
        dst = src;
        ASSERT_always_require(getRawPointer(dst)==NULL);
    }

    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    SharedPointer<MonitoredObject> src(obj);
    {
        SharedPointer<MonitoredObject> dst;
        dst = src;
        ASSERT_always_require(getRawPointer(src)==getRawPointer(dst));
        ASSERT_always_require(ownershipCount(src)==2);
        ASSERT_always_require(ownershipCount(dst)==2);
    }
    ASSERT_always_require(ownershipCount(src)==1);
    ASSERT_always_forbid(stats.deleted);
}

// Tests that assignment from SharedPointer<T> to SharedPointer<T> works when the destination is non-null
static void assignment() {
    {
        SharedPointer<MonitoredObject> src;
        ObjectStats dststats;
        MonitoredObject *dstobj = new MonitoredObject(dststats);
        SharedPointer<MonitoredObject> dst(dstobj);
        dst = src;
        ASSERT_always_require(getRawPointer(dst)==NULL);
        ASSERT_always_require(dststats.deleted);
    }

    {
        ObjectStats dststats;
        MonitoredObject *dstobj = new MonitoredObject(dststats);
        SharedPointer<MonitoredObject> dst(dstobj);
        dst = Nothing();
        ASSERT_always_require(getRawPointer(dst)==NULL);
        ASSERT_always_require(dststats.deleted);
    }

    ObjectStats srcstats, dststats;
    MonitoredObject *srcobj = new MonitoredObject(srcstats);
    SharedPointer<MonitoredObject> src(srcobj);
    {
        MonitoredObject *dstobj = new MonitoredObject(dststats);
        SharedPointer<MonitoredObject> dst(dstobj);
        dst = src;
        ASSERT_always_require(getRawPointer(src)==getRawPointer(dst));
        ASSERT_always_require(ownershipCount(src)==2);
        ASSERT_always_require(ownershipCount(dst)==2);
        ASSERT_always_require(dststats.deleted);
    }
    ASSERT_always_require(ownershipCount(src)==1);
    ASSERT_always_forbid(srcstats.deleted);
}

// Tests that assignment from SharedPointer<U> to SharedPointer<T> works when U is a subclass of T
static void assignment_upcast() {
    ObjectStats srcstats;
    Subclass *srcobj = new Subclass(srcstats, 123);
    SharedPointer<Subclass> src(srcobj);

    ObjectStats dststats;
    MonitoredObject *dstobj = new MonitoredObject(dststats);

    {
        SharedPointer<MonitoredObject> dst(dstobj);
        dst = src;
        ASSERT_always_require(getRawPointer(src)==getRawPointer(dst));
        ASSERT_always_require(ownershipCount(src)==2);
    }
    ASSERT_always_require(ownershipCount(src)==1);
}

// Tests that assigment like "ptr=ptr" works especially when the reference count is one.
static void assignment_self() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    {
        SharedPointer<MonitoredObject> dst(obj);
        dst = dst; // compiler warning expected -- we're testing self-assignment
        ASSERT_always_forbid(stats.deleted);
        ASSERT_always_require(getRawPointer(dst)==obj);
        ASSERT_always_require(ownershipCount(dst)==1);
    }
    ASSERT_always_require(stats.deleted);
}

// Tests that a pointer can be cleared
static void clear() {
    ObjectStats stats;
    MonitoredObject *obj = new MonitoredObject(stats);
    SharedPointer<MonitoredObject> ptr(obj);
    clear(ptr);
    ASSERT_always_require(getRawPointer(ptr)==NULL);
    ASSERT_always_require(stats.deleted);
    clear(ptr);
}

// Tests pointer dereference
static void dereference() {
    ObjectStats stats;
    Subclass *obj = new Subclass(stats, 123);
    SharedPointer<Subclass> ptr(obj);
    ASSERT_always_require(getRawPointer(ptr)==obj);
    ASSERT_always_require(&*ptr==obj);
    ASSERT_always_require((*ptr).dummy==123);
}

// Tests that dereferencing a null pointer fails
static void dereference_null() {
#if 0 // FIXME[Robb Matzke 2014-06-09]: needs to be able to throw so we can catch
    SharedPointer<Subclass> ptr;
    *ptr;
#endif
}

// Tests the arrow operator
static void arrow() {
    ObjectStats stats;
    Subclass *obj = new Subclass(stats, 123);
    SharedPointer<Subclass> ptr(obj);
    ASSERT_always_require(getRawPointer(ptr)==obj);
    ASSERT_always_require(ptr->dummy==123);
}

// Tests that the arrow operator fails for null pointers
static void arrow_null() {
#if 0 // FIXME[Robb Matzke 2014-06-09]: needs to be able to throw so we can catch
    SharedPointer<Subclass> ptr;
    ptr->dummy;
#endif
}

// Tests that we can say things like "if (ptr)"
static void implicit_bool() {
    ObjectStats stats;
    SharedPointer<Subclass> ptr;
    ASSERT_always_forbid(ptr);
    ASSERT_always_require(!ptr);
    ASSERT_always_require(ptr==NULL);
    ASSERT_always_require(ptr==0);
    ASSERT_always_forbid(ptr!=NULL);
    ASSERT_always_forbid(ptr!=0);
    ASSERT_always_forbid(ptr && ptr);
    ASSERT_always_forbid(ptr || ptr);

    {
        std::stringstream ss;
        ss <<ptr;
        ASSERT_always_require2(ss.str()=="0" || ss.str()=="0x0", "ss.str()==" + ss.str());
    }
    
    Subclass *obj = new Subclass(stats, 123);
    ptr = SharedPointer<Subclass>(obj);
    ASSERT_always_require(ptr);
    ASSERT_always_forbid(!ptr);
    ASSERT_always_forbid(ptr==NULL);
    ASSERT_always_forbid(ptr==0);
    ASSERT_always_require(ptr!=NULL);
    ASSERT_always_require(ptr!=0);
    ASSERT_always_require(ptr && ptr);
    ASSERT_always_require(ptr || ptr);

    {
        std::stringstream ss;
        ss <<ptr;
        ASSERT_always_require2(boost::starts_with(ss.str(), "0x"), "ss.str()==" + ss.str());
    }

#if 0 // [Robb Matzke 2014-06-09]: none of these should compile
    +ptr;
    -ptr;
    ++ptr;
    ptr++;
    --ptr;
    ptr--;
    ptr + 1;
    ptr + ptr;
    ptr += 1;
    ptr += ptr;
    ptr - 1;
    ptr - ptr;
    ptr -= 1;
    ptr -= ptr;
    ptr * 1;
    ptr * ptr;
    ptr *= 1;
    ptr *= ptr;
    ptr / 1;
    ptr / ptr;
    ptr /= 1;
    ptr /= ptr;
    ptr % 1;
    ptr % ptr;
    ptr %= 1;
    ptr %= ptr;
    ptr & ptr;
    ptr &= ptr;
    ptr | ptr;
    ptr |= ptr;
    ptr ^ ptr;
    ptr ^= ptr;
    ~ptr;
    ptr >> 1;
    ptr >>= 1;
    ptr << 1;
    ptr <<= 1;
    ptr[0];
#endif

}

static void compare(const SharedPointer<Subclass> &a, const SharedPointer<Subclass> &b) {
    // We want to make sure that the test is comparing the two object pointers rather than some address which is part
    // of the SharedPointer itself. Therefore, we'll make sure that the shared pointers' addresses are in the opposite order as
    // the object addresses.
    ASSERT_always_require(getRawPointer(a) < getRawPointer(b));
    ASSERT_always_require(&a > &b);

    ASSERT_always_forbid(a == b);
    ASSERT_always_require(a != b);
    ASSERT_always_require(a < b);
    ASSERT_always_require(a <= b);
    ASSERT_always_forbid(a > b);
    ASSERT_always_forbid(a >= b);

    ASSERT_always_require(a == getRawPointer(a));
    ASSERT_always_require(a != getRawPointer(b));
    ASSERT_always_require(a < getRawPointer(b));
    ASSERT_always_require(a <= getRawPointer(b));
    ASSERT_always_forbid(a > getRawPointer(b));
    ASSERT_always_forbid(a >= getRawPointer(b));
}

static void compare() {
    ObjectStats stats1, stats2;
    Subclass *objLo = new Subclass(stats1, 123);
    Subclass *objHi = new Subclass(stats2, 456);
    if (objLo > objHi)
        std::swap(objLo, objHi);
    SharedPointer<Subclass> ptr1;
    SharedPointer<Subclass> ptr2;
    if (&ptr1 < &ptr2) {
        ptr1 = SharedPointer<Subclass>(objHi);
        ptr2 = SharedPointer<Subclass>(objLo);
        compare(ptr2, ptr1);
    } else {
        ptr1 = SharedPointer<Subclass>(objLo);
        ptr2 = SharedPointer<Subclass>(objHi);
        compare(ptr1, ptr2);
    }
}

int main() {
    default_ctor();
    null_ctor();
    raw_ctor();
    raw_ctor_const();
    raw_ctor_upcast();
    double_ownership();
    copy_ctor();
    copy_ctor_const();
    copy_ctor_upcast();
    assignment_to_null();
    assignment();
    assignment_upcast();
    assignment_self();
    clear();
    dereference();
    dereference_null();
    arrow();
    arrow_null();
    implicit_bool();
    compare();
}

