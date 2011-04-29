// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"

using namespace std;

#define BOOL2STR(b) ((b) ? "true" : "false")

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################

SubclassListBuilder& SubclassListBuilder::operator|(const Terminal& t) {
  ROSE_ASSERT (&t);
  children.push_back(const_cast<Terminal*>(&t));
  return *this;
}

const std::vector<Terminal*>& SubclassListBuilder::getList() const {
  return children;
}

Terminal::~Terminal()
   {
   }

Terminal::Terminal ( const string& lexemeString , Grammar & X , const string& stringVar, const string& tagString, bool canHaveInstances, const SubclassListBuilder & builder )
   : name((stringVar.empty() ? lexemeString : stringVar)),
     baseName((stringVar.empty() ? lexemeString : stringVar)),
     baseClass(NULL),
     subclasses(builder.getList()),
     canHaveInstances(canHaveInstances),
     lexeme(lexemeString),
     tag(tagString.empty() ? (name + "Tag") : tagString),
     lowerLevelGramaticalElement(NULL), 
     buildDefaultConstructor(false),
     predeclarationString(""), postdeclarationString(""), 
     automaticGenerationOfDestructor(true),
     automaticGenerationOfConstructor(true),
     automaticGenerationOfDataAccessFunctions(true),
     automaticGenerationOfCopyFunction(true),
     associatedGrammar(&X)
   {
     for (size_t i = 0; i < subclasses.size(); ++i) {
       ROSE_ASSERT (subclasses[i]->getBaseClass() == NULL);
       ROSE_ASSERT (subclasses[i]);
       subclasses[i]->setBaseClass(this);
     }
     X.addGrammarElement(*this);
     ROSE_ASSERT(associatedGrammar != NULL);
   }

void
Terminal::setBaseClass(Terminal* bc) {baseClass = bc;}

Terminal*
Terminal::getBaseClass() const {return baseClass;}

void
Terminal::setCanHaveInstances(bool chi) {canHaveInstances = chi;}

bool
Terminal::getCanHaveInstances() const {return canHaveInstances;}

void
Terminal::setBuildDefaultConstructor ( bool X )
   {
     buildDefaultConstructor = X;
   }

bool
Terminal::getBuildDefaultConstructor () const
   {
     ROSE_ASSERT (buildDefaultConstructor == true || buildDefaultConstructor == false);
     return buildDefaultConstructor;
   }

// AJ ( 10/26/2004)
// 
string
Terminal::buildDestructorBody ()
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

     returnString += "\n";

     for( stringListIterator = localList.begin();
          stringListIterator != localList.end();
          stringListIterator++ )
        {
          if (!((*stringListIterator)->getToBeDeleted() == DEF_DELETE))
             continue;

       // string tempString        = "\n     delete p_$DATA;\n     p_$DATA = NULL;\n";
          string tempString        = "     delete p_$DATA;\n";

       // DQ (9/5/2006): Handle case of "char*" to use "delete []"
          string typeName     = (*stringListIterator)->getTypeNameString();
          bool typeIsCharString = typeName.find("char*") != string::npos && typeName.find("char**") == string::npos;
          if ( typeIsCharString )
               tempString = "     delete [] p_$DATA;\n";

          string variableNameString = (*stringListIterator)->getVariableNameString();

          tempString = StringUtility::copyEdit (tempString,"$DATA",variableNameString);

          returnString += tempString;
        }

     returnString += "\n";

  // bool exitAsTest = false;

  // Now generate code to reset the pointers to default values.
     for( stringListIterator = localList.begin();
          stringListIterator != localList.end();
          stringListIterator++ )
        {
       // Skip any staticly defined data
          string typeString = (*stringListIterator)->getTypeNameString();
          string variableNameString = (*stringListIterator)->getVariableNameString();
          string initializerString  = (*stringListIterator)->getDefaultInitializerString();
       // DQ (5/24/2006): The SgNode::operator delete() will set the p_freepointer so don't fool with it in the descructor!
          if (typeString.find("static ") == string::npos && (initializerString.empty() == false) && (variableNameString != "freepointer") )
             {
            // string tempString = "     p_$DATA $DEFAULT_VALUE;\n";
               string tempString = (*stringListIterator)->buildDestructorSource();

               tempString = StringUtility::copyEdit (tempString,"$DATA",variableNameString);
               tempString = StringUtility::copyEdit (tempString,"$DEFAULT_VALUE",initializerString);

               returnString += tempString;
             }
        }

     ROSE_ASSERT (localExcludeList.size() == 0);

  // printf ("In Terminal::buildDestructorBody(): returnString = %s \n",returnString.c_str());
     return returnString;
   }

string
Terminal::buildConstructorBody ( bool withInitializers, ConstructParamEnum config )
   {
  // This function builds a string that represents the initialization of member data
  // if the default initializers are used (for default constructors) then all member 
  // data is initialized using the initializer (all member data should be defined 
  // with an initializer string).  If withInitializers == false (for non-default 
  // constructors) then all member data is initialized with the name used for the 
  // for the associated constructor parameter.  Excluded data members are initialized
  // with their default initializer.

     string returnString;

     vector<GrammarString *> localList;
     vector<GrammarString *> localExcludeList;
     vector<GrammarString *>::iterator stringListIterator;

  // DQ (12/7/2003): This is never true, or so it seems
     ROSE_ASSERT (withInitializers == false);

  // Initialize with local node data
     localList        = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     localExcludeList = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);

  // Now edit the list to remove elements appearing within the exclude list
     Grammar::editStringList ( localList, localExcludeList );

     string prevParam;
     for( stringListIterator = localList.begin();
          stringListIterator != localList.end();
          stringListIterator++ )
        {
       // QY 11/9/04  added additional types of constructor parameters 
          string variableNameString = (*stringListIterator)->getVariableNameString();
          switch ( (*stringListIterator)->getIsInConstructorParameterList().getValue() )
             {
               case TAG_NO_CONSTRUCTOR_PARAMETER:
                 // DQ (11/20/2004): This test does not appear to work to skip cases where the initializer is empty
                 // the reason is that getDefaultInitializerString() returns a char* and the wrong operator!= is being used!
                 // if ((*stringListIterator)->getDefaultInitializerString() != "")
                    if (string( (*stringListIterator)->getDefaultInitializerString()) != "")
                       {
                         returnString = returnString + "     p_" + variableNameString + " " + 
                                        (*stringListIterator)->getDefaultInitializerString() + ";\n";
                       }
                    break;

               case TAG_CONSTRUCTOR_PARAMETER:
                    returnString = returnString + "     p_" + variableNameString+ " = " + variableNameString + ";\n";
                    break;
               default:
                    assert(false);     
             }
       }

  // printf ("In buildConstructorBody() localList: returnString = %s \n",returnString.c_str());
     ROSE_ASSERT (localExcludeList.size() == 0);

  // printf ("Leaving buildConstructorBody() \n");

     return returnString;
   }

StringUtility::FileWithLineNumbers Terminal::buildCopyMemberFunctionHeader ()
   {
  // DQ (3/25/3006): I put this back in because it had the logic for where the copy function required 
  // and not required which is required to match the other aspects of the copy mechanism code generation.
  // DQ (3/24/2006): This function declaration is now generated similar to the rest in Common.code
  // printf ("This function should no longer be called \n");
  // ROSE_ASSERT(false);

     StringUtility::FileWithLineNumbers returnString;
     if (automaticGenerationOfCopyFunction)
        {
       // DQ (10/13/2007): Make this the function prototype for the copy mechanism.
       // This also fixes a bug where the code above was causing more than just the 
       // prototype to be output in the Cxx_Grammar.h header file.
          returnString.push_back(StringUtility::StringWithLineNumber("          virtual SgNode* copy ( SgCopyHelp& help) const;", "" /* "<copy member function>" */, 1));
        }
       else
        {
       // DQ (10/13/2007): Not clear when or why but this applies to only the SgSymbol IR node!
          returnString.push_back(StringUtility::StringWithLineNumber("       // copy functions omitted for $CLASSNAME", "" /* "<copy member function>" */, 1));
        }

  // printf ("In Terminal::buildCopyMemberFunctionHeader(): returnString = %s \n",returnString);

     return returnString;
   }

