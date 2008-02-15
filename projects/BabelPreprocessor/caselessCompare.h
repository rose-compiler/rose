/*
 * File:        CassLessCompare.h
 * Copyright:   (c) 2005 The Regents of the University of California
 * Release:     $Name:  $
 * Revision:    @(#) $Revision: 1.1 $
 * Date:        $Date: 2005/07/06 20:18:17 $
 * Description: Compare two std::string's ignoring case
 *
 */
#ifndef ROSE_BABEL_PROGRAM_CASELESSCOMPARE_H
#define ROSE_BABEL_PROGRAM_CASELESSCOMPARE_H
#include <string>

class CaseLessCompare {

 private:
  static bool caseInsensitive(const char c1, const char c2);

 public:
  bool operator() (const std::string &s1, const std::string &s2) const;
};
   
#endif /* ROSE_BABEL_PROGRAM_CASELESSCOMPARE_H */
