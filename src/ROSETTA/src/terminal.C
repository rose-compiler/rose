// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include "grammarTreeNode.h"
#include "constraint.h"

using namespace std;

// ################################################################
// #       We need a defintion of an arbitrary identifier         #
// ################################################################
Terminal identifier = string("ROSE_Identifier");

// ################################################################
// #                 NonTerminal Member Functions                 #
// ################################################################

void
Terminal::initialize ()
   {
  // variant  = 0;
  // Pointer back to the GrammarTreeNode (initialize to NULL)
     grammarSubTree = NULL;

  // The default setting is TRUE (must be set to TRUE using setDefaultConstructor)
     buildDefaultConstructor = FALSE;

     tag = "";

  // setup the default pre and post declarations
  // for each class implemented for each terminal
     setPredeclarationString  ("");
     setPostdeclarationString ("");

  // All terminals should be associated with a grammar, however we don't associate
  // nonTerminals with a specific grammar (maybe later).
     associatedGrammar = NULL;

  // Pointer to the parent terminal applies only to terminals build from an other terminal but with a constraint
     parentTerminal = NULL;

  // By default set all terminals to have there constructors
  // and destructors automatically generated
     setAutomaticGenerationOfConstructor ();
     setAutomaticGenerationOfDestructor  ();
     setAutomaticGenerationOfDataAccessFunctions ();
     setAutomaticGenerationOfCopyFunction ();

  // By default everything is a non-temporary (operators set this to TRUE)
     setTemporary(FALSE);
   }

// void Terminal::initialize ( int inputVariant, char* lexemeString, char* label, char* tagString )
void
Terminal::initialize ( const string& lexemeString, const string& label, const string& tagString )
   {
     initialize();

  // variant = inputVariant;
     string labelTemp = label;
     if (labelTemp == "") labelTemp = lexemeString;
     name   = labelTemp;

  // baseName is the same as "name" but name will be modified later to 
  // include the $GRAMMAR_PREFIX_ we need to keep the base around so that 
  // the $PARANT_GRAMMARS_BASE_ can be applied in connections to lower 
  // level grammars (e.g. parser's input parameters are elements of the 
  // parent's grammar.

     baseName   = labelTemp;

  // This is the lexeme
     lexeme = lexemeString;

     if (tagString == "")
        {
       // printf ("In Terminal::initialize(): Setting tag for %s \n",name);
          tag = name + "Tag";
        }
     else tag = tagString;
   }

#if 0
void
Terminal::initialize ( int inputVariant, char* lexemeString, char* label, char* tagString )
   {
     ROSE_ASSERT (lexemeString != NULL);

     variant = inputVariant;
     if (label == NULL)
          label = GrammarString::stringDuplicate(lexemeString);
     name   = GrammarString::stringDuplicate(label);

  // baseName is the same as "name" but name will be modified later to 
  // include the $GRAMMAR_PREFIX_ we need to keep the base around so that 
  // the $PARANT_GRAMMARS_BASE_ can be applied in connections to lower 
  // level grammars (e.g. parser's input parameters are elements of the 
  // parent's grammar.

     baseName   = GrammarString::stringDuplicate(label);

  // This is the lexeme
     lexeme = GrammarString::stringDuplicate(lexemeString);

  // Pointer back to the GrammarTreeNode (initialize to NULL)
     grammarSubTree = NULL;

  // The default setting is TRUE (must be set to TRUE using setDefaultConstructor)
     buildDefaultConstructor = FALSE;

     if (tagString == NULL)
        {
       // printf ("In Terminal::initialize(): Setting tag for %s \n",name);
          ROSE_ASSERT (name != NULL);
          tagString = stringConcatinate ("",name);
          tagString = stringConcatinate (tagString,"Tag");
        }

     tag = GrammarString::stringDuplicate(tagString);

     ROSE_ASSERT (lexeme != NULL);
     ROSE_ASSERT (name   != NULL);
     ROSE_ASSERT (tag    != NULL);

  // printf ("tag = %s \n",tag);

  // setup the default pre and post declarations
  // for each class implemented for each terminal
     setPredeclarationString  ("");
     setPostdeclarationString ("");

  // All terminals should be associated with a grammar, however we don't associate
  // nonTerminals with a specific grammar (maybe later).
     associatedGrammar = NULL;

  // Pointer to the parent terminal applies only to terminals build from an other terminal but with a constraint
     parentTerminal = NULL;
     ROSE_ASSERT(parentTerminal == NULL);

  // By default set all terminals to have there constructors
  // and destructors automatically generated
     setAutomaticGenerationOfConstructor ();
     setAutomaticGenerationOfDestructor  ();
     setAutomaticGenerationOfDataAccessFunctions ();
     setAutomaticGenerationOfCopyFunction ();

  // By default everything is a non-temporary (operators set this to TRUE)
     setTemporary(FALSE);
   }
#endif

Terminal::~Terminal()
   {
  // printf ("Inside of Terminal Descructor! (tokenString = %s) \n",tokenString);
     initialize ();
     setName("NULL_Name");
     ROSE_ASSERT(associatedGrammar == NULL);
   }

#if 1
Terminal::Terminal()
   : name(""), baseName(""), lexeme(""), tag(""), 
     grammarSubTree(NULL), lowerLevelGramaticalElement(NULL), 
     predeclarationString(""), postdeclarationString(""), 
     associatedGrammar(NULL),
     parentTerminal(NULL)
   {
  // initialize (0,"DefaultName","DefaultInternalName","DefaultTag");
     initialize ("DefaultName","DefaultInternalName","DefaultNameTag");
   }
#endif

Terminal::Terminal ( const string& lexemeString )
   : name(""), baseName(""), lexeme(""), tag(""), 
     grammarSubTree(NULL), lowerLevelGramaticalElement(NULL), 
     predeclarationString(""), postdeclarationString(""), 
     associatedGrammar(NULL),
     parentTerminal(NULL)
   {
  // This function is only use in the initailization of a static variable

  // initialize (1,lexemeString,"DefaultInternalName","DefaultTag");
     initialize (lexemeString,"DefaultInternalName","DefaultTag");
   }

Terminal::Terminal ( const string& lexemeString , Grammar & X , const string& stringVar, const string& tagString )
   : name(""), baseName(""), lexeme(""), tag(""), 
     grammarSubTree(NULL), lowerLevelGramaticalElement(NULL), 
     predeclarationString(""), postdeclarationString(""), 
     associatedGrammar(NULL),
     parentTerminal(NULL)
   {
     if (tagString == "")
        {
          string tempTagString = lexemeString + "Tag";
          initialize (lexemeString,stringVar,tempTagString);
        }
       else
          initialize (lexemeString,stringVar,tagString);

  // Record the grammar with which we associate this terminal
     associatedGrammar = &X;
     X.addGrammarElement(*this);
     ROSE_ASSERT(associatedGrammar != NULL);
   }

bool
Terminal::isTemporary() const
   {
  // Temporaries are the result of expressions using terminals/nonterminals.
     ROSE_ASSERT(this!=NULL);
     if (temporary) ROSE_ASSERT(name == "DefaultInternalName");
     return temporary;
   }

void
Terminal::setTemporary( bool input )
   {
  // Temporaries are the result of expressions using terminals/nonterminals.
     temporary = input;
     if (temporary) ROSE_ASSERT(name == "DefaultInternalName");
   }


