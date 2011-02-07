// Author: Andreas Saebjoernsen
#include "GenRepresentation.h"
#ifndef GENERALREPRESENTATION_C
#define GENERALREPRESENTATION_C

GenRepresentation::GenRepresentation ()
{
  dotout = new ostringstream ();
  edges = new ostringstream ();
}

GenRepresentation::~GenRepresentation ()
{
  delete dotout;
}

string
  GenRepresentation::beginArray (void *classPointer, string arrayName,
                                 TypeOfPrint printType)
{
  string returnString;


  if (checkIfMapKeyExist (classPointer) != true)
    cerr << "The class key doesnt exist." << endl;

  if (printType == Container)
    {
      returnString =
        " \"" + pointerName (classPointer) + "\" [shape=record, label=\" <" +
        pointerName (classPointer) + "> Array " + arrayName + "|{";
    }
  else
    returnString =
      "|{ <" + pointerName (classPointer) + ">  Array " + arrayName + "|{";

  (*dotout) << returnString;

  return returnString;
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
  GenRepresentation::beginClass (void *classPointer, string className,
                                 TypeOfPrint printType)
{
  string returnString;


  if (checkIfMapKeyExist (classPointer) != true)
    cerr << "The class key doesnt exist." << endl;

  if (printType == Container)
    {
      returnString =
        " \"" + pointerName (classPointer) + "\" [shape=Mrecord, label=\" <" +
        pointerName (classPointer) + "> Class " + className + "|{";
    }
  else
    returnString =
      "|{ <" + pointerName (classPointer) + ">  Class " + className + "|{";

  (*dotout) << returnString;

  return returnString;
}


string
  GenRepresentation::beginStruct (void *structPointer, string structName,
                                  TypeOfPrint printType)
{
  string returnString;
  if (checkIfMapKeyExist (structPointer) != true)
    cerr << "The struct key doesnt exist." << endl;

  if (printType == Container)
    {
      returnString =
        " \"" + pointerName (structPointer) + "\" [shape=record, label=\"<" +
        pointerName (structPointer) + "> Struct " + structName + "|{";
    }
  else
    returnString =
      "|{ <" + pointerName (structPointer) + ">  Struct " + structName + "|{";

  (*dotout) << returnString;


  return returnString;
}

string
  GenRepresentation::beginUnion (void *unionPointer, string unionName,
                                 TypeOfPrint printType)
{
  string returnString;
  if (printType == Container)
    if (checkIfMapKeyExist (unionPointer) != true)
      cerr << "The union key doesnt exist." << endl;

  if (printType == Container)
    {
      returnString =
        " \"" + pointerName (unionPointer) + "\" [shape=record, label=\"<" +
        pointerName (unionPointer) + "> Union " + unionName + "|{";
    }
  else
    returnString =
      "|{ <" + pointerName (unionPointer) + ">  Union " + unionName + "|{";

  (*dotout) << returnString;


  return returnString;
}

string
GenRepresentation::endClass (TypeOfPrint printType)
{
  string returnString = "";

  if (printType == Container)
    {
      returnString = "}\"];\n";
      (*dotout) << returnString;
    }
  else
    {
      returnString = "}}";
      (*dotout) << returnString;
    }

  return returnString;
}

string
GenRepresentation::endStruct (TypeOfPrint printType)
{
  string returnString = "";

  if (printType == Container)
    {
      returnString = "}\"];\n";
      (*dotout) << returnString;
    }
  else
    {
      returnString = "}}";
      (*dotout) << returnString;
    }

  return returnString;
}

string
GenRepresentation::endUnion (TypeOfPrint printType)
{

  string returnString = "";

  if (printType == Container)
    {
      returnString = "}\"];\n";
      (*dotout) << returnString;
    }
  else
    {
      returnString = "}}";
      (*dotout) << returnString;
    }

  return returnString;
}

string
GenRepresentation::endArray (TypeOfPrint printType)
{

  string returnString = "";

  if (printType == Container)
    {
      returnString = "}\"];\n";
      (*dotout) << returnString;
    }
  else
    {
      returnString = "}}";
      (*dotout) << returnString;
    }

  return returnString;
}
/*
string GenRepresentation::addVariable(string variableType, string variableName, ostringstream* variableValue, string variableReferenceName = "", bool separator = false)
{

        string returnString;
        
        if(separator == false){
                if(variableReferenceName.length() > 0)
                        returnString = "<"+ variableReferenceName + ">  " + variableType + " " + variableName;
                else
                        returnString = " " + variableType + " " + variableName;
                
                (*dotout) << returnString; 
        }else{
                if(variableReferenceName.length() > 0)
                        returnString = " |  <" + variableReferenceName + ">  " +  variableType + " " + variableName;
                else
                        returnString = " |" + variableType + " " + variableName;
                (*dotout) << returnString ;
        }
        if(variableValue->str().length() > 0) 
               (*dotout) << " = " << variableValue->str();

        return returnString;
}
*/
string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  string variable,
                                  string variableReferenceName,
                                  bool separator)
{
  string returnString;

  if (separator == false)
    {
      if (variableReferenceName.length () > 0)
        returnString =
          "<" + variableReferenceName + ">  " + variableType + " " +
          variableName;
      else
        returnString = " " + variableType + " " + variableName;

      (*dotout) << returnString;
    }
  else
    {
      if (variableReferenceName.length () > 0)
        returnString =
          " |  <" + variableReferenceName + ">  " + variableType + " " +
          variableName;
      else
        returnString = " |" + variableType + " " + variableName;
      (*dotout) << returnString;
    }
  if (variable.length() > 0)
    (*dotout) << " = " << variable;

  return returnString;
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  void *variablePointer,
                                  string variableComment, bool separator)
{
  return addVariable ("int", variableName, variableComment,
                      getNextMapKeyName (variablePointer), separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  int variable, string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("int", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  bool variable, string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
/*  cout << "There is a bool here" << endl;
  cout << variableType << ":" << variableName << endl;*/
  return addVariable (variableType, variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  double variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("double", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  long variable, string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("long", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  float variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("float", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  char variable, string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("char", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  unsigned char variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("unsigned char", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}



string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  long long variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("long long", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  long double variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("long double", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  short variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("short", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  unsigned int variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("unsigned int", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  unsigned long variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("unsigned long", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  unsigned long long variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("unsigned long long", variableName,
                      (*variableValue).str (), variableReferenceName,
                      separator);
}

string
  GenRepresentation::addVariable (string variableType, string variableName,
                                  unsigned short variable,
                                  string variableReferenceName,
                                  bool separator)
{
  ostringstream *variableValue = new ostringstream ();
  (*variableValue) << variable;
  return addVariable ("unsigned short", variableName, (*variableValue).str (),
                      variableReferenceName, separator);
}

void
GenRepresentation::clear ()
{
  delete dotout;
  dotout = new ostringstream ();
}

void
GenRepresentation::addEdge (void *node1, void *node2, void *node2parent,
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
GenRepresentation::checkIfMapKeyExist (void *key)
{
  if (pointerNumberMap.find (key) == pointerNumberMap.end ())
    return false;
  else
    return true;
}

void
GenRepresentation::setMapKey (void *key)
{
  if (checkIfMapKeyExist (key) == true)
    {
      cerr << "The key does exist. Do not make sence to set it. Terminating"
        << endl;
      exit (1);
    }
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
GenRepresentation::setMapKeySaved (void *key)
{
  if (pointerHasBeenPrinted.find (key) != pointerHasBeenPrinted.end ())
    cerr << "The key doesnt exist." << endl;
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
GenRepresentation::pointerName (void *key)
{
  ostringstream ss;

  ss << key;

  return ss.str ();

}

string
GenRepresentation::getNextMapKeyName (void *key)
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
  dotfile << beginGraph (graphName) << (*dotout).str () << (*edges).
    str () << endGraph ();
}

void
GenRepresentation::writeToFile (string filename)
{
  ofstream dotfile (filename.c_str ());
  dotfile << (*dotout).str ();
}




#endif
