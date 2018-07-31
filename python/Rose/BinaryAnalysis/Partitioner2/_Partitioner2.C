#include <sage3basic.h>
#ifdef ROSE_ENABLE_PYTHON_API

#include <AsmUnparser.h>
#include <BinaryUnparserBase.h>
#include <Partitioner2/Engine.h>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace python = boost::python;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wraps Rose::BinaryAnalysis::Partitioner2::Partitioner
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class PythonPartitioner {
//    P2::Partitioner partitioner_;
//
//public:
//    PythonPartitioner() {
//        ROSE_INITIALIZE;
//    }
//    
//    explicit PythonPartitioner(const P2::Partitioner &p) {
//        ROSE_INITIALIZE;
//        partitioner_ = p;
//    }
//
//    void unparse() const {
//        partitioner_.unparser()->unparse(std::cout, partitioner_);
//    }
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wraps Rose::BinaryAnalysis::Partitioner2::Engine
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class PythonEngine: boost::noncopyable {
//    P2::Engine engine_;
//    P2::Partitioner partitioner_;
//    
//public:
//    PythonEngine() {
//        ROSE_INITIALIZE;
//    }
//    
//    // Top-level parsing takes command-line args (sans argv[0]), one-line purpose, and multi-line description and parses the
//    // command-line, disassembles and partitions the specimen indicated by positional arguments, saves an AST, and returns a
//    // partitioner as the result.
//    P2::Partitioner parse(boost::python::list &args_, const std::string &purpose, const std::string &description) {
//        engine_.reset();
//        std::vector<std::string> args;
//        for (int i = 0; i < len(args_); ++i)
//            args.push_back(boost::python::extract<std::string>(args_[i]));
//        std::vector<std::string> specimenNames = engine_.parseCommandLine(args, purpose, description).unreachedArgs();
//        if (specimenNames.empty())
//            throw std::runtime_error("no binary specimen specified; see --help");
//        P2::Partitioner partitioner = engine_.partition(specimenNames);
//
//        return partitioner;
//    }
//
//    // Simpler top-level parsing that takes just a single specimen string.
//    P2::Partitioner parse(const std::string &specimen) {
//        engine_.reset();
//        std::vector<std::string> args;
//        args.push_back(specimen);
//        std::vector<std::string> specimenNames = engine_.parseCommandLine(args, "", "").unreachedArgs();
//        if (specimenNames.empty())
//            throw std::runtime_error("no binary specimen specified; see --help");
//        P2::Partitioner partitioner = engine_.partition(specimenNames);
//
//        return partitioner;
//    }
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Python bindings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(_Partitioner2) {
    //-----------------------------------
    // Partitioner
    //-----------------------------------

    python::class_<P2::Partitioner>("Partitioner")
        .def("unparse", &P2::Partitioner::pythonUnparse);
    
    //-----------------------------------
    // Engine
    //-----------------------------------

    python::class_<P2::Engine, boost::noncopyable>("Engine")
        .def("parse", &P2::Engine::pythonParseVector)
        .def("parse", &P2::Engine::pythonParseSingle);
}

#endif
