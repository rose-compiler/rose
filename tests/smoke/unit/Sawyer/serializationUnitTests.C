// WARNING: Changes to this file must be contributed back to Sawyer or else they will
//          be clobbered by the next update from Sawyer.  The Sawyer repository is at
//          https://github.com/matzke1/sawyer.




#include <Sawyer/AddressMap.h>
#include <Sawyer/AddressSegment.h>
#include <Sawyer/AllocatingBuffer.h>
#include <Sawyer/BitVector.h>
#include <Sawyer/FileSystem.h>
#include <Sawyer/Graph.h>
#include <Sawyer/IndexedList.h>
#include <Sawyer/Interval.h>
#include <Sawyer/IntervalMap.h>
#include <Sawyer/IntervalSet.h>
#include <Sawyer/Map.h>
#include <Sawyer/MappedBuffer.h>
#include <Sawyer/NullBuffer.h>
#include <Sawyer/Optional.h>
#include <Sawyer/Set.h>
#include <Sawyer/SharedObject.h>
#include <Sawyer/SharedPointer.h>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/cstdint.hpp>
#include <iostream>
#include <sstream>

#include <rosePublicConfig.h>

// Serialize, then unserialize
template<class T>
static void
serunser(const T &t_out, T &t_in) {
    std::ostringstream oss;
    boost::archive::text_oarchive out(oss);
    out <<t_out;
#if 0 // DEBUGGING [Robb Matzke 2016-10-30]
    std::cerr <<oss.str() <<"\n";
#endif

    std::istringstream iss(oss.str());
    boost::archive::text_iarchive in(iss);
    in >>t_in;
}

template<class T, class U>
static void
serunser(const T &t_out, const U &u_out, T &t_in, U &u_in) {
    std::ostringstream oss;
    boost::archive::text_oarchive out(oss);
    out <<t_out <<u_out;
#if 0 // DEBUGGING [Robb Matzke 2016-10-30]
    std::cerr <<oss.str() <<"\n";
#endif

    std::istringstream iss(oss.str());
    boost::archive::text_iarchive in(iss);
    in >>t_in >>u_in;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Polymorphic objects
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class T0_base {
public:
    int i;

    T0_base(): i(0) {}
    explicit T0_base(int i): i(i) {}
    virtual ~T0_base() {}
    virtual std::string name() const = 0;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(i);
    }
};

class T0_a: public T0_base {
public:
    int j;

    T0_a(): j(0) {}
    T0_a(int i, int j): T0_base(i), j(j) {}

    std::string name() const {
        return "T0_a";
    }

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(T0_base);
        s & BOOST_SERIALIZATION_NVP(j);
    }
};

class T0_b: public T0_base {
public:
    int j;

    T0_b(): j(0) {}
    T0_b(int i, int j): T0_base(i), j(j) {}

    std::string name() const {
        return "T0_b";
    }

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(T0_base);
        s & BOOST_SERIALIZATION_NVP(j);
    }
};

BOOST_CLASS_EXPORT(T0_a);
BOOST_CLASS_EXPORT(T0_b);

typedef Sawyer::SharedPointer<T0_base> T0_ptr;