Terminal &
Terminal::operator= ( const Terminal & X )
   {
  // ROSE_ASSERT (X.variant  >= 0);

  // Save the parent since the initialize member function will reset it
  // Terminal* originalParentTerminal = parentTerminal;
  // We can't always assert that this is true!
  // ROSE_ASSERT(originalParentTerminal == NULL);

#if 1
  // This causes the names to be lost when the Rhs is a temporary built from a BNF expression
  // the value of the parentTerminal is also lost so it must be reset
     initialize (X.lexeme,X.name,X.tag);
#else
     if (!X.isTemporary())
        {
       // There may be other variables that should be set!
          setName(X.name);
          setTagName(X.tag);
          setLexeme(X.lexeme);
	  copyTerminalData(&X);
        }

  // There may be other variables that should be set!
     parentTerminal = NULL;
#endif

  // reset the parentTerminal pointer
  // parentTerminal = originalParentTerminal;
  // We can't always assert that this is true!
  // ROSE_ASSERT(parentTerminal == NULL);

     associatedGrammar = X.associatedGrammar;
  // associatedGrammar->addGrammarElement(*this);
     ROSE_ASSERT(X.associatedGrammar != NULL);
  // Don't add this to the grammar since it might be a temporary built
  // in the evaluation of the BNF syntax.
  // X.associatedGrammar->addGrammarElement(*this);
     ROSE_ASSERT(associatedGrammar != NULL);

#if 0
     for (int i=0; i < 2; i++)
          for (int j=0; j < 2; j++)
             {
               printf ("i = %d  j = %d \n",i,j);
               ROSE_ASSERT (X.getMemberFunctionPrototypeList(i,j).size() == 0);
               ROSE_ASSERT (X.getMemberDataPrototypeList(i,j).size()     == 0);
               ROSE_ASSERT (X.getMemberFunctionSourceList(i,j).size()    == 0);
             }
#endif

  // Error checking (we can't test this because the terminal/nonterminal is not
  // yet been added to the grammar).
  // consistencyCheck();

     return *this;
   }

#if 0
Terminal::Terminal ( const Terminal & X )
   : name(NULL), baseName(NULL), lexeme(NULL), tag(NULL), 
     grammarSubTree(NULL), lowerLevelGramaticalElement(NULL), 
     predeclarationString(""), postdeclarationString(""), 
     associatedGrammar(NULL),
     parentTerminal(NULL)
   {
    *this = X;
   }
#endif

void
Terminal::setExpression ( const Terminal & X )
   {
  // This function acts a lot like the operator= except it does not set the parentTerminal pointer!

  // printf ("Not implemented: Terminal::setExpression ( const Terminal & X ) \n");
  // ROSE_ABORT();

#if 0
     ROSE_ASSERT(X.associatedGrammar != NULL);
     setGrammar(X.associatedGrammar);
     ROSE_ASSERT(associatedGrammar != NULL);

     for (int i=0; i < 2; i++)
          for (int j=0; j < 2; j++)
             {
            // Declarations
               getMemberFunctionPrototypeList(i,j) = X.getMemberFunctionPrototypeList(i,j);
               getMemberDataPrototypeList    (i,j) = X.getMemberDataPrototypeList    (i,j);
               getMemberFunctionSourceList   (i,j) = X.getMemberFunctionSourceList   (i,j);

            // Edit substitution list
               getEditSubstituteTargetList (i,j) = X.getEditSubstituteTargetList (i,j);
               getEditSubstituteSourceList (i,j) = X.getEditSubstituteSourceList (i,j);
             }

  // The purpose of the remaining variable set here are explained in the header file
  // setIncludeInitializerInDataStrings (childTerminal->getIncludeInitializerInDataStrings());

     buildDefaultConstructor         = parentTerminal->buildDefaultConstructor;
     includeInitializerInDataStrings = parentTerminal->includeInitializerInDataStrings;

     predeclarationString            = GrammarString::stringDuplicate(parentTerminal->predeclarationString);
     postdeclarationString           = GrammarString::stringDuplicate(parentTerminal->postdeclarationString);

     automaticGenerationOfDestructor          = parentTerminal->automaticGenerationOfDestructor;
     automaticGenerationOfConstructor         = parentTerminal->automaticGenerationOfConstructor;
     automaticGenerationOfDataAccessFunctions = parentTerminal->automaticGenerationOfDataAccessFunctions;
     automaticGenerationOfCopyFunction        = parentTerminal->automaticGenerationOfCopyFunction;
#endif

  // ROSE_ASSERT (parentTerminal == NULL);
   }

#if 0
NonTerminal 
Terminal::operator[] ( const NonTerminal & X)
   {
     ROSE_ASSERT (name != NULL);
     ROSE_ASSERT (X.name != NULL);
  // return NonTerminal(*this & X);
     NonTerminal result = *this & X;
     result.defaultNonTerminal = TRUE;
     ROSE_ASSERT (X.name != NULL);
     ROSE_ASSERT (result.name != NULL);
     return result;
   }
#endif

void
Terminal::copyTerminalData ( Terminal* childTerminal )
   {
     ROSE_ASSERT(childTerminal != NULL);
//   ROSE_ASSERT(getParent() != NULL);
//   ROSE_ASSERT(isChild() == TRUE);

     ROSE_ASSERT(childTerminal->associatedGrammar != NULL);
     setGrammar(childTerminal->associatedGrammar);
     ROSE_ASSERT(associatedGrammar != NULL);

#if 1
     for (int i=0; i < 2; i++)
          for (int j=0; j < 2; j++)
             {
            // Declarations
               getMemberFunctionPrototypeList(i,j) = childTerminal->getMemberFunctionPrototypeList(i,j);
               getMemberDataPrototypeList    (i,j) = childTerminal->getMemberDataPrototypeList    (i,j);
               getMemberFunctionSourceList   (i,j) = childTerminal->getMemberFunctionSourceList   (i,j);

            // Edit substitution list
               getEditSubstituteTargetList (i,j) = childTerminal->getEditSubstituteTargetList (i,j);
               getEditSubstituteSourceList (i,j) = childTerminal->getEditSubstituteSourceList (i,j);
             }
#endif

  // The purpose of the remaining variable set here are explained in the header file
  // setIncludeInitializerInDataStrings (childTerminal->getIncludeInitializerInDataStrings());

     buildDefaultConstructor         = childTerminal->buildDefaultConstructor;
     includeInitializerInDataStrings = childTerminal->includeInitializerInDataStrings;

     predeclarationString            = childTerminal->predeclarationString;
     postdeclarationString           = childTerminal->postdeclarationString;

     automaticGenerationOfDestructor          = childTerminal->automaticGenerationOfDestructor;
     automaticGenerationOfConstructor         = childTerminal->automaticGenerationOfConstructor;
     automaticGenerationOfDataAccessFunctions = childTerminal->automaticGenerationOfDataAccessFunctions;
     automaticGenerationOfCopyFunction        = childTerminal->automaticGenerationOfCopyFunction;

  // ROSE_ASSERT (parentTerminal == NULL);
   }

Terminal &
Terminal::copy ( const string& inputLexeme, const string& inputTagString )
   {
  // builds child terminals from parent terminals

  // Declare this new terminal on the heap
     Terminal* childTerminal = new Terminal(*this);
     ROSE_ASSERT(childTerminal != NULL);

     addChild(childTerminal);

     childTerminal->setName(inputLexeme);
     childTerminal->setTagName(inputTagString);
     childTerminal->setParentTerminal(this);
     ROSE_ASSERT(childTerminal->parentTerminal != NULL);

     childTerminal->copyTerminalData (this);

  // Now add the terminal to the list of terminals in the associated grammar
     ROSE_ASSERT(childTerminal != NULL);
     ROSE_ASSERT(associatedGrammar != NULL);
     associatedGrammar->addGrammarElement(*childTerminal);

  // Error checking
     childTerminal->consistencyCheck();

     return *childTerminal;
   }

void
Terminal::setParentTerminal( Terminal* Xptr )
   {
     parentTerminal = Xptr;
     ROSE_ASSERT(parentTerminal != NULL);
   }

Terminal*
Terminal::getParentTerminal() const
   {
     return ((Terminal*) this)->parentTerminal;
   }

void
Terminal::addChild ( Terminal* Xptr )
   {
     childListOfConstrainedTerminals.push_back(Xptr);
   }

