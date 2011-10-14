#include "sage3basic.h"

/* What to test */
#define USE_EXTENTMAP
#define USE_RANGEMAP

#ifdef USE_EXTENTMAP
typedef ExtentMap ExtentMap1;
#endif

#ifdef USE_RANGEMAP
#include "rangemap.h"
typedef Range<rose_addr_t> Extent2;
typedef RangeMap<Extent2> ExtentMap2;
#endif

#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
/* Check that EXPR is true. If not, print an error message and abort. */
#define CHECK(I, WHAT, EXPR, MAP1, MAP2, STMT)                                                                                 \
    do {                                                                                                                       \
        if (!(EXPR)) {                                                                                                         \
            std::cerr <<"Failed at #" <<(I) <<" " <<(WHAT) <<": " <<#EXPR <<": " <<STMT <<"\n";                                \
            std::cerr <<"map1 at failure:\n";                                                                                  \
            (MAP1).dump_extents(stderr, "    ", NULL, false);                                                                  \
            std::cerr <<"map2 at failure:\n";                                                                                  \
            (MAP2).print(std::cerr, "    ");                                                                                   \
            abort();                                                                                                           \
        }                                                                                                                      \
    } while (0)
#endif


#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
/* Check that two maps are identical.  If not, print an error message and abort. */
static void
identical_maps(ExtentMap1 &map1, const ExtentMap2 &map2, size_t sequence, const char *what)
{
    {
        ExtentMap1::const_iterator i1 = map1.begin();
        ExtentMap2::const_iterator i2 = map2.begin();
        while (i1!=map1.end() && i2!=map2.end()) {
            CHECK(sequence, what, i1->first==i2->first.begin, map1, map2,
                  "begin=" <<i1->first <<" vs. " <<i2->first.begin);
            CHECK(sequence, what, i1->second==i2->first.size, map1, map2,
                  "size=" <<i1->second <<" vs. " <<i2->first.size);
            ++i1;
            ++i2;
        }
        CHECK(sequence, what, i1==map1.end(), map1, map2, 0);
        CHECK(sequence, what, i2==map2.end(), map1, map2, 0);
    }

    CHECK(sequence, what, map1.size()==map2.size(), map1, map2,
          "size=" <<map1.size() <<" vs. " <<map2.size());

    if (!map1.empty()) {
        ExtentMap1::const_iterator i1 = map1.highest_offset();
        ExtentMap2::const_reverse_iterator i2 = map2.rbegin();
        CHECK(sequence, what, i1!=map1.end(), map1, map2, "map1 highest_offset()");
        CHECK(sequence, what, i2!=map2.rend(), map1, map2, "map2 rend()");
        CHECK(sequence, what, i1->first==i2->first.begin, map1, map2,
              "highest range begin=" <<i1->first <<" vs. " <<i2->first.begin);
        CHECK(sequence, what, i1->second==i2->first.size, map1, map2,
              "highest range size=" <<i1->second <<" vs. " <<i2->first.size);
    }
}
#endif

/* This function exercises either the ExtentMap or RangeMap class (or both) to by inserting and removing lots of random
 * extents.  If both classes are enabled, then it also checks that the classes agree with each other.  This is a good test
 * since the implementations were completely independent of each other. */
