#include "rose_config.h"

#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>
#include <fstream>
#include <map>

using namespace std;

string replaceString(string patternInInput, string replacePattern, string input) {
  string::size_type posIter = input.find(patternInInput);
  while(posIter != string::npos) {
    input.replace(posIter, patternInInput.size(), replacePattern);
    posIter = input.find(patternInInput);
  }
  return input;
}

/**********************************
 * AstNodeClass/Nonterminal functions *
 **********************************/

#if 0
bool
Grammar::isTerminal ( const string& terminalName ) const {
  bool returnValue = terminalList.containedInList(terminalName);
  return returnValue;
}

bool
Grammar::isNonTerminal ( const string& nonTerminalName ) const {
  return nonTerminalList.containedInList(nonTerminalName);
}
#endif

//////////////////////////////////
// GRAMMAR OUTPUT/VISUALIZATION //
//////////////////////////////////
// MS: 2002

using namespace std;

Grammar::GrammarSynthesizedAttribute 
Grammar::CreateGrammarDotString(AstNodeClass* grammarnode,
                  vector<GrammarSynthesizedAttribute> v) {
  GrammarSynthesizedAttribute saDot;
  string s;

  if(grammarnode->isInnerNode()) {
    // (traversed) data member information for current grammar node
    vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
        stringListIterator != includeList.end();
        stringListIterator++) {
      if ((*stringListIterator)->getToBeTraversed()==DEF_TRAVERSAL) {
        string type = (*stringListIterator)->getTypeNameString();
        type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
        type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
        type=GrammarString::copyEdit (type,"*","");
        s+=string(grammarnode->getName())+" -> "+type
          +" [label=\""+(*stringListIterator)->getVariableNameString()+"\"];\n";
      }
    }
    // approximation: if the node has at least one successor that is a non-AstNodeClass then
    //                create successor information
    // unfortunately GrammarString does not contain this information
    bool createSuccessorInfo=false;
    for(vector<AstNodeClass *>::iterator succiter=grammarnode->subclasses.begin();
        succiter!=grammarnode->subclasses.end();
        succiter++) {
      if((*succiter)->isInnerNode())
        createSuccessorInfo=true;
    }
    
    // inheritance hierarchy information
    // create data for current node (edges to subtree nodes) (only edges for inner nodes)
    if(createSuccessorInfo) {
      for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
        s+=string(grammarnode->getName())+" -> "+(*viter).grammarnode->getName()+" [color=\"blue\"];\n";
      }
    }
  }
  // union data of subtree nodes
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    s+=(*viter).text;
  }
  saDot.grammarnode=grammarnode;
  saDot.text=s;
  return saDot;
}

bool 
Grammar::isAbstractTreeGrammarSymbol(AstNodeClass* t) {
  return t->getCanHaveInstances();
}

bool 
Grammar::isAbstractTreeGrammarSymbol(string s) {
  //set<string>::iterator posIter = traversedTerminals.find(s);
  //if (posIter != traversedTerminals.end())
  //  return true;
  //compute all basetypes of the above set and you are done!

  // hack: exclude symbols explicitely which are not traversed but are included in the type hierarchy
  //"SgFunctionType","SgNamedType","SgUnparse_Info"};
  return true;
  list<string> excl;
  excl.push_back("SgType");
  excl.push_back("SgFunctionType");
  excl.push_back("SgNamedType");
  excl.push_back("SgMemberFunctionType");
  excl.push_back("SgClassType");
  excl.push_back("SgPointerType");
  excl.push_back("SgUnparse_Info");
  excl.push_back("Sg_File_Info");
  for(list<string>::iterator i=excl.begin();i!=excl.end();i++) {
    string::size_type posIter = s.find(*i);
    if (posIter != string::npos)
      return false;
  }
  return true;
}