StringUtility::FileWithLineNumbers Terminal::buildCopyMemberFunctionSource ()
   {
  // This function builds the copy member function's body

  // printf ("In Terminal::buildCopyMemberFunctionSource(): class name = %s \n",name);

     StringUtility::FileWithLineNumbers returnString;
     if (automaticGenerationOfCopyFunction == true)
        {
          string constructArgCopy = "", constructArgList = "", postConstructCopy = "";
          string filename     = "../Grammar/copyMemberFunction.macro";
          StringUtility::FileWithLineNumbers functionTemplateString = Grammar::readFileWithPos (filename);
          bool emptyConstructorArg = (!generateConstructor()) || getBuildDefaultConstructor ();

       // printf ("Derived Class terminal name = %s \n",name);

       // DQ (9/28/2005): We can't enforce this, but perhaps it is a good goal for ROSETTA.
          ROSE_ASSERT(getBuildDefaultConstructor() == false);

       // DQ (9/28/2005): I think this should be a while loop and not a for 
       // loop since the terminatation is not easily determined statically.
          for (Terminal *t = this; t != NULL; t = t->getBaseClass())
             {
               string constructArgCopy1 = "", constructArgList1 = "", postConstructCopy1 = "";
               vector<GrammarString *> copyList;
               vector<GrammarString *>::iterator stringListIterator;

            // printf ("Possible base class terminal name = %s \n",(*t).name);

               copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);

               for( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
                  {
                    GrammarString *data = *stringListIterator;
                    if (data->getToBeCopied() == NO_COPY_DATA)
                       {
                      // printf ("Skipping the generation of code to copy this data member (class = %s, data member = %s) \n",name,data->variableNameString);
                         continue;
                       }

                 // Have all the code generation be in the lower level function (not mixed up)
                    string varNameString = string(data->getVariableNameString()) + "_copy";
                    string varDecl = "     " + string(data->getTypeNameString()) + " " + varNameString + "; \n";

                    if (!emptyConstructorArg && data->getIsInConstructorParameterList() != NO_CONSTRUCTOR_PARAMETER)
                       {
                      // DQ (9/24/2005): Added comments to generated code!
                         string comment = string("  // Copy constructor parameter data member: ") + varNameString + string("\n");
                         constructArgCopy1 += comment;

                      // Code built generated to build constructor parameters can't set the "result->xxx" 
                      // data members since the "result" class have not been built (see generated code 
                      // for more examples).
                         bool buildConstructorArgument = true;
                         constructArgCopy1 += data->buildCopyMemberFunctionSource( buildConstructorArgument );
                      // constructArgList1 = constructArgList1 + ", " + varNameString;

                         constructArgList1 += ", " + varNameString;

                         string setParentString = data->buildCopyMemberFunctionSetParentSource(varNameString);
                         postConstructCopy += setParentString;
                       }
                      else
                       {
                      // Code built generated to build constructor parameters can't set the "result->xxx" 
                      // data members since the "result" class have not been built (see generated code 
                      // for more examples).
                         bool buildConstructorArgument = false;

                         switch (data->automaticGenerationOfDataAccessFunctions.getValue())
                            {
                              case TAG_NO_ACCESS_FUNCTIONS:
                                 {
                                   string localVarNameString = "result->p_" + string(data->getVariableNameString());
                                   varNameString = localVarNameString;

                                // DQ (9/24/2005): Added comments to generated code!
                                   string comment = string("  // Copy non-constructor parameter data member (no access function): ") + varNameString + string("\n");
                                   postConstructCopy1 += comment;

                                // postConstructCopy1 += varDecl;
                                   postConstructCopy1 += data->buildCopyMemberFunctionSource(buildConstructorArgument);
#if 1
                                // string setParentString = "     " + string(data->getVariableNameString()) + "_copy->set_parent(result);\n";
                                   string setParentString = data->buildCopyMemberFunctionSetParentSource(localVarNameString);
                                   postConstructCopy1 += setParentString;
#endif
                                   break;
                                 }

                              case TAG_BUILD_LIST_ACCESS_FUNCTIONS:
                                 {
                                   string localVarNameString = "result->get_" + string(data->getVariableNameString()) + "()";
                                   varNameString = localVarNameString;

                                // DQ (9/24/2005): Added comments to generated code!
                                   string comment = string("  // Copy non-constructor parameter data member (list access function): ") + varNameString + string("\n");
                                   postConstructCopy1 += comment;

                                   postConstructCopy1 += data->buildCopyMemberFunctionSource(buildConstructorArgument);
#if 1
                                // string setParentString = "     " + string(data->getVariableNameString()) + "_copy.set_parent(result);\n";
                                   string setParentString = data->buildCopyMemberFunctionSetParentSource(localVarNameString);
                                   postConstructCopy1 += setParentString;
#endif
                                   break;
                                 }

                              case TAG_BUILD_ACCESS_FUNCTIONS:
                              case TAG_BUILD_FLAG_ACCESS_FUNCTIONS:
                                 {

                                // DQ (9/24/2005): Added comments to generated code!
                                   string comment = string("  // Copy non-constructor parameter data member (access function): ") + varNameString + string("\n");
                                   postConstructCopy1 += comment;

                                // postConstructCopy1 += varDecl;
                                // DQ (10/22/2005): Simpler code
                                   postConstructCopy1 += data->buildCopyMemberFunctionSource(buildConstructorArgument);
                                // string setParentString = "     " + string(data->getVariableNameString()) + "_copy.set_parent(result);\n";
                                   string setParentString = data->buildCopyMemberFunctionSetParentSource(varNameString);
                                // postConstructCopy = setParentString + postConstructCopy;
                                   postConstructCopy1 += setParentString;
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Default reached in Terminal::buildCopyMemberFunctionSource \n");
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }
                       }

               constructArgCopy  = constructArgCopy1  + constructArgCopy;
               constructArgList  = constructArgList1  + constructArgList;
               postConstructCopy = postConstructCopy1 + postConstructCopy;
             }

       // Not fond of this approach to fixing up the leading "," in what is generated above.
          if (constructArgList != "")
             {
               constructArgList = constructArgList.c_str() + 1; // get rid of the preceding ','
             }

       // DQ (10/23/2007): This function should not be called, the SageInterface::rebuildSymbolTable() function is called by the AST fixupCopy member functions.
       // postConstructCopy += generateCodeForSymbolTableFixup();

       // Put this string on the heap so it can be managed by copyEdit()
          returnString = functionTemplateString;
       // DQ (9/28/2005): Migrating to the use of string class everywhere in ROSETTA
          returnString = StringUtility::copyEdit (returnString,"virtual ", "");
          returnString = StringUtility::copyEdit (returnString,"$CONSTRUCT_ARG_COPY",constructArgCopy);
          returnString = StringUtility::copyEdit (returnString,"$CONSTRUCT_ARG_LIST",constructArgList);
          returnString = StringUtility::copyEdit (returnString,"$POST_CONSTRUCT_COPY",postConstructCopy);
        }

     return returnString;
   }


void Terminal::setConnectionToLowerLevelGrammar ( Terminal & X )
   {
     lowerLevelGramaticalElement = &X;
     ROSE_ASSERT (lowerLevelGramaticalElement != NULL);
   }

Terminal & Terminal::getConnectionToLowerLevelGrammar ()
   {
     ROSE_ASSERT (lowerLevelGramaticalElement != NULL);
     return *lowerLevelGramaticalElement;
   }

void
Terminal::show(size_t indent) const
   {
     ROSE_ASSERT (this != NULL);
     if (subclasses.empty()) { // Terminal
       printf ("%s ",name.c_str());
     } else { // Nonterminal
       for (size_t i=0; i < indent; i++) {
         printf ("..");
       }
       for(vector<Terminal*>::const_iterator terminalIterator =
             subclasses.begin(); 
           terminalIterator != subclasses.end(); 
           terminalIterator++) {
         ROSE_ASSERT((*terminalIterator)!=NULL);
         displayName(indent); cout << " -> "; (*terminalIterator)->Terminal::show(); cout << ";" << endl; //MS edge
         if ((*terminalIterator)->getCanHaveInstances()) {
           (*terminalIterator)->show();
           cout << "[style=bold];" << endl; // Terminals are bold! MS node
         }
       }
     }
   }

void
Terminal::displayName ( int indent ) const
   {
     ROSE_ASSERT (this != NULL);
     printf ("%s ",name.c_str());
   }

void 
Terminal::setLexeme ( const string& label )
   {
     this->lexeme = label;
   }

const string&
Terminal::getLexeme () const
   {
     return lexeme;
   }

void 
Terminal::setName ( const string& label, const string& tagName )
   {
  // baseName is the same as "name" but name will be modified later to 
  // include the $GRAMMAR_PREFIX_ we need to keep the base around so that 
  // the $PARANT_GRAMMARS_BASE_ can be applied in connections to lower 
  // level grammars (e.g. parser's input parameters are elements of the 
  // parent's grammar.

     ROSE_ASSERT (this != NULL);

  // printf ("In Terminal::setName ( label = %s tagName = %s ) \n",
  //      (label == NULL) ? "NULL" : label,(tagName == NULL) ? "NULL" : tagName);

     this->baseName = this->name = label;

  // set the lexeme to match the name
     setLexeme(name);

  // Setup the tag name using the name if the second parameter is NULL
     if (tagName != "")
          this->tag = tagName;
       else
          this->tag = name + "Tag";
   }

void
Terminal::setGrammar ( Grammar* grammarPointer )
   {
     ROSE_ASSERT (this != NULL);
     ROSE_ASSERT (grammarPointer != NULL);
     ((Terminal*)this)->associatedGrammar = grammarPointer;
   }

Grammar*
Terminal::getGrammar() const
   {
     ROSE_ASSERT (this != NULL);
     ROSE_ASSERT (associatedGrammar != NULL);
     return associatedGrammar;
   }

const string&
Terminal::getName () const
   {
     ROSE_ASSERT (this != NULL);
     ROSE_ASSERT (!name.empty());
#if 0
     printf ("In Terminal::getName(): name = %s \n",name);
#endif
     return name;
   }

const string&
Terminal::getBaseName () const
   {
     ROSE_ASSERT (this != NULL);
     return baseName;
   }


void
Terminal::addGrammarPrefixToName()
   {
  // In this function we prefix the grammar's name to the names of all terminals and nonterminals
  // This allows them to have unique names in the global namespace (allowing us to have multiple
  // grammars in use (each built from the same base level C++ grammar (using the new version of SAGE).
  // However we don't want to modify the names of the tags unless they are a part of a higher level 
  // grammar.  This is done to preserve the names already used in SAGE (and in the EDG/SAGE interface 
  // code) which we don't want to change.

  // To match the names used in SAGE the base level grammar will use "Sg" as its name

     string grammarName = "";
     ROSE_ASSERT (getGrammar() != NULL);
     grammarName = getGrammar()->getGrammarPrefixName();

  // printf ("In Terminal::addGrammarPrefixToName: grammarName = %s \n",grammarName);

  // Error Checking! Check to make sure that grammar's name does not already exist in the
  // terminal's name.  This helps make sure that elements are not represented twice!
     ROSE_ASSERT (GrammarString::isContainedIn(name,grammarName) == false);

  // Set the name to include the grammar's prefix
  // Modify this statement to avoid Insure++ warning
  // ((Terminal*)this)->name = stringConcatinate( GrammarString::stringDuplicate(grammarName), name );
     string newNameWithPrefix = grammarName + name;
     this->name = newNameWithPrefix;

  // Set the name to include the grammar's prefix

  // printf ("In Terminal::addGrammarPrefixToName() tag = %s \n",tag);
  // ROSE_ASSERT (getGrammar()->parentGrammar != NULL);

     string grammarTagName = "";
     if (getGrammar()->parentGrammar != NULL)
          grammarTagName = grammarName;

  // Modify this statement to avoid Insure++ warning
  // ((Terminal*)this)->tag  = stringConcatinate( GrammarString::stringDuplicate(grammarTagName), tag  );
  // char* tempTag = stringConcatinate( GrammarString::stringDuplicate(grammarTagName), tag  );
     string tempTag = grammarTagName + tag;
     this->tag  = tempTag;

  // display("Inside of addGrammarPrefixToName");
   }

void 
Terminal::setTagName ( const string& label )
   {
  // printf ("In Terminal::setTagName ( label = %s ) \n",
  //        (label == NULL) ? "NULL" : label);

     this->tag = label;
  // setLexeme(label);

  // Prepend the grammar's name to the tag so that multiple 
  // grammars will have unique tags
  // ((Terminal*)this)->tag = stringConcatinate(getGrammar()->getGrammarName(),tag);
     string grammarName = "";

     this->tag = grammarName + tag;
   }

const string&
Terminal::getTagName () const
   {
     return tag;
   }

void
Terminal::setFunctionPrototype ( const GrammarString & inputMemberFunction )
   {
     Terminal::addElementToList
        ( getMemberFunctionPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST),
          inputMemberFunction);
   }

#define SETUP_MARKER_STRINGS_MACRO   \
     string startSuffix = "_START";   \
     string endSuffix   = "_END";     \
     string startMarkerString = markerString + startSuffix; \
     string endMarkerString   = markerString + endSuffix; \
     string directory = "";                    \
     string functionString = StringUtility::toString(Grammar::extractStringFromFile ( startMarkerString, endMarkerString, filename, directory )); \
     GrammarString* codeString = new GrammarString(functionString);                  \
     codeString->setVirtual(pureVirtual);

void
Terminal::setFunctionPrototype ( const string& markerString, const string& filename, bool pureVirtual )
   {
  // We might want to include the path name into the filename string
  // so we don't need to have a directory input parameter
     SETUP_MARKER_STRINGS_MACRO
       // BP : 10/25/2001, fixing memory leak
     setFunctionPrototype (*codeString);
   }

void
Terminal::setFunctionSource ( const string& markerString, const string& filename, bool pureVirtual )
   {
  // We might want to include the path name into the filename string
  // so we don't need to have a directory input parameter
     SETUP_MARKER_STRINGS_MACRO
       // BP : 10/25/2001, fixing memory leak
  // memberFunctionSourceList.addElement(*codeString);
     Terminal::addElementToList ( getMemberFunctionSourceList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST),
                                           *codeString );
   }

