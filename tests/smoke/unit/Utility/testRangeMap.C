#include "rangemap.h"
#include <csignal>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <vector>

#include <boost/config.hpp>
#ifndef BOOST_WINDOWS
# include <unistd.h>
#endif

/* If defined, then check RangeMap implementation against a very simple bitmap implementation. */
#define CHECK

/* How long should this test run? */
static const size_t TIME_LIMIT = 10; // seconds
static bool time_expired = false;
static void sigalrm_handler(int) { time_expired = true; }

typedef Range<uint64_t> Extent1;
typedef RangeMap<Extent1> ExtentMap1;

typedef std::vector<bool> ExtentMap2;

std::ostream &
operator<<(std::ostream &o, const ExtentMap2 &map2)
{
    bool prev=false, had_output=false;
    size_t range_start=0;
    for (size_t i=0; i<map2.size(); i++) {
        if (map2[i]!=prev) {
            if (map2[i]) {
                o <<(had_output?", ":"") <<i;
            } else if (range_start+1<i) {
                o <<"-" <<(i-1);
            }
            prev = map2[i];
            had_output = true;
            range_start = i;
        }
    }
    if (prev) {
        if (range_start+1<map2.size())
            o <<"-" <<map2.size()-1;
    }

    return o;
}

void
error(const ExtentMap1 &map1, const ExtentMap2 &map2, const char *mesg, size_t where)
{
    std::cerr <<mesg <<" at element " <<where <<":\n"
              <<"range map  = {" <<map1 <<"}\n"
              <<"bit vector = {" <<map2 <<"}\n";
    abort();
}

bool
check(const ExtentMap1 &map1, const ExtentMap2 &map2) 
{
#ifdef CHECK
    size_t i2 = 0;
    for (ExtentMap1::const_iterator i1=map1.begin(); i1!=map1.end(); ++i1) {
        while (i2<i1->first.first()) {
            if (i2>=map2.size()) {
                error(map1, map2, "size mismatch", i2);
                return false;
            }
            if (map2[i2]) {
                error(map1, map2, "element mismatch", i2);
                return false;
            }
            ++i2;
        }
        while (i2<=i1->first.last()) {
            if (i2>=map2.size()) {
                error(map1, map2, "size mismatch", i2);
                return false;
            }
            if (!map2[i2]) {
                error(map1, map2, "element mismatch", i2);
                return false;
            }
            ++i2;
        }
    }

    while (i2<map2.size()) {
        if (map2[i2]) {
            error(map1, map2, "element mismatch", i2);
            return false;
        }
        ++i2;
    }
#endif
    return true;
}

/* Repeated insert, erase, and invert operations checked against a bit vector implementation. */
static void
test1(size_t niterations, uint64_t max_start_addr, size_t max_size)
{
    const bool verbose = false;
    ExtentMap1 map1;
    ExtentMap2 map2;

    alarm(0);
    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sigalrm_handler;
    sigaction(SIGALRM, &sa, NULL);
    time_expired = false;
    alarm(TIME_LIMIT);

    for (size_t i=0; i<niterations; ++i) {
        if (0==i%10000 && isatty(2))
            std::cerr <<i <<"          \r";
        if (time_expired) {
            std::cerr <<"time expired; canceling this test after " <<i <<" iterations.\n";
            return;
        }

        /**********************************************************************************************************************
         *                                      INSERT / ERASE
         **********************************************************************************************************************/
        {
            uint64_t start_va = rand() % max_start_addr;
            size_t size = rand() % max_size + 1;
            if (0==i%2) {
                if (verbose)
                    std::cout <<"#" <<i <<": insert (" <<start_va <<"+" <<size <<"=" <<(start_va+size) <<")...\n";
                map1.insert(Extent1(start_va, size));
#ifdef CHECK
                map2.resize(std::max((uint64_t)map2.size(), start_va+size), false);
                for (size_t j=0; j<size; j++)
                    map2[start_va+j] = true;
#endif
            } else {
                if (verbose)
                    std::cout <<"#" <<i <<": erase (" <<start_va <<"+" <<size <<"=" <<(start_va+size) <<")...\n";
                map1.erase(Extent1(start_va, size));
#ifdef CHECK
                map2.resize(std::max((uint64_t)map2.size(), start_va+size), false);
                for (size_t j=0; j<size; j++)
                    map2[start_va+j] = false;
#endif
            }
            if (!check(map1, map2)) {
                std::cerr <<"failed at #" <<i <<" insert/erase (" <<start_va <<"+" <<size <<"=" <<(start_va+size) <<")\n";
                abort();
            }
        }

        /**********************************************************************************************************************
         *                                      INVERT
         **********************************************************************************************************************/
        if (map1.nranges()<100000) { // skip for large ranges because its relatively slow
            size_t offset = rand() % std::max((uint64_t)1, max_start_addr/10);
            size_t size = rand() % max_start_addr + 1;
            if (verbose)
                std::cout <<"#" <<i <<": invert_within(" <<offset <<"+" <<size <<"=" <<(offset+size) <<")...\n";

            ExtentMap1 x1 = map1.invert_within<ExtentMap1>(Extent1(offset, size));

#ifdef CHECK
            ExtentMap2 x2(std::max(map1.max()+1, (uint64_t)offset+size), false);
            for (size_t j=0; j<size; j++)
                x2[offset+j] = offset+j<map2.size() ? !map2[offset+j] : true;
            if (!check(x1, x2)) {
                if (!check(map1, map2))
                    abort();
                std::cerr <<"range map  = {" <<map1 <<"}\n"
                          <<"bit vector = {" <<map2 <<"}\n";
                std::cerr <<"failed at #" <<i <<" invert_within (" <<offset <<"+" <<size <<"=" <<(offset+size) <<")\n";
                abort();
            }
#endif
        }
    }
}


        
/* This function mainly just invokes all the functions of the RangeMap template class to make sure everything compiles.  It
 * also calls the function, but doesn't check whether the results are valid. */
