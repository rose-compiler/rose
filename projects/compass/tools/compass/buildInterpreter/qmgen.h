#ifndef ROSE_QM_GEN_QM_H
#define ROSE_QM_GEN_QM_H

#include <string>
#include <vector>

std::string constructMangledTestName();
int get_argv_index( std::string arg, int argc, char **argv );

void initialize_qmtest_arguments_vector(
  int argc,
  char **argv,   std::string prefix,
  std::vector< std::string > &arguments );

void initialize_qmtest_arguments( int argc, char **argv );

void write_qmtest_raw();
void write_rose_db();

#endif