void
Terminal::setSubTreeFunctionPrototype ( const GrammarString & inputMemberFunction )
   {
  // When we add a string of code to the subtree we have to also add it explicitly to 
  // the root of the subtree (calling "setFunction" takes care of this).
  // setFunction (inputMemberFunction);
  // This function must be run ONLY after the grammar tree is built!
  // ROSE_ASSERT (grammarSubTree != NULL);
  // subTreeMemberFunctionPrototypeList.addElement(inputMemberFunction);
     Terminal::addElementToList ( getMemberFunctionPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST),
                                           inputMemberFunction );
     ROSE_ASSERT (getMemberFunctionPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() > 0);
   }

void
Terminal::setSubTreeFunctionPrototype ( const string& markerString, const string& filename, bool pureVirtual )
   {
  // We might want to include the path name into the filename string
  // so we don't need to have a directory input parameter
     SETUP_MARKER_STRINGS_MACRO
     setSubTreeFunctionPrototype (*codeString);
   }

void
Terminal::setSubTreeFunctionSource ( const string& markerString, const string& filename, bool pureVirtual )
   {
  // We might want to include the path name into the filename string
  // so we don't need to have a directory input parameter
     SETUP_MARKER_STRINGS_MACRO
  // subTreeMemberFunctionSourceList.addElement(*codeString);
     Terminal::addElementToList ( getMemberFunctionSourceList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST),
                                           *codeString );
   }


void
Terminal::setPredeclarationString  ( const string& markerString, const string& filename )
   {
     bool pureVirtual;     
     SETUP_MARKER_STRINGS_MACRO
       
     setPredeclarationString(functionString);
   }

void
Terminal::setPostdeclarationString  ( const string& markerString, const string& filename )
   {
     bool pureVirtual;     
     SETUP_MARKER_STRINGS_MACRO
     setPostdeclarationString(functionString);
   }

void
Terminal::setPredeclarationString  ( const string& declarationString )
   {
  // This function allows the specification of declarations 
  // that are to appear prefixed to a class declaration
     predeclarationString = declarationString;
   }

void
Terminal::setPostdeclarationString ( const string& declarationString )
   {
  // This function allows the specification of declarations 
  // that are to appear postfixed to a class declaration
     postdeclarationString = declarationString;
   }

const string&
Terminal::getPredeclarationString () const
   {
     return predeclarationString;
   }

const string&
Terminal::getPostdeclarationString() const
   {
     return postdeclarationString;
   }

void
Terminal::setDataPrototype ( const GrammarString & inputMemberData)
{
  // MK: This is the correct place to put the memberData info
  Terminal::addElementToList (getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST),
                                       inputMemberData );
  
  // Once the string for the class declaration is built we have to 
  // construct one for the data source code list.  This will be a 
  // string representing the access functions (get and set functions)
  // and will be placed into the buildGrammarStringForSourceList (inputMemberData);
  
  bool pureVirtual = false;
  
  string accessFunctionString = buildDataAccessFunctions (inputMemberData);
  GrammarString* sourceCodeString = new GrammarString(accessFunctionString);
  ROSE_ASSERT(sourceCodeString != NULL);
  sourceCodeString->setVirtual(pureVirtual);

  vector<GrammarString*>& l = getMemberFunctionSourceList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
  // Data access "functions" should be placed into the LOCAL_LIST since
  // they are accessable though the base classes by definition (of C++)
  Terminal::addElementToList ( l, *sourceCodeString );
  
}

string
Terminal::buildDataAccessFunctions ( const GrammarString & inputMemberData)
   {
  // This function builds the access functions that will be used with the 
  // associated data declared on this terminal (or non-terminal).

     string returnString = "";
     string filename = "";
     BuildAccessEnum config = inputMemberData.generateDataAccessFunctions();
  // NOTE: Here we check and see if we really want to use this Data Access String
     if ( (automaticGenerationOfDataAccessFunctions == false) || config == NO_ACCESS_FUNCTIONS )
        {
          return "\n";
        }
     switch (config.getValue())
        {
          case TAG_BUILD_ACCESS_FUNCTIONS:
               filename = "../Grammar/dataMemberAccessFunctions.macro";
               break;
          case TAG_BUILD_FLAG_ACCESS_FUNCTIONS:
               filename = "../Grammar/flagMemberAccessFunctions.macro";
               break;
          case TAG_BUILD_LIST_ACCESS_FUNCTIONS:
               filename = "../Grammar/listMemberAccessFunctions.macro";
               break;
          default:
               assert(false);
        }

     string markerString = "DATA_ACCESS_FUNCTION";

     bool pureVirtual = false;
     SETUP_MARKER_STRINGS_MACRO

  // We can't edit anything that would be tree position dependent be 
  // we need to get the correct variable names into place so do just 
  // that editing here!
     string typeName     = inputMemberData.getTypeNameString();
     string variableName = inputMemberData.getVariableNameString();

  // DQ (12/20/2005): strip the "static " substring from the typeName
  // so that we generate non-static member access functions and non-static 
  // parameter variable types (which are not legal C++).
     if (typeName.find("static ") != string::npos )
        {
          typeName = typeName.substr(7 /* strlen("static ") */);
        }

     functionString = GrammarString::copyEdit (functionString,"$DATA_TYPE",typeName);
     functionString = GrammarString::copyEdit (functionString,"$DATA",variableName);

     string setParentFunctionCallString = "";  // default setting

  // Skip generation of call to set_parent() member function
  // Check if the type name contains a '*' indicating that it is a pointer 
  // (then assume it is a pointer to a class which we will further assume 
  // has a specific interface which includes a 
  // "set_parent ( const CLASSNAME* thisPointer )" member function
  // QY: generate set_parent only when the data member is in traversal
   //  if ( toBeTraversed && strstr(typeName,"*") != NULL )
     if ( typeName.find('*') != string::npos )
        {
       // printf ("found a pointer type (type = %s) \n",typeName);

       // Now check that is it not something simple like "char*" since char 
       // (as a primative type) would not be a class with an interface.
       // We assume that the we don't have two "*" represented as "* *".
          if ( (typeName.find("char") != string::npos) || (typeName.find("**") != string::npos) )
             {
            // printf ("opps!, it is a (type = %s) \n",typeName);
             }
            else
             {
            // Use the older way of handling this (sometimes causes parents to be types (but for a class referenced in a typedef as in 
            //      typedef int (doubleArray::*doubleArrayMemberVoidFunctionPointerType) (void) const;
            // This seems to be OK, as long as the class declaration was a copy and not the original defining declaration
               string tempString = "\n#if DEBUG_SAGE_ACCESS_FUNCTIONS\n     if (p_$DATA != NULL && $DATA != NULL && p_$DATA != $DATA)\n        {\n          printf (\"Warning: $DATA = %p overwriting valid pointer p_$DATA = %p \\n\",$DATA,p_$DATA);\n#if DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION\n          printf (\"Error fails assertion (p_$DATA != NULL && $DATA != NULL && p_$DATA != $DATA) is false\\n\");\n          ROSE_ASSERT(false);\n#endif\n        }\n#endif";
               tempString = GrammarString::copyEdit (tempString,"$DATA",variableName);
               setParentFunctionCallString = tempString;
             }
        }

  // functionString = GrammarString::copyEdit (functionString,"$SET_PARENT_FUNCTION",setParentFunctionCallString);
     functionString = GrammarString::copyEdit (functionString,"$TEST_DATA_POINTER",setParentFunctionCallString);

#if 0
  // DQ (8/9/2008): Debugging output of access function for case of BUILD_LIST_ACCESS_FUNCTIONS
     if (config.getValue() == TAG_BUILD_LIST_ACCESS_FUNCTIONS)
          printf ("functionString = %s \n",functionString.c_str());
#endif

  // BP : 10/18/2001, delete unwanted memory
     delete codeString;
     return functionString;
   }

