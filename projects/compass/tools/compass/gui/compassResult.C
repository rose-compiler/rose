#include "rose.h"
#include "compassResult.h"

CompassResult::CompassResult() :
  violations(0)
{
}

CompassResult::~CompassResult()
{
}

void CompassResult::addOutput(Compass::OutputViolationBase* theOutput) 
{
   outputList.push_back(theOutput);

   if (violations) {
      violations->push_back(CompassViolation(theOutput));
   }
}

void CompassResult::setViolationList(std::vector<CompassViolation> *v) 
{
  violations = v;
} //CompassResult::setViolationList(std::vector<CompassViolation> *v)

void CompassResult::reset()
{
  if( violations )
    violations->clear();

  outputList.clear();
} //CompassResult::reset()

std::ostream& operator<<(std::ostream& os, const CompassResult& result)
{
  for( std::vector<Compass::OutputViolationBase*>::const_iterator 
       itr = result.outputList.begin(); itr != result.outputList.end(); itr++ )
  {
    os << (*itr)->getString() << std::endl;
  } //for, itr

  return os;
} // operator<<(std::ostream& os, const CompassResult& result)
