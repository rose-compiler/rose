// Demo how to use non-IR attributes. For IR nodes, use AstAttributeMechanism instead.
// The commentary for this program is the doxygen documentation for the rose::BinaryAnalysis::Attribute namespace.

#include <rose.h>

//! [setup]
#include <BinaryAttribute.h>
using namespace rose::BinaryAnalysis;
//! [setup]


//! [attribute types]
struct WindowPosition {
    int x, y;

    WindowPosition()
        : x(-1), y(-1) {}

    WindowPosition(int x, int y)
        : x(x), y(y) {}
};

typedef std::string FileName;
//! [attribute types]


//! [providing]
struct MyClass: public Attribute::Storage {
    // additional members...
};
//! [providing]



void
example_usage() {

    //! [declaring]
    const Attribute::Id DUCKS    = Attribute::declare("number of ducks");
    const Attribute::Id WINDOW   = Attribute::declare("GUI window information");
    const Attribute::Id GRAPHVIZ = Attribute::declare("GraphViz output file");
    //! [declaring]

    MyClass obj;


    //! [storing values]
    obj.setAttribute(DUCKS, 10);
    obj.setAttribute(WINDOW, WindowPosition(100, 200));
    obj.setAttribute(GRAPHVIZ, std::string("/dev/null"));
    //! [storing values]


    //! [retrieve with getAttribute]
    int nDucks = obj.attributeExists(DUCKS) ? obj.getAttribute<int>(DUCKS) : 1;

    WindowPosition win;
    if (obj.attributeExists(WINDOW))
        win = obj.getAttribute<WindowPosition>(WINDOW);

    std::string fname;
    if (obj.attributeExists(GRAPHVIZ))
        fname = obj.getAttribute<std::string>(GRAPHVIZ); 
    //! [retrieve with getAttribute]
    

    //! [retrieve with default]
    nDucks = obj.attributeOrElse(DUCKS, 1); // stored ducks or 1

    win = obj.attributeOrElse(WINDOW, WindowPosition());

    fname = obj.attributeOrDefault<std::string>(GRAPHVIZ);
    //! [retrieve with default]


    //! [retrieve optional]
    if (obj.optionalAttribute<int>(DUCKS).assignTo(nDucks)) {
        // reached even if nDucks == 0
    } else {
        // reached only if DUCKS attribute is not stored
    }
    //! [retrieve optional]


    //! [erasing]
    obj.eraseAttribute(DUCKS);
    //! [erasing]
}

int
something_that_might_throw() {
    static int ncalls = 0;
    if (ncalls++ < 1000)
        throw std::runtime_error("I did throw!");
    return ncalls;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Attribute value types we want to store
//
// Assume that the user does not have access to modify these types (they are in some library header file).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [comparison value types]
// Declared in a 3rd-party library
enum Approximation { UNDER_APPROXIMATED, OVER_APPROXIMATED, UNKNOWN_APPROXIMATION };

// Declared in a 3rd-party library
struct AnalysisTime {
    double cpuTime;
    double elapsedTime;

    AnalysisTime()
        : cpuTime(0.0), elapsedTime(0.0) {}

    AnalysisTime(double cpuTime, double elapsedTime)
        : cpuTime(cpuTime), elapsedTime(elapsedTime) {}
};
//! [comparison value types]



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Classes in which to store attributes.
//
// Assume that these types appear in the ROSE library header files.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [comparison preparing storage]
// Method 1: BinaryAnalysis::Attribute uses inheritence
class ObjectWithAttributes_1: public Attribute::Storage {
    // other members here...
};

// Method 2: AstAttributeMechanism uses containment with a single data member.
class ObjectWithAttributes_2 {
public:
    AstAttributeMechanism attributeMechanism;
    // other members here...
};

// Method 3: AttributeMechanism<K,V> uses containment with one data member per value type.
class ObjectWithAttributes_3 {
public:
    AttributeMechanism<std::string, Approximation> approximationAttributes;
    AttributeMechanism<std::string, AnalysisTime> analysisTimeAttributes;
    // Other members here...
};

// Method 4: Attributes in IR nodes. Users don't normally create new IR nodes, so this
// demo uses some existing IR type instead.
typedef SgAsmInstruction ObjectWithAttributes_4;
//! [comparison preparing storage]



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Wrappers around attribute values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Counts how many instances of class T are currently allocated.
template<class T>
class AllocationCounter {
public:
    static int nAllocated;
    static void* operator new(size_t size) {
        ++nAllocated;
        return ::operator new(size);
    }
    static void operator delete(void *ptr, size_t size) {
        if (ptr)
            --nAllocated;
        ::operator delete (ptr);
    }
};

template<class T>
int AllocationCounter<T>::nAllocated = 0;



//! [comparison attribute wrappers]
// Method 2: AstAttributeMechanism needs wrappers with "copy" methods.
class ApproximationAttribute_2: public AstAttribute, 
    public AllocationCounter<ApproximationAttribute_2> // ignore this, it's only for testing the implementation
{
public:
    Approximation approximation;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        ApproximationAttribute_2 *retval = new ApproximationAttribute_2;
        retval->approximation = approximation;
        return retval;
    }
};