bool
Terminal::isType() const
   {
     bool returnValue = FALSE;

  // For now we only call this for child terminals (so check this)
  // ROSE_ASSERT(parentTerminal != NULL);

  // If this is a child terminal then the type is found through an examination of the parent node
  // The name of the child type would be user defined and so would not be something we could recognize.
     if (parentTerminal != NULL)
          returnValue = parentTerminal->isType();
       else
        {
          string terminalName = getName();
          if ( (terminalName == "TypeUnknown") || 
               (terminalName == "TypeChar") ||
               (terminalName == "TypeSignedChar") ||
               (terminalName == "TypeUnsignedChar") ||
               (terminalName == "TypeShort") ||
               (terminalName == "TypeSignedShort") ||
               (terminalName == "TypeUnsignedShort") ||
               (terminalName == "TypeInt") ||
               (terminalName == "TypeSignedInt") ||
               (terminalName == "TypeUnsignedInt") ||
               (terminalName == "TypeLong") ||
               (terminalName == "TypeSignedLong") ||
               (terminalName == "TypeUnsignedLong") ||
               (terminalName == "TypeVoid") ||
               (terminalName == "TypeGlobalVoid") ||
               (terminalName == "TypeWchar") ||
               (terminalName == "TypeFloat") ||
               (terminalName == "TypeDouble") ||
               (terminalName == "TypeLongLong") ||
               (terminalName == "TypeUnsignedLongLong") ||
               (terminalName == "TypeLongDouble") ||
               (terminalName == "TypeString") ||
               (terminalName == "TypeBool") ||
               (terminalName == "Complex") ||
               (terminalName == "TypeDefault") ||
               (terminalName == "PointerMemberType") ||
               (terminalName == "ReferenceType") ||
               (terminalName == "ClassType") ||
               (terminalName == "EnumType") ||
               (terminalName == "TypedefType") ||
               (terminalName == "ModifierType") ||
               (terminalName == "PartialFunctionType") ||
               (terminalName == "ArrayType") ||
               (terminalName == "TypeEllipse") ||
               (terminalName == "UnknownMemberFunctionType") ||
               (terminalName == "MemberFunctionType") ||
               (terminalName == "FunctionType") ||
               (terminalName == "PointerType") ||
               (terminalName == "NamedType") )
             {
               returnValue = TRUE;
             }
        }

     return returnValue;
   }

bool
Terminal::isExpression() const
   {
     bool returnValue = FALSE;

  // For now we only call this for child terminals (so check this)
  // ROSE_ASSERT(parentTerminal != NULL);

  // If this is a child terminal then the type is found through an examination of the parent node
     if (parentTerminal != NULL)
          returnValue = parentTerminal->isExpression();
       else
        {
          string terminalName = getName();
     if ( (terminalName == "ExprListExp") ||
          (terminalName == "VarRefExp") ||
          (terminalName == "ClassNameRefExp") ||
          (terminalName == "FunctionRefExp") ||
          (terminalName == "MemberFunctionRefExp") ||
          (terminalName == "FunctionCallExp") ||
          (terminalName == "SizeOfOp") ||
          (terminalName == "TypeIdOp") ||
          (terminalName == "ConditionalExp") ||
          (terminalName == "NewExp") ||
          (terminalName == "DeleteExp") ||
          (terminalName == "ThisExp") ||
          (terminalName == "RefExp") ||
          (terminalName == "AggregateInitializer") ||
          (terminalName == "ConstructorInitializer") ||
          (terminalName == "AssignInitializer") ||
          (terminalName == "ExpressionRoot") ||
          (terminalName == "MinusOp") ||
          (terminalName == "UnaryAddOp") ||
          (terminalName == "NotOp") ||
          (terminalName == "PointerDerefExp") ||
          (terminalName == "AddressOfOp") ||
          (terminalName == "MinusMinusOp") ||
          (terminalName == "PlusPlusOp") ||
          (terminalName == "BitComplementOp") ||
          (terminalName == "CastExp") ||
          (terminalName == "ThrowOp") ||
          (terminalName == "ArrowExp") ||
          (terminalName == "DotExp") ||
          (terminalName == "DotStarOp") ||
          (terminalName == "ArrowStarOp") ||
          (terminalName == "EqualityOp") ||
          (terminalName == "LessThanOp") ||
          (terminalName == "GreaterThanOp") ||
          (terminalName == "NotEqualOp") ||
          (terminalName == "LessOrEqualOp") ||
          (terminalName == "GreaterOrEqualOp") ||
          (terminalName == "AddOp") ||
          (terminalName == "SubtractOp") ||
          (terminalName == "MultiplyOp") ||
          (terminalName == "DivideOp") ||
          (terminalName == "IntegerDivideOp") ||
          (terminalName == "ModOp") ||
          (terminalName == "AndOp") ||
          (terminalName == "OrOp") ||
          (terminalName == "BitXorOp") ||
          (terminalName == "BitAndOp") ||
          (terminalName == "BitOrOp") ||
          (terminalName == "CommaOpExp") ||
          (terminalName == "LshiftOp") ||
          (terminalName == "RshiftOp") ||
          (terminalName == "PntrArrRefExp") ||
          (terminalName == "ScopeOp") ||
          (terminalName == "AssignOp") ||
          (terminalName == "PlusAssignOp") ||
          (terminalName == "MinusAssignOp") ||
          (terminalName == "AndAssignOp") ||
          (terminalName == "IorAssignOp") ||
          (terminalName == "MultAssignOp") ||
          (terminalName == "DivAssignOp") ||
          (terminalName == "ModAssignOp") ||
          (terminalName == "XorAssignOp") ||
          (terminalName == "LshiftAssignOp") ||
          (terminalName == "RshiftAssignOp") ||
          (terminalName == "BoolValExp") ||
          (terminalName == "StringVal") ||
          (terminalName == "ShortVal") ||
          (terminalName == "CharVal") ||
          (terminalName == "UnsignedCharVal") ||
          (terminalName == "WcharVal") ||
          (terminalName == "UnsignedShortVal") ||
          (terminalName == "IntVal") ||
          (terminalName == "EnumVal") ||
          (terminalName == "UnsignedIntVal") ||
          (terminalName == "LongIntVal") ||
          (terminalName == "LongLongIntVal") ||
          (terminalName == "UnsignedLongLongIntVal") ||
          (terminalName == "UnsignedLongVal") ||
          (terminalName == "FloatVal") ||
          (terminalName == "DoubleVal") ||
          (terminalName == "LongDoubleVal") ||
          (terminalName == "Initializer") ||
          (terminalName == "UnaryOp") ||
          (terminalName == "BinaryOp") ||
          (terminalName == "ValueExp") )
        {
          returnValue = TRUE;
        }
        }
     
     return returnValue;
   }

bool
Terminal::isStatement() const
   {
     bool returnValue = FALSE;

  // For now we only call this for child terminals (so check this)
  // ROSE_ASSERT(parentTerminal != NULL);

  // If this is a child terminal then the type is found through an examination of the parent node
     if (parentTerminal != NULL)
          returnValue = parentTerminal->isStatement();
       else
        {
          string terminalName = getName();
     if ( (terminalName == "ParBlockStmt") ||
          (terminalName == "ParForStmt") ||
          (terminalName == "Global") ||
          (terminalName == "IfStmt") ||
          (terminalName == "FunctionDefinition") ||
          (terminalName == "ClassDefinition") ||
          (terminalName == "WhileStmt") ||
          (terminalName == "DoWhileStmt") ||
          (terminalName == "SwitchStatement") ||
          (terminalName == "CatchOptionStmt") ||
          (terminalName == "MemberFunctionDeclaration") ||
          (terminalName == "VariableDeclaration") ||
          (terminalName == "VariableDefinition") ||
          (terminalName == "ClassDeclaration") ||
          (terminalName == "EnumDeclaration") ||
          (terminalName == "AsmStmt") ||
          (terminalName == "TypedefDeclaration") ||
          (terminalName == "TemplateDeclaration") ||
          (terminalName == "FunctionTypeTable") ||
          (terminalName == "ExprStatement") ||
          (terminalName == "LabelStatement") ||
          (terminalName == "CaseOptionStmt") ||
          (terminalName == "TryStmt") ||
          (terminalName == "DefaultOptionStmt") ||
          (terminalName == "BreakStmt") ||
          (terminalName == "ContinueStmt") ||
          (terminalName == "ReturnStmt") ||
          (terminalName == "GotoStatement") ||
          (terminalName == "SpawnStmt") ||
          (terminalName == "PragmaStatement") ||
          (terminalName == "ForStatement") ||
          (terminalName == "BasicBlock") ||
          (terminalName == "ScopeStatement") ||
          (terminalName == "FunctionDeclaration") ||
          (terminalName == "DeclarationStatement") )
        {
          returnValue = TRUE;
        }
        }

     return returnValue;
   }

void
Terminal::setBuildDefaultConstructor ( bool X )
   {
     buildDefaultConstructor = X;
   }

