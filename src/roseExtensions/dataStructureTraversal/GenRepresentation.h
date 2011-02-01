// Author: Andreas Saebjornsen

#ifndef GENERALREPRESENTATION_H
#define GENERALREPRESENTATION_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

class GenRepresentation
{
public:

  enum TypeOfPrint
  {
    UnknownListElement = 0,
    Container = 1,
    Contained = 2,
    END_OF_LIST
  };

  void writeToFileAsGraph (string filename);
  void writeToFile (string filename);

    GenRepresentation ();
   ~GenRepresentation ();
  void clear ();

  string beginGraph (string graphName);
  string endGraph ();

  //Begins an representation of a class
  string beginArray (void *, string, TypeOfPrint printType);
  string beginClass (void *, string, TypeOfPrint printType);
  string beginStruct (void *, string, TypeOfPrint printType);
  string beginUnion (void *, string, TypeOfPrint printType);
  //Ends an representation of a class
  string endArray ( TypeOfPrint printType);
  string endClass (TypeOfPrint printType);
  string endStruct (TypeOfPrint printType);
  string endUnion (TypeOfPrint printType);

  // The form of the following addVariable function is 
  //        addVariable( variable type, variable name, a pointer to the variable, a comment, separator);
  // This function is meant to be used to add a pointer variable.
  // The DOT-specific separator tells if the variable is the last variable in for instance a class. If it is separator = false, if not
  // separator=true. You may specify a comment to print with the variable if it is to be graphed.
  string addVariable (string variableType, string variableName,
                      void *variablePointer, string variableComment =
                      "", bool separator = false);

//  string addVariable(string variableType, string variableName, ostringstream* variableValue, string variableReferenceName = "", bool separator = false);

// The form of the following addVariable functins is
//     addVariable( variable type, variable name, the variable reference, name to be referenced if necessary, separator)
// These functions is meant to be used to add a variable of a generic type which is not a pointer. In the future this may be
// replaces by a template. Reason why this has not been done is because of limitations in ROSE.
  string addVariable (string variableType, string variableName,
                      string variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      double variableValue, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      int variableValue, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      long variableValue, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      float variableValue, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName, char variable,
                      string variableReferenceName = "", bool separator =
                      false);
  string addVariable (string variableType, string variableName, bool variable,
                      string variableReferenceName = "", bool separator =
                      false);
  string addVariable (string variableType, string variableName,
                      unsigned char variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      long long variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      long double variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      short variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      unsigned int variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      unsigned long variable, string variableReferenceName =
                      "", bool separator = false);
  string addVariable (string variableType, string variableName,
                      unsigned long long variable,
                      string variableReferenceName = "", bool separator =
                      false);
  string addVariable (string variableType, string variableName,
                      unsigned short variable, string variableReferenceName =
                      "", bool separator = false);

  //Adds an edge between
  //        node1:node1variable -> node2
  void addEdge (void *node2, void *node1variable, void *node1,
                string edgeLabel = "", string option = "");

  //Checks to see if the pointer key has been referenced before 
  bool checkIfMapKeyExist (void *key);
  //Registers that the pointer has been referenced
  void setMapKey (void *key);

  //Gets a unique name for a reference to a pointer key
  string getNextMapKeyName (void *key);
  //Increase the visit number for a key
  void increaseMapKeyNumber (void *key);

  //Flag the pointer to tell that it has been saved before
  void setMapKeySaved (void *key);
  //Checks to see if the pointer has been saved before
  bool checkIfMapKeyHasBeenSaved (void *key);


private:
  //converts a pointer key to a name based on the pointer
    string pointerName (void *key);

  //two dot-streams. 'dotout' contains the nodes. 
  //'edges' contains the edges 
  ostringstream *dotout, *edges;

  //map<pointer, number of time pointer has been referenced>
  //(reference count)
    map < void *, int >pointerNumberMap;
  //set<pointer> 
  //(visit flag)
    set < void *>pointerHasBeenPrinted;
};
#endif