static void
test00() {
    std::cerr <<"Polymorphic class A through base pointer\n";
    T0_a a1(1, 2);
    const T0_base *base_a_out = &a1, *base_a_in = NULL;
    serunser(base_a_out, base_a_in);
    ASSERT_always_require(base_a_in != NULL);
    ASSERT_always_require(base_a_in->name() == "T0_a");

    std::cerr <<"Polymorphic class B through base pointer\n";
    T0_b b1(3, 4);
    const T0_base *base_b_out = &b1, *base_b_in = NULL;
    serunser(base_b_out, base_b_in);
    ASSERT_always_require(base_b_in != NULL);
    ASSERT_always_require(base_b_in->name() == "T0_b");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BitVector
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void
test01() {
    std::cerr <<"BitVector: empty\n";
    Sawyer::Container::BitVector empty_out, empty_in(8, true);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_out.compare(empty_in) == 0);

    std::cerr <<"BitVector: non-empty\n";
    Sawyer::Container::BitVector bv_out(800, true), bv_in;
    serunser(bv_out, bv_in);
    ASSERT_always_require(bv_out.compare(bv_in) == 0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SharedPointer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct T02: Sawyer::SharedObject {
public:
    typedef Sawyer::SharedPointer<T02> Ptr;
private:
    int i_;
private:
    friend class boost::serialization::access;
    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(i_);
    }
protected:
    T02(int i = 0): i_(i) {}
public:
    static Ptr instance(int i = 0) {
        return Ptr(new T02(i));
    }
    bool operator==(const T02 &other) const {
        return i_ == other.i_;
    }
};

static void
test02() {
    T02::Ptr obj1 = T02::instance(1);

    std::cerr <<"SharedPointer: null\n";
    T02::Ptr p1_out, p1_in = obj1;
    ASSERT_always_require(ownershipCount(obj1) == 2);
    serunser(p1_out, p1_in);
    ASSERT_always_require(p1_in == NULL);
    ASSERT_always_require(ownershipCount(obj1) == 1);

    std::cerr <<"SharedPointer: non-null\n";
    T02::Ptr p2_out = obj1, p2_in;
    ASSERT_always_require(ownershipCount(obj1) == 2);
    serunser(p2_out, p2_in);
    ASSERT_always_require(p2_in != NULL);
    ASSERT_always_require(ownershipCount(obj1) == 2);
    ASSERT_always_require(ownershipCount(p2_in) == 1);  // new object was created

    std::cerr <<"SharedPointer: shared-object\n";
    T02::Ptr p3_out = p2_out, p3_in1, p3_in2;
    ASSERT_always_require(ownershipCount(p3_out) == 3);
    serunser(p2_out, p3_out, p3_in1, p3_in2);
    ASSERT_always_require(p3_in1 != NULL);
    ASSERT_always_require(p3_in1 == p3_in2);
    ASSERT_always_require(ownershipCount(p3_in1) == 2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interval
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
test03() {
    typedef Sawyer::Container::Interval<int> T03;

    std::cerr <<"Interval<int> empty\n";
    T03 empty_out, empty_in = 123;
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in == empty_out);

    std::cerr <<"Interval<int> singleton\n";
    T03 single_out = -123, single_in;
    serunser(single_out, single_in);
    ASSERT_always_require(single_in == single_out);

    std::cerr <<"Interval<int> multiple values\n";
    T03 multi_out = T03::hull(-10, 10), multi_in;
    serunser(multi_out, multi_in);
    ASSERT_always_require(multi_in == multi_out);

    std::cerr <<"Interval<int> whole space\n";
    T03 whole_out = T03::whole(), whole_in;
    serunser(whole_out, whole_in);
    ASSERT_always_require(whole_in == whole_out);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AllocatingBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::AllocatingBuffer<size_t, int> T04;

// Since the derived class is a template, we can't call BOOST_CLASS_EXPORT until we know the template arguments.
BOOST_CLASS_EXPORT(T04);

static void
test04() {
    static const int data[] = {1, 2, 3};

    std::cerr <<"AllocatingBuffer<size_t,int> empty\n";
    T04::Ptr empty_out = T04::instance(0), empty_in;
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in != NULL);
    ASSERT_always_require(empty_in->size() == 0);

    std::cerr <<"AllocatingBuffer<size_t,int> nonempty\n";
    T04::Ptr nonempty_out = T04::instance(3), nonempty_in;
    size_t nwrite = nonempty_out->write(data, 0, 3);
    ASSERT_always_require(nwrite == 3);
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in != NULL);
    ASSERT_always_require(nonempty_in->size() == 3);
    int buf[3];
    buf[0] = buf[1] = buf[2] = 0;
    size_t nread = nonempty_in->read(buf, 0, 3);
    ASSERT_always_require(nread == 3);
    ASSERT_always_require(buf[0] = data[0]);
    ASSERT_always_require(buf[1] = data[1]);
    ASSERT_always_require(buf[2] = data[2]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MappedBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::MappedBuffer<size_t, boost::uint8_t> T05;

// Since the derived class is a template, we can't call BOOST_CLASS_EXPORT until we know the template arguments.
// However, binary analysis support in librose also registers this same class, so we need to avoid an ODR violation here.
#ifndef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
BOOST_CLASS_EXPORT(T05);
#endif

static void
test05() {
    static const boost::uint8_t data[] = {1, 2, 3};

    Sawyer::FileSystem::TemporaryFile f;
    f.stream().write((const char*)data, sizeof data);
    ASSERT_always_require(f.stream().good());
    f.stream().close();

    std::cerr <<"MappedBuffer<size_t,uint8_t>\n";
    T05::Ptr out = T05::instance(f.name());
    ASSERT_always_require(out->size() == 3);

    T05::Ptr in;
    serunser(out, in);
    ASSERT_always_require(in != NULL);
    ASSERT_always_require(in->size() == 3);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// NullBuffer
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::NullBuffer<size_t, boost::uint8_t> T06;

// Since the derived class is a template, we can't call BOOST_CLASS_EXPORT until we know the template arguments.
// However, binary analysis support in librose also registers this same class, so we need to avoid an ODR violation here.
#ifndef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
BOOST_CLASS_EXPORT(T06);
#endif

static void
test06() {
    std::cerr <<"NullBuffer<size_t,uint8_t>\n";
    T06::Ptr out = T06::instance(100);
    ASSERT_always_require(out->size() == 100);

    T06::Ptr in;
    serunser(out, in);
    ASSERT_always_require(in != NULL);
    ASSERT_always_require(in->size() == 100);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Map
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Map<int, int> T07;

static void
test07() {
    std::cerr <<"Map<int,int> empty\n";
    T07 empty_out, empty_in;
    empty_in.insert(1, 2);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"Map<int,int> non-empty\n";
    T07 nonempty_out, nonempty_in;
    nonempty_out.insert(3, 4);
    nonempty_out.insert(5, 6);
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.size() == 2);
    ASSERT_always_require(nonempty_in.exists(3));
    ASSERT_always_require(nonempty_in[3] == 4);
    ASSERT_always_require(nonempty_in.exists(5));
    ASSERT_always_require(nonempty_in[5] == 6);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressSegment<size_t, boost::uint8_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::AddressSegment<size_t, boost::uint8_t> T08;
typedef Sawyer::Container::AllocatingBuffer<size_t, boost::uint8_t> T08_buffer;

// Binary analysis support in librose also registers this same class, so we need to avoid an ODR violation here.
#ifndef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
BOOST_CLASS_EXPORT(T08_buffer);
#endif

static void
test08() {
    static const boost::uint8_t data[] = {1, 2, 3, 4};

    std::cerr <<"AddressSegment<size_t, uint8_t>\n";

    T08_buffer::Ptr buf = T08_buffer::instance(4);
    size_t nwrite = buf->write(data, 0, 4);
    ASSERT_always_require(nwrite == 4);

    T08 out(buf, 1, Sawyer::Access::READABLE|Sawyer::Access::WRITABLE, "the_name");
    T08 in;
    serunser(out, in);

    ASSERT_always_require(in.buffer() != NULL);
    ASSERT_always_require(in.buffer()->size() == 4);
    ASSERT_always_require(in.offset() == 1);
    ASSERT_always_require(in.accessibility() == (Sawyer::Access::READABLE | Sawyer::Access::WRITABLE));
    ASSERT_always_require(in.name() == "the_name");
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IntervalMap<unsigned, float>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Interval<unsigned> T09_interval;
typedef Sawyer::Container::IntervalMap<T09_interval, float> T09;

static void
test09() {
    std::cerr <<"IntervalMap<unsigned, float> empty\n";
    T09 empty_out, empty_in;
    empty_in.insert(1, 2);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"IntervalMap<unsigned, float> non-empty\n";
    T09 nonempty_out, nonempty_in;
    nonempty_out.insert(999, 100.0);
    nonempty_out.insert(T09_interval::hull(5, 7), 101.0);
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.size() == nonempty_out.size());
    ASSERT_always_require(nonempty_in.exists(999));
    ASSERT_always_require(nonempty_in[999] == nonempty_out[999]);
    ASSERT_always_require(nonempty_in.exists(5));
    ASSERT_always_require(nonempty_in[5] == nonempty_out[5]);
    ASSERT_always_require(nonempty_in.exists(6));
    ASSERT_always_require(nonempty_in[6] == nonempty_out[6]);
    ASSERT_always_require(nonempty_in.exists(7));
    ASSERT_always_require(nonempty_in[7] == nonempty_out[7]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// AddressMap<size_t, boost::uint8_t>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Interval<size_t> T10_interval;
typedef Sawyer::Container::AddressMap<size_t, boost::uint8_t> T10;

static void
test10() {
    std::cerr <<"AddressMap empty\n";
    T10 empty_out, empty_in;
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"AddressMap non-empty\n";
    static const boost::uint8_t data[] = {65, 66, 67, 68};
    T10 nonempty_out, nonempty_in;
    nonempty_out.insert(T10_interval::baseSize(10, 4),
                        T10::Segment::anonymousInstance(4, Sawyer::Access::READABLE, "test_name"));
    size_t nwrite = nonempty_out.at(10).limit(4).write(data).size();
    ASSERT_always_require(nwrite == 4);
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.size() == nonempty_out.size());
    ASSERT_always_require(nonempty_in.at(10).available().size() == 4);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Stack temporaries
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void
test11() {
    std::cerr <<"Restoring into stack\n";

    std::vector<std::string> vs_out;
    vs_out.push_back("abcde");
    vs_out.push_back("fghij");
    vs_out.push_back("klmno");
    vs_out.push_back("pqrst");
    vs_out.push_back("uvwxy");

    // Save some strings
    std::ostringstream oss;
    boost::archive::text_oarchive out(oss);
    for (size_t i=0; i<vs_out.size(); ++i)
        out <<vs_out[i];

    // Restore the normal way, by pre-allocating the locations
    std::istringstream iss1(oss.str());
    boost::archive::text_iarchive in1(iss1);
    std::vector<std::string> vs_in1;
    vs_in1.resize(vs_out.size());
    for (size_t i=0; i<vs_out.size(); ++i) {
        in1 >>vs_in1[i];
        ASSERT_always_require(vs_in1[i] == vs_out[i]);
    }

    // Restore via temporary stack variable to make sure that boost::serialize does not think that the local var is always a
    // reference to the same string every time.
    std::istringstream iss2(oss.str());
    boost::archive::text_iarchive in2(iss2);
    std::vector<std::string> vs_in2;
    for (size_t i=0; i<vs_out.size(); ++i) {
        std::string s;
        in2 >>s;
        ASSERT_always_require(s == vs_out[i]);
        vs_in2.push_back(s);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Polymorphic class with mutable members
// Trying to reproduce a bug found in ROSE.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class T12_SgAsmExpression {
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S&, const unsigned /*version*/) {}

public:
    virtual ~T12_SgAsmExpression() {}
    virtual double get_double() const { ASSERT_not_reachable("simulated virtual function"); }
};


class T12_SgAsmValueExpressionStorageClass {};

class T12_SgSymbol {};

class T12_SgAsmValueExpression: public T12_SgAsmExpression {
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(T12_SgAsmExpression);
        s & BOOST_SERIALIZATION_NVP(unfolded_expression_tree);
        s & BOOST_SERIALIZATION_NVP(bit_offset);
        s & BOOST_SERIALIZATION_NVP(bit_size);
        ASSERT_require(symbol == NULL);
        // s & symbol;
    }

public:
    T12_SgAsmValueExpression(const T12_SgAsmValueExpressionStorageClass&) {
        ASSERT_not_reachable("shouldn't get here");
    }

    virtual ~T12_SgAsmValueExpression() {}

    T12_SgAsmValueExpression()
        : unfolded_expression_tree(NULL), bit_offset(0), bit_size(0), symbol(NULL) {}
    
protected:
    T12_SgAsmValueExpression *unfolded_expression_tree;
    unsigned short bit_offset;
    unsigned short bit_size;
    T12_SgSymbol *symbol;
};

class T12_SgAsmConstantExpressionStorageClass {};

class T12_SgAsmConstantExpression: public T12_SgAsmValueExpression {
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(T12_SgAsmValueExpression);
        s & BOOST_SERIALIZATION_NVP(bitVector);
    }

public:
    T12_SgAsmConstantExpression(const T12_SgAsmConstantExpressionStorageClass&) {
        ASSERT_not_reachable("shouldn't get here");
    }

    virtual ~T12_SgAsmConstantExpression() {}

    T12_SgAsmConstantExpression() {}

protected:
    Sawyer::Container::BitVector bitVector;
};

class T12_SgAsmType {};

class T12_SgAsmFloatValueExpressionStorageClass {};

class T12_SgAsmFloatValueExpression: public T12_SgAsmConstantExpression {
private:
    mutable double nativeValue;
    mutable bool nativeValueIsValid;

    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(T12_SgAsmConstantExpression);
        s & BOOST_SERIALIZATION_NVP(nativeValue);
        s & BOOST_SERIALIZATION_NVP(nativeValueIsValid);
    }

public:
    T12_SgAsmFloatValueExpression()
        : nativeValue(0.0), nativeValueIsValid(false) {}
    T12_SgAsmFloatValueExpression(double x, T12_SgAsmType*)
        : nativeValue(x), nativeValueIsValid(true) {}
    T12_SgAsmFloatValueExpression(const Sawyer::Container::BitVector &bv, T12_SgAsmType*)
        : nativeValue(0), nativeValueIsValid(false) {
        bitVector = bv;
    }
    T12_SgAsmFloatValueExpression(const T12_SgAsmFloatValueExpressionStorageClass&) {
        ASSERT_not_reachable("shouldn't get here");
    }
    virtual ~T12_SgAsmFloatValueExpression() {}

    virtual double get_double() const {
        return nativeValue;
    }
};

template<class S>
static void
test12_register(S &s) {
    s.template register_type<T12_SgAsmExpression>();
    s.template register_type<T12_SgAsmValueExpression>();
    s.template register_type<T12_SgAsmConstantExpression>();
    s.template register_type<T12_SgAsmFloatValueExpression>();
}

static void
test12() {
    std::cerr <<"Polymorphic class with mutable members\n";

    T12_SgAsmExpression *a = new T12_SgAsmFloatValueExpression(3.14, NULL);
    std::ostringstream oss;
    boost::archive::text_oarchive out(oss);
    test12_register(out);
    out <<a;

    T12_SgAsmExpression *b = NULL;
    std::istringstream iss(oss.str());
    boost::archive::text_iarchive in(iss);
    test12_register(in);
    in >>b;

    ASSERT_always_require(b->get_double() == a->get_double());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IndexedList
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct T13_value {
    int a, b;

    T13_value(): a(0), b(0) {}
    T13_value(int a, int b): a(a), b(b) {}

    bool operator==(const T13_value &other) const {
        return a == other.a && b == other.b;
    }

private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(a);
        s & BOOST_SERIALIZATION_NVP(b);
    }
};

static void
test13() {
    typedef Sawyer::Container::IndexedList<T13_value> T13;

    std::cerr <<"IndexedList empty\n";
    T13 empty_out, empty_in;
    empty_in.pushBack(T13_value(1, 2));
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"IndexedList non-empty\n";
    T13 nonempty_out, nonempty_in;
    nonempty_out.pushBack(T13_value(10, 0));
    nonempty_out.pushBack(T13_value(11, 1));
    nonempty_out.pushFront(T13_value(9, 2));
    nonempty_out.pushBack(T13_value(12, 3));
    nonempty_out.pushFront(T13_value(8, 4));
    serunser(nonempty_out, nonempty_in);

    T13::ConstNodeIterator iter = nonempty_in.nodes().begin();
    ASSERT_always_require(iter->value() == T13_value(8, 4));
    ASSERT_always_require(iter->id() == 4);
    ++iter;
    ASSERT_always_require(iter->value() == T13_value(9, 2));
    ASSERT_always_require(iter->id() == 2);
    ++iter;
    ASSERT_always_require(iter->value() == T13_value(10, 0));
    ASSERT_always_require(iter->id() == 0);
    ++iter;
    ASSERT_always_require(iter->value() == T13_value(11, 1));
    ASSERT_always_require(iter->id() == 1);
    ++iter;
    ASSERT_always_require(iter->value() == T13_value(12, 3));
    ASSERT_always_require(iter->id() == 3);
    ++iter;
    ASSERT_always_require(iter == nonempty_in.nodes().end());

    ASSERT_always_require(nonempty_in[0] == T13_value(10, 0));
    ASSERT_always_require(nonempty_in[1] == T13_value(11, 1));
    ASSERT_always_require(nonempty_in[2] == T13_value(9, 2));
    ASSERT_always_require(nonempty_in[3] == T13_value(12, 3));
    ASSERT_always_require(nonempty_in[4] == T13_value(8, 4));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Graph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct T14_vertex {
    int a;

    explicit T14_vertex(int a = 0): a(a) {}

    bool operator==(const T14_vertex &other) const {
        return a == other.a;
    }
    
    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(a);
    }
};

struct T14_edge {
    int b;

    explicit T14_edge(int b = 0): b(b) {}

    bool operator==(const T14_edge &other) const {
        return b == other.b;
    }
    
    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(b);
    }
};

typedef Sawyer::Container::Graph<T14_vertex, T14_edge> T14;

static void
test14() {
    T14 init;
    T14::ConstVertexIterator v1 = init.insertVertex(T14_vertex(1));
    T14::ConstVertexIterator v2 = init.insertVertex(T14_vertex(2));
    init.insertEdge(v1, v2, T14_edge(100));
    init.insertEdge(v1, v1, T14_edge(101));

    std::cerr <<"Graph empty\n";
    T14 empty_out, empty_in = init;
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"Graph non-empty\n";
    T14 nonempty_out = init, nonempty_in;
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.nVertices() == nonempty_out.nVertices());
    ASSERT_always_require(nonempty_in.nEdges() == nonempty_in.nEdges());
    for (size_t i=0; i<nonempty_in.nVertices(); ++i)
        ASSERT_always_require(nonempty_in.findVertex(i)->value() == nonempty_out.findVertex(i)->value());
    for (size_t i=0; i<nonempty_in.nEdges(); ++i) {
        ASSERT_always_require(nonempty_in.findEdge(i)->value() == nonempty_out.findEdge(i)->value());
        ASSERT_always_require(nonempty_in.findEdge(i)->source()->id() == nonempty_out.findEdge(i)->source()->id());
        ASSERT_always_require(nonempty_in.findEdge(i)->target()->id() == nonempty_out.findEdge(i)->target()->id());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Graph with index
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Graph<std::string, int, std::string, int> T15;

static void
test15() {
    T15 init;
    T15::ConstVertexIterator v1 = init.insertVertex("foo");
    T15::ConstVertexIterator v2 = init.insertVertex("bar");
    init.insertEdge(v1, v2, 100);
    init.insertEdge(v1, v2, 101);
    init.insertEdge(v2, v2, 102);

    std::cerr <<"Graph indexed empty\n";
    T15 empty_out, empty_in = init;
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"Graph indexed non-empty\n";
    T15 nonempty_out = init, nonempty_in;
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.nVertices() == nonempty_out.nVertices());
    ASSERT_always_require(nonempty_in.nEdges() == nonempty_in.nEdges());
    for (size_t i=0; i<nonempty_in.nVertices(); ++i)
        ASSERT_always_require(nonempty_in.findVertex(i)->value() == nonempty_out.findVertex(i)->value());
    for (size_t i=0; i<nonempty_in.nEdges(); ++i) {
        ASSERT_always_require(nonempty_in.findEdge(i)->value() == nonempty_out.findEdge(i)->value());
        ASSERT_always_require(nonempty_in.findEdge(i)->source()->id() == nonempty_out.findEdge(i)->source()->id());
        ASSERT_always_require(nonempty_in.findEdge(i)->target()->id() == nonempty_out.findEdge(i)->target()->id());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Optional
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct T16 {
    int a;

    explicit T16(int a = 0): a(a) {}

    bool operator==(const T16 &other) const {
        return a == other.a;
    }
    
    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_NVP(a);
    }
};

static void
test16() {
    std::cerr <<"Optional empty\n";
    Sawyer::Optional<T16> empty_out, empty_in;
    empty_in = T16(1);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.orDefault() == T16());

    std::cerr <<"Optional non-empty\n";
    Sawyer::Optional<T16> nonempty_out = T16(2), nonempty_in;
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.orDefault() == T16(2));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Set
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Set<int> T17;

static void
test17() {
    std::cerr <<"Set empty\n";
    T17 empty_out, empty_in;
    empty_in.insert(1);
    empty_in.insert(2);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"Set non-empty\n";
    T17 nonempty_out, nonempty_in;
    nonempty_out.insert(3);
    nonempty_out.insert(4);
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.size() == nonempty_out.size());
    ASSERT_always_require(nonempty_in.least() == nonempty_out.least());
    ASSERT_always_require(nonempty_in.greatest() == nonempty_out.greatest());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IntervalSet<unsigned>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef Sawyer::Container::Interval<unsigned> T18_interval;
typedef Sawyer::Container::IntervalSet<T18_interval> T18;

static void
test18() {
    std::cerr <<"IntervalSet<unsigned> empty\n";
    T18 empty_out, empty_in;
    empty_in.insert(1);
    serunser(empty_out, empty_in);
    ASSERT_always_require(empty_in.isEmpty());

    std::cerr <<"IntervalSet<unsigned> non-empty\n";
    T18 nonempty_out, nonempty_in;
    nonempty_out.insert(999);
    nonempty_out.insert(T18_interval::hull(5, 7));
    serunser(nonempty_out, nonempty_in);
    ASSERT_always_require(nonempty_in.size() == nonempty_out.size());
    ASSERT_always_require(nonempty_in.exists(999));
    ASSERT_always_require(nonempty_in.exists(5));
    ASSERT_always_require(nonempty_in.exists(6));
    ASSERT_always_require(nonempty_in.exists(7));
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main() {
    test00();
    test01();
    test02();
    test03();
    test04();
    test05();
    test06();
    test07();
    test08();
    test09();
    test10();
    test11();
    test12();
    test13();
    test14();
    test15();
    test16();
    test17();
    test18();
}