bool
Terminal::getBuildDefaultConstructor () const
   {
     ROSE_ASSERT (buildDefaultConstructor == TRUE || buildDefaultConstructor == FALSE);
     return buildDefaultConstructor;
   }

// AJ ( 10/26/2004)
// 
string
Terminal::buildDestructorBody ()
   {
     string returnString;
     list<GrammarString *> localList;
     list<GrammarString *> localExcludeList;
     list<GrammarString *>::iterator stringListIterator;

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
          if (!((*stringListIterator)->getToBeDeleted()))
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
#if 0
          if ( (typeString.find("static ") == string::npos) && 
               (typeString.find("*") != string::npos) && 
               (initializerString.empty() == false) && 
               (variableNameString != "freepointer") )
#endif
       // DQ (5/24/2006): The SgNode::operator delete() will set the p_freepointer so don't fool with it in the descructor!
          if (typeString.find("static ") == string::npos && (initializerString.empty() == false) && (variableNameString != "freepointer") )
             {
            // string tempString = "     p_$DATA $DEFAULT_VALUE;\n";
               string tempString = (*stringListIterator)->buildDestructorSource();

               tempString = StringUtility::copyEdit (tempString,"$DATA",variableNameString);
               tempString = StringUtility::copyEdit (tempString,"$DEFAULT_VALUE",initializerString);

               returnString += tempString;
             }
#if 0
          if (variableNameString == "preprocessorDirectivesAndCommentsList")
             exitAsTest = true;
#endif
        }

     ROSE_ASSERT (localExcludeList.size() == 0);

  // printf ("In Terminal::buildDestructorBody(): returnString = %s \n",returnString.c_str());
#if 0
     if (exitAsTest == true)
        {
          printf ("Exiting as test in Terminal::buildDestructorBody() \n");
          ROSE_ASSERT(false);
        }
#endif
     return returnString;
   }

string
Terminal::buildConstructorBody ( bool withInitializers, ConstructParamEnum config )
   {
  // This function builds a string that represents the initialization of member data
  // if the default initializers are used (for default constructors) then all member 
  // data is initialized using the initializer (all member data should be defined 
  // with an initializer string).  If withInitializers == FALSE (for non-default 
  // constructors) then all member data is initialized with the name used for the 
  // for the associated constructor parameter.  Excluded data members are initialized
  // with their default initializer.

     string returnString;

     list<GrammarString *> localList;
     list<GrammarString *> localExcludeList;
     list<GrammarString *>::iterator stringListIterator;

  // DQ (12/7/2003): This is never true, or so it seems
     ROSE_ASSERT (withInitializers == false);

  // Initialize with local node data
     localList        = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
     localExcludeList = getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST);

#if 0
     printf ("name = %s localList.size() = %zu  localExcludeList.size() = %zu \n",
          name,localList.size(),localExcludeList.size());
#endif

  // Now edit the list to remove elements appearing within the exclude list
     Grammar::editStringList ( localList, localExcludeList );

     string prevParam;
     for( stringListIterator = localList.begin();
          stringListIterator != localList.end();
          stringListIterator++ )
        {
       // QY 11/9/04  added additional types of constructor parameters 
          string variableNameString = (*stringListIterator)->getVariableNameString();
          switch ( (*stringListIterator)->getIsInConstructorParameterList() )
             {
               case NO_CONSTRUCTOR_PARAMETER:
                 // DQ (11/20/2004): This test does not appear to work to skip cases where the initializer is empty
                 // the reason is that getDefaultInitializerString() returns a char* and the wrong operator!= is being used!
                 // if ((*stringListIterator)->getDefaultInitializerString() != "")
                    if (string( (*stringListIterator)->getDefaultInitializerString()) != "")
                       {
                         returnString = returnString + "     p_" + variableNameString + " " + 
                                        (*stringListIterator)->getDefaultInitializerString() + ";\n";
                       }
                    break;

               case CONSTRUCTOR_PARAMETER:
                    returnString = returnString + "     p_" + variableNameString+ " = " + variableNameString + ";\n";
                    break;
               case INDIRECT_CONSTRUCTOR_PARAMETER:
                    prevParam = variableNameString;
                    break;
               case WRAP_CONSTRUCTOR_PARAMETER:
                    assert(prevParam != "");
                    if (config == INDIRECT_CONSTRUCTOR_PARAMETER)
                       {
                         returnString = returnString + "     if(" + prevParam + " == NULL) \n"
                                                     + "        p_" + variableNameString + " = 0; \n"
                                                     + "     else \n";
                         string t = (*stringListIterator)->getTypeNameString();
                         if (t[t.size()-1] == '*')
                              t = string(t, 0,t.size()-1);

                      // DQ (6/5/2006): Avoid sharing the Sg_File_Info objects
                      // returnString = returnString + "       p_" + variableNameString + " = new " + t + "(" + prevParam + "->get_file_info()," + prevParam + ");\n";
                         returnString = returnString + "       p_" + variableNameString + " = new " + t + "(New_File_Info(" + prevParam + ")," + prevParam + ");\n";
                       }
                      else 
                         returnString = returnString + "     p_" + variableNameString+ " = " + variableNameString + ";\n";
                    prevParam = "";
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
#if 0
          const char* filename     = "../Grammar/copyMemberFunction.macro";
          ifstream buf(filename);
          string tmp, tmpresult;
          while ( getline(buf, tmp, '\n') )
             {
               if (strstr(tmp.c_str(), "$CLASSNAME::") != 0)
                  {
                    tmpresult = tmpresult + "      " + tmp + "; \n";
                  }
             }
          returnString = GrammarString::stringDuplicate(tmpresult.c_str());
#else
       // DQ (10/13/2007): Make this the function prototype for the copy mechanism.
       // This also fixes a bug where the code above was causing more than just the 
       // prototype to be output in the Cxx_Grammar.h header file.
          returnString.push_back(StringUtility::StringWithLineNumber("          virtual SgNode* copy ( SgCopyHelp& help) const;", "" /* "<copy member function>" */, 1));
#endif
        }
       else
        {
       // DQ (10/13/2007): Not clear when or why but this applies to only the SgSymbol IR node!
          returnString.push_back(StringUtility::StringWithLineNumber("       // copy functions omitted for $CLASSNAME", "" /* "<copy member function>" */, 1));
        }
#if 0
  // DQ (10/13/2007): Not required now that we specify the prototype explicit (above).
     if (returnString != NULL)
        {
          returnString = GrammarString::copyEdit ( returnString,"$CLASSNAME::", "");
        }
#endif

  // printf ("In Terminal::buildCopyMemberFunctionHeader(): returnString = %s \n",returnString);

     return returnString;
   }

#if 0
// DQ (10/23/2007): This function should not be called, the SageInterface::rebuildSymbolTable() function is called by the AST fixupCopy member functions.
string generateCodeForSymbolTableFixup()
   {
     printf ("Error: We now call the  SageInterface::rebuildSymbolTable() function from the AST fixupCopy member functions \n");
     ROSE_ASSERT(false);

     string returnString = string("  // If this is a scope then rebuild the local symbol table \n")
                         + string("     SgScopeStatement* localScope = isSgScopeStatement(result); \n")
                      // + string("     if ( localScope != NULL && localScope->get_symbol_table() == NULL ) \n")
                         + string("     if ( localScope != NULL ) \n")
                         + string("        { \n")
                      // + string("       // Overwrite the previous symbol table (memory leak for now) \n")
                      // + string("          localScope->set_symbol_table(NULL); \n")
                         + string("          SageInterface::rebuildSymbolTable(localScope); \n")
                         + string("        } \n");
     return returnString;
   }
#endif


