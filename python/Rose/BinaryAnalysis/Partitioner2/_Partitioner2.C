#include <sage3basic.h>
#ifdef ROSE_ENABLE_PYTHON_API

#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <boost/noncopyable.hpp>
#include <boost/python.hpp>

namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace python = boost::python;

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