void
Terminal::setDataPrototype (
     const string& inputTypeNameString,
     const string& inputVariableNameString,
     const string& inputDefaultInitializer,
     const ConstructParamEnum& constructorParameter,
     const BuildAccessEnum& buildAccessDataFunctions,
     const TraversalFlag& toBeTraversedDuringTreeTraversal,
     const DeleteFlag& delete_flag,
     const CopyConfigEnum& toBeCopied)
   {
     GrammarString *temp = 
          new GrammarString (inputTypeNameString,
                             inputVariableNameString, 
                             inputDefaultInitializer,
                             constructorParameter,
                             buildAccessDataFunctions,
                             toBeTraversedDuringTreeTraversal,
                             delete_flag,
                             toBeCopied);
     ROSE_ASSERT(temp != NULL);
     setDataPrototype (*temp);
   }

// Mechanism for excluding code from specific node or subtrees
void
Terminal::excludeFunctionPrototype ( const GrammarString & inputMemberFunction )
   {
     Terminal::addElementToList
        ( getMemberFunctionPrototypeList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST),inputMemberFunction);
   }

void 
Terminal::excludeFunctionPrototype
   ( const string& markerString, const string& filename, bool pureVirtual )
   {
     SETUP_MARKER_STRINGS_MACRO
     Terminal::addElementToList (
          getMemberFunctionPrototypeList (Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST), *codeString);
   }

void
Terminal::excludeSubTreeFunctionPrototype ( const GrammarString & inputMemberFunction )
   {
     Terminal::addElementToList ( 
          getMemberFunctionPrototypeList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST),
          inputMemberFunction);
   }

void 
Terminal::excludeSubTreeFunctionPrototype 
   ( const string& markerString, const string& filename, bool pureVirtual )
   {
     SETUP_MARKER_STRINGS_MACRO
     Terminal::addElementToList(
          getMemberFunctionPrototypeList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST),
          *codeString);
   }

void
Terminal::excludeSubTreeDataPrototype ( const GrammarString & inputMemberData )
{
  // Note that the exclusion of data works slightly differently than for function prototypes
  Terminal::addElementToList(getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST), 
                                      inputMemberData);
}

void
Terminal::excludeSubTreeDataPrototype (
     const string& inputTypeNameString, 
     const string& inputVariableNameString, 
     const string& inputDefaultInitializer )
   {
     GrammarString *temp = 
          new GrammarString (inputTypeNameString,
                             inputVariableNameString, 
                             inputDefaultInitializer,
                             CONSTRUCTOR_PARAMETER,
                             BUILD_ACCESS_FUNCTIONS,
                             DEF_TRAVERSAL, NO_DELETE, NO_COPY_DATA);
     excludeSubTreeDataPrototype (*temp);
   }

void 
Terminal::excludeFunctionSource           
   ( const string& markerString, const string& filename, bool pureVirtual )
   {
     SETUP_MARKER_STRINGS_MACRO
     Terminal::addElementToList(
          getMemberFunctionSourceList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST),
          *codeString);
   }

void 
Terminal::excludeSubTreeFunctionSource
   ( const string& markerString, const string& filename, bool pureVirtual )
   {
     SETUP_MARKER_STRINGS_MACRO
     Terminal::addElementToList(
          getMemberFunctionSourceList (Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST),
          *codeString);
   }

// test of work around for Insure++ (removing the const declaration to avoid generation of a copy)
#if !ROSE_MICROSOFT_OS
#if INSURE_BUG
void Terminal::addElementToList ( vector<GrammarString *> & targetList, GrammarString & element )
#else
void Terminal::addElementToList ( vector<GrammarString *> & targetList, const GrammarString & element )
#endif
#else
void Terminal::addElementToList ( vector<GrammarString *> & targetList, const GrammarString & element )
#endif
   {
  // This function abstracts the details of adding GrammarString objects to the lists
  // that are stored internally.  It allows us to implement tests to check for redundencies in 
  // the list of elements (for now it allows us to test the != operator which will be used heavily later)

  // printf ("Inside of Terminal::addElementToList \n");

     int i = 0;
     bool duplicateEntryFound = false;
     vector<GrammarString *>::iterator stringListIterator;
  // Error checking (make sure the element is not already in the list!)
     for( stringListIterator = targetList.begin();
          stringListIterator != targetList.end();
          stringListIterator++ )
       {
         if (*stringListIterator == &element)
           {
             printf ("Duplicate entry found in list! (targetList[%d] == element) \n",i);
             duplicateEntryFound = true;
             
             // If any of the GrammarString objects is marked to skip construction 
             // of access functions (for all the data at a node!) then mark it in the list's copy
             if (element.generateDataAccessFunctions() == NO_ACCESS_FUNCTIONS)
               (*stringListIterator)->setAutomaticGenerationOfDataAccessFunctions(NO_ACCESS_FUNCTIONS);

           }

         i++;
         // ROSE_ASSERT (targetList[i] != element);
       }

  // Since the grammar tree is not build yet we cannot search the parents
  // so this abstraction for adding elements to the lists is not used to
  // exclude any elements (this is post processed).
     if (duplicateEntryFound == false)
       {
         const GrammarString *const &tmpRef = new GrammarString(element);
         targetList.push_back( (GrammarString *const &) tmpRef );
       }
   }

vector<GrammarString *> & Terminal::getMemberFunctionPrototypeList(int i, int j) const
   {
  // printf ("Inside of Terminal::getMemberFunctionPrototypeList() \n");
     return (vector<GrammarString *> &) memberFunctionPrototypeList[i][j];
  // return memberFunctionPrototypeList[i][j];
   }

vector<GrammarString *> & Terminal::getMemberDataPrototypeList(int i, int j) const
   {
     return (vector<GrammarString *> &) memberDataPrototypeList[i][j];
   }

vector<GrammarString *> & Terminal::getMemberFunctionSourceList(int i, int j) const
   {
     return (vector<GrammarString *> &) memberFunctionSourceList[i][j];
   }

vector<GrammarString *> & Terminal::getEditSubstituteTargetList( int i, int j ) const
   {
     return (vector<GrammarString *> &) editSubstituteTargetList[i][j];
   }

vector<GrammarString *> & Terminal::getEditSubstituteSourceList( int i, int j ) const
   {
     return (vector<GrammarString *> &) editSubstituteSourceList[i][j];
   }


// This marco adds the oldString and newString to lists that are later used to
// drive the substitution mechanism.
#define EDIT_SUBSTITUTE_MACRO(X,Y) \
     GrammarString* targetString = new GrammarString (oldString);                 \
     GrammarString* sourceString = new GrammarString (newString);                 \
     getEditSubstituteTargetList(Terminal:: X ,Terminal:: Y ).push_back(targetString);  \
     getEditSubstituteSourceList(Terminal:: X ,Terminal:: Y ).push_back(sourceString);  \
     ROSE_ASSERT (getEditSubstituteTargetList(Terminal:: X,Terminal:: Y ).size() == \
                  getEditSubstituteSourceList(Terminal:: X,Terminal:: Y ).size());

  // printf ("getEditSubstituteTargetList(Terminal:: X,Terminal:: Y ).size() = %d \n",
  //          getEditSubstituteTargetList(Terminal:: X,Terminal:: Y ).size());

void
Terminal::editSubstitute ( const string& oldString, const string& newString )
   {
  // Add these to storage and use then just before writting out the final strings
  // printf ("Terminal::editSubstitute() oldString = %s   newString = %s \n",oldString,newString);

     EDIT_SUBSTITUTE_MACRO(LOCAL_LIST,INCLUDE_LIST)

  // printf ("getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST).size() = %d \n",
  //      getEditSubstituteTargetList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST).size());
   }

void
Terminal::editSubstituteSubTree        ( const string& oldString, const string& newString )
   {
  // printf ("Terminal::editSubstituteSubTree() oldString = %s   newString = %s \n",oldString,newString);
     EDIT_SUBSTITUTE_MACRO(SUBTREE_LIST,INCLUDE_LIST)
   }

void
Terminal::editSubstituteExclude        ( const string& oldString, const string& newString )
   {
     EDIT_SUBSTITUTE_MACRO(LOCAL_LIST,EXCLUDE_LIST)
   }

void
Terminal::editSubstituteExcludeSubTree ( const string& oldString, const string& newString )
   {
     EDIT_SUBSTITUTE_MACRO(SUBTREE_LIST,EXCLUDE_LIST)
   }

void
Terminal::editSubstitute ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = false;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
   }

void
Terminal::editSubstituteSubTree        ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = false;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
     
   }

void
Terminal::editSubstituteExclude        ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = false;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
   }

void
Terminal::editSubstituteExcludeSubTree ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = false;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
   }


void
Terminal::setAutomaticGenerationOfConstructor  ( bool X )
   {
  // All terminals can have there constructors and destructors 
  // automatically generated.  This function controls the automatic
  // generation of the constructor (one with the declared data input 
  // as paramteres).
     automaticGenerationOfConstructor = X;
   }

void
Terminal::setAutomaticGenerationOfDestructor   ( bool X )
   {
  // See description of setAutomaticGenerationOfConstructor(bool) above
     automaticGenerationOfDestructor = X;
   }

bool
Terminal::generateDestructor () const
   {
  // See description of setAutomaticGenerationOfConstructor(bool) above
  // return automaticGenerationOfDestructor;

     return automaticGenerationOfDestructor;
   }

bool
Terminal::generateConstructor() const
   {
  // See description of setAutomaticGenerationOfConstructor(bool) above
     return automaticGenerationOfConstructor;
   }

void
Terminal::setAutomaticGenerationOfDataAccessFunctions ( bool X )
   {
  // These functions set the construction of access functions for all member data of a terminal!
  // See description of setAutomaticGenerationOfConstructor(bool) above
     automaticGenerationOfDataAccessFunctions = X;
   }

bool
Terminal::generateDataAccessFunctions() const
   {
  // These functions set the construction of access functions for all member data of a terminal!
  // See description of setAutomaticGenerationOfConstructor(bool) above
     return automaticGenerationOfDataAccessFunctions;
   }

void
Terminal::setAutomaticGenerationOfCopyFunction ( bool X )
   {
  // These functions set the construction of access functions for all member data of a terminal!
  // See description of setAutomaticGenerationOfConstructor(bool) above
     automaticGenerationOfCopyFunction = X;
   }

bool
Terminal::generateCopyFunction() const
   {
  // These functions set the construction of access functions for all member data of a terminal!
  // See description of setAutomaticGenerationOfConstructor(bool) above
     return automaticGenerationOfCopyFunction;
   }