// char* Terminal::buildCopyMemberFunctionSource ()
StringUtility::FileWithLineNumbers Terminal::buildCopyMemberFunctionSource ()
   {
  // This function builds the copy member function's body

  // printf ("In Terminal::buildCopyMemberFunctionSource(): class name = %s \n",name);

  // char* returnString = NULL;
     StringUtility::FileWithLineNumbers returnString;
     if (automaticGenerationOfCopyFunction == TRUE)
        {
          string constructArgCopy = "", constructArgList = "", postConstructCopy = "";
          string filename     = "../Grammar/copyMemberFunction.macro";
	  StringUtility::FileWithLineNumbers functionTemplateString = Grammar::readFileWithPos (filename);
          bool emptyConstructorArg = (!generateConstructor()) || getBuildDefaultConstructor ();

       // printf ("Derived Class terminal name = %s \n",name);

       // DQ (9/28/2005): We can't enforce this, but perhaps it is a good goal for ROSETTA.
       // ROSE_ASSERT(generateConstructor() == true);
          ROSE_ASSERT(getBuildDefaultConstructor() == false);

       // DQ (9/28/2005): I think this should be a while loop and not a for 
       // loop since the terminatation is not easily determined statically.
          for (Terminal *t = this; t != NULL; t = t->grammarSubTree->hasParent() ?  &t->grammarSubTree->getParent().getToken() : NULL)
             {
               string constructArgCopy1 = "", constructArgList1 = "", postConstructCopy1 = "";
               list<GrammarString *> copyList;
               list<GrammarString *>::iterator stringListIterator;

            // printf ("Possible base class terminal name = %s \n",(*t).name);

               copyList        = t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
            // copyList.merge( t->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::EXCLUDE_LIST));
            // copyList.merge( t->getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST));
            // copyList.merge( t->getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::EXCLUDE_LIST));

            // bool firstArgument = true;

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

                    if (!emptyConstructorArg && data->getIsInConstructorParameterList())
                       {
                      // DQ (9/24/2005): Added comments to generated code!
                         string comment = string("  // Copy constructor parameter data member: ") + varNameString + string("\n");
                         constructArgCopy1 += comment;

                      // constructArgCopy1 += varDecl;

                      // Code built generated to build constructor parameters can't set the "result->xxx" 
                      // data members since the "result" class have not been built (see generated code 
                      // for more examples).
                         bool buildConstructorArgument = true;
                         constructArgCopy1 += data->buildCopyMemberFunctionSource( buildConstructorArgument );
                      // constructArgList1 = constructArgList1 + ", " + varNameString;

#if 0
                      // Need to add "," between each constructor parameter argument (to build arument list)
                         if (firstArgument == true)
                            {
                              firstArgument = false;
                              constructArgList1 += ", ";
                            }
                           else
                            {
                              constructArgList1 += ", ";
                            }
                         constructArgList1 += varNameString;
#else
                         constructArgList1 += ", " + varNameString;
#endif

#if 1
                         string setParentString = data->buildCopyMemberFunctionSetParentSource(varNameString);
                         postConstructCopy += setParentString;
#endif
                       }
                      else
                       {
                      // Code built generated to build constructor parameters can't set the "result->xxx" 
                      // data members since the "result" class have not been built (see generated code 
                      // for more examples).
                         bool buildConstructorArgument = false;

                         switch (data->automaticGenerationOfDataAccessFunctions)
                            {
                              case NO_ACCESS_FUNCTIONS:
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

                              case BUILD_LIST_ACCESS_FUNCTIONS:
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

                              case BUILD_ACCESS_FUNCTIONS:
                                 {

                                // DQ (9/24/2005): Added comments to generated code!
                                   string comment = string("  // Copy non-constructor parameter data member (access function): ") + varNameString + string("\n");
                                   postConstructCopy1 += comment;

                                // postConstructCopy1 += varDecl;
#if 0
                                // DQ (10/22/2005): I would like to get rid of the redundant set function call (old code)
                                   postConstructCopy1 += data->buildCopyMemberFunctionSource(buildConstructorArgument)
                                                      + "     result->set_"  + string(data->getVariableNameString())
                                                      + "(" + string(data->getVariableNameString()) + "_copy); // old code \n";
#else
                                // DQ (10/22/2005): Simpler code
                                   postConstructCopy1 += data->buildCopyMemberFunctionSource(buildConstructorArgument);
#endif
#if 1
                                // string setParentString = "     " + string(data->getVariableNameString()) + "_copy.set_parent(result);\n";
                                   string setParentString = data->buildCopyMemberFunctionSetParentSource(varNameString);
                                // postConstructCopy = setParentString + postConstructCopy;
                                   postConstructCopy1 += setParentString;
#endif
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Default reached in Terminal::buildCopyMemberFunctionSource \n");
                                   ROSE_ASSERT(false);
                                 }
                            }
#if 0
                      // Skip adding a comment for each variable (many don't cause code to be generated)
                         string parentCopyComment = "\n  // Now set the parents! (e.g. "
                                                  + string(data->getVariableNameString()) 
                                                  + "_copy->set_parent(result);) \n";
                         postConstructCopy = parentCopyComment + postConstructCopy;
#endif
                       }
	               }

               constructArgCopy  = constructArgCopy1  + constructArgCopy;
               constructArgList  = constructArgList1  + constructArgList;
               postConstructCopy = postConstructCopy1 + postConstructCopy;
             }

#if 1
       // Not fond of this approach to fixing up the leading "," in what is generated above.
          if (constructArgList != "")
             {
               constructArgList = constructArgList.c_str() + 1; // get rid of the preceding ','
             }
#endif

       // DQ (10/23/2007): This function should not be called, the SageInterface::rebuildSymbolTable() function is called by the AST fixupCopy member functions.
       // postConstructCopy += generateCodeForSymbolTableFixup();

       // Put this string on the heap so it can be managed by copyEdit()
          returnString = functionTemplateString;
#if 0
          returnString = GrammarString::copyEdit (returnString,"virtual ", "");
          returnString = GrammarString::copyEdit (returnString,"$CONSTRUCT_ARG_COPY",constructArgCopy.c_str() );
          returnString = GrammarString::copyEdit (returnString,"$CONSTRUCT_ARG_LIST",constructArgList.c_str());
          returnString = GrammarString::copyEdit (returnString,"$POST_CONSTRUCT_COPY",postConstructCopy.c_str());
#else
       // DQ (9/28/2005): Migrating to the use of string class everywhere in ROSETTA
          returnString = StringUtility::copyEdit (returnString,"virtual ", "");
          returnString = StringUtility::copyEdit (returnString,"$CONSTRUCT_ARG_COPY",constructArgCopy);
          returnString = StringUtility::copyEdit (returnString,"$CONSTRUCT_ARG_LIST",constructArgList);
          returnString = StringUtility::copyEdit (returnString,"$POST_CONSTRUCT_COPY",postConstructCopy);
#endif
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
Terminal::show() const
   {
     ROSE_ASSERT (this != NULL);
     printf ("%s ",name.c_str());
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
     ROSE_ASSERT (GrammarString::isContainedIn(name,grammarName) == FALSE);

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
#if 0
     if (getGrammar() != NULL)
          grammarName = getGrammar()->getGrammarName();
#endif

     this->tag = grammarName + tag;
   }

const string&
Terminal::getTagName () const
   {
     return tag;
   }

void
Terminal::setParentTreeNode ( const GrammarTreeNode & inputParent )
   {
     ROSE_ASSERT (grammarSubTree != NULL);
     ROSE_ASSERT (grammarSubTree->parentTreeNode == NULL);
     grammarSubTree->parentTreeNode = &((GrammarTreeNode &) inputParent);
     ROSE_ASSERT (grammarSubTree->parentTreeNode != NULL);
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
  
  bool pureVirtual = FALSE;
  
  string accessFunctionString = buildDataAccessFunctions (inputMemberData);
  GrammarString* sourceCodeString = new GrammarString(accessFunctionString);
  ROSE_ASSERT(sourceCodeString != NULL);
  sourceCodeString->setVirtual(pureVirtual);

  list<GrammarString*>& l = getMemberFunctionSourceList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
  if ( inputMemberData.automaticGenerationOfDataAccessFunctions == BUILD_WRAP_ACCESS_FUNCTIONS) {
     GrammarString* cur = l.back();
     string functionString = cur->functionNameString;
     string typeName     = inputMemberData.getTypeNameString();
     string t = string(typeName,0,typeName.size()-1);
     functionString = GrammarString::copyEdit (functionString,"$WRAP_TYPE", t);
     string variableName = inputMemberData.getVariableNameString();
     functionString = GrammarString::copyEdit (functionString,"$WRAP", variableName);
     functionString = GrammarString::copyEdit (functionString,"$WDATA",
                                 inputMemberData.getDefaultInitializerString());
     cur->functionNameString = functionString; 
  } 
  // Data access "functions" should be placed into the LOCAL_LIST since
  // they are accessable though the base classes by definition (of C++)
  Terminal::addElementToList ( l, *sourceCodeString );
  
#if 0
  inputMemberData.display("Inside of Terminal::setDataPrototype()");
  
  // The number of constraint objects (they can have NULL constraint strings internally) should
  // match that of the number of data members added in the construction of the terminals/nonterminals
  printf ("getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() = %d \n",
          getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size());
  printf ("constraintList.size() = %zu \n",constraintList.size());
  
  // Since constraints can be applied to data of base classes the size of the lists 
  // don't have to match so it is an error to assert that they should.
  ROSE_ASSERT ( getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() == constraintList.size()+1 );
  
  // Now add a contraint that this data member can use in to define additional grammars
  Constraint* dataMemberConstraint = new Constraint(inputMemberData.getVariableNameString());
  ROSE_ASSERT(dataMemberConstraint != NULL);
  constraintList.addConstraint(dataMemberConstraint);
  
  // The number of constraint objects (they can have NULL constraint strings internally) should
  // match that of the number of data members added in the construction of the terminals/nonterminals
  ROSE_ASSERT ( getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() == constraintList.size() );
#endif
}

void
Terminal::addConstraint ( const string& terminalName, const string& constraintString )
   {
     Constraint* dataMemberConstraint = new Constraint(terminalName,constraintString);
     ROSE_ASSERT(dataMemberConstraint != NULL);
     constraintList.addConstraint(dataMemberConstraint);
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
     switch (config)
        {
          case BUILD_ACCESS_FUNCTIONS:
          case BUILD_WRAP_ACCESS_FUNCTIONS:
               filename = "../Grammar/dataMemberAccessFunctions.macro";
               break;
          case BUILD_LIST_ACCESS_FUNCTIONS:
               filename = "../Grammar/listMemberAccessFunctions.macro";
               break;
          case BUILD_INDIRECT_ACCESS_FUNCTIONS:
               filename = "../Grammar/dataWrapAccessFunctions.macro";
               break;
          default:
               assert(false);
        }

     string markerString = "DATA_ACCESS_FUNCTION";

     bool pureVirtual = FALSE;
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
            // We can't assert that the input pointer is a valid pointer (at least not yet using SAGE2)
            // char* tempString = 
            //      "assert($DATA != NULL);\n     if ($DATA != NULL)\n          $DATA->set_parent(this);";
            // strings handed off to the copyEdit function must be allocated on the stack!
            // char* tempString = "     if ($DATA != NULL)\n          $DATA->set_parent(this);";
//             char* tempString = GrammarString::stringDuplicate("     if ($DATA != NULL)\n          $DATA->set_parent(this);");
            // Only set the parent pointer if it is null
            // char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if ( ($DATA != NULL) && ($DATA->get_parent() == NULL) )\n          $DATA->set_parent(this);\n#endif");
            // Always reset the parent pointer (what was previously implemented)
            // char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if ($DATA != NULL)\n        {\n          if ($DATA->get_parent() == NULL)\n               printf (\"parent currently null \\n\");\n            else\n               printf (\"parent previously set: reset \\n\");\n          $DATA->set_parent(this);\n        }\n#endif");
            // char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if ($DATA != NULL)\n          $DATA->set_parent(this);\n#endif");

#if 0
            // DQ (9/10/2004): Modified to avoid setting parents to types
//             char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if (($DATA != NULL) && (isSgType(this) == NULL))\n          $DATA->set_parent(this);\n#endif");
               char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if (($DATA != NULL) && (isSgType(this) == NULL) && (isSgSymbol(this) == NULL))\n          $DATA->set_parent(this);\n#endif");
#else
            // Use the older way of handling this (sometimes causes parents to be types (but for a class referenced in a typedef as in 
            //      typedef int (doubleArray::*doubleArrayMemberVoidFunctionPointerType) (void) const;
            // This seems to be OK, as long as the class declaration was a copy and not the original defining declaration
            // char* tempString = GrammarString::stringDuplicate("\n#ifndef REMOVE_SET_PARENT_FUNCTION\n     if ($DATA != NULL)\n          $DATA->set_parent(this);\n#endif");
               string tempString = "\n#if DEBUG_SAGE_ACCESS_FUNCTIONS\n     if (p_$DATA != NULL && $DATA != NULL && p_$DATA != $DATA)\n        {\n          printf (\"Warning: $DATA = %p overwriting valid pointer p_$DATA = %p \\n\",$DATA,p_$DATA);\n#if DEBUG_SAGE_ACCESS_FUNCTIONS_ASSERTION\n          printf (\"Error fails assertion (p_$DATA != NULL && $DATA != NULL && p_$DATA != $DATA) is false\\n\");\n          ROSE_ASSERT(false);\n#endif\n        }\n#endif";
#endif
               tempString = GrammarString::copyEdit (tempString,"$DATA",variableName);
               setParentFunctionCallString = tempString;
             }
        }

  // functionString = GrammarString::copyEdit (functionString,"$SET_PARENT_FUNCTION",setParentFunctionCallString);
     functionString = GrammarString::copyEdit (functionString,"$TEST_DATA_POINTER",setParentFunctionCallString);