static void
test1(size_t niterations, rose_addr_t max_start_addr, size_t max_size)
{
    static const bool verbose = false;

#ifdef USE_EXTENTMAP
    // The old interface
    ExtentMap1 map1;
#endif

#ifdef USE_RANGEMAP
    // The new interface
    ExtentMap2 map2;
#endif

    for (size_t i=0; i<niterations; ++i) {
        if (0==i%10000)
            std::cerr <<i <<"          \r";

        /**********************************************************************************************************************
         *                                      INSERT / ERASE
         **********************************************************************************************************************/
        {
            rose_addr_t start_va = rand() % max_start_addr;
            size_t size = rand() % max_size + 1;
            if (0==i%2) {
                if (verbose)
                    std::cout <<"#" <<i <<": insert (" <<start_va <<"+" <<size <<"=" <<(start_va+size) <<")...\n";
#ifdef USE_EXTENTMAP
                map1.insert(start_va, size);
#endif
#ifdef USE_RANGEMAP
                map2.insert(Extent2(start_va, size));
#endif
            } else {
                if (verbose)
                    std::cout <<"#" <<i <<": erase (" <<start_va <<"+" <<size <<"=" <<(start_va+size) <<")...\n";
#ifdef USE_EXTENTMAP
                map1.erase(start_va, size);
#endif
#ifdef USE_RANGEMAP
                map2.erase(Extent2(start_va, size));
#endif
            }
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            identical_maps(map1, map2, i, "insert/erase");
#endif
        }
        
        /**********************************************************************************************************************
         *                                      BEST FIT
         **********************************************************************************************************************/
        {
            size_t bf_size = rand() % (2*max_size);
#ifdef USE_EXTENTMAP
            ExtentMap1::iterator bf1 = map1.best_fit(bf_size);
#endif
#ifdef USE_RANGEMAP
            ExtentMap2::iterator bf2 = map2.best_fit(bf_size, map2.begin());
#endif
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            CHECK(i, "bestfit", (bf1==map1.end() && bf2==map2.end()) || (bf1!=map1.end() && bf2!=map2.end()), map1, map2,
                  "request size=" <<bf_size);
            if (bf1!=map1.end()) {
                CHECK(i, "bestfit", bf1->first==bf2->first.begin, map1, map2,
                      "request size=" <<bf_size <<"; got first=" <<bf1->first <<" vs. " <<bf2->first.begin);
                CHECK(i, "bestfit", bf1->second==bf2->first.size, map1, map2,
                      "request size=" <<bf_size <<"; got size=" <<bf1->second <<" vs. " <<bf2->first.size);
            }
#endif
        }

        /**********************************************************************************************************************
         *                                      SUBTRACT FROM
         **********************************************************************************************************************/
        {
            size_t offset = rand() % std::max(1ul, max_start_addr/10);
            size_t size = rand() % max_start_addr + 1;

#ifdef USE_EXTENTMAP
            ExtentMap1 x1 = map1.subtract_from(offset, size);
#endif
#ifdef USE_RANGEMAP
            ExtentMap2 x2 = map2.invert_within<ExtentMap2>(Extent2(offset, size));
#endif
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            identical_maps(map1, map2, i, "subtract from [1]");
#endif

#ifdef USE_EXTENTMAP
            x1 = map1.subtract_from(0, (rose_addr_t)(-1));
#endif
#ifdef USE_RANGEMAP
            x2 = map2.invert_within<ExtentMap2>(Extent2(0, (rose_addr_t)(-1)));
#endif
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            identical_maps(map1, map2, i, "subtract from [2]");
#endif
        }

        /**********************************************************************************************************************
         *                                      OVERLAP WITH
         **********************************************************************************************************************/
        {
            size_t offset = rand() % max_start_addr;
            size_t size = rand() % max_start_addr + 1;

#ifdef USE_EXTENTMAP
            ExtentMap1 x1 = map1.overlap_with(offset, size);
#endif
#ifdef USE_RANGEMAP
            ExtentMap2 x2;
            Extent2 e(offset, size);
            for (ExtentMap2::const_iterator ri=map2.begin(); ri!=map2.end(); ++ri) {
                if (e.overlaps(ri->first))
                    x2.insert(ri->first);
            }
#endif
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            identical_maps(map1, map2, i, "overlap with");
#endif
        }

        /**********************************************************************************************************************
         *                                      EXISTS ALL
         **********************************************************************************************************************/
        {
            size_t offset = rand() % max_start_addr;
            size_t size = rand() % max_start_addr + 1;

#ifdef USE_EXTENTMAP
            bool b1 __attribute__((unused)) = map1.exists_all(ExtentPair(offset, size));
#endif
#ifdef USE_RANGEMAP
            bool b2 __attribute__((unused)) = map2.contains(Extent2(offset, size));
#endif
#if defined(USE_EXTENTMAP) && defined(USE_RANGEMAP)
            CHECK(i, "exists all", b1==b2, map1, map2,
                  "extent=[" <<offset <<"," <<size <<"]; result=" <<b1 <<" vs. " <<b2);
#endif
        }
    }
}


        
/* This function mainly just invokes all the functions of the RangeMap template class to make sure everything compiles.  It
 * also calls the function, but doesn't check whether the results are valid. */
