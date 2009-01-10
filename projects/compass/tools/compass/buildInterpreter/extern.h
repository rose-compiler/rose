#ifndef ROSE_QM_GEN_EXTERN_H
#define ROSE_QM_GEN_EXTERN_H

#include <vector>
#include <string>
#include <map>

#include <boost/program_options.hpp>

#include "defs.h"

/*
 * These are global data structures meant to be accessed from any part of this
 * program, please refer to roseQMGen.C for their descriptions.
 */

extern std::vector< std::string > includes;
extern std::vector< std::string > defines;
extern std::vector< std::string > libincludes;
extern std::vector< std::string > objects;
extern std::vector< std::string > headers;
extern std::vector< std::string > sources;
extern std::vector< std::string > libraries;
extern std::vector< std::string > relative_paths;
extern std::vector< std::string > unrecognized_arguments;
extern std::map<int, std::pair<std::string, std::string*> > qmtest_arguments;
extern pair_vector_options option_passing_arguments;
extern std::string qmtest_name;
extern std::string executable;
extern std::string shell;
extern std::string output;
extern std::string regression_root;
extern std::string uplevel;
extern std::string test_cc;
extern std::string test_cxx;
extern std::string host_cc;
extern std::string host_cxx;
extern std::string db_name;
extern std::string pwd;
extern std::string dotpwd;
extern std::string dotdotpwd;
extern std::string envoptions;
extern bool isCompileOnly;
extern boost::program_options::variables_map cmdLineMap;
extern boost::program_options::variables_map confMap;

#endif