#if 0
     if (functionString != NULL)
          printf ("functionString = \n%s\n",functionString);
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
     ConstructParamEnum  constructorParameter,
     BuildAccessEnum  buildAccessDataFunctions,
     bool  toBeTraversedDuringTreeTraversal,
     bool delete_flag,
     CopyConfigEnum   toBeCopied)
   {
#if 0
  // DQ (5/23/2006): This is commented out now to experiment with having it be sometimes true!
  // DQ & AJ (12/3/2004): Added assertion for error checking
     if (toBeTraversedDuringTreeTraversal == true)
        {
          ROSE_ASSERT(delete_flag == false);
        }
#endif

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

#if 0
  // This is not enough information to properly build a data member and it's access functions
void
Terminal::setDataPrototype ( const char* markerString, const char* filename )
   {
  // We might want to include the path name into the filename string
  // so we don't need to have a directory input parameter
     bool pureVirtual = FALSE;
     SETUP_MARKER_STRINGS_MACRO
     Terminal::addElementToList ( getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST),
                                           *codeString );

  // Once the string for the class declaration is built we have to 
  // construct one for the data source code list.  This will be a 
  // string representing an access function (get and set functions)
     buildGrammarStringForSourceList (inputMemberData);
   }
#endif

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
  // ROSE_ASSERT (grammarSubTree != NULL);
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
#if INSURE_BUG
void Terminal::addElementToList ( list<GrammarString *> & targetList, GrammarString & element )
#else
void Terminal::addElementToList ( list<GrammarString *> & targetList, const GrammarString & element )
#endif
   {
  // This function abstracts the details of adding GrammarString objects to the lists
  // that are stored internally.  It allows us to implement tests to check for redundencies in 
  // the list of elements (for now it allows us to test the != operator which will be used heavily later)

  // printf ("Inside of Terminal::addElementToList \n");

     int i = 0;
     bool duplicateEntryFound = FALSE;
     list<GrammarString *>::iterator stringListIterator;
  // Error checking (make sure the element is not already in the list!)
     for( stringListIterator = targetList.begin();
	  stringListIterator != targetList.end();
	  stringListIterator++ )
       {
	 if (*stringListIterator == &element)
	   {
	     printf ("Duplicate entry found in list! (targetList[%d] == element) \n",i);
	     duplicateEntryFound = TRUE;
	     
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
     if (duplicateEntryFound == FALSE)
       {
	 const GrammarString *const &tmpRef = new GrammarString(element);
	 targetList.push_back( (GrammarString *const &) tmpRef );
       }
   }

list<GrammarString *> & Terminal::getMemberFunctionPrototypeList(int i, int j) const
   {
  // printf ("Inside of Terminal::getMemberFunctionPrototypeList() \n");
     return (list<GrammarString *> &) memberFunctionPrototypeList[i][j];
  // return memberFunctionPrototypeList[i][j];
   }

list<GrammarString *> & Terminal::getMemberDataPrototypeList(int i, int j) const
   {
     return (list<GrammarString *> &) memberDataPrototypeList[i][j];
   }

list<GrammarString *> & Terminal::getMemberFunctionSourceList(int i, int j) const
   {
     return (list<GrammarString *> &) memberFunctionSourceList[i][j];
   }

list<GrammarString *> & Terminal::getEditSubstituteTargetList( int i, int j ) const
   {
     return (list<GrammarString *> &) editSubstituteTargetList[i][j];
   }

list<GrammarString *> & Terminal::getEditSubstituteSourceList( int i, int j ) const
   {
     return (list<GrammarString *> &) editSubstituteSourceList[i][j];
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

#if 0
// This is an older mechanism for putting state into the Terminal objects
// I think it was a bad direction and another mechanism using function 
// pointers was implemented.  The problem that we solved was that
// the initializers were only sometimes needed in the parameter strings 
// in constructors (needed them in prototypes, but not in constructor definitions).
void
Terminal::setIncludeInitializerInDataStrings ( bool X )
   {
     includeInitializerInDataStrings = X;
   }

bool
Terminal::getIncludeInitializerInDataStrings ()
   {
  // return includeInitializerInDataStrings;
     bool returnValue = FALSE;

     if (isChild() == TRUE)
          ROSE_ASSERT (automaticGenerationOfConstructor == getParent()->automaticGenerationOfConstructor);

     returnValue = includeInitializerInDataStrings;

     return returnValue;
   }
#endif

void
Terminal::editSubstitute ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = FALSE;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
   }

void
Terminal::editSubstituteSubTree        ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = FALSE;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
     
   }

void
Terminal::editSubstituteExclude        ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = FALSE;
  // char* returnString = "";

  // Call MACRO to read new string
     SETUP_MARKER_STRINGS_MACRO
     string newString = codeString->getRawString();
     editSubstitute (oldString,newString);
   }