static void
test2()
{
#ifdef USE_RANGEMAP
    typedef Range<unsigned short> SmallExtent;
    typedef RangeMap<SmallExtent> SmallExtentMap;

    ExtentMap2 map;

    map.clear();
    map.begin();
    map.end();

    assert(map.lower_bound(0)==map.end());
    assert(map.lower_bound(1)==map.end());
    assert(map.lower_bound(99999)==map.end());

    /* Some insertions that don't overlap */
    for (size_t i=0; i<10; i++) {
        Extent2 e(i*2, 1);
        map.erase(e);
        map.insert(e);
    }

    /* Some erasures that end at range boundaries */
    map.erase(Extent2(4, 1));
    map.erase(Extent2(8, 2));
    map.erase(Extent2(12, 3));
    map.erase(Extent2(0, Extent2::maximum()));

    /* Some insertions that should merge with left */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent2 lt(i*5+0, 2);
        Extent2 rt(i*5+2, 2);
        map.insert(lt);
        map.insert(rt);
    }

    /* Some insertions that should merge with right */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent2 lt(i*5+0, 2);
        Extent2 rt(i*5+2, 2);
        map.insert(rt);
        map.insert(lt);
    }

    /* Some insertions that merge in the middle */
    map.clear();
    for (size_t i=0; i<10; i++) {
        Extent2 lt(i*10+0, 2);
        Extent2 md(i*10+2, 2);
        Extent2 rt(i*10+4, 2);
        map.insert(lt);
        map.insert(rt);
        map.insert(md);
    }

    /* Erasure from the right */
    map.clear();
    map.insert(Extent2(10, 10));
    map.erase(Extent2(15, 5));

    /* Erasure from the left */
    map.clear();
    map.insert(Extent2(10, 10));
    map.erase(Extent2(10, 5));

    /* Erasure from the middle */
    map.clear();
    map.insert(Extent2(10, 10));
    map.erase(Extent2(11, 8));

    /* Overlap, etc. */
    map.overlaps(map);
    map.distinct(map);
    map.clear();
    map.overlaps(map);
    map.distinct(map);

    /* Conversion of one map type to another */
    map.clear();
    map.insert(Extent2(10, 10));
    map.insert(Extent2(30, 30));
    SmallExtentMap map2(map);
    map.erase_ranges(map2);

    /* Operators */
    map.contains(Extent2(0, 1));
    map.contains(map);
#endif
}

/* Tests of various operators */
static void
test3()
{
#ifdef USE_RANGEMAP

    /* contains */
    ExtentMap2 map;
    for (size_t i=0; i<3; i++) {
        switch (i) {
            case 0: map.insert(Extent2(10, 10)); break;
            case 1: map.insert(Extent2(4, 4));   break;
            case 2: map.insert(Extent2(30, 10)); break;
        }
        assert(!map.contains(Extent2(8, 1)));
        assert(!map.contains(Extent2(8, 2)));
        assert(!map.contains(Extent2(8, 3)));
        assert(!map.contains(Extent2(8, 4)));
        assert(!map.contains(Extent2(8, 12)));
        assert(!map.contains(Extent2(8, 13)));
        assert(map.contains(Extent2(10, 1)));
        assert(map.contains(Extent2(10, 9)));
        assert(map.contains(Extent2(10, 10)));
        assert(!map.contains(Extent2(10, 11)));
        assert(map.contains(Extent2(19, 1)));
        assert(!map.contains(Extent2(19, 2)));
        assert(!map.contains(Extent2(20, 1)));
        assert(!map.contains(Extent2(21, 1)));
    }
#endif
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