void
Terminal::consistencyCheck() const
   {
     ROSE_ASSERT (associatedGrammar != NULL);

#if 1
  // local index variable
     int j = 0, k = 0;

  // check the list for valid objects

     for (k=0; k < 2; k++)
          for (j=0; j < 2; j++)
             {
            // int i = 0;
               vector<GrammarString *>::const_iterator it;
               for( it = memberDataPrototypeList[j][k].begin(); 
                    it != memberDataPrototypeList[j][k].end(); it++ ) 
                  {
                    assert( *it != NULL );
                    (*it)->consistencyCheck();
                  }

               for( it = memberFunctionPrototypeList[j][k].begin(); 
                    it != memberFunctionPrototypeList[j][k].end(); it++ ) 
                  {
                    assert( *it != NULL );
                    (*it)->consistencyCheck();
                  }

               for( it = memberFunctionSourceList[j][k].begin(); 
                    it != memberFunctionSourceList[j][k].end(); it++ ) 
                  {
                    assert( *it != NULL );
                    (*it)->consistencyCheck();
                  }
               }
#endif

  // check the subclass list for valid objects
     for( vector<Terminal*>::const_iterator terminalIterator = subclasses.begin(); 
          terminalIterator != subclasses.end(); 
          terminalIterator++)
       {
         // printf ("terminalList[%d].getName() = %s \n",i,terminalList[i].getName());
         ROSE_ASSERT((*terminalIterator)!=NULL);
         (*terminalIterator)->consistencyCheck();
         ROSE_ASSERT ((*terminalIterator)->getBaseClass() == this);
        }
   }


void
Terminal::display( const string& label ) const
   {
     printf ("In Terminal::display ( %s ) \n",label.c_str());

     printf ("Name     = %s \n", getName().c_str());
     printf ("Lexeme   = %s \n", getLexeme().c_str());
     printf ("Tag Name = %s \n", getTagName().c_str());

     for( vector<Terminal*>::const_iterator terminalIterator = subclasses.begin(); 
          terminalIterator != subclasses.end(); 
          terminalIterator++)
       {
         ROSE_ASSERT((*terminalIterator)!=NULL);
         string nameInProductionRule = (*terminalIterator)->name;
         printf ("     OR:    nameInProductionRule = %s \n", nameInProductionRule.c_str());
       }

   }

// MK: This function is used to check the lists of GrammarString objects that
// are used during the generation phase of the classes of the AST restructuring tool
void
Terminal::checkListOfGrammarStrings(vector<GrammarString *>& checkList)
{
  // Check list for uniqueness and for elements of length 0
  vector<GrammarString *>::iterator it;
  vector<GrammarString *>::iterator jt;
  for( it = checkList.begin(); it != checkList.end(); it++ )
    {
      jt = it; jt++;
      for( jt = checkList.begin(); jt != checkList.end(); jt++ )
        if(*it==*jt) ROSE_ABORT();
    }      
  return;
}

// JH (10/28/2005) :
string
Terminal::buildPointerInMemoryPoolCheck ()
   {
  // DQ & JH (1/17/2006): This function generates the code for each IR node to check all its 
  // non-NULL data members pointing to IR nodes to make sure that each IR node is:
  //   1) Is a valid IR node
  //       a. contains p_freepointer equal to AST_FileIO::IS_VALID_POINTER() value 
  //          indicating that it has been allocated using the IR node's new operator
  //       b. has not be deleted using the IR node's delete operator
  //   2) Is in a block of the correct memory pool (STL vector of allocated memory 
  //      blocks for that IR node)

     vector<GrammarString *> copyList;
     vector<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
     for (Terminal *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               int length = varTypeString.size();
               if (varNameString != "freepointer" )
                  {
                    bool typeIsStarPointer = ( varTypeString.find("*") != std::string::npos) ;
                    if ( (varTypeString == "$CLASSNAME *" ) || ( ( ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) || ( varTypeString.substr(0,2) == "Sg" ) ) && typeIsStarPointer ) )
                       {
                         s += "          if ( p_" + varNameString + " != NULL )\n" ;
                         s += "             { \n" ;
                         s += "                 if ( p_" + varNameString + "->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )\n" ;
                         s += "                    { \n" ;
                         // s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                       if ( p_" + varNameString + "->isInMemoryPool() == false ) \n" ;
                         s += "                         { \n" ;
                         // s += "                             std::cout << \" p_" + varNameString + " is not in memory pool of \" << p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                             std::cout << \"" + classNameString + " :: \";\n";
                         s += "                             std::cout << \" p_" + varNameString + " is not in memory pool of \"; \n";
                         s += "                             std::cout <<    p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                         } \n" ;
                         s += "                    } \n" ;
                         s += "                  else \n" ;
                         s += "                    { \n" ;
                         s += "                       std::cout << \"" + classNameString + " :: \" << std::flush;\n" ;
                      // s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " = \" << p_" + varNameString + " << \" --> \" << std::flush;\n" ;
                         s += "                       std::cout << \" not valid \" << std::endl;\n" ;
                      // s += "                       ROSE_ASSERT(false); \n" ;
                         s += "                    } \n" ;
                         s += "             } \n" ;
                         s += "\n" ;
                       }
#if 1
                    if (  7 < length && varTypeString.substr( length-7, length) == "PtrList" )
                       {
                         s += "     " + varTypeString + "::iterator i_" + varNameString + " = p_" + varNameString + ".begin() ; \n" ;
                         s += "     for ( ; i_" + varNameString + " != p_" + varNameString + ".end(); ++i_" + varNameString + " ) \n";
                         s += "        {\n";
                         s += "          if ( (*i_" + varNameString + ") != NULL )\n" ;
                         s += "             { \n" ;
                         s += "                 if ( (*i_" + varNameString + ")->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )\n" ;
                         s += "                    { \n" ;
                         s += "                       if ( (*i_" + varNameString + ")->isInMemoryPool() == false ) \n" ;
                         s += "                         { \n" ;
                      // s += "                             std::cout << \" p_" + varNameString + " is not in memory pool of \" << p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                             std::cout << \"" + classNameString + " :: \";\n";
                         s += "                             std::cout << \" p_" + varNameString + " ( list of poitners to IR nodes ), entry is not in memory pool of \"; \n";
                         s += "                             std::cout <<    (*i_" + varNameString + ")->class_name() << std::endl;\n" ;
                         s += "                         } \n" ;
                         s += "                    } \n" ;
                         s += "                  else \n" ;
                         s += "                    { \n" ;
                         s += "                       std::cout << \"" + classNameString + " :: \" << std::flush;\n" ;
                         s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                       std::cout << \" entry not valid \" << std::endl;\n" ;
                      // s += "                       ROSE_ASSERT(false); \n" ;
                         s += "                    } \n" ;
                         s += "             } \n" ;
#if 1
                         s += "          else \n" ;
                         s += "             { \n" ;
                         s += "                 std::cout << \"" + varTypeString + " p_" + varNameString + " --> NULL \" << std::endl;\n" ;
                         s += "             } \n" ;
#endif
                         s += "        }\n";
                         s += "\n" ;
                       }
#endif
#if 1
                    if (  9 < length && varTypeString.substr( length-9, length) == "PtrVector" )
                       {
                         s += "     " + varTypeString + "::iterator i_" + varNameString + " = p_" + varNameString + ".begin() ; \n" ;
                         s += "     for ( ; i_" + varNameString + " != p_" + varNameString + ".end(); ++i_" + varNameString + " ) \n";
                         s += "        {\n";
                         s += "          if ( (*i_" + varNameString + ") != NULL )\n" ;
                         s += "             { \n" ;
                         s += "                 if ( (*i_" + varNameString + ")->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )\n" ;
                         s += "                    { \n" ;
                         s += "                       if ( (*i_" + varNameString + ")->isInMemoryPool() == false ) \n" ;
                         s += "                         { \n" ;
                      // s += "                             std::cout << \" p_" + varNameString + " is not in memory pool of \" << p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                             std::cout << \"" + classNameString + " :: \";\n";
                         s += "                             std::cout << \" p_" + varNameString + " ( list of poitners to IR nodes ), entry is not in memory pool of \"; \n";
                         s += "                             std::cout <<    (*i_" + varNameString + ")->class_name() << std::endl;\n" ;
                         s += "                         } \n" ;
                         s += "                    } \n" ;
                         s += "                  else \n" ;
                         s += "                    { \n" ;
                         s += "                       std::cout << \"" + classNameString + " :: \" << std::flush;\n" ;
                         s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                       std::cout << \" entry not valid \" << std::endl;\n" ;
                      // s += "                       ROSE_ASSERT(false); \n" ;
                         s += "                    } \n" ;
                         s += "             } \n" ;
#if 1
                         s += "          else \n" ;
                         s += "             { \n" ;
                         s += "                 std::cout << \"" + varTypeString + " p_" + varNameString + " --> NULL \" << std::endl;\n" ;
                         s += "             } \n" ;
#endif
                         s += "        }\n";
                         s += "\n" ;
                       }
#endif
#if 1
                    if (  10 < length && varTypeString.substr( length-10, length) == "PtrListPtr" )
                       {
                         std::string varTypeStringWithoutPtr = varTypeString.substr(0,varTypeString.size()-3) ;
                         s += "     if ( p_" + varNameString + " != NULL )\n" ;
                         s += "        { \n" ;
                         s += "          " + varTypeStringWithoutPtr + "::iterator i_" + varNameString + " = p_" + varNameString + "->begin() ; \n" ;
                         s += "          for ( ; i_" + varNameString + " != p_" + varNameString + "->end(); ++i_" + varNameString + " ) \n";
                         s += "             {\n";
                         s += "               if ( (*i_" + varNameString + ") != NULL )\n" ;
                         s += "                  { \n" ;
                         s += "                      if ( (*i_" + varNameString + ")->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )\n" ;
                         s += "                         { \n" ;
                         s += "                            if ( (*i_" + varNameString + ")->isInMemoryPool() == false ) \n" ;
                         s += "                              { \n" ;
                      // s += "                                  std::cout << \" p_" + varNameString + " is not in memory pool of \" << p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                                  std::cout << \"" + classNameString + " :: \";\n";
                         s += "                                  std::cout << \" p_" + varNameString + " ( list of poitners to IR nodes ), entry is not in memory pool of \"; \n";
                         s += "                                  std::cout <<    (*i_" + varNameString + ")->class_name() << std::endl;\n" ;
                         s += "                              } \n" ;
                         s += "                         } \n" ;
                         s += "                       else \n" ;
                         s += "                         { \n" ;
                         s += "                            std::cout << \"" + classNameString + " :: \" << std::flush;\n" ;
                         s += "                            std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                            std::cout << \" entry not valid \" << std::endl;\n" ;
                      // s += "                            ROSE_ASSERT(false); \n" ;
                         s += "                         } \n" ;
                         s += "                  } \n" ;
#if 1
                         s += "               else \n" ;
                         s += "                  { \n" ;
                         s += "                      std::cout << \"" + varTypeString + " p_" + varNameString + " --> NULL \" << std::endl;\n" ;
                         s += "                  } \n" ;
#endif
                         s += "             }\n";
                         s += "        } \n" ;
                         s += "\n" ;
                       }
#endif
#if 1
                    if (  varTypeString == " rose_hash_multimap*" )
                       {
                         s += "     if ( p_" + varNameString + " != NULL )\n" ;
                         s += "        { \n" ;
                         // CH (4/8/2010): Use boost::unordered instead
                         //                      s += "#ifdef _MSCx_VER \n" ;
                         //s += "          rose_hash::unordered_multimap<SgName, SgSymbol*>::iterator it; \n" ;
                         //                      s += "#else \n" ;
                         //s += "          rose_hash::unordered_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n" ;
                         s += "          rose_hash_multimap::iterator it; \n" ;
                         //                      s += "#endif \n" ;
                         s += "          for (it = p_" + varNameString + "->begin(); it != p_" + varNameString + "->end(); ++it)\n" ;
                         s += "             {\n";
                         s += "               if ( it->second != NULL )\n" ;
                         s += "                  { \n" ;
                         s += "                      if ( it->second->get_freepointer() == AST_FileIO::IS_VALID_POINTER() )\n" ;
                         s += "                         { \n" ;
                         s += "                            if ( it->second->isInMemoryPool() == false ) \n" ;
                         s += "                              { \n" ;
                      // s += "                                  std::cout << \" p_" + varNameString + " is not in memory pool of \" << p_" + varNameString + "->class_name() << std::endl;\n" ;
                         s += "                                  std::cout << \"" + classNameString + " :: \";\n";
                         s += "                                  std::cout << \" p_" + varNameString + " ( rose_hash_multimap, second entries (SgSymbol) ), entry is not in memory pool of \"; \n";
                         s += "                                  std::cout <<    it->second->class_name() << std::endl;\n" ;
                         s += "                              } \n" ;
                         s += "                         } \n" ;
                         s += "                       else \n" ;
                         s += "                         { \n" ;
                         s += "                            std::cout << \"" + classNameString + " :: \" << std::flush;\n" ;
                         s += "                            std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                         s += "                            std::cout << \" entry not valid \" << std::endl;\n" ;
                      // s += "                            ROSE_ASSERT(false); \n" ;
                         s += "                         } \n" ;
                         s += "                  } \n" ;
#if 1
                         s += "               else \n" ;
                         s += "                  { \n" ;
                         s += "                      std::cout << \"" + varTypeString + " p_" + varNameString + " --> NULL \" << std::endl;\n" ;
                         s += "                  } \n" ;
#endif
                         s += "             }\n";
                         s += "        } \n" ;
                         s += "\n" ;
                       }
#endif
                  }
             }
        }

     return s;
   }