void
Terminal::editSubstituteExcludeSubTree ( const string& oldString, const string& markerString, const string& filename )
   {
     bool pureVirtual = FALSE;
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

     bool returnValue = FALSE;
     if (isChild() == TRUE)
          ROSE_ASSERT (automaticGenerationOfDestructor == getParentTerminal()->automaticGenerationOfDestructor);

     returnValue = automaticGenerationOfDestructor;
     return returnValue;
   }

bool
Terminal::generateConstructor() const
   {
  // See description of setAutomaticGenerationOfConstructor(bool) above
  // return automaticGenerationOfConstructor;

     bool returnValue = FALSE;

  // We need to check if the setting of the value for "automaticGenerationOfConstructor"
  // is the same for both the parent and the child.
     if (isChild() == TRUE)
        {
          if (automaticGenerationOfConstructor != getParentTerminal()->automaticGenerationOfConstructor)
             {
               printf ("ERROR: child inconsistant with parent node: name = %s \n",name.c_str());
             }

          ROSE_ASSERT (automaticGenerationOfConstructor == getParentTerminal()->automaticGenerationOfConstructor);
        }

     returnValue = automaticGenerationOfConstructor;
     return returnValue;
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
  // return automaticGenerationOfDataAccessFunctions;

  // We need to check if the setting of the value for "automaticGenerationOfConstructor"
  // is the same for both the parent and the child.
     bool returnValue = FALSE;
     if (isChild() == TRUE)
          ROSE_ASSERT (automaticGenerationOfDataAccessFunctions == getParentTerminal()->automaticGenerationOfDataAccessFunctions);

     returnValue = automaticGenerationOfDataAccessFunctions;
     return returnValue;
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
  // return automaticGenerationOfCopyFunction;

  // We need to check if the setting of the value for "automaticGenerationOfConstructor"
  // is the same for both the parent and the child.
     bool returnValue = FALSE;
     if (isChild() == TRUE)
          ROSE_ASSERT (automaticGenerationOfCopyFunction == getParentTerminal()->automaticGenerationOfCopyFunction);

     returnValue = automaticGenerationOfCopyFunction;
     return returnValue;
   }

#if 0
void
Terminal::constrainName ( char* nameString )
   {
     ROSE_ASSERT (nameString != NULL);
     constraint.name = GrammarString::stringDuplicate(nameString);
   }
#endif

void
Terminal::consistencyCheck() const
   {
  // ROSE_ASSERT (grammarSubTree != NULL);

     ROSE_ASSERT (associatedGrammar != NULL);

  // The number of constraint objects (they can have NULL constraint strings internally) should
  // match that of the number of data members added in the construction of the terminals/nonterminals
  // This is not a correct test to assert (since it can be false for data within base classes)
  // ROSE_ASSERT ( getMemberDataPrototypeList(Terminal::SUBTREE_LIST,Terminal::INCLUDE_LIST).size() == constraintList.size() );

  // Call consistencyCheck on list data member
     constraintList.consistencyCheck();

  // Make sure that this terminal/nonterminal is contained in the
  // lists of terminals and nontrminals associated with this grammar
     ROSE_ASSERT ( associatedGrammar->isTerminal(name) || associatedGrammar->isNonTerminal(name) );

  // Numerous variables should be the same for the parent and child!
     if (isChild() == TRUE)
        {
          if (automaticGenerationOfDestructor != getParentTerminal()->automaticGenerationOfDestructor)
             {
               printf ("ERROR: child inconsistant with parent node: name = %s \n",name.c_str());
             }

          ROSE_ASSERT (automaticGenerationOfConstructor         == getParentTerminal()->automaticGenerationOfConstructor);
          ROSE_ASSERT (automaticGenerationOfDestructor          == getParentTerminal()->automaticGenerationOfDestructor);
          ROSE_ASSERT (automaticGenerationOfDataAccessFunctions == getParentTerminal()->automaticGenerationOfDataAccessFunctions);
          ROSE_ASSERT (automaticGenerationOfCopyFunction        == getParentTerminal()->automaticGenerationOfCopyFunction);
        }

#if 1
  // local index variable
     int j = 0, k = 0;

  // check the list for valid objects

     for (k=0; k < 2; k++)
          for (j=0; j < 2; j++)
             {
            // int i = 0;
               list<GrammarString *>::const_iterator it;
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
   }


void
Terminal::display( const string& label ) const
   {
     printf ("In Terminal::display ( %s ) \n",label.c_str());

     printf ("Name     = %s \n", getName().c_str());
     printf ("Lexeme   = %s \n", getLexeme().c_str());
     printf ("Tag Name = %s \n", getTagName().c_str());
     printf ("parentTerminal = %s \n", (parentTerminal != NULL) ? "<VALID POINTER>" : "<NULL POINTER>");
     if (parentTerminal != NULL)
        {
          printf ("PARENT TERMINAL: \n");
          parentTerminal->display("parentTerminal");
        }
   }


bool
Terminal::isChild() const
   {
  // This function reports if a terminal is a child of an existing terminal.  Child terminals are build to
  // provide the grammar with additional support for anotated (constrained) terminals.  For example,
  // the addition of a terminal using a constraint will generate child terminals for expressions and
  // statements representative of expressions andd statements using that type.

  // ROSE_ASSERT (isTemporary() == FALSE);
     return (parentTerminal == NULL) ? FALSE : TRUE;
   }


// MK: This function is used to check the lists of GrammarString objects that
// are used during the generation phase of the classes of the AST restructuring tool
void
Terminal::checkListOfGrammarStrings(list<GrammarString *>& checkList)
{
  // Check list for uniqueness and for elements of length 0
  list<GrammarString *>::iterator it;
  list<GrammarString *>::iterator jt;
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

     list<GrammarString *> copyList;
     list<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
     for (Terminal *t = this; t != NULL; t = t->grammarSubTree->hasParent() ?  &t->grammarSubTree->getParent().getToken() : NULL)
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
                         if ( (*stringListIterator)->generateDataAccessFunctions() != BUILD_INDIRECT_ACCESS_FUNCTIONS)
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
                              s += "                       std::cout << \"" + varTypeString + " p_" + varNameString + " --> \" << std::flush;\n" ;
                              s += "                       std::cout << \" not valid \" << std::endl;\n" ;
                              s += "                    } \n" ;
                              s += "             } \n" ;
#if 0
                              s += "          else \n" ;
                              s += "             { \n" ;
                              s += "                 std::cout << \"" + varTypeString + " p_" + varNameString + " --> NULL \" << std::endl;\n" ;
                              s += "             } \n" ;
#endif
                              s += "\n" ;
                            }
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
                         s += "          hash_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n" ;
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
     list<GrammarString *> copyList;
     list<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s("std::vector<std::pair<SgNode*,std::string> > returnVector;\n") ;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
  // AS Iterate over the terminal and its parents (base-classes).
  // PS! Everything is treated as terminals; even non-terminals
  // printf ("Derived Class terminal name = %s \n",name);

     for (Terminal *t = this; t != NULL; t = t->grammarSubTree->hasParent() ?  &t->grammarSubTree->getParent().getToken() : NULL)
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
                         if ( (*stringListIterator)->generateDataAccessFunctions() != BUILD_INDIRECT_ACCESS_FUNCTIONS)
                            {
                              
                              s += "          returnVector.push_back(pair<SgNode*,std::string>( p_" + varNameString + ",\""+varNameString+"\"));\n";
                            }
                    
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
*  supports buildReturnDataMemberReferenceToPointers() by building the string representing the code 
*  necessary to return references (pointers) all data member pointers to IR nodes contained in STL lists.
*************************************************************************************************/
string Terminal::buildListIteratorStringForReferenceToPointers(string typeName, string variableName, string classNameString)
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
               returnString += "     if (" + originalList + "== NULL)\n";
            // Return a NULL pointer so that the graph shows that this pointer equals NULL
               returnString += "        returnVector.push_back(pair<SgNode**,std::string>( NULL,\""+variableName+"\"));\n";

               returnString += "     else\n";
             }

          returnString += "     for ( " +iteratorBaseType + "::const_iterator " + iteratorName + " = " + originalList + accessOperator + "begin() \n"
                       + "; " + iteratorName
                       + " != " + originalList + accessOperator + "end(); ++" 
                       + iteratorName + ") \n        { \n";

       // Declare the a loop variable (reference to current element of list)
          returnString += "          returnVector.push_back(pair<SgNode**,std::string>( (SgNode**)(&(*" + iteratorName + ")),\""+variableName+"\"));\n";
                
       // close off the loop
          returnString += "        } \n";

        }
     return returnString;
   }

