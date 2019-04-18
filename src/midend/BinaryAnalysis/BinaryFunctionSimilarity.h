#ifndef ROSE_BinaryAnalysis_FunctionSimilarity_H
#define ROSE_BinaryAnalysis_FunctionSimilarity_H

#include <BinaryMatrix.h>
#include <Partitioner2/Function.h>
#include <Progress.h>
#include <RoseException.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Map.h>

#ifdef ROSE_HAVE_DLIB
    #include <dlib/optimization.h>
#endif

namespace Rose {
namespace BinaryAnalysis {

/** Analysis to test the similarity of two functions.
 *
 *  This analysis compares two functions and returns a floating point value that represents how different they are from one
 *  another. The scale of the returned value depends on the metrics that are used, but generally a difference of zero
 *  represents two functions that are equivalent according to the metric, and values larger than zero indicate the presence of
 *  differences.
 *
 *  This @ref FunctionSimilarity class provides the following things:
 *
 *  @li an interface for manipulating categories, and their metrics (defined below).
 *
 *  @li a container for associating the characteristic values produced by metrics with the functions on which the metrics were
 *      applied.
 *
 *  @li an interface for comparing functions with computed characteristic values.
 *
 * @section ROSE_BinaryAnalysis_FunctionSimilarity_Defs Definitions
 *
 *  A <em>characteristic value</em> is data that measures some characteristic of a function. This analysis supports two types
 *  of characteristic values: floating-point Cartesian points and ordered lists of integers.  Floating-point scalar values are
 *  a special case of Cartesian points.
 *
 *  A <em>metric</em> is a functor that takes input related to a disassembled function and produces characteristic values. The
 *  characteristic data can be either zero or more floating-point Cartesian points (all having the same dimensionality) or an
 *  ordered list of zero or more integers. Examples are: (1) a metric that returns a four-dimensional point describing the
 *  number of immediate and second-level predecessors and successors for each vertex of the function control flow graph; (2) a
 *  metric that returns an ordered list describing local variables detected by a data-flow analysis; (3) a metric that returns
 *  a histogram (multi-dimensional point) of the classes of instructions found in the function.
 *
 *  A <em>metric category</em> (or just @em category) is a collection of characteristic values from compatible metrics (usually
 *  just one metric).
 *
 *  @section ROSE_BinaryAnalysis_FunctionSimilarity_Algorithm Queries
 *
 *  Use this analysis by performing these steps. Steps 2 and 3 can be interleaved.
 *
 *  1. Create a @c FunctionSimilarity analysis object
 *  2. Declare metric categories
 *  3. Populate metric categories with characteristic data for each function
 *  4. Query results */
class FunctionSimilarity {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Public types and data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /* Exceptions thrown by this analysis. */
    class Exception: public Rose::Exception {
    public:
        Exception(const std::string &what): Rose::Exception(what) {}
        ~Exception() throw () {}
    };

    /** Kinds of characteristic values. */
    enum CValKind {
        CARTESIAN_POINT,                                /**< Values are N-dimensional Cartesian points. */
        ORDERED_LIST                                    /**< Values are lists of integers. */
    };
    typedef std::vector<double> CartesianPoint;         /**< Characteristic value that's a Cartesian point. */
    typedef std::vector<int> OrderedList;               /**< Characteristic value that's an ordered list of integers. */

    // Collections of characteristic values
    typedef std::vector<CartesianPoint> PointCloud;     /**< Unordered collection of Cartesian points. */
    typedef std::vector<OrderedList> OrderedLists;      /**< Ordered collection of ordered lists of integers. */

    /** Ways that values can be combined. */
    enum Statistic { AVERAGE, MEDIAN, MAXIMUM };

    typedef size_t CategoryId;                          /**< ID number unique within this analysis context. */
    static const CategoryId NO_CATEGORY = -1;           /**< Invalid category ID. */

    /** Function and distance to some other function. */
    typedef std::pair<Partitioner2::Function::Ptr, double /*distance*/> FunctionDistancePair;