// MS: 2003
// MS: We compute the set of traversed terminals to restrict the abstract grammar to traversed nodes only.
Grammar::GrammarSynthesizedAttribute
Grammar::CreateMinimalTraversedGrammarSymbolsSet(AstNodeClass* grammarnode,
                                                 vector<Grammar::GrammarSynthesizedAttribute> v) {
  if(grammarnode->isLeafNode()) {
    vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
    stringListIterator != includeList.end(); // ", " only between the elements of the list
    stringListIterator++) {
      if ( (*stringListIterator)->getToBeTraversed()==DEF_TRAVERSAL) {
        traversedTerminals.insert(restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode,"",""));
      }
    }  
  }
  GrammarSynthesizedAttribute dummy;
  return dummy;
}

// MS: 2003
string
Grammar::restrictedTypeStringOfGrammarString(GrammarString* gs, AstNodeClass* grammarnode, string grammarSymListOpPrefix, string grammarSymListOpPostfix) {
  string type=typeStringOfGrammarString(gs);
  
  if(type.find("static ")!=string::npos) {
    return "IGNORE"; // special case: static types are ignored entirely
  }

  // $CLASSNAME in type
  type=replaceString("$CLASSNAME",grammarnode->name,type);
  type=replaceString("std::","",type);

  // pointer type
  type=replaceString("*","",type);

  // other characters: '<','>'," ","::" => "_"
  type=replaceString("<","_",type);
  type=replaceString(">","_",type);
  type=replaceString(" ","_",type);
  type=replaceString("::","_",type);
  type=replaceString(",","_",type);

  // reduce sequences of underscores to one underscore
  type=replaceString("__","_",type);

  /* beautify type-names */
  if(type.size()>0) {
    // remove single leading "_"
    string::size_type firstPos=0;
    if(type[firstPos]=='_') {
      type.replace(firstPos, 1, "");
    }
    // remove single trailing "_"
    string::size_type lastPos=type.size()-1;
    if(type[lastPos]=='_') {
      type.replace(lastPos, 1, "");
    }
  }

  GrammarNodeInfo gInfo=getGrammarNodeInfo(grammarnode); // MS: should be a member function of GrammarNode
  if(gInfo.numContainerMembers>0) { // there can be only one container member!
    //cout << "ContainerMembers>0: " << type << endl;
    type = replaceString("PtrList","",type);
    type = replaceString("List","",type); // only SgInitializedNameList as of 05/20/03, MS
    if(!generateSDFTreeGrammar) {
      type = grammarSymListOpPrefix+type+grammarSymListOpPostfix; // EBNF notation for lists
    } else {
      type = string("ListStarOf")+type;
      sdfTreeGrammarContainerTypes.insert(type);
    }
  } else {
    type = replaceString("PtrListPtr","",type);
    //cout << "ContainerMembers<=0: " << type << endl;
  }
  return type;
}




/* infos:
   
   std::vector<GrammarString *> memberDataPrototypeList[2][2];
   AstNodeClass->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
   
*/

// Rasmussen (04/17/2019): Support for ATerms has been deprecated. I believe the following function can just return true?
//
bool Grammar::isFilteredMemberVariable(string varName) {
  // c++11: set<string> filteredMemberVariablesSet={...};
  string nonAtermMemberVariables[]={"parent","freepointer","isModified","containsTransformation","startOfConstruct","endOfConstruct",
                                    "attachedPreprocessingInfoPtr","containsTransformationToSurroundingWhitespace","attributeMechanism",
                                    "source_sequence_value","need_paren","lvalue","operatorPosition","originalExpressionTree","uses_operator_syntax",
                                    "globalQualifiedNameMapForNames","globalQualifiedNameMapForTypes","globalQualifiedNameMapForTemplateHeaders",
                                    "globalTypeNameMap","globalMangledNameMap","globalTypeTable","shortMangledNameCache","globalFunctionTypeTable",
                                    "globalQualifiedNameMapForMapsOfTypes"
  };
  set<string> filteredMemberVariablesSet(nonAtermMemberVariables, nonAtermMemberVariables + sizeof(nonAtermMemberVariables)/sizeof(nonAtermMemberVariables[0]) );
  return filteredMemberVariablesSet.find(varName)!=filteredMemberVariablesSet.end();
}

