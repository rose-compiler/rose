// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>

using namespace std;


string
// Grammar::outputClassesAndFields ( GrammarTreeNode & node, fstream & outputFile )
Grammar::outputClassesAndFields ( Terminal & node )
   {
     string className = node.getName();

     string dataFields = node.outputClassesAndFields();

     string returnString = className + "\n" + dataFields;
  // printf ("returnString = \n%s\n",returnString.c_str());

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<Terminal *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

       // outputClassesAndFields(**treeNodeIterator,outputFile);
          returnString += outputClassesAndFields(**treeNodeIterator);
        }
#endif

  // returnString = GrammarString::copyEdit(returnString.c_str(),"$CLASSNAME",node.getName());
     const string target = "$CLASSNAME";
     const string name   = node.getName();
  // returnString = GrammarString::copyEdit(returnString,target,name);
  // returnString = GrammarString::copyEdit(string(returnString),"$CLASSNAME",string(node.getName()));
  // returnString = GrammarString::copyEdit(string(returnString),target,name);
  // returnString = GrammarString::copyEdit(returnString,target,name);
     string copy = returnString;
     returnString = GrammarString::copyEdit(copy,target,name);

     return returnString;
   }


string
Terminal::outputClassesAndFields ()
   {
     string returnString;
     vector<GrammarString *> localList;
     vector<GrammarString *> localExcludeList;
     vector<GrammarString *>::iterator stringListIterator;

  // Initialize with local node data
     localList        = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     localExcludeList = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);

  // Now edit the list to remove elements appearing within the exclude list
     Grammar::editStringList ( localList, localExcludeList );

  // returnString += "\n";

     for( stringListIterator = localList.begin();
          stringListIterator != localList.end();
          stringListIterator++ )
        {
          string typeName           = (*stringListIterator)->getTypeNameString();
          string variableNameString = (*stringListIterator)->getVariableNameString();
          string tempString         = "-- data member -- " + typeName + " p_" + variableNameString + "\n";

          returnString += tempString;
        }

     returnString += "\n";

     ROSE_ASSERT (localExcludeList.size() == 0);

  // printf ("In Terminal::outputClassesAndFields(): returnString = %s \n",returnString.c_str());

     return returnString;
   }


