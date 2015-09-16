/**********************************
 * Terminal/Nonterminal functions *
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
Grammar::CreateGrammarDotString(Terminal* grammarnode,
                  vector<GrammarSynthesizedAttribute> v) {
  GrammarSynthesizedAttribute saDot;
  string s;

  if(grammarnode->isInnerNode()) {
    // (traversed) data member information for current grammar node
    vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
        stringListIterator != includeList.end();
        stringListIterator++) {
      if ((*stringListIterator)->getToBeTraversed()==TraversalFlag(true)) {
        string type = (*stringListIterator)->getTypeNameString();
        type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
        type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
        type=GrammarString::copyEdit (type,"*","");
        s+=string(grammarnode->getName())+" -> "+type
          +" [label=\""+(*stringListIterator)->getVariableNameString()+"\"];\n";
      }
    }
    // approximation: if the node has at least one successor that is a non-terminal then
    //                create successor information
    // unfortunately GrammarString does not contain this information
    bool createSuccessorInfo=false;
    for(vector<Terminal *>::iterator succiter=grammarnode->subclasses.begin();
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
Grammar::CreateMinimalTraversedGrammarSymbolsSet(Terminal* grammarnode,
                                                 vector<Grammar::GrammarSynthesizedAttribute> v) {
  if(grammarnode->isLeafNode()) {
    vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
    stringListIterator != includeList.end(); // ", " only between the elements of the list
    stringListIterator++) {
      if ( (*stringListIterator)->getToBeTraversed()==TraversalFlag(true)) {
        traversedTerminals.insert(restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode,"",""));
      }
    }  
  }
  GrammarSynthesizedAttribute dummy;
  return dummy;
}

// MS: 2003
string
Grammar::restrictedTypeStringOfGrammarString(GrammarString* gs, Terminal* grammarnode, string grammarSymListOpPrefix, string grammarSymListOpPostfix) {
  string type=typeStringOfGrammarString(gs);
  
  type = replaceString("$CLASSNAME",grammarnode->name,type);
  type = replaceString("std::","",type);

  // SgPartialFunctionModifier has "static" as part of its type
  std::size_t found = type.find("static");
  if (found!=std::string::npos) {
    return "IGNORE";
  }
  type=replaceString("static ","",type);

  string::size_type posIter = type.find("*");
  if (posIter != string::npos)
    type.replace(posIter, 1, "");
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
   terminal->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
   
*/

