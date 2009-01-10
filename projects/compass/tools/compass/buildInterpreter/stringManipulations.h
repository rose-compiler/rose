#ifndef ROSE_QM_GEN_STRING_MANIPULATION_H
#define ROSE_QM_GEN_STRING_MANIPULATION_H

#include <string>
#include <vector>

std::string joinOptions( 
  std::vector< std::string >::iterator b,
  std::vector< std::string >::iterator e,
  const char *jc );

void escapeQuotations( std::vector< std::string > &arguments );
void escapeQuotations( std::string &argument );

void insertQuotations( std::vector< std::string > &arguments );
void insertQuotations( std::string &argument );

void setRelativePathToRegressionRoot( pair_vector_options & options );
void setRelativePathToRegressionRoot( std::vector< std::string > &arguments );
void setRelativePathToRegressionRoot( std::string &argument );

void setRealRelativePath( std::string &argument );
void setRealRelativePath( std::vector< std::string > &arguments );

void setRealRelativePathToRegressionRoot( std::vector<std::string> &arguments );void setRealRelativePathToRegressionRoot( std::string &argument );

#endif