/*************************************************************************************************
*  The function
*       Terminal::buildListIteratorString()
*  supports buildReturnDataMemberPointers() by building the string representing the code 
*  necessary to return all data member pointers to IR nodes contained in STL lists.
*************************************************************************************************/
std::string
Terminal::buildListIteratorString( string typeName, string variableName, string classNameString)
   {
        // AS(2/14/2006) Builds the strings for the list of data member pointers.
        string returnString;

        // Control variables for code generation
           bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
           bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
           bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

        // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
           bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;
           bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers;
           bool typeIsSgNode                     = typeName.find('*') != string::npos;

#if 0
           printf ("typeName                         = %s \n",typeName.c_str());
           printf ("variableName                     = %s \n",variableName.c_str());
           printf ("classNameString                  = %s \n",classNameString.c_str());
           printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers ? "true" : "false");
           printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
           printf ("typeIsPointerToList              = %s \n",typeIsPointerToList ? "true" : "false");
           printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers ? "true" : "false");
           printf ("typeIsList                       = %s \n",typeIsList ? "true" : "false");
           printf ("typeIsSgNode                     = %s \n",typeIsSgNode ? "true" : "false");
#endif 

        // One of these should be true!
           if(typeIsList!=true)
             return "";

           if ((typeIsList && typeIsSgNode) || (!typeIsList && !typeIsSgNode)) {
               cerr << (typeIsList ? "cannot be both" : "must be either")
                    <<" a list and a Sage node: "
                    <<typeName <<" " <<classNameString <<"::" <<variableName <<endl;
           }
           ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
           ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

           string listElementType = "default-error-type";


           if (typeIsList == true)
              {

             // name constant for all cases below (in this scope)
                string listElementName       = "source_list_element";

             // names that are set differently for different cases
                string iteratorBaseType;
                string needPointer;
                string originalList;
                string iteratorName;

             // Access member functions using "->" or "." (set to some string
             // that will cause an error if used, instead of empty string).
                string accessOperator = "error string for access operator";

                if (typeIsPointerToList == true)
                   {
                     if (typeIsPointerToListOfPointers == true)
                        {
                          needPointer = "*";
                          accessOperator = "->";
                        }
                       else
                        {
                          ROSE_ASSERT(typeIsPointerToListOfNonpointers == true);
                          accessOperator = ".";
                        }

                  // iteratorBaseType = string("NeedBaseType_of_") + typeName;
                     int positionOfListPtrSubstring = typeName.find("ListPtr");
                     int positionOfPtrSubstring     = typeName.find("Ptr",positionOfListPtrSubstring);
                     iteratorBaseType = typeName.substr(0,positionOfPtrSubstring);

                  // copyOfList = variableName + "_source";
                     //originalList = string("get_") + variableName + "()";
                     originalList = string("p_") +variableName;

                     iteratorName = variableName + "_iterator";

                   }
                  else
                   {

                     ROSE_ASSERT(typeIsSimpleListOfPointers == true);
                     iteratorBaseType = typeName;
                     needPointer = "*";
                     accessOperator = ".";

                  // Need to generate different code, for example:
                  //      SgStatementPtrList::const_iterator cpinit_stmt = get_init_stmt().begin();
                  // instead of:
                  //      SgStatementPtrList::const_iterator init_stmt_copy_iterator = init_stmt_copy.begin();

                     //originalList = string("get_") + variableName + "()";
                     originalList = string("p_") + variableName;

                     iteratorName = string("source_") + variableName + "_iterator";
                   }

             // Need to get the prefix substring to strings like "SgFilePtrList" (i.e. "SgFile")
                int positionOfPtrListSubstring = iteratorBaseType.find("PtrList");
                int positionOfListSubstring    = iteratorBaseType.find("Ptr",positionOfPtrListSubstring);
                listElementType = typeName.substr(0,positionOfListSubstring) + needPointer;

             // Open up the loop over the list elements
                if(accessOperator=="->"){
                    returnString += "     if(" + originalList + "==NULL)\n";
                    //Return a NULL pointer so that th graph shows that this pointer equals NULL
                    returnString += "        returnVector.push_back(pair<SgNode*,std::string>( NULL,\""+variableName+"\"));\n";

                    returnString += "     else\n";
                }
                returnString += "     for ( " +iteratorBaseType + "::const_iterator " + iteratorName + " = " + originalList + accessOperator + "begin() \n"
                         + "; " + iteratorName
                         + " != " + originalList + accessOperator + "end(); ++" 
                         + iteratorName + ") \n        { \n";

             // Declare the a loop variable (reference to current element of list)
                returnString += "          returnVector.push_back(pair<SgNode*,std::string>( *" + iteratorName + ",\""+variableName+"\"));\n";
                
            // close off the loop
                returnString += "        } \n";

              }
      return returnString;
};