//DQ (4/30/2006): This function is similar to buildReturnDataMemberPointers but returns reference to the pointers.
/*************************************************************************************************
*  The function
*       Terminal::buildReturnDataMemberReferenceToPointers()
*  builds the code for returning references to all data member pointers to IR nodes in the AST.
*************************************************************************************************/
string
Terminal::buildReturnDataMemberReferenceToPointers ()
   {
  // DQ (4/30/2006): This is a modified version of the code for buildReturnDataMemberPointers()

  // AS (2/14/2006): This function generates the code for each IR node to return
  // a pair of all data members pointing to IR nodes and their name to:
  //   1) Graph the whole AST
  //   2) Create a general mechanism to do the mechanism mentioned above
  //
     list<GrammarString *> copyList;
     list<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
     string s("std::vector<std::pair<SgNode**,std::string> > returnVector;\n") ;
  // s += "   std::cout << \"------------ checking pointers of " + classNameString + "  -------------------\" << std::endl;\n" ;
  // s += "   ROSE_ASSERT ( pointer->p_freepointer == AST_FileIO::IS_VALID_POINTER() );\n";
  // AS Iterate over the terminal and its parents (base-classes).
  // PS! Everything is treated as terminals; even non-terminals
  // printf ("Derived Class terminal name = %s \n",name);

     for (Terminal *t = this; t != NULL; t = t->grammarSubTree->hasParent() ?  &t->grammarSubTree->getParent().getToken() : NULL)
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
                 // AS Check to see if it this is an pointer to any IR-node. The varTypeString == "$CLASSNAME *" checks to see if it
                 // is a ir-node pointer which is *not* yet replaced. $CLASSNAME is inside the string and will later be replaced with 
                 // e.g SgTypeInt etc. 'varTypeString.substr(0,15) == "$GRAMMAR_PREFIX"' checks to see if it is part of the grammar.
                 // 'varTypeString.substr(0,2) == "Sg" ' and to see if it is a Sg node of some type.
                    s += buildListIteratorStringForReferenceToPointers(varTypeString, varNameString,classNameString);

                    if ( (varTypeString == "$CLASSNAME *" ) || ( ( ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) || ( varTypeString.substr(0,2) == "Sg" ) ) && typeIsStarPointer ) )
                       {
                      // AS Checks to see if the pointer is a data member. Because the mechanism for generating access to variables
                      // is the same as the one accessing access member functions. We do not want the last case to show up here.
                         if ( (*stringListIterator)->generateDataAccessFunctions() != BUILD_INDIRECT_ACCESS_FUNCTIONS)
                            {
                              s += "          returnVector.push_back(pair<SgNode**,std::string>( (SgNode**)(&(p_" + varNameString + ")),\""+varNameString+"\"));\n";
                            }
                       }
                      else
                       {
                         if( varTypeString.find("rose_hash_multimap") != std::string::npos )
                            {
                           // AS(02/24/06) Generate code for returning data member pointers to IR
                              std::string accessOperator = ".";
                              accessOperator = "->";
#if 0
                           // DQ (5/22/2007): Old code with redundant test (unclear logic)
                              if ( varTypeString.find("rose_hash_multimap*") != std::string::npos )
                                 {
                                   accessOperator = "->";
                                   s += "     if (p_" + varNameString + " == NULL)\n";
                                   s += "        {\n";
                                // Return a NULL pointer so that th graph shows that this pointer equals NULL
                                   s += "        returnVector.push_back(pair<SgNode**,std::string>( NULL,\"" + varNameString + "\"));\n";
                                   s += "        }\n";
                                   s += "     else\n";
                                 }
#else
                              s += "     if (p_" + varNameString + " == NULL)\n";
                              s += "        {\n";
                           // Return a NULL pointer so that th graph shows that this pointer equals NULL
                              s += "          returnVector.push_back(pair<SgNode**,std::string>( NULL,\"" + varNameString + "\"));\n";
                              s += "        }\n";
                              s += "     else\n";
#endif
                              s += "        {\n";
                              s += "          for ( rose_hash_multimap::const_iterator it_"+varNameString+ "= p_" + varNameString + accessOperator + "begin(); it_" + varNameString + "!= p_" + varNameString + accessOperator + "end(); ++" + "it_" + varNameString + ")\n";
                              s += "             {\n";
                           // Declare the a loop variable (reference to current element of list)
                           // s += "          returnVector.push_back(pair<SgNode**,std::string>( &(it_" + varNameString + "->second), std::string(it_"+varNameString+"->first.str()) ) );\n";
                           // s += "          returnVector.push_back(pair<SgNode**,std::string>( NULL , std::string(it_"+varNameString+"->first.str()) ) );\n";
                           // s += "          returnVector.push_back(pair<SgNode**,std::string>( (SgNode**)(&(*it_" + varNameString + "->second)), std::string(it_"+varNameString+"->first.str()) ) );\n";
                              s += "               returnVector.push_back(pair<SgNode**,std::string>( (SgNode**)(&(it_" + varNameString + "->second)), std::string(it_" + varNameString + "->first.str()) ) );\n";
                           // close off the loop
                              s += "             }\n";
                              s += "        }\n";
                            }
                       }
                  }
             }
        }

     s += "     return returnVector;\n";

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

     list<GrammarString *> copyList;
     list<GrammarString *>::iterator stringListIterator;

     string classNameString = this-> name;
  // string s = "int indexCounter = 0, returnValue = -1;\n";
     string s = "int indexCounter = 0;\n";

     for (Terminal *t = this; t != NULL; t = t->grammarSubTree->hasParent() ?  &t->grammarSubTree->getParent().getToken() : NULL)
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
                         if ( (*stringListIterator)->generateDataAccessFunctions() != BUILD_INDIRECT_ACCESS_FUNCTIONS)
                            {
                           // s += "     if ( p_" + varNameString + " == childNode ) { returnValue = indexCounter; } indexCounter++;\n";
                              s += "     if ( p_" + varNameString + " == childNode ) { return indexCounter; } indexCounter++;\n";
                            }
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
