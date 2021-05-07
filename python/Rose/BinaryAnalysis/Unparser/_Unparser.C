#include <sage3basic.h>
#ifdef ROSE_ENABLE_PYTHON_API

#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <boost/python.hpp>

using namespace Rose::BinaryAnalysis;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace python = boost::python;

BOOST_PYTHON_MODULE(_Unparser) {

    python::class_<Unparser::PythonBase>("Base")
        .def("unparse", &Unparser::PythonBase::unparse)
        .def("print", &Unparser::PythonBase::print);
}

#endif