//AS (021406)
/*************************************************************************************************
*  The function
*       Terminal::buildReturnDataMemberPointers()
*  builds the code for returning all data member pointers to IR nodes in the AST.
*************************************************************************************************/
string
Terminal::buildReturnDataMemberPointers ()
   {
  // AS (2/14/2006): This function generates the code for each IR node to return
  // a pair of all data members pointing to IR nodes and their name to:
  //   1) Graph the whole AST
  //   2) Create a general mechanism to do the mechanism mentioned above
  //
     vector<GrammarString *> copyList;
     vector<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s("std::vector<std::pair<SgNode*,std::string> > returnVector;\n") ;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
  // AS Iterate over the terminal and its parents (base-classes).
  // PS! Everything is treated as terminals; even non-terminals
  // printf ("Derived Class terminal name = %s \n",name);

     for (Terminal *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
       // printf ("Possible base class terminal name = %s \n",(*t).name);
       // AS Iterate over data memeber of (non-) terminal class we are looking at 
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               //AS ???
               GrammarString *data = *stringListIterator;
               //AS Name of the actual data member. All the data members in rosetta is called p_<name>, but our name string only
               //contains <name>
               string varNameString = string(data->getVariableNameString());
               //AS A type name is always a type name, but if it is 'const' or 'static' those will we in front of the typename
               //And there are some typedef-types
               string varTypeString = string(data->getTypeNameString());
               //AS Freepointer is a specific data member that contains no AST information, so lets skip that

              if (varNameString != "freepointer" )
                  {
                    
                    //AS Analyse the types to see if it contains a '*' because then it is a pointer
                    bool typeIsStarPointer = ( varTypeString.find("*") != std::string::npos) ;
                    //AS Check to see if it this is an pointer to any IR-node. The varTypeString == "$CLASSNAME *" checks to see if it
                    //is a ir-node pointer which is *not* yet replaced. $CLASSNAME is inside the string and will later be replaced with 
                    //e.g SgTypeInt etc. 'varTypeString.substr(0,15) == "$GRAMMAR_PREFIX"' checks to see if it is part of the grammar.
                    //'varTypeString.substr(0,2) == "Sg" ' and to see if it is a Sg node of some type.
                    s += buildListIteratorString(varTypeString, varNameString,classNameString);

                    if ( (varTypeString == "$CLASSNAME *" ) || ( ( ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) || ( varTypeString.substr(0,2) == "Sg" ) ) && typeIsStarPointer ) )
                       {
                         //AS Checks to see if the pointer is a data member. Because the mechanism for generating access to variables
                         //is the same as the one accessing access member functions. We do not want the last case to show up here.
                         s += "          returnVector.push_back(pair<SgNode*,std::string>( p_" + varNameString + ",\""+varNameString+"\"));\n";
                    
                       }
                      else
                       {
                         if( varTypeString.find("rose_hash_multimap") != std::string::npos )
                            {
                           // AS(02/24/06) Generate code for returning data member pointers to IR
                              std::string accessOperator = ".";
                              if ( varTypeString.find("rose_hash_multimap*") != std::string::npos )
                                 {
                                   accessOperator = "->";
                                   s += "     if(p_" + varNameString + "==NULL)\n";
                                // Return a NULL pointer so that th graph shows that this pointer equals NULL
                                   s += "        returnVector.push_back(pair<SgNode*,std::string>( NULL,\""+varNameString+"\"));\n";
                                   s += "     else\n";
                                 }

                              s +=  "     for ( rose_hash_multimap::const_iterator it_"+varNameString+ "= p_" + varNameString + accessOperator + "begin();"
                                +   "it_" + varNameString
                                +   "!= p_" + varNameString + accessOperator+ "end(); ++"
                                +   "it_"+varNameString+") \n        { \n";
                           // Declare the a loop variable (reference to current element of list)
                              s += "          returnVector.push_back(pair<SgNode*,std::string>( it_" + varNameString + "->second,std::string(it_"+varNameString+"->first.str()) ) );\n";

                           // close off the loop
                              s += "        } \n";
                            }
                       }
                  }
             }
        }

     s +="     return returnVector;\n";

     return s;
   }

/*************************************************************************************************
*  The function
*       Terminal::buildListIteratorStringForReferenceToPointers()
*  supports buildProcessDataMemberReferenceToPointers() by building the string representing the code 
*  necessary to process references (pointers) all data member pointers to IR nodes contained in STL lists.
*  The traverse parameter indicates whether the property is normally traversed.  This parameter is
*  propagated through to the ReferenceToPointerHandler.
*************************************************************************************************/
string Terminal::buildListIteratorStringForReferenceToPointers(string typeName, string variableName, string classNameString, bool traverse)
   {
  // AS(2/14/2006) Builds the strings for the list of data member pointers.
     string returnString;

  // Control variables for code generation
     bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
     bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
     bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

  // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
     bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;
     bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers;
     bool typeIsSgNode                     = typeName.find('*') != string::npos;
            
#if 0
     printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers ? "true" : "false");
     printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
     printf ("typeIsPointerToList              = %s \n",typeIsPointerToList ? "true" : "false");
     printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers ? "true" : "false");
     printf ("typeIsList                       = %s \n",typeIsList ? "true" : "false");
     printf ("typeIsSgNode                     = %s \n",typeIsSgNode ? "true" : "false");
#endif 

  // One of these should be true!
     if(typeIsList!=true)
          return "";

     ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
     ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

     string listElementType = "default-error-type";


     if (typeIsList == true)
        {
       // name constant for all cases below (in this scope)
          string listElementName       = "source_list_element";

       // names that are set differently for different cases
          string iteratorBaseType;
          string needPointer;
          string originalList;
          string iteratorName;

       // Access member functions using "->" or "." (set to some string
       // that will cause an error if used, instead of empty string).
          string accessOperator = "error string for access operator";

          if (typeIsPointerToList == true)
             {
               if (typeIsPointerToListOfPointers == true)
                  {
                    needPointer = "*";
                    accessOperator = "->";
                  }
                 else
                  {
                    ROSE_ASSERT(typeIsPointerToListOfNonpointers == true);
                    accessOperator = ".";
                  }

            // iteratorBaseType = string("NeedBaseType_of_") + typeName;
               int positionOfListPtrSubstring = typeName.find("ListPtr");
               int positionOfPtrSubstring     = typeName.find("Ptr",positionOfListPtrSubstring);
               iteratorBaseType = typeName.substr(0,positionOfPtrSubstring);

            // copyOfList = variableName + "_source";
            // originalList = string("get_") + variableName + "()";
               originalList = string("p_") +variableName;

               iteratorName = variableName + "_iterator";

             }
            else
             {
               ROSE_ASSERT(typeIsSimpleListOfPointers == true);
               iteratorBaseType = typeName;
               needPointer = "*";
               accessOperator = ".";

            // Need to generate different code, for example:
            //      SgStatementPtrList::const_iterator cpinit_stmt = get_init_stmt().begin();
            // instead of:
            //      SgStatementPtrList::const_iterator init_stmt_copy_iterator = init_stmt_copy.begin();

            // originalList = string("get_") + variableName + "()";
               originalList = string("p_") + variableName;

               iteratorName = string("source_") + variableName + "_iterator";
             }

       // Need to get the prefix substring to strings like "SgFilePtrList" (i.e. "SgFile")
          int positionOfPtrListSubstring = iteratorBaseType.find("PtrList");
          int positionOfListSubstring    = iteratorBaseType.find("Ptr",positionOfPtrListSubstring);
          listElementType = typeName.substr(0,positionOfListSubstring) + needPointer;

       // Open up the loop over the list elements
          if (accessOperator=="->")
             {
               returnString += "     if (" + originalList + "== NULL) {}\n";
               returnString += "     else\n";
             }

          returnString += "     for ( " +iteratorBaseType + "::iterator " + iteratorName + " = " + originalList + accessOperator + "begin() \n"
                       + "; " + iteratorName
                       + " != " + originalList + accessOperator + "end(); ++" 
                       + iteratorName + ") \n        { \n";

       // Declare the a loop variable (reference to current element of list)
          returnString += "          handler->apply(*" + iteratorName + ",SgName(\""+variableName+"\"), " + BOOL2STR(traverse) + ");\n";
                
       // close off the loop
          returnString += "        } \n";

        }
     return returnString;
   }

//DQ (4/30/2006): This function is similar to buildReturnDataMemberPointers but returns reference to the pointers.
/*************************************************************************************************
*  The function
*       Terminal::buildProcessDataMemberReferenceToPointers()
*  builds the code for processing references to all data member pointers to IR nodes in the AST.
*************************************************************************************************/
string
Terminal::buildProcessDataMemberReferenceToPointers ()
   {
  // DQ (4/30/2006): This is a modified version of the code for buildReturnDataMemberPointers()

  // AS (2/14/2006): This function generates the code for each IR node to return
  // a pair of all data members pointing to IR nodes and their name to:
  //   1) Graph the whole AST
  //   2) Create a general mechanism to do the mechanism mentioned above
  //
     vector<GrammarString *> copyList;
     vector<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s ;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
  // AS Iterate over the terminal and its parents (base-classes).
  // PS! Everything is treated as terminals; even non-terminals
  // printf ("Derived Class terminal name = %s \n",name);

     for (Terminal *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
       // printf ("Possible base class terminal name = %s \n",(*t).name);
       // AS Iterate over data memeber of (non-) terminal class we are looking at 
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
            // AS ???
               GrammarString *data = *stringListIterator;
            // AS Name of the actual data member. All the data members in rosetta is called p_<name>, but our name string only
            // contains <name>
               string varNameString = string(data->getVariableNameString());
            // AS A type name is always a type name, but if it is 'const' or 'static' those will we in front of the typename
            // And there are some typedef-types
               string varTypeString = string(data->getTypeNameString());

            // AS Freepointer is a specific data member that contains no AST information, so lets skip that
              if (varNameString != "freepointer" )
                  {
                 // AS Analyse the types to see if it contains a '*' because then it is a pointer
                    bool typeIsStarPointer = ( varTypeString.find("*") != std::string::npos) ;

                 // PC Check whether this member should be traversed.
                    bool traverse = data->getToBeTraversed() == DEF_TRAVERSAL;

                 // AS Check to see if it this is an pointer to any IR-node. The varTypeString == "$CLASSNAME *" checks to see if it
                 // is a ir-node pointer which is *not* yet replaced. $CLASSNAME is inside the string and will later be replaced with 
                 // e.g SgTypeInt etc. 'varTypeString.substr(0,15) == "$GRAMMAR_PREFIX"' checks to see if it is part of the grammar.
                 // 'varTypeString.substr(0,2) == "Sg" ' and to see if it is a Sg node of some type.
                    s += buildListIteratorStringForReferenceToPointers(varTypeString, varNameString,classNameString, traverse);

                    if ( (varTypeString == "$CLASSNAME *" ) || ( ( ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) || ( varTypeString.substr(0,2) == "Sg" ) ) && typeIsStarPointer ) )
                       {
                      // AS Checks to see if the pointer is a data member. Because the mechanism for generating access to variables
                      // is the same as the one accessing access member functions. We do not want the last case to show up here.
                         s += "          handler->apply(p_" + varNameString + ",SgName(\""+varNameString+"\"), " + BOOL2STR(traverse) + ");\n";
                       }
                      else
                       {
                         if( varTypeString.find("rose_hash_multimap") != std::string::npos )
                            {
                           // AS(02/24/06) Generate code for returning data member pointers to IR
                              std::string accessOperator = ".";
                              accessOperator = "->";
                              s += "     if (p_" + varNameString + " == NULL)\n";
                              s += "        {\n";
                              s += "        }\n";
                              s += "     else\n";
                              s += "        {\n";
                              s += "          for ( rose_hash_multimap::iterator it_"+varNameString+ "= p_" + varNameString + accessOperator + "begin(); it_" + varNameString + "!= p_" + varNameString + accessOperator + "end(); ++" + "it_" + varNameString + ")\n";
                              s += "             {\n";
                           // Declare the a loop variable (reference to current element of list)
                              s += "               handler->apply(it_" + varNameString + "->second, it_" + varNameString + "->first, " + BOOL2STR(traverse) + ");\n";
                           // close off the loop
                              s += "             }\n";
                              s += "        }\n";
                            }
                       }
                  }
             }
        }

     return s;
   }


