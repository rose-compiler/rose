#include "compassChecker.h"
#include <iostream>


#include "compassInterface.h"

CompassInterface::CompassInterface() :
  project(0),
  result(new CompassResult),
  params(0),
  argc(0),
  argv(0)
{
} //CompassInterface::CompassInterface()

CompassInterface::~CompassInterface()
{
} //CompassInteface::~CompassInterface()

// defined in libCompassCheckers.la
void buildCheckers( std::vector<const Compass::Checker*> &,
                    Compass::Parameters &,
                    Compass::OutputObject &,
                    SgProject* );

void CompassInterface::init( int argc_, char **argv_ )
{
  argc = argc_;
  argv = argv_;

  Rose_STL_Container<std::string> commandLineArray = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

  Compass::commandLineProcessing(commandLineArray);


  params = new Compass::Parameters(Compass::findParameterFile());

  //The buildcheckers interface seems not to require the project node
  //so did this change to support the visualization of the DB
  if ( Compass::UseDbOutput == false )
      project = frontend(argc_,argv_);


  //  std::vector<const Compass::Checker*> theCheckers;

  buildCheckers( theCheckers, *params, *result, project );

  for( std::vector< const Compass::Checker* >::const_iterator itr = theCheckers.begin(); itr != theCheckers.end(); itr++ )
  {
    ROSE_ASSERT( *itr );

    if ( Compass::UseDbOutput == false )
      Compass::runPrereqs(*itr, project);
    const Compass::Checker *ch = *itr;
    compassCheckers.push_back(new CompassChecker(ch));
  } //for, itr 

  return;
} //CompassInterface::init( int argc, char **argv )

const CompassCheckers_v & CompassInterface::getCompassCheckers() const
{
  return compassCheckers;
} //CompassInterface::getCompassCheckers()

std::vector<const Compass::Checker*>& CompassInterface::getCheckers()
{
  return theCheckers;
} //CompassInterface::getCheckers()

const CompassChecker* CompassInterface::getCompassChecker(int pos) const
{
  return compassCheckers.at(pos);
} //CompassInterface::getCompassChecker(int pos)

int CompassInterface::numCheckers() const
{
  return compassCheckers.size();
} //CompassInterface::numCheckers()

int CompassInterface::numEnabledCheckers() const
{
  int n = 0;

  for( CompassCheckers_v::const_iterator itr = compassCheckers.begin();
       itr != compassCheckers.end(); itr++ )
  {
    if((*itr)->isEnabled()) n++;
  } //for, itr

  return n;
} //CompassInterface::numEnalbedCheckers()

SgProject* CompassInterface::getProject()
{
  return project;
} //CompassInterface::getProject()

CompassResult* CompassInterface::getResult()
{
  return result;
} //CompassInterface::getResult()

Compass::Parameters* CompassInterface::getParams()
{
  return params;
} //CompassInterface::getParams()

void CompassInterface::reset()
{
  for( CompassCheckers_v::iterator itr = compassCheckers.begin(); 
       itr != compassCheckers.end(); itr++ )
  {
    (*itr)->reset();
  } //for

  return;
} //CompassInterface::reset()

void CompassInterface::sort(int sortType)
{
  switch (sortType) 
  {
    case SORT_NAME: std::sort(compassCheckers.begin(), compassCheckers.end(), &CompassChecker::lessBasedOnName); break;
    case SORT_TIMING: std::sort(compassCheckers.begin(), compassCheckers.end(), &CompassChecker::lessBasedOnTiming); break;
    case SORT_VIOLATIONS: std::sort(compassCheckers.begin(), compassCheckers.end(), &CompassChecker::lessBasedOnViolations); break;
    default: break;
  } //switch (sortType)

  return;
} //CompassInterface::sort(int sortType)