static void
test2()
{
    typedef Range<unsigned short> SmallExtent;
    typedef RangeMap<SmallExtent> SmallExtentMap;

    ExtentMap1 map;

    map.clear();
    map.begin();
    map.end();

    assert(map.lower_bound(0)==map.end());
    assert(map.lower_bound(1)==map.end());
    assert(map.lower_bound(99999)==map.end());

    /* Some insertions that don't overlap */
    for (size_t i=0; i<10; i++) {
        Extent1 e(i*2, 1);
        map.erase(e);
        map.insert(e);
    }

    /* Some erasures that end at range boundaries */
    map.erase(Extent1(4, 1));
    map.erase(Extent1(8, 2));
    map.erase(Extent1(12, 3));
    map.erase(Extent1(0, Extent1::maximum()));

    /* Some insertions that should merge with left */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent1 lt(i*5+0, 2);
        Extent1 rt(i*5+2, 2);
        map.insert(lt);
        map.insert(rt);
    }

    /* Some insertions that should merge with right */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent1 lt(i*5+0, 2);
        Extent1 rt(i*5+2, 2);
        map.insert(rt);
        map.insert(lt);
    }

    /* Some insertions that merge in the middle */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent1 lt(i*10+0, 2);
        Extent1 md(i*10+2, 2);
        Extent1 rt(i*10+4, 2);
        map.insert(lt);
        map.insert(rt);
        map.insert(md);
    }

    /* Erasure from the right */
    map.clear();
    map.insert(Extent1(10, 10));
    map.erase(Extent1(15, 5));

    /* Erasure from the left */
    map.clear();
    map.insert(Extent1(10, 10));
    map.erase(Extent1(10, 5));

    /* Erasure from the middle */
    map.clear();
    map.insert(Extent1(10, 10));
    map.erase(Extent1(11, 8));

    /* Overlap, etc. */
    map.overlaps(map);
    map.distinct(map);
    map.clear();
    map.overlaps(map);
    map.distinct(map);

    /* Conversion of one map type to another */
    map.clear();
    map.insert(Extent1(10, 10));
    map.insert(Extent1(30, 30));
    SmallExtentMap map2(map);
    map.erase_ranges(map2);

    /* Operators */
    map.contains(Extent1(0, 1));
    map.contains(map);
}

/* Tests of various operators */
static void
test3()
{
    /* contains */
    ExtentMap1 map;
    for (size_t i=0; i<3; i++) {
        switch (i) {
            case 0: map.insert(Extent1(10, 10)); break;
            case 1: map.insert(Extent1(4, 4));   break;
            case 2: map.insert(Extent1(30, 10)); break;
        }
        assert(!map.contains(Extent1(8, 1)));
        assert(!map.contains(Extent1(8, 2)));
        assert(!map.contains(Extent1(8, 3)));
        assert(!map.contains(Extent1(8, 4)));
        assert(!map.contains(Extent1(8, 12)));
        assert(!map.contains(Extent1(8, 13)));
        assert(map.contains(Extent1(10, 1)));
        assert(map.contains(Extent1(10, 9)));
        assert(map.contains(Extent1(10, 10)));
        assert(!map.contains(Extent1(10, 11)));
        assert(map.contains(Extent1(19, 1)));
        assert(!map.contains(Extent1(19, 2)));
        assert(!map.contains(Extent1(20, 1)));
        assert(!map.contains(Extent1(21, 1)));
    }
}

int
main()
{
    std::cout <<"Small map with lots of interfering operations\n";
    test1(1000000, 100, 10);

    std::cout <<"Map that ends up with lots of elements\n";
    test1(1000000, 10000, 10);

    std::cout <<"Simple tests (mostly just compile checks that we now call)\n";
    test2();

    std::cout <<"Hand-selected operations with verification\n";
    test3();

    return 0;
}