class AnalysisTimeAttribute_2: public AstAttribute,
    public AllocationCounter<AnalysisTimeAttribute_2> // ignore this, it's only for testing the implementation
{
public:
    AnalysisTime analysisTime;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        AnalysisTimeAttribute_2 *retval = new AnalysisTimeAttribute_2;
        retval->analysisTime = analysisTime;
        return retval;
    }
};

// Method 4: IR node attributes need to be wrapped
class ApproximationAttribute_4: public AstAttribute,
    public AllocationCounter<ApproximationAttribute_4> // ignore this, it's only for testing the implementation
{
public:
    Approximation approximation;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        ApproximationAttribute_4 *retval = new ApproximationAttribute_4;
        retval->approximation = approximation;
        return retval;
    }
};

class AnalysisTimeAttribute_4: public AstAttribute,
    public AllocationCounter<AnalysisTimeAttribute_4> // ignore this, it's only for testing the implementation
{
public:
    AnalysisTime analysisTime;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        AnalysisTimeAttribute_4 *retval = new AnalysisTimeAttribute_4;
        retval->analysisTime = analysisTime;
        return retval;
    }
};
//! [comparison attribute wrappers]


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Method 1: BinaryAnalysis::Attribute accessors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
method_1() {
    ObjectWithAttributes_1 obj_1;

    //! [comparison declare 1]
    // Method 1: BinaryAnalysis::Attribute
    const Attribute::Id APPROXIMATION_ATTR = Attribute::declare("type of approximation performed");
    const Attribute::Id ANALYSIS_TIME_ATTR = Attribute::declare("time taken for the analysis");
    //! [comparison declare 1]

    ASSERT_always_require(APPROXIMATION_ATTR != ANALYSIS_TIME_ATTR);
    ASSERT_always_require(APPROXIMATION_ATTR != Attribute::INVALID_ID);
    ASSERT_always_require(ANALYSIS_TIME_ATTR != Attribute::INVALID_ID);

    //! [comparison insert 1]
    // Method 1: BinaryAnalysis::Attribute
    obj_1.setAttribute(APPROXIMATION_ATTR, UNDER_APPROXIMATED);
    obj_1.setAttribute(ANALYSIS_TIME_ATTR, AnalysisTime(1.0, 2.0));
    //! [comparison insert 1]

    ASSERT_always_require(obj_1.attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_1.attributeExists(ANALYSIS_TIME_ATTR));

    //! [comparison retrieve 1]
    // Method 1: BinaryAnalysis::Attribute
    Approximation approx_1 = obj_1.attributeOrElse(APPROXIMATION_ATTR, UNKNOWN_APPROXIMATION);
    double cpuTime_1 = obj_1.attributeOrDefault<AnalysisTime>(ANALYSIS_TIME_ATTR).cpuTime;
    //! [comparison retrieve 1]

    ASSERT_always_require(approx_1 == UNDER_APPROXIMATED);
    ASSERT_always_require(cpuTime_1 == 1.0);

    // Copy the containing object and its attributes.
    ObjectWithAttributes_1 obj_2 = obj_1;
    ASSERT_always_require(obj_2.attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_2.attributeExists(ANALYSIS_TIME_ATTR));
    Approximation approx_2 = obj_2.attributeOrElse(APPROXIMATION_ATTR, UNKNOWN_APPROXIMATION);
    double cpuTime_2 = obj_2.attributeOrDefault<AnalysisTime>(ANALYSIS_TIME_ATTR).cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison erase 1]
    // Method 1: BinaryAnalysis::Attribute
    obj_1.eraseAttribute(APPROXIMATION_ATTR);
    obj_2.eraseAttribute(ANALYSIS_TIME_ATTR);
    //! [comparison erase 1]

    ASSERT_always_require(!obj_1.attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(!obj_2.attributeExists(ANALYSIS_TIME_ATTR));

    int retval = 0;
    try {
        //! [comparison cleanup 1]
        // Method 1: BinaryAnalysis::Attribute: value destructors called automatically
        // if containing object is destroyed by exception unwinding.
        int x = something_that_might_throw();
        //! [comparison cleanup 1]
        retval = x;
    } catch (...) {
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Method 2: AstAttributeMechanism accessors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
method_2() {
    ObjectWithAttributes_2 obj_1;

    //! [comparison declare 2]
    // Method 2: AstAttributeMechanism
    const std::string APPROXIMATION_ATTR = "type of approximation performed";
    const std::string ANALYSIS_TIME_ATTR = "time taken for the analysis";
    //! [comparison declare 2]

    //! [comparison insert 2]
    // Method 2: AstAttributeMechanism
    if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR))
        delete obj_1.attributeMechanism[APPROXIMATION_ATTR];
    ApproximationAttribute_2 *approximationAttribute = new ApproximationAttribute_2;
    approximationAttribute->approximation = UNDER_APPROXIMATED;
    obj_1.attributeMechanism.set(APPROXIMATION_ATTR, approximationAttribute);
    if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
        delete obj_1.attributeMechanism[ANALYSIS_TIME_ATTR];
    AnalysisTimeAttribute_2 *analysisTimeAttribute = new AnalysisTimeAttribute_2;
    analysisTimeAttribute->analysisTime = AnalysisTime(1.0, 2.0);
    obj_1.attributeMechanism.set(ANALYSIS_TIME_ATTR, analysisTimeAttribute);
    //! [comparison insert 2]

    ASSERT_always_require(obj_1.attributeMechanism.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR));
    ASSERT_always_require(AllocationCounter<ApproximationAttribute_2>::nAllocated == 1);
    
    //! [comparison retrieve 2]
    // Method 2: AstAttributeMechanism
    Approximation approx_1 = UNKNOWN_APPROXIMATION;
    if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR)) {
        ApproximationAttribute_2 *tmp = dynamic_cast<ApproximationAttribute_2*>(obj_1.attributeMechanism[APPROXIMATION_ATTR]);
        if (tmp != NULL)
            approx_1 = tmp->approximation;
    }
    double cpuTime_1 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR)) {
        AnalysisTimeAttribute_2 *tmp = dynamic_cast<AnalysisTimeAttribute_2*>(obj_1.attributeMechanism[ANALYSIS_TIME_ATTR]);
        if (tmp != NULL)
            cpuTime_1 = tmp->analysisTime.cpuTime;
    }
    //! [comparison retrieve 2]

    ASSERT_always_require(approx_1 == UNDER_APPROXIMATED);
    ASSERT_always_require(cpuTime_1 == 1.0);
    ASSERT_always_require(AllocationCounter<ApproximationAttribute_2>::nAllocated == 1);

    // Copy the containing object and its attributes.
    ObjectWithAttributes_2 obj_2 = obj_1;               // requires "copy" implementations in attribute value types
    ASSERT_always_require(AllocationCounter<ApproximationAttribute_2>::nAllocated == 2);
    ASSERT_always_require(obj_2.attributeMechanism.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR));
    ASSERT_always_not_null(obj_2.attributeMechanism[APPROXIMATION_ATTR]);
    ASSERT_always_not_null(obj_2.attributeMechanism[ANALYSIS_TIME_ATTR]);
    ASSERT_always_require(obj_1.attributeMechanism[APPROXIMATION_ATTR] !=
                          obj_2.attributeMechanism[APPROXIMATION_ATTR]);
    ASSERT_always_require(obj_1.attributeMechanism[ANALYSIS_TIME_ATTR] !=
                          obj_2.attributeMechanism[ANALYSIS_TIME_ATTR]);
    Approximation approx_2 =
        dynamic_cast<ApproximationAttribute_2*>(obj_2.attributeMechanism[APPROXIMATION_ATTR])->approximation;
    double cpuTime_2 =
        dynamic_cast<AnalysisTimeAttribute_2*>(obj_2.attributeMechanism[ANALYSIS_TIME_ATTR])->analysisTime.cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison erase 2]
    // Method 2: AstAttributeMechanism
    if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR)) {
        delete obj_1.attributeMechanism[APPROXIMATION_ATTR];
        obj_1.attributeMechanism.remove(APPROXIMATION_ATTR);
    }
    if (obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR)) {
        delete obj_2.attributeMechanism[ANALYSIS_TIME_ATTR];
        obj_2.attributeMechanism.remove(ANALYSIS_TIME_ATTR);
    }
    //! [comparison erase 2]
    ASSERT_always_require(!obj_1.attributeMechanism.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(AllocationCounter<ApproximationAttribute_2>::nAllocated == 1);
    ASSERT_always_require(!obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR));
    ASSERT_always_require(AllocationCounter<AnalysisTimeAttribute_2>::nAllocated == 1);

    int retval = 0;
    try {
        //! [comparison cleanup 2]
        // Method 2: AstAttributeMechanism: manual cleanup required if containng
        // object would be destroyed by exception unwinding.
        int x = 0;
        try {
            x = something_that_might_throw();
        } catch (...) {
            if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR))
                delete obj_1.attributeMechanism[APPROXIMATION_ATTR];
            if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
                delete obj_1.attributeMechanism[ANALYSIS_TIME_ATTR];
            if (obj_2.attributeMechanism.exists(APPROXIMATION_ATTR))
                delete obj_2.attributeMechanism[APPROXIMATION_ATTR];
            if (obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
                delete obj_2.attributeMechanism[ANALYSIS_TIME_ATTR];
            throw;
        }
        //! [comparison cleanup 2]
        retval = x;
    } catch (...) {
    }
    return retval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Method 3: AttributeMechanism accessors
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
method_3() {
    ObjectWithAttributes_3 obj_1;

    //! [comparison declare 3]
    // Method 3: AttributeMechanism
    const std::string APPROXIMATION_ATTR = "type of approximation performed";
    const std::string ANALYSIS_TIME_ATTR = "time taken for the analysis";
    //! [comparison declare 3]

    //! [comparison insert 3]
    // Method 3: AttributeMechanism
    obj_1.approximationAttributes.set(APPROXIMATION_ATTR, UNDER_APPROXIMATED);
    obj_1.analysisTimeAttributes.set(ANALYSIS_TIME_ATTR, AnalysisTime(1.0, 2.0));
    //! [comparison insert 3]

    ASSERT_always_require(obj_1.approximationAttributes.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_1.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR));

    //! [comparison retrieve 3]
    // Method 3: AttributeMechanism
    Approximation approx_1 = UNKNOWN_APPROXIMATION;
    if (obj_1.approximationAttributes.exists(APPROXIMATION_ATTR))
        approx_1 = obj_1.approximationAttributes[APPROXIMATION_ATTR];
    double cpuTime_1 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_1.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR))
        cpuTime_1 = obj_1.analysisTimeAttributes[ANALYSIS_TIME_ATTR].cpuTime;
    //! [comparison retrieve 3]

    ASSERT_always_require(approx_1 == UNDER_APPROXIMATED);
    ASSERT_always_require(cpuTime_1 == 1.0);

    // Copy the containing object and its attributes.
    ObjectWithAttributes_3 obj_2 = obj_1;
    ASSERT_always_require(obj_2.approximationAttributes.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_2.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR));
    Approximation approx_2 = obj_2.approximationAttributes[APPROXIMATION_ATTR];
    double cpuTime_2 = obj_2.analysisTimeAttributes[ANALYSIS_TIME_ATTR].cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison erase 3]
    // Method 3: AttributeMechanism
    if (obj_1.approximationAttributes.exists(APPROXIMATION_ATTR))
        obj_1.approximationAttributes.remove(APPROXIMATION_ATTR);
    if (obj_2.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR))
        obj_2.analysisTimeAttributes.remove(ANALYSIS_TIME_ATTR);
    //! [comparison erase 3]

    ASSERT_always_require(!obj_1.approximationAttributes.exists(APPROXIMATION_ATTR));
    ASSERT_always_require(!obj_2.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR));

    int retval = 0;
    try {
        //! [comparison cleanup 3]
        // Method 3: AttributeMechanism: value destructors called automatically if
        // containing object is destroyed by exception unwinding.
        int x = something_that_might_throw();
        //! [comparison cleanup 3]
        retval = x;
    } catch (...) {
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Method 4: IR node attributes
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
method_4() {
    ObjectWithAttributes_4 *obj_1 = new ObjectWithAttributes_4; // IR nodes are always in the heap

    //! [comparison declare 4]
    // Method 4: Attributes in IR nodes
    const std::string APPROXIMATION_ATTR = "type of approximation performed";
    const std::string ANALYSIS_TIME_ATTR = "time taken for the analysis";
    //! [comparison declare 4]

    //! [comparison insert 4]
    // Method 4: Attributes in IR nodes
    if (obj_1->attributeExists(APPROXIMATION_ATTR))
        delete obj_1->getAttribute(APPROXIMATION_ATTR);
    ApproximationAttribute_4 *approximationAttribute = new ApproximationAttribute_4;
    approximationAttribute->approximation = UNDER_APPROXIMATED;
    obj_1->setAttribute(APPROXIMATION_ATTR, approximationAttribute);
    if (obj_1->attributeExists(ANALYSIS_TIME_ATTR))
        delete obj_1->getAttribute(ANALYSIS_TIME_ATTR);
    AnalysisTimeAttribute_4 *analysisTimeAttribute = new AnalysisTimeAttribute_4;
    analysisTimeAttribute->analysisTime = AnalysisTime(1.0, 2.0);
    obj_1->setAttribute(ANALYSIS_TIME_ATTR, analysisTimeAttribute);
    //! [comparison insert 4]
    
    ASSERT_always_require(obj_1->attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_1->attributeExists(ANALYSIS_TIME_ATTR));
    
    //! [comparison retrieve 4]
    // Method 4: Attributes in IR nodes
    Approximation approx_1 = UNKNOWN_APPROXIMATION;
    if (obj_1->attributeExists(APPROXIMATION_ATTR)) {
        ApproximationAttribute_4 *tmp = dynamic_cast<ApproximationAttribute_4*>(obj_1->getAttribute(APPROXIMATION_ATTR));
        if (tmp != NULL)
            approx_1 = tmp->approximation;
    }
    double cpuTime_1 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_1->attributeExists(ANALYSIS_TIME_ATTR)) {
        AnalysisTimeAttribute_4 *tmp = dynamic_cast<AnalysisTimeAttribute_4*>(obj_1->getAttribute(ANALYSIS_TIME_ATTR));
        if (tmp != NULL)
            cpuTime_1 = tmp->analysisTime.cpuTime;
    }
    //! [comparison retrieve 4]

    ASSERT_always_require(approx_1 == UNDER_APPROXIMATED);
    ASSERT_always_require(cpuTime_1 == 1.0);

    // Copy the containing object and its attributes.
#if 0 // this version doesn't copy attributes
    SgTreeCopy deep;
    ObjectWithAttributes_4 *obj_2 = dynamic_cast<ObjectWithAttributes_4*>(obj_1->copy(deep));
#else // Markus recommends this one instead, but it also doesn't copy attributes
    ObjectWithAttributes_4 *obj_2 = dynamic_cast<ObjectWithAttributes_4*>(SageInterface::deepCopyNode(obj_1));
#endif
    ASSERT_not_null(obj_2);

#if 0 // [Robb Matzke 2015-11-09]: Does not pass. Copying the AST apparently does not copy attributes.
    ASSERT_always_require(obj_2->attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_2->attributeExists(ANALYSIS_TIME_ATTR));
    ASSERT_always_not_null(obj_2->getAttribute(APPROXIMATION_ATTR));
    ASSERT_always_not_null(obj_2->getAttribute(ANALYSIS_TIME_ATTR));
    ASSERT_always_require(obj_1->getAttribute(APPROXIMATION_ATTR) !=
                          obj_2->getAttribute(APPROXIMATION_ATTR));
    ASSERT_always_require(obj_1->getAttribute(ANALYSIS_TIME_ATTR) !=
                          obj_2->getAttribute(ANALYSIS_TIME_ATTR));
    Approximation approx_2 =
        dynamic_cast<ApproximationAttribute_4*>(obj_2->getAttribute(APPROXIMATION_ATTR))->approximation;
    double cpuTime_2 =
        dynamic_cast<AnalysisTimeAttribute_4*>(obj_2->getAttribute(ANALYSIS_TIME_ATTR))->analysisTime.cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);
