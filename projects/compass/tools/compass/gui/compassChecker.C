#include "compassChecker.h"
#include "compassInterface.h"

CompassChecker::CompassChecker(const Compass::Checker *checker_) :
  checker(checker_),
  enabled(false),
  ellapsedTime(0.0)
{
} //CompassChecker::CompassChecker(const Compass::Checker *checker_)

CompassChecker::~CompassChecker()
{
} //CompassChecker::~CompassChecker()

std::string CompassChecker::getName() const
{
  return checker->checkerName;
} //std::string CompassChecker::getName()

std::string CompassChecker::getLongDescription() const
{
  return checker->longDescription;
} //std::string CompassChecker::getLongDescription()

std::string CompassChecker::getShortDescription() const
{
  return checker->shortDescription;
} //std::string CompassChecker::getShortDescription()

bool CompassChecker::isEnabled() const
{
  return enabled;
} //CompassChecker::isEnabled()

void CompassChecker::setEnabled(bool en)
{
  enabled = en;
} //CompassChecker::setEnabled(bool en)

void CompassChecker::reset()
{
  enabled = false;
  error.clear();
  violations.clear();
} //CompassChecker::reset()

void CompassChecker::process(bool enable, CompassInterface &compassInterface)
{
   SgProject *project = compassInterface.getProject();
   ROSE_ASSERT(project);

   // we pass the checker's violation's list to be filled
   // by the output object
   compassInterface.getResult()->setViolationList(&violations);
   this->reset(); // clear previous result
   if (enable) {
      try {
         Timer t;
         checker->run(*compassInterface.getParams(),
                       compassInterface.getResult()); // run the checker
      } catch (const std::exception& e) {
         error = e.what(); // if there is an exception,
                           // we keep the message in _error
         enabled = false;
      }
      enabled = enable;
      ellapsedTime = Timer::getTimeEllapsed();
   }
   else enabled = false;

   compassInterface.getResult()->setViolationList(0);

  return;
} //CompassChecker::process(bool enable)

const std::vector<CompassViolation>* CompassChecker::getViolations() const
{
  return &violations;
} //CompassChecker::getViolations()

int CompassChecker::getNumViolations() const
{
  return violations.size();
} //CompassChecker::getNumViolations()

bool CompassChecker::hasErrorMsg() const
{
  return !error.empty();
} //CompassChecker::hasErrorMsg()

double CompassChecker::getEllapsedTime() const
{
  return ellapsedTime;
} //CompassChecker::getEllapsedTime()

std::string CompassChecker::getErrorMsg() const
{
  return error;
} //CompassChecker::getErrorMsg()

bool CompassChecker::lessBasedOnName(
  const CompassChecker *left, const CompassChecker *right) 
{
   return left->getName() < right->getName();
} //CompassChecker::lessBasedOnName()

bool CompassChecker::lessBasedOnTiming(
  const CompassChecker *left, const CompassChecker *right) 
{
   if (left->isEnabled() && right->isEnabled()) {
      if (!left->hasErrorMsg() && !right->hasErrorMsg()) {
         if (left->getEllapsedTime() == right->getEllapsedTime()) {
            return left->getName() < right->getName();
         } else {
            return left->getEllapsedTime() > right->getEllapsedTime();
         }
      } else if (left->hasErrorMsg() && right->hasErrorMsg()) {
         return left->getName() < right->getName();
      } else {
         return right->hasErrorMsg();
      }
   } else if (!left->isEnabled() && !right->isEnabled()) {
      return left->getName() < right->getName();
   } else {
      return left->isEnabled();
   }
}

bool CompassChecker::lessBasedOnViolations(
  const CompassChecker *left, const CompassChecker *right) 
{
   if (left->isEnabled() && right->isEnabled()) {
      if (!left->hasErrorMsg() && !right->hasErrorMsg()) {
         if (left->getNumViolations() == right->getNumViolations()) {
            return left->getName() < right->getName();
         } else {
            return left->getNumViolations() > right->getNumViolations();
         }
      } else if (left->hasErrorMsg() && right->hasErrorMsg()) {
         return left->getName() < right->getName();
      } else {
         return left->hasErrorMsg();
      }
   } else if (!left->isEnabled() && !right->isEnabled()) {
      return left->getName() < right->getName();
   } else {
      return left->isEnabled();
   }
}

