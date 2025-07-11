//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         THIS FILE IS MACHINE GENERATED  --  DO NOT MODIFY
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// This file was generated by Rosebud
//     /home/pirkelbauer2/build-rose/cil2/src/Rosebud/rosebud --no-locations --backend=sawyer '--serializer=boost,cereal'
//         --sawyer-generated=/home/pirkelbauer2/git/rose.edg//src/generated/Rose/Sarif --sawyer-header-prefix=rose/Rose/Sarif
//
// See documentation for the ::Rosebud namespace for information about the input language and how to best run
// the commands to (re)generate this file. See `rosebud --help` for documentation about command-line switches.
//
// The ::Rosebud namespace might be documented at one of these places:
//     https://rpm.is/rose
//     http://rosecompiler.org/ROSE_HTML_Reference/index.html
//
#ifndef ROSE_Sarif_Log_H
#define ROSE_Sarif_Log_H
#include <RoseFirst.h>

#include <rosePublicConfig.h>
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>
#endif

#include <rosePublicConfig.h>
#ifdef ROSE_HAVE_CEREAL
#include <cereal/access.hpp>
#include <cereal/cereal.hpp>
#include <cereal/types/memory.hpp>
#endif

#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Node.h>

#ifdef ROSE_Sarif_Log_IMPL
#include <Rose/Sarif/Analysis.h>
#endif

namespace Rose {
namespace Sarif {

/** SARIF results log.
 *
 *  This class represents an entire SARIF log, which accumulates results from analyses and eventually emits them as a document that
 *  can be stored in a file.  The log is the root of a tree data structure, an internal representation of the SARIF data, which does
 *  not impart any particular file format to the data. See @ref Rose::Sarif for an overview.
 *
 *  A @ref Sarif::Log is a list of @ref Sarif::Analysis objects which correspond to analyses run individually or as part of a larger
 *  tool.
 *
 *  Example:
 *
 *  @snippet{trimleft} sarifUnitTests.C sarif_example */
class Log: public Node {
public:
    // Automatically generated; do not modify!
    /** Shared-ownership pointer to @ref Log.
     *
     *  @{ */
    using Ptr = LogPtr;
    using ConstPtr = LogConstPtr;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:

    /** List of analyses.
     *
     *  A log has a list of zero or more analyses, each of which has zero or more results. 
     *  
     *  @{ */
public:
    // Automatically generated; do not modify!
    EdgeVector<Analysis> const& analyses() const;
public:
    // Automatically generated; do not modify!
    EdgeVector<Analysis>& analyses();
private:
    // Automatically generated; do not modify!
    EdgeVector<Analysis> analyses_P2039_;
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Overrides
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    bool emit(std::ostream&) override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Generated constructors, etc.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:
    /** Default constructor. */
    Log();

public:
    // Automatically generated; do not modify!
    /** Default allocating constructor. */
    static Ptr instance();

public:
    virtual ~Log();

    //----------------------- Boost serialization for Log -----------------------
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Node);
        s & BOOST_SERIALIZATION_NVP(analyses_P2039_);
    }
#endif // ROSE_ENABLE_BOOST_SERIALIZATION

#ifdef ROSE_HAVE_CEREAL
private:
    friend class cereal::access;

    // Automatically generated; do not modify!
    template<class Archive>
    void CEREAL_SAVE_FUNCTION_NAME(Archive &archive) const {
        archive(cereal::base_class<Node>(this));
        archive(cereal::make_nvp("analyses", analyses_P2039_));
    }

    // Automatically generated; do not modify!
    template<class Archive>
    void CEREAL_LOAD_FUNCTION_NAME(Archive &archive) {
        archive(cereal::base_class<Node>(this));
        archive(cereal::make_nvp("analyses", analyses_P2039_));
    }
#endif // ROSE_HAVE_CEREAL
};

} // namespace
} // namespace
#endif

#endif // include once
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         THIS FILE IS MACHINE GENERATED  --  DO NOT MODIFY
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