#endif

    //! [comparison erase 4]
    // Method 4: Attributes in IR nodes
    if (obj_1->attributeExists(APPROXIMATION_ATTR)) {
        delete obj_1->getAttribute(APPROXIMATION_ATTR);
        obj_1->removeAttribute(APPROXIMATION_ATTR);
    }
    if (obj_2->attributeExists(ANALYSIS_TIME_ATTR)) {
        delete obj_2->getAttribute(ANALYSIS_TIME_ATTR);
        obj_2->removeAttribute(ANALYSIS_TIME_ATTR);
    }
    //! [comparison erase 4]

    ASSERT_always_require(!obj_1->attributeExists(APPROXIMATION_ATTR));
    ASSERT_always_require(!obj_2->attributeExists(ANALYSIS_TIME_ATTR));
    ASSERT_always_require(AllocationCounter<ApproximationAttribute_4>::nAllocated = 1);
    ASSERT_always_require(AllocationCounter<AnalysisTimeAttribute_4>::nAllocated = 1);

    int retval = 0;
    try {
        //! [comparison cleanup 4]
        // Method 4: Attributes in IR nodes: manual cleanup is required if
        // an IR node is deleted. IR nodes are seldom deleted, especially
        // during exception handling.
        int x = something_that_might_throw(); // potential leak
        //! [comparison cleanup 4]
        retval = x;
    } catch (...) {
    }
    return retval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              Miscellanous tests
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
test_01(const AttributeMechanism<std::string, AstAttribute*> &am) {
    am.exists("x");
}

int
main() {
    example_usage();
    method_1();
    method_2();
    method_3();
    method_4();
}
