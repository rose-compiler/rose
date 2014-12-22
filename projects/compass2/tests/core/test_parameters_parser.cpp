/**
 * \file test_parameters_parser.C
 * \author Justin Too <too1@llnl.gov>
 *
 * TODO: refactor to accept command line arguments that specify
 *       parameters expectations.
 */

#include <iostream>
#include <map>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign.hpp>

#include "rose.h"
#include "compass2/compass.h"

void test_basic_indexing (Compass::Parameters parameters);
void test_regex_indexing (Compass::Parameters parameters);

int
main(int argc, char** argv)
  {
    SgProject* sageProject = frontend (argc, argv);

    Compass::Xml::ParametersParser parser;
    Compass::Parameters parameters = parser.parse_parameters ();

    test_basic_indexing (parameters);
    test_regex_indexing (parameters);

    return 0;
  }

void test_basic_indexing(Compass::Parameters a_parameters)
  {
    static const char* param_name = "general::foo";
    static const char* expected_value = "bar";

    Compass::ParameterValues parameters = a_parameters[param_name];
    ROSE_ASSERT (parameters.size () == 1);

    Compass::ParameterValues::iterator it;
    for (it = parameters.begin (); it != parameters.end (); ++it)
    {
        std::string value = *it;
        std::cout << value << std::endl;
        ROSE_ASSERT (value == expected_value);
    }
  }

void test_regex_indexing (Compass::Parameters a_parameters)
  {
    static const boost::regex expression("allowed.*::");

    typedef std::map<std::string, std::string> ExpectedValuesMap;
    static const ExpectedValuesMap expected_values =
        boost::assign::map_list_of
            ("allowedFunctions::boost", "join")
            ("allowedFunctions::std", "push")
            ("allowedVariableNames::variable1", "foo,bar")
            ("allowedVariableNames::variable2", "xyz");

    Compass::ParametersMap parameters  = a_parameters[expression];
    ROSE_ASSERT (parameters.size() == 4);

    BOOST_FOREACH (const Compass::ParametersMap::value_type& pair, parameters)
    {
        std::string param_name = pair.first;
        Compass::ParameterValues values = pair.second;
        std::string values_str = boost::algorithm::join(values, ",");
        std::cout << param_name << "=" << values_str << std::endl;

        ExpectedValuesMap::const_iterator f = expected_values.find (param_name);
        ROSE_ASSERT (f != expected_values.end ());
        ROSE_ASSERT (f->first == param_name);
        ROSE_ASSERT (f->second == values_str);
    }
  }

