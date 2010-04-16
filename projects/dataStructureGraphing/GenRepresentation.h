// Author: Andreas Saebjornsen

#ifndef GENERALREPRESENTATION_H
#define GENERALREPRESENTATION_H

#include <cassert>   // assert
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <vector>
// using namespace std;
class GenRepresentation
{
  typedef std::ostringstream         value_type;

  typedef std::ostringstream*        pointer;
  typedef const std::ostringstream*  const_pointer;

  typedef std::ostringstream&        reference;
  typedef const std::ostringstream&  const_reference;


private:
  //PS!! Watch this limit of a 100 elements in queue. 
  //May have to increase this. See beginNode and endNode.
  std::ostringstream* queue[100];
  int numberBeginNodes, numberEndNodes;
  std::string classNameDebug, charactersToFind;
public:

  enum TypeOfPrint
  {
    UnknownListElement = 0,
    Container = 1,
    Contained = 2,
    END_OF_LIST
  };

  void writeToFileAsGraph (std::string filename);
  void writeToFile (std::string filename);

  GenRepresentation ();
  ~GenRepresentation ();
  void clear ();

  std::string beginGraph (std::string graphName);
  std::string endGraph ();

  //Begins an representation of a class
  std::string beginNode (std::string, TypeOfPrint printType, const void *classPointer = NULL);
  //Ends an representation of a class
  std::string endNode (TypeOfPrint printType);

  // The form of the following addVariable function is 
  //        addVariable( variable type, variable name, a pointer to the variable, a comment, separator);
  // This function is meant to be used to add a pointer variable.
  // The DOT-specific separator tells if the variable is the last variable in for instance a class. If it is separator = false, if not
  // separator=true. You may specify a comment to print with the variable if it is to be graphed.

  std::string addVariable (void *variablePointer, std::string variableType,
		      std::string variableName, std::string variableComment = "");

/*  template<typename T>
    std::string addVariable (std::string variableType, std::string variableName,
		      T* variablePointer, std::string variableComment =
		      ""){
  cout << "This thang" << endl;
  return addVariable (variableType, variableName, variableComment,
		      getNextMapKeyName ((void*)variablePointer));
  };*/
//  std::string addVariable(std::string variableType, std::string variableName, std::ostringstream* variableValue, std::string variableReferenceName = "", bool separator = false);

// The form of the following addVariable functins is
//     addVariable( variable type, variable name, the variable reference, name to be referenced if necessary, separator)
// These functions is meant to be used to add a variable of a generic type which is not a pointer. In the future this may be
// replaces by a template. Reason why this has not been done is because of limitations in ROSE.

  template < typename T >
    std::string addVariable (std::string variableType, std::string variableName,
			const T variable, std::string variableReferenceName = "")
  {

    std::string returnString;
    std::string notationOfDot="<>{}";
    assert(queue[numberBeginNodes]!=NULL);    
    std::ostringstream *conversion = new std::ostringstream ();
    (*conversion) << variable;
    std::string variableValue = (*conversion).str();
    if (variableReferenceName.length () > 0)
     	returnString =
	  " |  <" + variableReferenceName + ">  " + variableType + " " +
	  variableName;
    else
	  returnString = " |" + variableType + " " + variableName;
    (*queue[numberBeginNodes]) << returnString;

    bool acceptValue = false;

   if (variableValue.length() > 0){
      for( unsigned int i = 0; i < charactersToFind.length(); i++ ) {
        if(charactersToFind.at(i)==variableValue.at(0))
           acceptValue = true;
      }
      for( unsigned int i = 0; i < variableValue.length(); i++ ) {
        //To avoid that the file conatins a newline when contents of a
        //string is supposed to be printed out.
        if(variableValue.at(i)=='\n'){
          variableValue.replace(i,1,"\\n");
          i++;
        }
        for(unsigned int j = 0; j < notationOfDot.length(); j++){
          if(notationOfDot.at(j)==variableValue.at(i)){
            variableValue.insert(i,"\\");
            i++;
          }
        }
      }
   if(acceptValue)
      (*queue[numberBeginNodes]) << " = " << variableValue;
/*   else if(dynamic_cast<unsigned char>(variable)){
      unsigned int v2 = (unsigned int) variable; 
      (*queue[numberBeginNodes]) << " = #" << v2;
   }*/
  }
  
  return returnString;

/*
    return addVariable (variableType, variableName, (*variableValue).str (),
			variableReferenceName);*/

  }
/*
  string addVariable (string variableType, string variableName,
		      string variable, string variableReferenceName = "");
*/
  //Adds an edge between
  //        node1:node1variable -> node2
  void addEdge (const void *node2, const void *node1variable, const void *node1,
		std::string edgeLabel = "", std::string option = "");

  //Checks to see if the pointer key has been referenced before 
  bool checkIfMapKeyExist (const void *key);
  //Registers that the pointer has been referenced
  void setMapKey (const void *key);

  //Gets a unique name for a reference to a pointer key
  std::string getNextMapKeyName (const void *key);
  //Increase the visit number for a key
  void increaseMapKeyNumber (void *key);

  //Flag the pointer to tell that it has been saved before
  void setMapKeySaved (const void *key);
  //Checks to see if the pointer has been saved before
  bool checkIfMapKeyHasBeenSaved (void *key);

private:
  //converts a pointer key to a name based on the pointer
  std::string pointerName (const void *key);

  //two dot-streams. 'dotout' contains the nodes. 
  //'edges' contains the edges 
  std::ostringstream *dotout, *edges;

  //map<pointer, number of time pointer has been referenced>
  //(reference count)
  std::map < const void *, int >pointerNumberMap;
  //set<pointer> 
  //(visit flag)
  std::set < const void *>pointerHasBeenPrinted;
};
#endif