    /** Pair of functions. */
    typedef std::pair<Partitioner2::Function::Ptr, Partitioner2::Function::Ptr> FunctionPair;

    /** Diagnostic streams */
    static Sawyer::Message::Facility mlog;

    /** Square matrix representing distances. */
    typedef Matrix<double> DistanceMatrix;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Private types and data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    // Declaration for metric categories.
    struct Category {
        std::string name;                               // name of category (or metric for singleton categories)
        CValKind kind;                                  // kind of characteristic values stored here
        double weight;                                  // weight when combining category distances
        size_t dimensionality;                          // dimensionality of Cartesian points; 0 => unknown
        double dflt;                                    // default value when one of the functions is null

        explicit Category(const std::string &name, CValKind kind, double weight = 1.0)
            : name(name), kind(kind), weight(weight), dimensionality(0), dflt(1.0) {}
    };

    std::vector<Category> categories_;
    Sawyer::Container::Map<std::string /*category_name*/, size_t /* categories index */> categoryNames_;

    // Characteristic values for some function + category pair.  Only one of these members is populated with data.
    struct CharacteristicValues {
        PointCloud pointCloud;
        OrderedLists orderedLists;
    };

    // Info about each function
    struct FunctionInfo {
        std::vector<CharacteristicValues> categories;   // characteristic values orgnized by CategoryId
        FunctionInfo(): categories() {}                 // LLVM's clang++ 3.5 and 3.7 don't generate this for const objs
    };

    // Info about all functions
    typedef Sawyer::Container::Map<Partitioner2::Function::Ptr, FunctionInfo> Functions;
    Functions functions_;

    // How to combine category distances to obtain a function distance
    Statistic categoryAccumulatorType_;

    Progress::Ptr progress_;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    FunctionSimilarity()
        : categoryAccumulatorType_(AVERAGE), progress_(Progress::instance()) {}