// MS: 2002, 2003, 2014
Grammar::GrammarSynthesizedAttribute 
Grammar::CreateAbstractTreeGrammarString(AstNodeClass* grammarnode,
                                         vector<GrammarSynthesizedAttribute> v) {
  GrammarSynthesizedAttribute synAttr;

  // EBNF generated Grammar symbols (this can be parameterized in future)
  // tree grammar
  string grammarSymTreeLB="(";
  string grammarSymTreeRB=")";
  string GrammarSymTreeElSep=", ";
  string grammarSymArrow=" = ";
  string grammarSymOr     ="    | ";
  string grammarSymEndRule="    ;\n\n";
  string grammarSymListOpPrefix="";
  string grammarSymListOpPostfix="*";
  
 
  string rhsTerminalSuccessors;
  vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
  for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
      stringListIterator != includeList.end();
      stringListIterator++) {
    string type=restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode, grammarSymListOpPrefix, grammarSymListOpPostfix);
      
    // ignores static types
    if(type=="IGNORE")
      continue;
    if(type=="SgToken::ROSE_Fortran_Keywords") type="<int>";
    if(type=="string"||type=="std::string") type="<string>";
    if(type=="unsigned int") type="<int>";
    
    string varName=(*stringListIterator)->getVariableNameString();
    if(isFilteredMemberVariable(varName)) {
      continue;
    }
    if(rhsTerminalSuccessors!="") {
      // before each element but not the first one
      rhsTerminalSuccessors+=GrammarSymTreeElSep;
    }
    if(generateSDFTreeGrammar) {
      rhsTerminalSuccessors+=type+"/*"+varName+"*/";
      // grammar->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
      //string dataMembers=grammarnode->outputFields ();
    } else {
      rhsTerminalSuccessors+=varName+":"+type;
      string infoFields=(*stringListIterator)->infoFieldsToString();
      rhsTerminalSuccessors+=" ["+infoFields+"]";
      string dataMembers=grammarnode->outputFields ();
      rhsTerminalSuccessors+=" [["+dataMembers+"]]";
    }
  }
  synAttr.nodetext=string(grammarnode->getName())+" "+grammarSymTreeLB+rhsTerminalSuccessors+grammarSymTreeRB;
  synAttr.terminalname=string(grammarnode->getName());
  synAttr.isTerminal=true;
  
  // create grammar rule for current grammar node and its successors
  string grammarRule;
  grammarRule=grammarnode->getName()+" -> "+grammarnode->getName()+grammarSymTreeLB+rhsTerminalSuccessors+grammarSymTreeRB;
  if(!grammarnode->getCanHaveInstances()) {
    // add info, but do not change grammar
    grammarRule+=" /* ABSTRACT CLASS */";
  }
  grammarRule+="\n";

  bool first=true;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    if((*viter).nodetext!="" /*&& isAbstractTreeGrammarSymbol(string(grammarnode->getName()))*/ ) {
      if(generateSDFTreeGrammar) {
        if(false && (*viter).isTerminal) {
          // nothing to do
        } else {
          grammarRule+=string(grammarnode->getName()) + " -> " + (*viter).terminalname+"\n";
        }
      } else {
        if(first) {
          grammarRule+=string(grammarnode->getName()) + grammarSymArrow + (*viter).nodetext+"\n";
          first=false;
        } else {
          grammarRule+=grammarSymOr + (*viter).nodetext+"\n";
        }
      }
    }
  }
  if(v.size()>0 && isAbstractTreeGrammarSymbol(string(grammarnode->getName())) ) {
    if(!generateSDFTreeGrammar) {
      grammarRule+=grammarSymEndRule;
    }
  }
  
  // union data of subtree nodes
  synAttr.grammarnode=grammarnode;
  synAttr.text=grammarRule;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    synAttr.text+=(*viter).text;
  }
  
  // create problematic node info
  GrammarNodeInfo gInfo=getGrammarNodeInfo(grammarnode); // MS: should be a member function of GrammarNode
  if(gInfo.numSingleDataMembers>0 && gInfo.numContainerMembers>0) 
    synAttr.problematicnodes+=string(grammarnode->getName())+"\n";
  
  // ------------------------------------------------------------
  // create AstNodeClass and nonterminal (and problematic node) lists 
  // ------------------------------------------------------------

  // create AstNodeClass or non-AstNodeClass entry
  if(grammarnode->isLeafNode()) {
    synAttr.terminalsbunch+=string(grammarnode->getName())+"\n";
  } else {
    synAttr.nonterminalsbunch+=string(grammarnode->getName())+"\n";
  }
  // union non-AstNodeClass, AstNodeClass, and problematic nodes data of subtree nodes
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    // union subtrees
    synAttr.nonterminalsbunch+=(*viter).nonterminalsbunch;
    synAttr.terminalsbunch+=(*viter).terminalsbunch;
    synAttr.problematicnodes+=(*viter).problematicnodes;
  }
  return synAttr;
}