// DQ (3/7/2007): This is support for buildChildIndex() (see below)
/*************************************************************************************************
*  The function
*       Terminal::buildListIteratorStringForChildIndex()
*  supports buildChildIndex() by building the string representing the code 
*  necessary to count the index of all data member pointers to IR nodes 
*  contained in STL lists.
*************************************************************************************************/
std::string Terminal::buildListIteratorStringForChildIndex(string typeName, string variableName, string classNameString)
   {
  // AS(2/14/2006) Builds the strings for the list of data member pointers.
     string returnString;

  // Control variables for code generation
     bool typeIsPointerToListOfPointers    = typeName.find("PtrListPtr") != string::npos;
     bool typeIsPointerToListOfNonpointers = (typeIsPointerToListOfPointers == false) && typeName.find("ListPtr") != string::npos;
     bool typeIsPointerToList              = typeIsPointerToListOfPointers || typeIsPointerToListOfNonpointers;

  // By "simple list" we mean NOT a pointer to a list (just a list, e.g. STL list)
     bool typeIsSimpleListOfPointers       = (typeIsPointerToListOfPointers == false) && typeName.find("PtrList") != string::npos;
     bool typeIsList                       = typeIsPointerToList || typeIsSimpleListOfPointers;
     bool typeIsSgNode                     = typeName.find('*') != string::npos;
            
#if 0
     printf ("typeIsPointerToListOfPointers    = %s \n",typeIsPointerToListOfPointers ? "true" : "false");
     printf ("typeIsPointerToListOfNonpointers = %s \n",typeIsPointerToListOfNonpointers ? "true" : "false");
     printf ("typeIsPointerToList              = %s \n",typeIsPointerToList ? "true" : "false");
     printf ("typeIsSimpleListOfPointers       = %s \n",typeIsSimpleListOfPointers ? "true" : "false");
     printf ("typeIsList                       = %s \n",typeIsList ? "true" : "false");
     printf ("typeIsSgNode                     = %s \n",typeIsSgNode ? "true" : "false");
#endif 

  // One of these should be true!
     if (typeIsList != true)
          return "";


     ROSE_ASSERT(typeIsList == true  || typeIsSgNode == true);
     ROSE_ASSERT(typeIsList == false || typeIsSgNode == false);

     string listElementType = "default-error-type";


     if (typeIsList == true)
        {
       // name constant for all cases below (in this scope)
          string listElementName       = "source_list_element";

       // names that are set differently for different cases
          string iteratorBaseType;
          string needPointer;
          string originalList;
          string iteratorName;

       // Access member functions using "->" or "." (set to some string
       // that will cause an error if used, instead of empty string).
          string accessOperator = "error string for access operator";

          if (typeIsPointerToList == true)
             {
               if (typeIsPointerToListOfPointers == true)
                  {
                    needPointer = "*";
                    accessOperator = "->";
                  }
                 else
                  {
                    ROSE_ASSERT(typeIsPointerToListOfNonpointers == true);
                    accessOperator = ".";
                  }

            // iteratorBaseType = string("NeedBaseType_of_") + typeName;
               int positionOfListPtrSubstring = typeName.find("ListPtr");
               int positionOfPtrSubstring     = typeName.find("Ptr",positionOfListPtrSubstring);
               iteratorBaseType = typeName.substr(0,positionOfPtrSubstring);

            // copyOfList = variableName + "_source";
            // originalList = string("get_") + variableName + "()";
               originalList = string("p_") +variableName;

               iteratorName = variableName + "_iterator";

             }
            else
             {
               ROSE_ASSERT(typeIsSimpleListOfPointers == true);
               iteratorBaseType = typeName;
               needPointer = "*";
               accessOperator = ".";

            // Need to generate different code, for example:
            //      SgStatementPtrList::const_iterator cpinit_stmt = get_init_stmt().begin();
            // instead of:
            //      SgStatementPtrList::const_iterator init_stmt_copy_iterator = init_stmt_copy.begin();

            // originalList = string("get_") + variableName + "()";
               originalList = string("p_") + variableName;

               iteratorName = string("source_") + variableName + "_iterator";
             }

       // Need to get the prefix substring to strings like "SgFilePtrList" (i.e. "SgFile")
          int positionOfPtrListSubstring = iteratorBaseType.find("PtrList");
          int positionOfListSubstring    = iteratorBaseType.find("Ptr",positionOfPtrListSubstring);
          listElementType = typeName.substr(0,positionOfListSubstring) + needPointer;

       // Open up the loop over the list elements
          if (accessOperator=="->")
             {
               returnString += "     if ( " + originalList + " == NULL )\n";

            // Return a NULL pointer so that th graph shows that this pointer equals NULL
            // returnString += "          returnVector.push_back(pair<SgNode*,std::string>( NULL,\""+variableName+"\"));\n";
               returnString += "        { /* do nothing because this is not an IR node */ } \n";
               returnString += "       else\n";
             }

          returnString += "          for ( " +iteratorBaseType + "::const_iterator " + iteratorName + " = " + originalList + accessOperator + "begin(); " + iteratorName
                       + " != " + originalList + accessOperator + "end(); ++" + iteratorName + ") \n             { \n";

       // Declare the a loop variable (reference to current element of list)
       // returnString += "          returnVector.push_back(pair<SgNode*,std::string>( *" + iteratorName + ",\""+variableName+"\"));\n";
       // returnString += "               if ( *" + iteratorName + " == childNode ) { returnValue = indexCounter; } indexCounter++;\n";
          returnString += "               if ( *" + iteratorName + " == childNode ) { return indexCounter; } indexCounter++;\n";

       // close off the loop
          returnString += "             } \n";
        }

     return returnString;
   }

// DQ (3/7/2007): This is support for buildChildIndex() (see below)
/*************************************************************************************************
*  The function
*       Terminal::buildChildIndex()
*  builds the code for returning all data member pointers to IR nodes in the AST.
*************************************************************************************************/
string
Terminal::buildChildIndex()
   {
  // AS (2/14/2006): This function generates the code for each IR node to return
  // a signed integer of the index associated with the child in the IR node. A negative
  // value indicates that the IR node is not a child.  Uses include:
  //   1) Generating unique names for children (e.g. declarations in 
  //      scopes where overloaded template functions in a class don't 
  //      have the parameter list information availalbe so we need to 
  //      prevent over sharing.
  //   2) Forms a lower level implementation of "isChild(SgNode*)" function.
  //   3) Useful in Jeremiah's control flow graph work.

     vector<GrammarString *> copyList;
     vector<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
  // string s = "int indexCounter = 0, returnValue = -1;\n";
     string s = "int indexCounter = 0;\n";

     for (Terminal *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
       // printf ("Possible base class terminal name = %s \n",(*t).name);
       // AS Iterate over data memeber of (non-) terminal class we are looking at 
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               GrammarString *data = *stringListIterator;
            // AS Name of the actual data member. All the data members in rosetta is called p_<name>, but our name string only
            // contains <name>
               string varNameString = string(data->getVariableNameString());
            // AS A type name is always a type name, but if it is 'const' or 'static' those will we in front of the typename
            // And there are some typedef-types
               string varTypeString = string(data->getTypeNameString());
            // AS Freepointer is a specific data member that contains no AST information, so lets skip that

              if (varNameString != "freepointer" )
                  {
                 // AS Analyse the types to see if it contains a '*' because then it is a pointer
                    bool typeIsStarPointer = ( varTypeString.find("*") != std::string::npos) ;
                 // AS Check to see if it this is an pointer to any IR-node. The varTypeString == "$CLASSNAME *" checks to see if it
                 // is a ir-node pointer which is *not* yet replaced. $CLASSNAME is inside the string and will later be replaced with 
                 // e.g SgTypeInt etc. 'varTypeString.substr(0,15) == "$GRAMMAR_PREFIX"' checks to see if it is part of the grammar.
                 // 'varTypeString.substr(0,2) == "Sg" ' and to see if it is a Sg node of some type.
                    s += buildListIteratorStringForChildIndex(varTypeString, varNameString,classNameString);

                    if ( (varTypeString == "$CLASSNAME *" ) || ( ( ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) || ( varTypeString.substr(0,2) == "Sg" ) ) && typeIsStarPointer ) )
                       {
                         //AS Checks to see if the pointer is a data member. Because the mechanism for generating access to variables
                         //is the same as the one accessing access member functions. We do not want the last case to show up here.
                         // s += "     if ( p_" + varNameString + " == childNode ) { returnValue = indexCounter; } indexCounter++;\n";
                         s += "     if ( p_" + varNameString + " == childNode ) { return indexCounter; } indexCounter++;\n";
                       }
                      else
                       {
                         if( varTypeString.find("rose_hash_multimap") != std::string::npos )
                            {
                           // AS(02/24/06) Generate code for returning data member pointers to IR
                              std::string accessOperator = ".";
                              if ( varTypeString.find("rose_hash_multimap*") != std::string::npos )
                                 {
                                   accessOperator = "->";
                                   s += "     if ( p_" + varNameString + " == NULL )\n";
                                // Return a NULL pointer so that th graph shows that this pointer equals NULL
                                   s += "          indexCounter++;\n";
                                   s += "       else\n";
                                 }

                              s += "          for ( rose_hash_multimap::const_iterator it_"+varNameString+ "= p_" + varNameString + accessOperator + "begin(); it_" + varNameString + " != p_" + varNameString + accessOperator+ "end(); ++it_"+varNameString+") \n";
                              s += "             {\n";
                           // Declare the a loop variable (reference to current element of list)
                           // s += "               if ( it_" + varNameString + "->second == childNode ) { returnValue = indexCounter; } indexCounter++;\n";
                              s += "               if ( it_" + varNameString + "->second == childNode ) { return indexCounter; } indexCounter++;\n";
                           // close off the loop
                              s += "             }\n";
                            }
                       }
                  }
             }
        }

     s += "  // Child not found, return -1 (default value for returnValue) as index position to signal this.\n";
  // s += "     return returnValue;";
     s += "     return -1;";

     return s;
   }
