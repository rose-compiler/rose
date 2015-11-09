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

static int
something_that_might_throw() {
    static int ncalls = 0;
    if (ncalls++ != 0)
        throw std::runtime_error("I did throw!");
    return ncalls;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Attribute value types we want to store
//
// Assume that the user does not have access to modify these types (they are in some library header file).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [comparison value types]
enum Approximation { UNDER_APPROXIMATED, OVER_APPROXIMATED, UNKNOWN_APPROXIMATION };

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
//! [comparison preparing storage]



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Wrappers around attribute values
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//! [comparison attribute wrappers]
// Method 1: BinaryAnalysis::Attribute does not use wrappers.

// Method 2: AstAttributeMechanism needs wrappers with "copy" methods.
class ApproximationAttribute: public AstAttribute {
public:
    Approximation approximation;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        ApproximationAttribute *retval = new ApproximationAttribute;
        retval->approximation = approximation;
        return retval;
    }
};

class AnalysisTimeAttribute: public AstAttribute {
public:
    AnalysisTime analysisTime;

    virtual AstAttribute* copy() /*const*/ ROSE_OVERRIDE {
        AnalysisTimeAttribute *retval = new AnalysisTimeAttribute;
        retval->analysisTime = analysisTime;
        return retval;
    }
};

// Method 3: AttributeMechanism does not use wrappers.
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

    //! [comparison insert 1]
    // Method 1: BinaryAnalysis::Attribute
    obj_1.setAttribute(APPROXIMATION_ATTR, UNDER_APPROXIMATED);
    obj_1.setAttribute(ANALYSIS_TIME_ATTR, AnalysisTime(1.1, 2.2));
    //! [comparison insert 1]

    //! [comparison retrieve 1]
    // Method 1: BinaryAnalysis::Attribute
    Approximation approx_1 = obj_1.attributeOrElse(APPROXIMATION_ATTR, UNKNOWN_APPROXIMATION);
    double cpuTime_1 = obj_1.attributeOrDefault<AnalysisTime>(ANALYSIS_TIME_ATTR).cpuTime;
    //! [comparison retrieve 1]

    // Copy the containing object and its attributes.
    ObjectWithAttributes_1 obj_2 = obj_1;

    // Query the attributes from the second object without knowing if they exist
    Approximation approx_2 = obj_2.attributeOrElse(APPROXIMATION_ATTR, UNKNOWN_APPROXIMATION);
    double cpuTime_2 = obj_2.attributeOrDefault<AnalysisTime>(ANALYSIS_TIME_ATTR).cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison cleanup 1]
    // Method 1: BinaryAnalysis::Attribute: value destructors called automatically
    return something_that_might_throw();
    //! [comparison cleanup 1]
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
    ApproximationAttribute *approximationAttribute = new ApproximationAttribute;
    approximationAttribute->approximation = UNDER_APPROXIMATED;
    obj_1.attributeMechanism.set(APPROXIMATION_ATTR, approximationAttribute);
    if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
        delete obj_1.attributeMechanism[ANALYSIS_TIME_ATTR];
    AnalysisTimeAttribute *analysisTimeAttribute = new AnalysisTimeAttribute;
    analysisTimeAttribute->analysisTime = AnalysisTime(1.1, 2.2);
    obj_1.attributeMechanism.set(ANALYSIS_TIME_ATTR, analysisTimeAttribute);
    //! [comparison insert 2]
    
    //! [comparison retrieve 2]
    // Method 2: AstAttributeMechanism
    Approximation approx_1 = UNKNOWN_APPROXIMATION;
    if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR)) {
        ApproximationAttribute *tmp = dynamic_cast<ApproximationAttribute*>(obj_1.attributeMechanism[APPROXIMATION_ATTR]);
        if (tmp != NULL)
            approx_1 = tmp->approximation;
    }
    double cpuTime_1 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR)) {
        AnalysisTimeAttribute *tmp = dynamic_cast<AnalysisTimeAttribute*>(obj_1.attributeMechanism[ANALYSIS_TIME_ATTR]);
        if (tmp != NULL)
            cpuTime_1 = tmp->analysisTime.cpuTime;
    }
    //! [comparison retrieve 2]

    // Copy the containing object and its attributes.
    ObjectWithAttributes_2 obj_2 = obj_1;               // requires "copy" implementations in attribute value types

    // Query the attributes from the second object not knowing if they exist.
    Approximation approx_2 = UNKNOWN_APPROXIMATION;
    if (obj_2.attributeMechanism.exists(APPROXIMATION_ATTR)) {
        ApproximationAttribute *tmp = dynamic_cast<ApproximationAttribute*>(obj_2.attributeMechanism[APPROXIMATION_ATTR]);
        if (tmp != NULL)
            approx_2 = tmp->approximation;
    }
    double cpuTime_2 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR)) {
        AnalysisTimeAttribute *tmp = dynamic_cast<AnalysisTimeAttribute*>(obj_2.attributeMechanism[ANALYSIS_TIME_ATTR]);
        if (tmp != NULL)
            cpuTime_2 = tmp->analysisTime.cpuTime;
    }
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison cleanup 2]
    // Method 2: AstAttributeMechanism: manual cleanup required
    int retval = 0;
    try {
        retval = something_that_might_throw();
    } catch (...) {
        if (obj_1.attributeMechanism.exists(APPROXIMATION_ATTR))
            delete obj_1.attributeMechanism[APPROXIMATION_ATTR];
        if (obj_1.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
            delete obj_1.attributeMechanism[APPROXIMATION_ATTR];
        if (obj_2.attributeMechanism.exists(APPROXIMATION_ATTR))
            delete obj_2.attributeMechanism[APPROXIMATION_ATTR];
        if (obj_2.attributeMechanism.exists(ANALYSIS_TIME_ATTR))
            delete obj_2.attributeMechanism[APPROXIMATION_ATTR];
        throw;
    }
    return retval;
    //! [comparison cleanup 2]
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
    obj_1.analysisTimeAttributes.set(ANALYSIS_TIME_ATTR, AnalysisTime(1.1, 2.2));
    //! [comparison insert 3]
    
    //! [comparison retrieve 3]
    // Method 3: AttributeMechanism
    Approximation approx_1 = UNKNOWN_APPROXIMATION;
    if (obj_1.approximationAttributes.exists(APPROXIMATION_ATTR))
        approx_1 = obj_1.approximationAttributes[APPROXIMATION_ATTR];
    double cpuTime_1 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_1.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR))
        cpuTime_1 = obj_1.analysisTimeAttributes[ANALYSIS_TIME_ATTR].cpuTime;
    //! [comparison retrieve 3]

    // Copy the containing object and its attributes.
    ObjectWithAttributes_3 obj_2 = obj_1;

    // Query the attributes from the second object not knowing if they exist.
    Approximation approx_2 = UNKNOWN_APPROXIMATION;
    if (obj_2.approximationAttributes.exists(APPROXIMATION_ATTR))
        approx_2 = obj_2.approximationAttributes[APPROXIMATION_ATTR];
    double cpuTime_2 = AnalysisTime().cpuTime;          // the default, assuming we don't want to hard-code it.
    if (obj_2.analysisTimeAttributes.exists(ANALYSIS_TIME_ATTR))
        cpuTime_2 = obj_2.analysisTimeAttributes[ANALYSIS_TIME_ATTR].cpuTime;
    ASSERT_always_require(approx_1 == approx_2);
    ASSERT_always_require(cpuTime_1 == cpuTime_2);

    //! [comparison cleanup 3]
    // Method 3: AttributeMechanism: value destructors called automatically
    return something_that_might_throw();
    //! [comparison cleanup 3]
}

int
main() {
    example_usage();
    method_1();
    method_2();
    method_3();
}
