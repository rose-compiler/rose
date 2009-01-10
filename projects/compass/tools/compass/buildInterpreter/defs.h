#ifndef ROSE_QM_GEN_DEFS_H
#define ROSE_QM_GEN_DEFS_H

#include <vector>
#include <string>

/*
 * This data structure is used in parsing command line options that pass
 * options to sub-components of a compiler such as gcc, -Wl, -Wp,
 *
 * The intention is to have a pair of vector of string and vector of vector
 * of strings. The first vector of strings holds the original argument used
 * to locate its index in the argv array. The second vector of vector of
 * strings holds the corresponding argument split into many strings using
 * boost string algorithms based on the comma ',' split delimiter.
 *
 * The split allows one to control the string manipulation only on the last
 * string result of the split such that only the passed argument is modified
 * for relative path to regression_root, etc. The completed and modified
 * argument is re-joined to produce the file qmtest argument.
 *
 * This data structure is designed to work with joinOptions()
 */
typedef std::pair< std::vector< std::string >, std::vector< std::vector< std::string > > > pair_vector_options;

#endif
