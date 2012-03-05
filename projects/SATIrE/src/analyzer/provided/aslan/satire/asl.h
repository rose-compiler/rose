#ifndef ASL_H
#define ASL_H

/**
 ********************************************
 * @file asl.h
 * @author Dietmar Schreiner
 * @version 1.0
 * @date 2009-11-02
 ********************************************
 */ 
 
#include <string>


/** 
 * @class AbstractSourceLocation represents an ASL.
 */
class AbstractSourceLocation{
 public:
  enum ASLType {CALL_SITE,LOOP};

 private:
  std::string functionName;
  ASLType type;
  unsigned int index;

 public:
  AbstractSourceLocation(std::string functionName,ASLType type,unsigned int index);
  std::string toString();
};

#endif
