// Author: Andreas Saebjoernsen
#include "rose.h"
#include "GenRepresentation.h"
#ifndef GENERALREPRESENTATION_C
#define GENERALREPRESENTATION_C

using namespace std;

#include <stdlib.h>

GenRepresentation::GenRepresentation ()
{
  dotout = new ostringstream ();
  edges = new ostringstream ();
//  queue.push_back(dotout); 
  queue[0] = new ostringstream();
  for(int i=1; i<100;i++)
     queue[i] = NULL;
  charactersToFind="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  numberBeginNodes=numberEndNodes=0;
}

GenRepresentation::~GenRepresentation ()
{
  delete dotout;
}


string
GenRepresentation::beginGraph (string graphName)
{
  string
    returnString = "digraph " + graphName + " {\n node [shape=record]; \n";
  return returnString;
}

string
GenRepresentation::endGraph ()
{
  return "\n };\n";
}

string 
GenRepresentation::beginNode(string className,
                             TypeOfPrint printType, const void* classPointer)
{
  string returnString;
  assert(queue[numberBeginNodes]!=NULL);
  
  //When a new node is started, the code of the Container (n-2) may require
  //another Container (n-1).
  //When printType==Container, a new placement in queue[C1,C2,...,Cn] is
  //used to contained the code for Container (n-1) so that it will not be
  //mixed up with the code for (n-2).
  if(printType==Container){
     assert(classPointer!=NULL);
     //cout << endl << className  << endl;
     if(numberBeginNodes==numberEndNodes)
         numberEndNodes+=1;
     numberBeginNodes+=1; 
     if(queue[numberBeginNodes]==NULL)
        queue[numberBeginNodes]=new ostringstream();
   }

  if ((checkIfMapKeyExist (classPointer) != true)&(printType!=Contained))
    cerr << "The class key doesnt exist." << endl;

  if (printType == Container)
    {
      classNameDebug=className;
      returnString =
	" \"" + pointerName (classPointer) + "\" [shape=Mrecord, label=\" <" +
	pointerName (classPointer) + "> " + className + "|{";
    }
  else
    returnString =
      "|{ " + className + "|{";

  (*queue[numberBeginNodes]) << returnString;

  return returnString;
}


string
GenRepresentation::endNode (TypeOfPrint printType)
{
  string returnString = "";
  assert(queue[numberBeginNodes]!=NULL);

  if (printType == Container)
      returnString = "}\"];\n";
  else
      returnString = "}}|";
  (*queue[numberBeginNodes]) << returnString;

   if((printType == Container)&(numberEndNodes>numberBeginNodes)){
     (*queue[numberBeginNodes]) <<  (*queue[numberEndNodes]).str(); 
     queue[numberEndNodes]->flush();
     numberEndNodes-=1;
  }

  //When the correspondeing call to beginNode (n-2) in queue[1,2,3,...,n] has 
  //been called. endNode(n-2) will put the contents of node (n-2) into node (n-3).    
  if(printType == Container){
   (*queue[0]) << (*queue[numberBeginNodes]).str();
//    (*queue[numberBeginNodes]).flush();
   delete queue[numberBeginNodes];
   queue[numberBeginNodes]=new ostringstream();

   numberBeginNodes-=1;  

  }

  return returnString;
}
/*
string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  string variable,
                                  string variableReferenceName)
{
  string returnString;
   if (variableReferenceName.length () > 0)
        returnString =
          " |  <" + variableReferenceName + ">  " + variableType + " " +
          variableName;
      else
        returnString = " |" + variableType + " " + variableName;
      (*dotout) << returnString;
//    }
  if (variable.length() > 0)
    (*dotout) << " = ?";
    //Commented out because of problems with some values
    //(*dotout) << " = " << variable;

  return returnString;
}
*/
string
  GenRepresentation::addVariable (void* variablePointer, string variableType, string variableName,
				  string variableComment)
{

  return addVariable (variableType, variableName, variableComment,
		      getNextMapKeyName (variablePointer));
}


void
GenRepresentation::clear ()
{
  delete dotout;
  dotout = new ostringstream ();
}

void
GenRepresentation::addEdge (const void *node1, const void *node2, const void *node2parent,
			    string edgeLabel, string option)
{
  //AS (01/13/04) increaseMapKeyNumber(node2);    
  if(node2 != node2parent)
  (*edges) << "\"" << node2parent << "\":" << "\"" <<
    getNextMapKeyName (node2) << "\"" << " -> " << "\"" << node1 << "\"" <<
    "  [label=\"" << edgeLabel << "\" " << option << " ];" << endl;
  else
  (*edges) << "\"" << node2 << "\"" << " -> " << "\"" << node1 << "\"" <<
    "  [label=\"" << edgeLabel << "\" " << option << " ];" << endl;
  
}

bool
GenRepresentation::checkIfMapKeyExist (const void *key)
{
  if (pointerNumberMap.find (key) == pointerNumberMap.end ())
    return false;
  else
    return true;
}

void
GenRepresentation::setMapKey (const void *key)
{
  if(key == NULL){
     cerr << "The pointer key is NULL. Terminating" << endl;
     exit(1);
  }else if (checkIfMapKeyExist (key) == true)
    {
      cerr << "The key " << key << "allready exist. Do not make sence to set it. Terminating"
	<< endl;
      exit (1);
    }
#ifdef GC_REP_DEBUG
  cout << "The key is" << key << endl;
#endif
  pointerNumberMap[key];
}

bool GenRepresentation::checkIfMapKeyHasBeenSaved (void *key)
{

  if (pointerHasBeenPrinted.find (key) != pointerHasBeenPrinted.end ())
    return true;
  else
    return false;
}

void
GenRepresentation::setMapKeySaved (const void *key)
{
  //if (pointerHasBeenPrinted.find (key) != pointerHasBeenPrinted.end ())
  //  cerr << "The key doesnt exist." << endl;
  pointerHasBeenPrinted.insert (key);

}

void
GenRepresentation::increaseMapKeyNumber (void *key)
{

  if (checkIfMapKeyExist (key) == true)
    pointerNumberMap[key]++;
  else
    pointerNumberMap[key];

}

string
GenRepresentation::pointerName (const void *key)
{
  ostringstream ss;

  ss << key;

  return ss.str ();

}

string
GenRepresentation::getNextMapKeyName (const void *key)
{
  ostringstream ss;
  if (checkIfMapKeyExist (key) != true)
    cerr << "The key doesnt exist." << endl;
  if (pointerNumberMap[key] == 0)
    ss << pointerName (key);
  else
    ss << pointerName (key) << pointerNumberMap[key];

  return ss.str ();
}

void
GenRepresentation::writeToFileAsGraph (string filename)
{
  string graphName = "\"G" + filename + "\"";
  ofstream dotfile (filename.c_str ());
  dotfile << beginGraph (graphName) << (*queue[numberBeginNodes]).str () << (*edges).
    str () << endGraph ();
}

void
GenRepresentation::writeToFile (string filename)
{
  ofstream dotfile (filename.c_str ());
  dotfile << (*queue[numberBeginNodes]).str ();
}




#endif
