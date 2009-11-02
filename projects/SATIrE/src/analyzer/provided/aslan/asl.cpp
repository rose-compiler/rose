/**
 *******************************************
 * @file asl.cpp
 * @author Dietmar Schreiner
 * @version 1.0
 * @date 2009-11-02
 *******************************************
 */ 

#include <sstream>
#include "asl.h"

// -----------------------------------------------------------------
/** 
 * Constructor.
 * @param functionName full qualified name of function 
 *                     (including compilation unit).
 * @param type the type of the ASL 
 *             (currently only CALL_SITE is supported).
 * @param index the enumeration identifier of the location.
 */
// -----------------------------------------------------------------

AbstractSourceLocation::AbstractSourceLocation(std::string functionName, ASLType type,unsigned int index){
  this->functionName=functionName;
  this->type=type;
  this->index=index;
}

// -----------------------------------------------------------------
/** 
 * serialize ASL to string.
 * @result a string representing the ASL.
 */
// -----------------------------------------------------------------

std::string AbstractSourceLocation::toString(){
  std::stringstream result;

  result<<functionName<<"@";

  switch(type){
    case CALL_SITE:
      result<<"C";
      break;
    case LOOP:
      result<<"L";
      break;
    default:
      result<<"?";
  }

  result<<index;

  return result.str();
}