    void clear() {
        categories_.clear();
        categoryNames_.clear();
        functions_.clear();
        categoryAccumulatorType_ = AVERAGE;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Property: Statistic for combining category distances.
     *
     *  When computing the distance between two functions, we first compute the distance between the categories by comparing
     *  the characteristic values in those categories. The category distance are then combined using the specified statistic to
     *  obtain the distance between the functions.
     *
     *  @{ */
    Statistic categoryAccumulatorType() const { return categoryAccumulatorType_; }
    void categoryAccumulatorType(Statistic s) { categoryAccumulatorType_ = s; }
    /** @} */

    /** Property: Object to which progress reports are made. */
    Rose::Progress::Ptr progress() const { return progress_; }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Category declarations
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Declare a new category of Cartesian points.
     *
     *  All characteristics values of this category have the same specified dimensionality.  The order that points are inserted
     *  into this category is irrelevant.  If @p allowExisting is false then the specified name must not already exist or else
     *  an <code>std::runtime_error</code> is thrown; if @p allowExisting is true then either a new category is created or the
     *  ID of an existing category is returned.  Returns a new category ID number, which are consecutive small integers
     *  starting at zero. */
    CategoryId declarePointCategory(const std::string &name, size_t dimensionality, bool allowExisting = true);

    /** Declare a new category of ordered lists of integers. Each characteristic value inserted into this category may be a
     * different length, but the order that they're inserted defines how they're compared when comparing two such categories
     * from different functions. If @p allowExisting is false then the specified name must not already exist or else an
     * <code>std::runtime_error</code> is thrown; if @p allowExisting is true then either a new category is created or the ID
     * of an existing category is returned.  Returns a new category ID number, which are consecutive small integers starting at
     * zero. */
    CategoryId declareListCategory(const std::string &name, bool allowExisting = true);

    /** Number of categories.
     *
     *  Category ID numbers are values zero through one less than the number of categories. */
    size_t nCategories() const { return categories_.size(); }

    /** Find a category by name.
     *
     *  Returns the ID number for the category with the specified name, or @ref NO_CATEGORY if no such name exists wihtin this
     *  analysis context. */
    CategoryId findCategory(const std::string &name) const { return categoryNames_.getOrElse(name, NO_CATEGORY); }
    /** Kind of category. */

    CValKind categoryKind(CategoryId) const;

    /** Dimensionality of Cartesian characteristic points. */
    size_t categoryDimensionality(CategoryId) const;

    /** Property: category weight.
     *
     * @{ */
    double categoryWeight(CategoryId) const;
    void categoryWeight(CategoryId, double);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Predefined metrics
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Control flow graph connectivity.
     *
     *  This metric creates a point cloud with one Cartesian point for each basic block in the function. The space is four
     *  dimensional with the following dimensions:
     *
     *  @li Number of immediate succesors.
     *  @li Number of immediate predecessors.
     *  @li Number of second level successors averaged over the immediate successors.
     *  @li Number of second level predecessors averaged over the immediate predecessors.
     *
     *  Each coordinate is normalized to be one of the following values:
     *
     *  @li 0.0 if there are no neighbors
     *  @li 1/3 if there is one neighbor
     *  @li 2/3 if there are two neighbors
     *  @li 1.0 if there are more than two neigbors or any neighbor is indeterminate.
     *
     *  If @p maxPoints is specified, then functions having more than the specified maximum number of CFG vertices will be
     *  truncated arbititrarily to limit the number of vertices.
     *
     * @{ */
    CategoryId declareCfgConnectivity(const std::string &categoryName);
    void measureCfgConnectivity(CategoryId, const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&,
                                size_t maxPoints = (size_t)(-1));
    /** @} */

    /** Function calls.
     *
     *  This metric creates a point cloud with one Cartesian point for each function called from the specified function.
     *
     * @{ */
    CategoryId declareCallGraphConnectivity(const std::string &categoryName);
    void measureCallGraphConnectivity(CategoryId, const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&);
    /** @} */

    /** Instruction mnemonic stream.
     *
     *  This metric creates an ordered list of instruction mnemonics for the entire function by ordering the function's basic
     *  block by address and then, for each basic block, appending its instruction mnemonic to the list.
     *
     * @{ */
    CategoryId declareMnemonicStream(const std::string &categoryName);
    void measureMnemonicStream(CategoryId, const Partitioner2::Partitioner&, const Partitioner2::Function::Ptr&);
    /** @} */


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Low level functions for manipulating characteristic values
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** Insert a Cartesian point characteristic value.
     *
     *  Inserts the specified Cartesian point into the collection of characteristic values for the category.  The first point
     *  inserted into a category defines the dimensionality of all points in that category. An exception is thrown if the point
     *  does not match the dimensionality of the category or if the category is not one that stores points.
     *
     *  When comparing two functions, the order of points in the point clouds is irrelevant. */
    void insertPoint(const Partitioner2::Function::Ptr&, CategoryId, const CartesianPoint&);

    /** Insert an ordered list characteristic value.
     *
     *  Inserts the specified ordered list of integers into the collection of characteristic values for the category.  Ordered
     *  lists may have zero or more integers. The lists within a category need not all be the same length.
     *
     *  When comparing two functions, the order that ordered lists were inserted into the category matters since the distance
     *  between two categories is some function of the edit distances between corresponding ordered lists.  If you want to
     *  insert list-type characteristic values in any order then each list needs to be in its own category. */
    void insertList(const Partitioner2::Function::Ptr&, CategoryId, const OrderedList&);

    /** Number of characteristic points in a category. */
    size_t size(const Partitioner2::Function::Ptr&, CategoryId) const;

    /** Catesian points contained in a category. */
    const PointCloud& points(const Partitioner2::Function::Ptr&, CategoryId) const;

    /** Ordered lists contained in a category. */
    const OrderedLists& lists(const Partitioner2::Function::Ptr&, CategoryId) const;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Comparison interface
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Compare two functions.
     *
     *  Compare the two specified functions after having populated their characteristic values. Returns a floating-point
     *  distance between the functions where zero indicates that the functions are as similar as possible judging from their
     *  characteristic values.
     *
     *  The distance is calculated as follows:
     *
     *  @em Given the two functions, compute the pair-wise distance between each of their corresponding categories and then
     *      combine these distances according to the category weights using maximum, mean, or median.
     *
     *  @em The distance between two categories containing ordered lists is computed as follows: for each pair of
     *      characteristic values (paired according to the order they were inserted into the category), compute the Levenshtein
     *      edit distance divided by the longer of the two lists.  If one category has more lists than the other then pad the
     *      smaller category with empty lists.  This gives a list of floating-point values in the closed interval zero to
     *      one, which are then combined using maximum, mean, or median.
     *
     *  @em The distance between two categories containing Cartesian points is computed as follows: If one category has fewer
     *      points than the other then temporarily pad the smaller category with points at the origin. Compute the minimum cost
     *      1:1 mapping from points in one category to those in the other and use the total cost as the distance between the
     *      categories.
     *
     *  If either function has not been analyzed, or has been analyzed but resulted in no data, then return the @p dflt value. */
    double compare(const Partitioner2::Function::Ptr&, const Partitioner2::Function::Ptr&, double dflt = NAN) const;

    /** Compare one function with all others.
     *
     *  Compare the given function with all other functions and return a list of function+cost pairs. The returned list is
     *  unsorted. */
    std::vector<FunctionDistancePair> compareOneToAll(const Partitioner2::Function::Ptr&) const;

    /** Compare one function with many others.
     *
     *  Compare the given @p needle function with all other @p haystack functions. The returned list is unsorted.
     *
     *  This analysis operates in parallel using multi-threading. It honors the global thread count usually specified with the
     *  <code>--threads=N</code> switch.
     *
     * @{ */
    template<class FunctionIterator>
    std::vector<FunctionDistancePair>
    compareOneToMany(const Partitioner2::Function::Ptr &needle,
                     const boost::iterator_range<FunctionIterator> &haystack) const {
        std::vector<Partitioner2::Function::Ptr> others;
        BOOST_FOREACH (const Partitioner2::Function::Ptr &other, haystack)
            others.push_back(other);
        return compareOneToMany(needle, others);
    }

    template<class FunctionIterator>
    std::vector<FunctionDistancePair>
    compareOneToMany(const Partitioner2::Function::Ptr &needle,
                     const FunctionIterator &begin, const FunctionIterator &end) const {
        return compareOneToMany(needle, boost::iterator_range<FunctionIterator>(begin, end));
    }

    std::vector<FunctionDistancePair>
    compareOneToMany(const Partitioner2::Function::Ptr &needle,
                     const std::vector<Partitioner2::Function::Ptr> &haystack) const;
    /** @} */

    /** Compare many functions to many others.
     *
     *  Given two ordered lists of functions, calculate the distances from all functions of the first list to all functions of
     *  the second list.  The return value is a rectangular matrix whose rows are indexed by the functions of the first list
     *  and whose columns are indexed by the functions of the second list.  See also, @ref compareManyToManyMatrix, which
     *  returns a square matrix of function distances.
     *
     *  This analysis operates in parallel using multi-threading. It honors the global thread count usually specified with the
     *  <code>--threads=N</code> switch. */
    std::vector<std::vector<double> > compareManyToMany(const std::vector<Partitioner2::Function::Ptr>&,
                                                        const std::vector<Partitioner2::Function::Ptr>&) const;

    /** Compare many functions to many others.
     *
     *  Given two ordered lists of functions, temporarily pad the shorter list with null functions to make both lists equal in
     *  length. Then calculate the distances from all functions of the first list to all functions of the second
     *  list, returning a square distance matrix.  See also, @ref compareManyToMany, which may be much faster if the two
     *  function lists have wildly different sizes.
     *
     *  This analysis operates in parallel using multi-threading. It honors the global thread count usually specified with the
     *  <code>--threads=N</code> switch. */
    DistanceMatrix compareManyToManyMatrix(std::vector<Partitioner2::Function::Ptr>,
                                           std::vector<Partitioner2::Function::Ptr>) const;

    /** Minimum cost 1:1 mapping.
     *
     *  Compute the minimum cost 1:1 mapping of functions in the first list to those in the second.  The algorithm first calls
     *  @ref compareManyToManyMatrix to obtain a square matrix of all functions compared with all other functions (null
     *  functions are added as necessary to make the result square).  It then calls @ref findMinimumAssignment to find a 1:1
     *  mapping between the two (padded) lists of functions. The return value represents the 1:1 mapping.
     *
     *  Since @ref findMinimumAssignment only works if ROSE is configured with dlib support, this function throws an @ref
     *  Exception if that support is missing. */
    std::vector<FunctionPair> findMinimumCostMapping(const std::vector<Partitioner2::Function::Ptr> &list1,
                                                     const std::vector<Partitioner2::Function::Ptr> &list2) const;

    /** Compute distances between sets of functions.
     *
     *  This is a low-level function to compute the distance between all pairs of functions from list1 and list2 in
     *  parallel. The return value contains the distances so that the distance between the function @c list1[i] and @c list2[j]
     *  is at index <code>i * list2.size() + j</code> in the return value. */
    std::vector<double> computeDistances(const std::vector<Partitioner2::Function::Ptr> &list1,
                                         const std::vector<Partitioner2::Function::Ptr> &list2,
                                         size_t nThreads) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Sorting
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Predicate for sorting by ascending distance. */
    static bool sortByIncreasingDistance(const FunctionDistancePair &a, const FunctionDistancePair &b) {
        return a.second < b.second;
    }

    /** Predicate for sorting by descending distance. */
    static bool sortByDecreasingDistance(const FunctionDistancePair &a, const FunctionDistancePair &b) {
        return b.second < a.second;
    }

    /** Predicate for sorting by function address. */
    static bool sortByAddress(const FunctionDistancePair &a, const FunctionDistancePair &b) {
        if (a.first == NULL || b.first == NULL)
            return a.first == NULL && b.first != NULL;
        return a.first->address() < b.first->address();
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Printing and debugging
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Initializes and registers disassembler diagnostic streams. See Diagnostics::initialize(). */
    static void initDiagnostics();

    /** Print characteristic values for this analysis.
     *
     *  This is a multi-line output intended for debugging. */
    void printCharacteristicValues(std::ostream&) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Utility functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Cartesian distance between two points. */
    static double cartesianDistance(const FunctionSimilarity::CartesianPoint&, const FunctionSimilarity::CartesianPoint&);

    /** Find minimum mapping from rows to columns.
     *
     *  Finds a 1:1 mapping from rows to columns of the specified square matrix such that the total cost is minimized. Returns
     *  a vector V such that V[i] = j maps rows i to columns j.
     *
     *  This function will only work if ROSE has been compiled with dlib support. Otherwise it throws an @ref Exception. */
    static std::vector<size_t> findMinimumAssignment(const DistanceMatrix&);

    /** Total cost of a mapping.
     *
     *  Given a square matrix and a 1:1 mapping from rows to columns, return the total cost of the mapping. The @p assignment
     *  is like the value returned by @ref findMinimumAssignment. */
    static double totalAssignmentCost(const DistanceMatrix&, const std::vector<size_t> &assignment);

    /** Maximum value in the distance matrix. */
    static double maximumDistance(const DistanceMatrix&);

    /** Average distance in the matrix. */
    static double averageDistance(const DistanceMatrix&);

    /** Median distance in the matrix. */
    static double medianDistance(const DistanceMatrix&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Internal functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    static double comparePointClouds(const PointCloud&, const PointCloud&);
    static double compareOrderedLists(const OrderedLists&, const OrderedLists&);
};

std::ostream& operator<<(std::ostream&, const FunctionSimilarity&);

} // namespace
} // namespace

#endif
