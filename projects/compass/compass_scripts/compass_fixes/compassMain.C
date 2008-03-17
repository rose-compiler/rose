//This is an automatically generated file
#include "compass.h"
#include <rose.h>
#include <iostream>
#include <vector>

#include "checkers.h"

void buildCheckers( std::vector<Compass::TraversalBase*> &retVal, 
		    Compass::Parameters &params, 
		    Compass::OutputObject &output );

int main(int argc, char** argv)
{
  SgProject* sageProject = frontend(argc,argv);
  std::vector<Compass::TraversalBase*> traversals;

  Compass::Parameters params(Compass::findParameterFile());
  Compass::PrintingOutputObject output(std::cerr);

  buildCheckers( traversals, params, output );

  for( std::vector<Compass::TraversalBase*>::iterator itr = traversals.begin();
       itr != traversals.end(); itr++ )
  {
    if( (*itr) != NULL )
    {
      (*itr)->run( sageProject );
    } //if( (*itr) != NULL )
    else
    {
      std::cerr << "Error: Traversal failed to initialize" << std::endl;
      return 1;
    } //else
  } //for()

  return 0;
} //main()