void Grammar::buildGrammarDotFile(AstNodeClass* rootNode, ostream& GrammarDotFile) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateGrammarDotString);
  GrammarDotFile << "digraph G {\n";
  GrammarDotFile << a.text;
  GrammarDotFile << "\n}" << endl;
}

// MS:2002,2014
void Grammar::buildAbstractTreeGrammarFile(AstNodeClass* rootNode, ostream& AbstractTreeGrammarFile) {
  generateSDFTreeGrammar=false;
  GrammarSynthesizedAttribute dummy=BottomUpProcessing(rootNode, &Grammar::CreateMinimalTraversedGrammarSymbolsSet);
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateAbstractTreeGrammarString);
  AbstractTreeGrammarFile << "//  Abstract Tree Grammar"<<endl<<endl;
  //AbstractTreeGrammarFile << "Grammar G=<NonTerminals, Terminals, Rules, SgNode>\n\n";
  AbstractTreeGrammarFile << "/*  Problematic nodes (fixed number of children and container(s)):\n" << a.problematicnodes<<"*/"<<endl<<endl;
  AbstractTreeGrammarFile << "NONTERMINALS:\n" << a.nonterminalsbunch<<endl;
  AbstractTreeGrammarFile << "TERMINALS:\n" << a.terminalsbunch<<endl;
  AbstractTreeGrammarFile << "PRODUCTIONS:\n" << a.text<<endl;
  AbstractTreeGrammarFile << "END"<<endl;
}

// MS:2014
void Grammar::buildSDFTreeGrammarFile(AstNodeClass* rootNode, ostream& SDFTreeGrammarFile) {
  generateSDFTreeGrammar=true;
  GrammarSynthesizedAttribute dummy=BottomUpProcessing(rootNode, &Grammar::CreateMinimalTraversedGrammarSymbolsSet);
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateAbstractTreeGrammarString);
  SDFTreeGrammarFile << "regular tree grammar"<< endl;
  SDFTreeGrammarFile << "start SgNode"<< endl;
  SDFTreeGrammarFile << "productions"<< endl;
  SDFTreeGrammarFile << a.text;
  SDFTreeGrammarFile <<endl;
  //SDFTreeGrammarFile << "#CONTAINERTYPES:"<<sdfTreeGrammarContainerTypes.size()<<endl;
  // generate rtg rules for container types 
  for(set<string>::iterator i=sdfTreeGrammarContainerTypes.begin();i!=sdfTreeGrammarContainerTypes.end();++i) {
    string listName=*i;
    string elemName=replaceString("ListStarOf","",listName);
    SDFTreeGrammarFile << listName << " -> <nil>()"<<endl;
    SDFTreeGrammarFile << listName << " -> <cons>("<<elemName<<","<<listName<<")"<<endl;
  }
  
}