// MS: 2002, 2003, 2014
Grammar::GrammarSynthesizedAttribute 
Grammar::CreateAbstractTreeGrammarString(Terminal* grammarnode,
                                         vector<GrammarSynthesizedAttribute> v) {
  //cout << "Creating grammar latex string:" << endl;
  GrammarSynthesizedAttribute saLatex;
  string s;

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
  
  // c++11: set<string> filteredMemberVariablesSet={...};
  string nonAtermMemberVariables[]={"parent","freepointer","isModified","containsTransformation","startOfConstruct","endOfConstruct","attachedPreprocessingInfoPtr"
                                    ,"containsTransformationToSurroundingWhitespace","attributeMechanism","source_sequence_value","need_paren","lvalue","operatorPosition","originalExpressionTree"};
  set<string> filteredMemberVariablesSet(nonAtermMemberVariables, nonAtermMemberVariables + sizeof(nonAtermMemberVariables)/sizeof(nonAtermMemberVariables[0]) );

  if(grammarnode->isLeafNode()) {
    string rhsTerminalSuccessors;
    /*
      bool containermembers=0; // (non-pointer)
      bool singledatamembers=0;      // (pointer or non-pointer)
    */
    vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
    for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
        stringListIterator != includeList.end(); // ", " only between the elements of the list
        stringListIterator++) {
      if (true|| (*stringListIterator)->getToBeTraversed()==TraversalFlag(true) ) {
        string type=restrictedTypeStringOfGrammarString(*stringListIterator,grammarnode, grammarSymListOpPrefix, grammarSymListOpPostfix);
        string varName=(*stringListIterator)->getVariableNameString();
#if 1
        // ignores static types
        if(type=="IGNORE")
          continue;
        if(type=="SgToken::ROSE_Fortran_Keywords") type="<int>";
        if(type=="string"||type=="std::string") type="<string>";
        if(type=="unsigned int") type="<int>";

        // TODO: use filteredMemberVariablesSet HERE
        if(filteredMemberVariablesSet.find(varName)!=filteredMemberVariablesSet.end()) {
          continue;
        }
#else
        if(type=="IGNORE") {}
        if(varName=="parent"||varName=="freepointer")
          varName="IGNORE";
#endif
        if(rhsTerminalSuccessors!="") {
          // before each element but not the first one
          rhsTerminalSuccessors+=GrammarSymTreeElSep;
        }
        /*
          char* type =new char[300];type[0]='\0';
          (void) strcat( type, (*stringListIterator)->getTypeNameString());
          type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
          type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
          //type=GrammarString::copyEdit (type,"*","");
          */
        if(generateSDFTreeGrammar) {
          rhsTerminalSuccessors+=type; //+":"+varName;
          // grammar->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
          //string dataMembers=grammarnode->outputFields ();
        } else {
          rhsTerminalSuccessors+=varName+":"+type;
          string infoFields=(*stringListIterator)->infoFieldsToString();
          rhsTerminalSuccessors+=" ["+infoFields+"]";
          string dataMembers=grammarnode->outputFields ();
          rhsTerminalSuccessors+=" [["+dataMembers+"]]";
        }
        // based on the type*name* used it is infered whether it is a
        // a container or a single data member (single pointer or single obj)
        // this is just a "heuristic" test. Changing the typenames invalidates
        // it
        //string stype=string(type);
        /*
          if( (stype.find("*") == string::npos) // not found, not a pointer
          && (stype.find("List") == stype.size()-4) ) // postfix
          containermembers++;
          else 
          singledatamembers++;
        */
      }
    }
        
    // assert: s2=="" means that no members are traversed of this terminal 'grammarnode'
    /*
      for(list<GrammarString*>::iterator stringListIterator = includeList.begin();
      stringListIterator != includeList.end(); // ", " only between the elements of the list
      stringListIterator++) {
      if ( !(*stringListIterator)->getToBeTraversed()) {
      if(s3!="") // before each element but not the first one
      s3+=",\n";
      char* type =new char[300];type[0]='\0';
      (void) strcat( type, (*stringListIterator)->getTypeNameString());
    type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
    type=GrammarString::copyEdit (type,"$GRAMMAR_X_MARKER_","");
    type=GrammarString::copyEdit (type,"*","");
    s3+=string("          ")+type+string(" "); //MS: NO VARNAME: +string((*stringListIterator)->getVariableNameString());
    }
    }
    
    if(false) {
    saLatex.nodetext=string(grammarnode->getName())+grammarSymTreeLB+s2+grammarSymTreeRB//+"\n"
    //+"{\n" s3 + "\n}\n"
    ;
    }
    else {
      */
    if(isAbstractTreeGrammarSymbol(string(grammarnode->getName())) ) {
      saLatex.nodetext=string(grammarnode->getName())+" "+grammarSymTreeLB+rhsTerminalSuccessors+grammarSymTreeRB;
      saLatex.terminalname=string(grammarnode->getName());
      saLatex.isTerminal=true;
    } else {
      saLatex.nodetext="";
    }
  } // end of leaf node handling
  else {
    if(isAbstractTreeGrammarSymbol(string(grammarnode->getName())) ) {
      saLatex.nodetext=grammarnode->getName();
      saLatex.terminalname="";
      saLatex.isTerminal=false;
    } else {
      saLatex.nodetext="";
    }
  }
  // create grammar rule for current grammar node and its successors
  string grammarRule;
  bool first=true;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    if((*viter).nodetext!="" && isAbstractTreeGrammarSymbol(string(grammarnode->getName())) ) {
      if(generateSDFTreeGrammar) {
        if((*viter).isTerminal) {
          // SDF: generate two rules for terminals: A->B; B->B(...);
          grammarRule+=string(grammarnode->getName()) + " -> " + (*viter).terminalname+"\n";
          grammarRule+=string((*viter).terminalname) + " -> " + (*viter).nodetext+"\n";
        } else {
          grammarRule+=string(grammarnode->getName()) + " -> " + (*viter).nodetext+"\n";
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
  saLatex.grammarnode=grammarnode;
  saLatex.text=grammarRule;
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    saLatex.text+=(*viter).text;
  }
  
  // create problematic node info
  GrammarNodeInfo gInfo=getGrammarNodeInfo(grammarnode); // MS: should be a member function of GrammarNode
  if(gInfo.numSingleDataMembers>0 && gInfo.numContainerMembers>0) 
    saLatex.problematicnodes+=string(grammarnode->getName())+"\n";
  
  // ------------------------------------------------------------
  // create terminal and nonterminal (and problematic node) lists 
  // ------------------------------------------------------------

  // create terminal or non-terminal entry
  if(grammarnode->isLeafNode()) {
    saLatex.terminalsbunch+=string(grammarnode->getName())+"\n";
  } else {
    saLatex.nonterminalsbunch+=string(grammarnode->getName())+"\n";
  }
  // union non-terminal, terminal, and problematic nodes data of subtree nodes
  for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {
    // union subtrees
    saLatex.nonterminalsbunch+=(*viter).nonterminalsbunch;
    saLatex.terminalsbunch+=(*viter).terminalsbunch;
    saLatex.problematicnodes+=(*viter).problematicnodes;
  }
  return saLatex;
}

void Grammar::buildGrammarDotFile(Terminal* rootNode, ostream& GrammarDotFile) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::CreateGrammarDotString);
  GrammarDotFile << "digraph G {\n";
  GrammarDotFile << a.text;
  GrammarDotFile << "\n}" << endl;
}

// MS:2002,2014
void Grammar::buildAbstractTreeGrammarFile(Terminal* rootNode, ostream& AbstractTreeGrammarFile) {
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
void Grammar::buildSDFTreeGrammarFile(Terminal* rootNode, ostream& SDFTreeGrammarFile) {
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
