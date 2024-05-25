/**
 * \file LocalRWSetGenerator
 *
 * \brief Write out the Read/Write sets for each function, but not from the called functions.
 *
 * This tool writes out Read/Write sets for each function to a json file.  This 
 * means that each function has 2 sets, one containing the variables it reads, 
 * and the other containing the variables it writes.  
 * There are a couple of notes about the format that are useful:
 *  1. If a variable name starts with '!' an error occurred on determining 
 *     that varialbe and the read/write sets for the function are INVALID.  
 *     For example:
 *     "!ERROR: Call to undefined function: ::vxsimHostPrintf Invalid R/W sets!"
 *     Despite this the rest of the R/W set will be generated as well as possible.
 *  2. Local variable and function arguments get their function name surrounded 
 *     by "@".  Because we combine the sets of called functions into the caller's 
 *     set, we need some way to designate which function those locals belonged to.  
 *     So a local variable references look like this: "::sqr": ["@::sqr@x" ],
 *  3. Member variables are prepended by the class they come from with
 *     '$' symbols, for the same reason as the @ signs.  
 *
 *
 * \author Jim Leek
 * \date   01-19-2022
 * Copyright:  (c) 2011-2022 Lawrence Livermore National Security.
 **/

#include <sage3basic.h>
#include <sageInterface.h>
//~ #include <VariableIdMapping.h>
#include <Rose/CommandLine.h>
#include <Sawyer/Graph.h>
#include <Sawyer/Message.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include "LocalRWSetGenerator.h"
#include "SageUtils.h"
#include "VxUtilFuncs.h"
#include <unordered_map>
#include <unordered_set>
#include <boost/filesystem.hpp>
#include "RoseAst.h"
#include <nlohmann/json.hpp>
#include <Sawyer/Yaml.h>
#include <Rose/AST/NodeId.h>



using namespace Rose;
using namespace SageInterface;
using namespace Sawyer::Message::Common;
using namespace nlohmann;
using namespace ReadWriteSets;
using namespace VxUtilFuncs;
using namespace Sawyer::Message;
namespace fs = boost::filesystem;
namespace ast = Rose::AST;

/**
 * \function isJustALocalStructDecl
 * 
 * This is an attempt to get around a bug in the RWSets that come back
 * from Qing's code.  If a struct is declared inside a function, that
 * comes back as a write to just an SgInitializedName.  Which gets
 * labeled as a local variable, sans the surrounding struct.  
 * If it's a struct variable, we only want it if it's actually in an
 * object access, not just when it's declared.
 * So, to figure this out, I'm going to try to check for this pattern:
   |___ SgBasicBlock
        |___ SgClassDeclaration
             |___ SgClassDefinition
                  |___ SgVariableDeclaration
                       |___ SgInitializedName 
 *
 * test is struct5.c
**/
bool isJustALocalStructDecl(SgInitializedName* initName) 
{
  if(initName->get_parent()->variantT() == V_SgVariableDeclaration) {
    SgVariableDeclaration* varDecl = isSgVariableDeclaration(initName->get_parent());
    if(varDecl && varDecl->get_parent()->variantT() == V_SgClassDefinition) {
      SgClassDefinition* clsDef = isSgClassDefinition(varDecl->get_parent());
      SgBasicBlock* bb = SageInterface::getEnclosingNode<SgBasicBlock>(clsDef);
      if(bb) {
        return true;
      }
    }
    
  }

  return false;
}


/**
 * \function isFunctionPointer
 * 
 * When a function pointer reference is found in the code, Qing's RWSet
 * code returns a bad SgVarRefExp.  Which naievely would just be a 
 * normal variable read.
 * So we need to look up the tree to see if the SgVarRefExp is
 * contained in an SgFunctionCallExp or not.  We walk up the parents
 * until either:
 * A) We find an SgBasicBlock, in which case we return null.
 * B) Find an SgAssignInitializer, in which case we return null
 * (constructor initlaizer list)
 * C) We find an SgFunctionCallExp, in which case we need to do some
 * more checking.
 *
 * The problem is that any member function call (s.foo()), or function
 * with arguments (foo(x)) will have a successor node that is an
 * SgVarRefExp.   So we can't look for an SgVarRefExp.  But a normal
 * function call will always have some kind of SgFunctionRefExp
 * somewhere in the tree decending from funcCall->get_function();  So
 * we look for that.  If there is a SgFunctionRefExp, it's NOT a
 * function pointer.  If we can't find anything like that, I think it
 * is a function pointer call.
 *
 * (I did try looking for OUR SgVarRefExp in the get_function(), but
 * that also caught s.foo() )
 *
 * \param[in] varRef: The SgVarRefExp we got from the readSet
 * \return: SgFunctionCallExp* if found, NULL otherwise
 *
 **/
bool LocalRWSetGenerator::isFunctionPointer(SgVarRefExp* inVarRef) 
{
  SgNode* current = inVarRef->get_parent();
  SgNode* previous = inVarRef;
  //SgBasicBlock shows this is not a function call exp, so not a
  //function pointer call.
      
  while(current != nullptr && current->variantT() != V_SgBasicBlock && current->variantT() != V_SgAssignInitializer) {

    if(current->variantT() == V_SgFunctionCallExp) {
      //We found a function call, now check if it references an actual
      //function, if it does, it is NOT a function pointer.  If it
      //only references SgVarRefExps, I'm guessing it is a function pointer
      SgFunctionCallExp* funcCall = isSgFunctionCallExp(current);
      SgExpression* funcExp = funcCall->get_function();

      //I wish these shared a type I could check for
      Rose_STL_Container<SgNode*> funcList = NodeQuery::querySubTree (funcExp, V_SgFunctionRefExp);
      if(!funcList.empty()) 
      {
        return false;
      }
      Rose_STL_Container<SgNode*> memberList = NodeQuery::querySubTree (funcExp, V_SgMemberFunctionRefExp);
      if(!memberList.empty()) 
      {
        return false;
      }
      //Note: I used to have SgTemplateFunctionRefExp and
      //SgTemplateMemberFunctionRefExp here, but it turns out those
      //can't ever appear because I don't handle uninstantited templates.

      //If we didn't find any FunctionRefExps, there must only be SgVarRefExp, so not a function call
      return true;
            
    }
    
    previous = current;
    current = current->get_parent();
  }
  return false;
}

/**
 * In the read/write sets a plain function call like "foo()" is
 * basically never useful.  So we take plain functions out. 
 * However, member functions and functions involved in expressions are
 * useful.  For example "a->foo()" should be in the read set.
 * As should "a->foo().b"
 *
 **/
void filterPlainFunctions(std::set<ReadWriteSets::AccessSetRecord>& thisSet) 
{
  for (auto it = thisSet.begin(); it != thisSet.end(); ) {
    if(it->varType == FUNCTIONS && it->fields.size() == 0) {
      it = thisSet.erase(it);
    } else {
      ++it;
    }
  }
}

/**
 * Qing has started conservatively returning a->foo() as a part of the
 * write set, on the idea that foo might modify a.  But I take care of
 * that case in TestabilityGrader, so I don't want a->foo() in the
 * write set.  (It should be in the read set though.)
 *
 * So the algorithm is, recursively enter the AccessSetRecord.
 * Delete any nodes that 
 **/
void filterOutThisNodeDueToMemberFunction(std::set<ReadWriteSets::AccessSetRecord>& thisSet, ReadWriteSets::FunctionReadWriteRecord& record) 
{
  for (auto curNode = thisSet.begin(); curNode != thisSet.end(); ) {
    std::set<ReadWriteSets::AccessSetRecord>& childSet = curNode->fields;
    filterOutThisNodeDueToMemberFunction(childSet, record);
    
    //This filter deletes items as it recurses back out.  So it deleted the Member function, and deletes the parents if they didn't
    //also point at anything else.
    if(childSet.size() == 0 && 
       (curNode->varType == MEMBER_FUNCTIONS || curNode->accessType == POINTER_ARROW || curNode->accessType == FIELD_ACCESS)) {
      //We should make sure the function declaration is in the called
      //functions set, so if it's a member function,
      if(curNode->varType == MEMBER_FUNCTIONS) {
        SgFunctionDeclaration* calledFuncDecl = isSgFunctionDeclaration(curNode->nodePtr);
        record.calledFunctions.insert(VxUtilFuncs::compileInternalFunctionName(calledFuncDecl, VxUtilFuncs::getNodeRelativePath(calledFuncDecl)));
      }
      curNode = thisSet.erase(curNode);
    } else {
      ++curNode;
    } 
  }
}




/**
 * This function attempts to insert a node into a read or writeset
 * or a message explaining why it can't. This can be quite a fraught 
 * process. 
 *
 * \param[in] funcDef: The function definition the read/write was 
 *                     found in.  Mostly used for scope info
 * \param[in] current: The node given as a read or write.  Not always 
 *                     sensible.
 *
 **/
/*std::string LocalRWSetGenerator::generateAccessNameStrings(SgFunctionDefinition* funcDef, SgInitializedName* coarseName) 
{

  SgStatement* coarseNameScope = coarseName->get_scope();
  SgType* coarseType = coarseName->get_type();
  ROSE_ASSERT(coarseType);

  //global case
  if(isSgGlobal(coarseNameScope)) {
    // If it's a class type, need to get the full access string
    if(isSgClassType(coarseType)) {
      return "::"+coarseName->unparseToString();
    } else {
      return coarseName->get_qualified_name().getString();
    }
  } else if(isSgClassDefinition(coarseNameScope)) {
    
    SgClassDefinition* clsDef = isSgClassDefinition(coarseNameScope);
    if(clsDef) {
      std::string clsName = clsDef->get_qualified_name();
      return "$" + clsName + "$" + coarseName->unparseToString();
    } else {
      return coarseName->get_qualified_name().getString();
    }
  
      
    //Otherwise, check that it's a local variable or an argument
  } else {  
    bool isDefAncestor = isAncestor(funcDef, coarseName);
    //A little annoying, any parameter arguments will be associated with the declaration attached to
    //the definition, not the first non-defining declaration.  So get the parameter list that way
    SgFunctionDeclaration* funcDecl =funcDef->get_declaration();
    bool isDeclAncestor = isAncestor(funcDecl, coarseName);  //Writing function arguments is OK.
    if(!isDefAncestor && !isDeclAncestor) {
      return "!access of non-local variable " + coarseName->get_qualified_name().getString();
    }
    if(isSgClassType(coarseType)) {
      return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
        coarseName->unparseToString();
    } else {
      return "@" + VxUtilFuncs::getExpandedFunctionName(funcDecl) + "@" + 
        coarseName->get_qualified_name().getString();        
    }
  }
  
    mlog[Sawyer::Message::Common::ERROR] <<
    "generateAccessNameStrings: " <<
    coarseName->get_file_info()->get_filename() << ":" <<
    coarseName->get_file_info()->get_line() << "-" << coarseName->get_file_info()->get_col()<<std::endl;
  mlog[Sawyer::Message::Common::ERROR] <<
    "In generateAccessNameStrings: got to end of function:"  <<
    coarseName->class_name()<<std::endl;
  ROSE_ABORT();
}
*/


/**
 * This function attempts to determine the AccessType of a given access.
 * It has to be done recursively.  For example, if an array is seen,
 * we recursively check the element type of the array.
 *
 * \param[in] curType: Most coarse grain view of the variable written
 * to.  So foo.bar.a, we start from foo.  Then recurively walk foo,
 * then bar, looking for pointers.
 **/
VarType LocalRWSetGenerator::determineType(SgType* curType) {
  //Aside from cases where we know exactly what the type is, we're going to have
  //to match along looking for the worst possible type.  (For example, is a struct a
  //simple struct or a complex one?  We can only tell by marching over the whole classTree
  //tooking for a pointer.
  //So we start with the lowest possible type, and everytime we see a worse one, we upgrade it.
  VarType returnType = PRIMITIVES;
  if(isSgArrayType(curType)){
    SgArrayType* arrayType = isSgArrayType(curType);
    VarType elemType = determineType(arrayType->get_base_type());
    if(elemType <= ARRAYS) {
      return ARRAYS;
    }
    return ARRAYS_OF_STRUCTS;        
  }
  else if(isSgModifierType(curType)){
    SgType* baseType = curType->stripType(SgType::STRIP_MODIFIER_TYPE);
    return determineType(baseType);
  }
  else if(isSgReferenceType(curType)){
    SgType* baseType = curType->stripType(SgType::STRIP_REFERENCE_TYPE);
    return determineType(baseType);
  }
  else if(isSgTypedefType(curType)){
    SgType* baseType = curType->stripType(SgType::STRIP_TYPEDEF_TYPE);
    return determineType(baseType);
  }
  
  else if(isSgClassType(curType)) {
    
    SgClassType* classType = isSgClassType(curType);
    SgDeclarationStatement* sgDeclStmt = classType->get_declaration();
    SgClassDeclaration* sgClassDecl = isSgClassDeclaration(sgDeclStmt);
    ROSE_ASSERT(sgClassDecl);
    SgClassDefinition* sgClassDef = getClassDefinitionFromDeclaration(sgClassDecl);
    if(sgClassDef == NULL) {
      mlog[WARN] << "In determineType: Unable to get class definition: " << sgClassDecl->get_qualified_name().getString() << std::endl;
      return VARTYPE_UNKNOWN; //If we can't define the class, it's unmockable
    }
    return STRUCTS;

  }
  else if(isSgPointerType(curType)) {
    //Saw a pointer in class, but it wasn't dereferenced, so it's a primative?
    return POINTERS;
  }
  else if(curType->isPrimativeType()) {
    return PRIMITIVES;
  }
  else if(isSgEnumType(curType)) {
    return PRIMITIVES;
  }
  else {
    // This happens with ellipses (...) in function arguments.  We
    // don't support those yet, so there's nothing to do about it.
    // The function used with ellipses (va_start etc.) produce
    // SgTypeDefault, which I can't do anything with.  So leave this
    // warning here for now. -Jim 2023/06/14
    // tests:   ellipses_1.cxx  ellipses_2.c
    mlog[WARN] << "In determineType got unexpected type " << curType->class_name() << std::endl;
    Sg_File_Info* finfo = curType->get_file_info(); 
    if(finfo) {
      mlog[WARN] <<  VxUtilFuncs::getNodeRelativePath(curType) << ":" << finfo->get_line() << std::endl;
    } else {
      mlog[WARN] << "FileInfo was null" << std::endl;
    }
    return VarType::VARTYPE_UNKNOWN;
  }
  return returnType; //Return whatever the worst type was.
    
}


/**
 * This code was mostly taken from sageInterface.C SageInterface::convertRefToInitializedName
 * It was originally written by Leo.  But his version doesn't give back as much information
 * as I need.
 *
 * This is a further improvment that returns every step in a call like
 * this->a.b.c, rather than just either this, or c, as the original
 * did.
 *
 * Variable references can be introduced by SgVarRef, SgPntrArrRefExp,
 * SgInitializedName, SgMemberFunctionRef etc. This function will
 * convert them all to  a top level SgInitializedName, except an
 * thisExp. It makes an AccessSetRecord for each one and returns it.
 *
 * HOWEVER, in some cases the reference returned IS NOT a
 * SgInitializedName!  The normal case is something like foo.bar()  In
 * this case Qing's code considered bar() a reference and passes it
 * back, but it's NOT an SgInitializedName.  So we just ignore that, 
 * and a couple of other cases...
 *
 * \param[in] funcDef: The function where this statement comes from.  Required for determining
 *                     the scope of the reference
 * \param[in] current: The SgNode we're trying to turn into one or more
 *                     InitializedNames
 * \param[in] accessOrigin: The original statement.  Used for making notestring, maybe delte
 * \param[in] field: Due to the way ROSE lays out SgDotExp on the AST,
 * I need to assemble the fields in BFS order instead of DFS, so I
 * have to pass the field Set record in (if there is one.)
 *
 * \return: A set of AccessSetRecords: I really didn't want to return a vector, but it's the best way to
 * get both sides of a binary op, and merge everything easily
 */

std::set<ReadWriteSets::AccessSetRecord> LocalRWSetGenerator::recursivelyMakeAccessRecord(SgFunctionDefinition* funcDef, SgNode* current, SgNode* accessOrigin, SgThisExp** thisFuncThis)
{
    ROSE_ASSERT(current != NULL);

    if (isSgInitializedName(current))
    {
      SgInitializedName* curName = isSgInitializedName(current);
      //Kludge to get around a problem with processing constructor
      //initilaizer lists.  Qing's code gives me an SgInitializedName,
      //but it's a different name than you get from the symbol table,
      //so it has a different NodeId.  So we use the symbol table to
      //get the declaration's SgInitializedName, used as canonical
      if(isSgCtorInitializerList(curName->get_parent())) 
      {
        SgVariableSymbol* varSym = funcDef->get_scope()->lookup_variable_symbol(curName->get_name());
        //If varSym is found, the rest should be true, but may as
        //well check everything before proceeding.
        if(varSym && isSgInitializedName(varSym->get_declaration())) {
          SgInitializedName* testCurName = isSgInitializedName(varSym->get_declaration());
          ROSE_ASSERT(curName); 
          curName = testCurName;
          current = curName;

          //We can get function calls and constructor calls inside of a CtroInitialzierList, handling that here.  (Maybe should be broken out)
        } else {
          SgConstructorInitializer* initptr = isSgConstructorInitializer(curName->get_initptr());
          if(initptr) {
            SgMemberFunctionDeclaration* calledFuncDecl = initptr->get_declaration();
            //if(funcSym && isSgFunctionDeclaration(funcSym->get_declaration())) {
            //SgFunctionDeclaration* calledFuncDecl = isSgFunctionDeclaration(funcSym->get_declaration());
            if(calledFuncDecl == NULL) {
              std::stringstream ss;  //I don't think this can happen
              ss << "recursivelyMakeAccessRecord: Got call expression in a CtorInitilaizer," << std::endl;
              ss << "but was unable to resolve the function." << std::endl;
              ss << "Relevent file info: " << current->get_file_info()->get_filename() << ":" <<
                current->get_file_info()->get_line() << "-" << current->get_file_info()->get_col()<< std::endl;
              mlog[Sawyer::Message::Common::WARN] << ss.str();
              mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
              mlog[Sawyer::Message::Common::WARN] << current->unparseToString() << std::endl;
              return std::set<ReadWriteSets::AccessSetRecord> {
                AccessSetRecord(current, "UNKNOWN", GLOBALITY_UNKNOWN, VARTYPE_UNKNOWN, ACCESSTYPE_UNKNOWN, "", "", ss.str()) }; 
            }
            std::string name = VxUtilFuncs::compileInternalFunctionName(calledFuncDecl, VxUtilFuncs::getNodeRelativePath(calledFuncDecl));
            std::string noteStr;
            Globality globality = MEMBERS;
            VarType varType = MEMBER_FUNCTIONS;
            return std::set<ReadWriteSets::AccessSetRecord> {
              AccessSetRecord(calledFuncDecl, name, globality, varType, NORMAL, "", "", noteStr) };
          } else {          
            mlog[Sawyer::Message::Common::WARN] << "Unknown symbol CtorInitializationList.  Currently Unhandled " << funcDef->get_qualified_name().getString() << std::endl;
            return std::set<ReadWriteSets::AccessSetRecord>();
          } 
        }
      }
      
      
      //Continuing with a variable
      std::string noteStr;
      Globality globality = ReadWriteSets::determineGlobality(funcDef, current, accessOrigin, noteStr);
      VarType varType = determineType(curName->get_type());
      std::string name = VxUtilFuncs::generateAccessNameStrings(curName);
      SgType* thistype = curName->get_type();
      SgType* disambiguatedType = VxUtilFuncs::getTypedefBaseType(thistype); 
      std::string typeName = disambiguatedType->unparseToString();
      std::string filename;
      //Bug workaround
      if(isJustALocalStructDecl(curName)) {
        return std::set<ReadWriteSets::AccessSetRecord>();
      }
      

      if(curName->get_definition() && curName->get_definition()->get_file_info()) {        
        filename = curName->get_definition()->get_file_info()->get_filename();
      } else if(funcDef->get_file_info()) {
        mlog[Sawyer::Message::Common::WARN] << "Could not get definition for variable " << name << std::endl;
        filename = funcDef->get_file_info()->get_filename();
      } else {
        mlog[Sawyer::Message::Common::WARN] << "Could not get definition for variable " << name << std::endl;
        mlog[Sawyer::Message::Common::WARN] << "Or for the function it was accessed from " << name << std::endl;
        filename = "UNKNOWN";
      }
      
      
      //We make the access NORMAL here for now.  If there's a & or *
      //the type will be changed as we unwind up the stack.
      return std::set<ReadWriteSets::AccessSetRecord> { AccessSetRecord(current, name, globality, varType, NORMAL, typeName, filename, noteStr) };
      
    }

    //Non member functions are not related to variables, so ignore them
    else if (isSgFunctionRefExp(current) != NULL) {
      //Got an access to a normal function, so we don't really need
      //type and filename, but maybe I should get filename from the
      //class definition anyway. -Jim Leek
      SgFunctionRefExp* callExp = isSgFunctionRefExp(current);
      Globality globality = GLOBALS;
      VarType varType = FUNCTIONS;
      AccessType accessType = NORMAL;
      SgFunctionDeclaration* calledFuncDecl = VxUtilFuncs::getUniqueDeclaration(callExp);
      if(calledFuncDecl == NULL) {
        std::stringstream ss;
        ss << "recursivelyMakeAccessRecord: Got call expression," << std::endl;
        ss << "but was unable to resolve the function. Maybe a Function Pointer?" << std::endl;
        ss << "Relevent file info: " << current->get_file_info()->get_filename() << ":" <<
          current->get_file_info()->get_line() << "-" << current->get_file_info()->get_col()<< std::endl;
        mlog[Sawyer::Message::Common::WARN] << ss.str();
        mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << current->unparseToString() << std::endl;
        return std::set<ReadWriteSets::AccessSetRecord> {
          AccessSetRecord(current, "UNKNOWN", GLOBALITY_UNKNOWN, VARTYPE_UNKNOWN, ACCESSTYPE_UNKNOWN, "", "", ss.str()) }; 
        
      }
      std::string name = VxUtilFuncs::compileInternalFunctionName(calledFuncDecl, VxUtilFuncs::getNodeRelativePath(calledFuncDecl));
      std::string noteStr;
      return std::set<ReadWriteSets::AccessSetRecord> {
          AccessSetRecord(calledFuncDecl, name, globality, varType, accessType, "", "", noteStr) }; 
    } 
    //The constructor initializer itself doesn't tell us anything, so
    //just ignore it.  Whatever it's constructing will be referenced,
    //and that will tell us accessType and varType  
    else if (isSgConstructorInitializer(current) != NULL) {
      return std::set<ReadWriteSets::AccessSetRecord>();
    } 
    else if(isSgMemberFunctionRefExp(current) != NULL) {
        //Got an access to a member function, so we don't really need
        //type and filename, but maybe I should get filename from the
        //class definition anyway. -Jim Leek
      SgMemberFunctionRefExp* callExp = isSgMemberFunctionRefExp(current);
      Globality globality = MEMBERS;
      VarType varType = MEMBER_FUNCTIONS;
      AccessType accessType = NORMAL;
      SgFunctionDeclaration* calledFuncDecl = VxUtilFuncs::getUniqueDeclaration(callExp);
      if(calledFuncDecl == NULL) {
        std::stringstream ss;
        ss << "recursivelyMakeAccessRecord: Got call expression on a dot expression," << std::endl;
        ss << "but was unable to resolve the function. Maybe a Function Pointer?" << std::endl;
        ss << "Relevent file info: " << current->get_file_info()->get_filename() << ":" <<
          current->get_file_info()->get_line() << "-" << current->get_file_info()->get_col()<< std::endl;
        mlog[Sawyer::Message::Common::WARN] << ss.str();
        mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << current->unparseToString() << std::endl;
        return std::set<ReadWriteSets::AccessSetRecord> {
          AccessSetRecord(current, "UNKNOWN", GLOBALITY_UNKNOWN, VARTYPE_UNKNOWN, ACCESSTYPE_UNKNOWN, "", "", ss.str()) }; 
        
      }
      std::string name = VxUtilFuncs::compileInternalFunctionName(calledFuncDecl, VxUtilFuncs::getNodeRelativePath(calledFuncDecl));
      std::string noteStr;
      return std::set<ReadWriteSets::AccessSetRecord> {
          AccessSetRecord(calledFuncDecl, name, globality, varType, accessType, "", "", noteStr) }; 
    }
    else if(isSgTemplateMemberFunctionRefExp(current) != NULL || isSgTemplateFunctionRefExp(current) != NULL) {
      //I used to have a case to handle this, but it turns out that
      //TemplateFunctionRefExp only happen when an uninstantiated
      //template calls a templated function.  Which should never been
      //seen in the RWSets.  So this is left here as a safetly check.
      mlog[Sawyer::Message::Common::ERROR] << "We got an SgTemplateMemberFunctionRefExp, which shouldn't be possible.  Contact ROSE team."  << std::endl;
      ROSE_ABORT();      
    }
    
    else if (isSgPntrArrRefExp(current) != NULL)
    {
        //Weird name, but this represents arr[0] like expressions
        //I'm not sure if arr[0] should count as a pointer or not.  
        //Let's say 'no' for now. --JL 2021
        bool suc=false;
        SgExpression* exp = isSgExpression(current);
        ROSE_ASSERT(exp != NULL);
        SgExpression* nameExp = NULL;
        suc = SageInterface::isArrayReference(exp,&nameExp);
        ROSE_ASSERT(suc == true);
        // has to resolve this recursively
        std::set<ReadWriteSets::AccessSetRecord> records = recursivelyMakeAccessRecord(funcDef, nameExp, accessOrigin, thisFuncThis);
        return updateAccessVarTypes(records, ARRAYS, ARRAY_INDEX);
/**
   Jim Leek 2023/03/02: Matt decided he doesn't want this array index
   exp stuff after all.
        bool suc=false;
        SgExpression* exp = isSgExpression(current);
        ROSE_ASSERT(exp != NULL);
        SgExpression* nameExp = NULL;
        std::vector<SgExpression*> subscripts; //Subscripts to this array (there can be more than one?)
        std::vector<SgExpression*>* ptr_subscripts = &subscripts; //I don't know why this call required a double pointer to a vector.  Weird.
        suc = SageInterface::isArrayReference(exp,&nameExp, &ptr_subscripts);
        ROSE_ASSERT(suc == true);
        // has to resolve this recursively
        std::set<ReadWriteSets::AccessSetRecord> records = recursivelyMakeAccessRecord(funcDef, nameExp, accessOrigin, thisFuncThis);
        records = updateAccessTypes(records, ARRAYS);
        
        for (SgExpression* idxExpr : subscripts) {
          std::string noteStr;
          Globality globality = records.begin()->globality;
          AccessType accessType = ARRAY_INDEX_EXPRESSIONS;
          std::string name = idxExpr->unparseToString();
          std::set<ReadWriteSets::AccessSetRecord> inner {
          AccessSetRecord(idxExpr, name, globality, accessType, "", "",  noteStr) };
          leafIndexInsert(records, inner);
        }
        return records;
**/
    }
    else if (isSgVarRefExp(current) != NULL)
    {
      //First check for function pointer, if not, assume normal access
      SgVarRefExp* varRef = isSgVarRefExp(current);
      
      std::set<ReadWriteSets::AccessSetRecord> res = recursivelyMakeAccessRecord(funcDef, varRef->get_symbol()->get_declaration(), accessOrigin, thisFuncThis);

      if(isFunctionPointer(varRef)) {
        //We think this is a pointer deref, so update whatever came back
        res = updateAccessTypes(res, FUNCTION_POINTER_DEREF);
      }
      
      return res;
      
    }
    else if (isSgNonrealRefExp(current) != NULL)
    {
      //Turned this into an error so I can get a reproducer.
      //SgNonrealRefExp is not a reasonable thing to convert to an
      //SgInitializedName (I think) -Jim Leek
      mlog[Sawyer::Message::Common::ERROR] << "In the isSgNonrealRefExp thing I was looking for" << std::endl;
      ROSE_ABORT();
      
      return std::set<ReadWriteSets::AccessSetRecord>();
    }
    else if (isSgDotExp(current))
    {
        //Left hand side goes in first, then RHS is a sub to that.
        std::set<ReadWriteSets::AccessSetRecord> lhs = recursivelyMakeAccessRecord(funcDef, isSgDotExp(current)->get_lhs_operand(), accessOrigin, thisFuncThis);
        std::set<ReadWriteSets::AccessSetRecord> rhs = recursivelyMakeAccessRecord(funcDef, isSgDotExp(current)->get_rhs_operand(), accessOrigin, thisFuncThis);
        lhs = updateAccessTypes(lhs, FIELD_ACCESS);
        //Updates the access type only if the parent is a dot or
        //arrow, which will affect rhs
        AccessType rhsAT = checkParentAccessTypeForBinaryOps(current);
        if(rhsAT != ACCESSTYPE_UNKNOWN) {
          rhs = updateAccessTypes(rhs, rhsAT);
        }
        
        leafFieldInsert(lhs, rhs);
        return lhs;
    }
    else if (isSgArrowExp(current))
    {
        // Liao 9/12/2016, special handling for variables inside of C++11 lambda functions
        // They capture variables outside of the lambda function.
        // They are represented as a class variable of an anonymous class, this->a[i]
        // So, we have to recognize this pattern, and pass the rhs variable to obtain initialized name.
        // has to resolve this recursively
        SgFunctionDeclaration* efunc =  getEnclosingFunctionDeclaration (current);
        if (isLambdaFunction (efunc) ) {
          //I'm not sure this is right, we may have to upgrade to POINTERS.  None of my test codes are C++11
          std::set<ReadWriteSets::AccessSetRecord> lhs = recursivelyMakeAccessRecord(funcDef, isSgArrowExp(current)->get_lhs_operand(), accessOrigin, thisFuncThis);
          return lhs;
    
        } else {
          //Arrow is the same as a dot, but the lhs access type has to be upgraded to POINTERS
          //Left hand side goes in first, then RHS is a sub to that.
          std::set<ReadWriteSets::AccessSetRecord> lhs = recursivelyMakeAccessRecord(funcDef, isSgArrowExp(current)->get_lhs_operand(), accessOrigin, thisFuncThis);
          std::set<ReadWriteSets::AccessSetRecord> rhs = recursivelyMakeAccessRecord(funcDef, isSgArrowExp(current)->get_rhs_operand(), accessOrigin, thisFuncThis);
          lhs = updateAccessTypesIfNotThis(lhs, POINTER_ARROW);
          //Updates the access type only if the parent is a dot or
          //arrow, which will affect rhs
          AccessType rhsAT = checkParentAccessTypeForBinaryOps(current);
          if(rhsAT != ACCESSTYPE_UNKNOWN) {
            rhs = updateAccessTypes(rhs, rhsAT);
          }
          leafFieldInsert(lhs, rhs);
          return lhs;
     
        }
    }
    
    else if (isSgArrowStarOp(current) || isSgDotStarOp(current))
    {
      // Arrow stars ->* and dot star .* are specificially for member
      // function pointers  annoying!
      std::set<ReadWriteSets::AccessSetRecord> lhs = recursivelyMakeAccessRecord(funcDef, isSgBinaryOp(current)->get_lhs_operand(), accessOrigin, thisFuncThis);
      lhs = updateAccessTypesIfNotThis(lhs, FUNCTION_POINTER_DEREF);
      std::set<ReadWriteSets::AccessSetRecord> rhs = recursivelyMakeAccessRecord(funcDef, isSgBinaryOp(current)->get_rhs_operand(), accessOrigin, thisFuncThis);
      leafFieldInsert(lhs, rhs);
      return lhs;
      

    } // The following expression types are usually introduced by left hand operands of DotExp, ArrowExp
    else if (isSgThisExp(current))
    {
      SgThisExp* thisExp = isSgThisExp(current);
      if(*thisFuncThis == NULL) {
          *thisFuncThis = thisExp;
      }      
          
      //Matt decided he'd rather have the thisExp than the SgClassDeclaration
      //      SgClassDeclaration* clsDecl = thisExp->get_class_symbol()->get_declaration();
      //But we only want on thisExp for the function, so reuse it
      return std::set<ReadWriteSets::AccessSetRecord>{ AccessSetRecord(VxUtilFuncs::getUniqueDeclaration(funcDef), *thisFuncThis) };
    }
    else if (isSgPointerDerefExp(current))
    {
       std::set<ReadWriteSets::AccessSetRecord> records = recursivelyMakeAccessRecord(funcDef, isSgPointerDerefExp(current)->get_operand(), accessOrigin, thisFuncThis);
       return updateAccessTypes(records, POINTER_DEREFERENCE);
    }
    else if (isSgAddressOfOp(current))
    {
       std::set<ReadWriteSets::AccessSetRecord> records = recursivelyMakeAccessRecord(funcDef, isSgAddressOfOp(current)->get_operand(), accessOrigin, thisFuncThis);
       return updateAccessTypes(records, ADDRESS_OF);
    }
    else if(isSgUnaryOp(current)) { //Written for SgAddressOfOp, but seems generally aplicable to all Unary Ops (replace above?) -JL
       return recursivelyMakeAccessRecord(funcDef, isSgUnaryOp(current)->get_operand(), accessOrigin, thisFuncThis);
    }
    else if (isSgCastExp(current))
    {
        return recursivelyMakeAccessRecord(funcDef, isSgCastExp(current)->get_operand(), accessOrigin, thisFuncThis);
    }
    //The only remaining binaryOps should be stuff like Add, subtract, not dot
    else if (isSgBinaryOp(current))
    {
      //Tests for this are in test7.c and ptr_test4.c
      std::set<ReadWriteSets::AccessSetRecord> lhs = recursivelyMakeAccessRecord(funcDef, isSgBinaryOp(current)->get_lhs_operand(), accessOrigin, thisFuncThis);
      std::set<ReadWriteSets::AccessSetRecord> rhs = recursivelyMakeAccessRecord(funcDef, isSgBinaryOp(current)->get_rhs_operand(), accessOrigin, thisFuncThis);
      recursiveMerge(lhs,rhs);
      return lhs;
    }
    else if (SgFunctionCallExp * func_call = isSgFunctionCallExp(current))
    {  //Does this do anything?  Seems like it would always return empty
      return recursivelyMakeAccessRecord(funcDef, func_call->get_function(), accessOrigin, thisFuncThis);
    }
    else if (isSgValueExp(current))
    {
      //Sometimes a constant is treated as a variable in customer code
      //The case I've seen is a pointer, but it's not an SgInitializedName, so...
      //making a special case here
      //test is constant_1.c and constant_2.c
      std::string noteStr("Constant used as variable ");
      noteStr += current->class_name();
      ReadWriteSets::AccessSetRecord constantExp(current, "!" + current->unparseToString() + "!", GLOBALS, PRIMITIVES, NORMAL, "","", "Constant used as variable " + current->class_name());
      mlog[Sawyer::Message::Common::INFO] << "got isSgValExp : " << current->unparseToString() << std::endl;
      constantExp.variableName = "!" + current->unparseToString() + "!";
      return std::set<ReadWriteSets::AccessSetRecord> { constantExp };
    }
    else if (isSgPseudoDestructorRefExp(current))
    {
        //PseudoDestructors do literally nothing at all.
        return std::set<ReadWriteSets::AccessSetRecord>();;
    }
    else if(isSgNewExp(current))
    {
      //I don't know why this got returned as a Read/Write.  Ignore
      return std::set<ReadWriteSets::AccessSetRecord>();
    }
    else if(isSgConditionalExp(current)) {
        //This isn't really how this should work, due to a bug in
        //Qings code I used to generate the RWSets.  The RWsets should've
        //returned this as 3 seperate references, one: A? B:C, because
        //A is a write and B & C are reads.  But Matt thinks it's OK
        //for now 
        // Test is ternary_test2.cxx
        SgConditionalExp* condExp= isSgConditionalExp(current);
        std::set<ReadWriteSets::AccessSetRecord> conditionalExp = recursivelyMakeAccessRecord(funcDef, condExp->get_conditional_exp(), accessOrigin, thisFuncThis);
        std::set<ReadWriteSets::AccessSetRecord> trueExp = recursivelyMakeAccessRecord(funcDef, condExp->get_true_exp(), accessOrigin, thisFuncThis);
        std::set<ReadWriteSets::AccessSetRecord> falseExp = recursivelyMakeAccessRecord(funcDef, condExp->get_false_exp(), accessOrigin, thisFuncThis);

        recursiveMerge(conditionalExp,trueExp);
        recursiveMerge(conditionalExp,falseExp);
      return conditionalExp;
    } else if(isSgAggregateInitializer(current)) {
      //I don't know why this got returned as a Read/Write.  Ignore
      //SgAggregateInitializers aren't referenceable variable AFAIK
      return std::set<ReadWriteSets::AccessSetRecord>();
    } 

    mlog[Sawyer::Message::Common::ERROR] <<
      " LocalRWSetGenerator::recursivelyMakeAccessRecord: " <<
      current->get_file_info()->get_filename() << ":" <<
      current->get_file_info()->get_line() << ":" << current->get_file_info()->get_col()<<std::endl;
    mlog[Sawyer::Message::Common::ERROR] <<
      "In LocalRWSetGenerator::recursivelyMakeAccessRecord: unhandled reference type:"  <<
      current->class_name()<<std::endl;
    mlog[Sawyer::Message::Common::ERROR] << current->unparseToString() << std::endl;

    ROSE_ABORT();
    
}

 

/**
 * Collect the Read/Write sets of a single function, without recursion.
 *
 * \param[in] funcDef: The function definition to process
 * \param[out] readSet: All the variables read
 * \param[out] writeSet: All the variable written
 *
 **/
//! Collects the read and write sets for just one function, no recursion
void LocalRWSetGenerator::collectRWSetsNoRecursion(SgFunctionDefinition* funcDef, 
                                                   ReadWriteSets::FunctionReadWriteRecord& record) 
{
  std::vector <SgNode* > readRefs, writeRefs;

  //call to frontend/SageIII/sageInterface/sageInterface.C (Written byLeo)
  collectReadWriteRefs(funcDef, readRefs, writeRefs);
  
  //"this" pointer is function specific, we want all accesses to use the same "this"
  //But each thisOpExp has a different NodeId, so we use this to just save one
  SgThisExp* thisFuncThis = NULL;

  std::vector<SgNode*>::iterator iterr = readRefs.begin();
  for (; iterr!=readRefs.end();iterr++)
    {
      SgNode* current = *iterr;
      ROSE_ASSERT (current != NULL);
      current = SageUtils::walkupDots(current); //Dealing with a bug in R/W sets
      ROSE_ASSERT(current != NULL);

      std::set<ReadWriteSets::AccessSetRecord> thisSet = recursivelyMakeAccessRecord(funcDef, current, current, &thisFuncThis);

      //If the access record is just a function call, skip it.
      filterPlainFunctions(thisSet);
      
      recursiveMerge(record.readSet, thisSet);
    }

  std::vector<SgNode*>::iterator iterw = writeRefs.begin();
  for (; iterw!=writeRefs.end();iterw++)
    {
      SgNode* current = *iterw;
      ROSE_ASSERT (current != NULL);
      current = SageUtils::walkupDots(current); //Dealing with a bug in R/W sets
      ROSE_ASSERT(current != NULL);
      std::set<ReadWriteSets::AccessSetRecord> thisSet = recursivelyMakeAccessRecord(funcDef, current, current, &thisFuncThis);
      
      //If the access record is just a function call, skip it.      
      filterPlainFunctions(thisSet);

      //Special case for write sets
      filterOutThisNodeDueToMemberFunction(thisSet, record);
      
      
      recursiveMerge(record.writeSet, thisSet);
      
    }
  return;
}

/**
 * The Cache is a fairly complex data structure.  This function digs around in 
 * it to find the R/W set caches for THIS function, so they can be filled in
 * (or read out at the end)
 *
 * \param[in] funcDef: The function definition to process, filename is extracted as well
 * \param[out] readSet: The Cache entry for the read set
 * \param[out] writeSet: The Cache entry for the write set
 *
 **/
//void LocalRWSetGenerator::initRWSets(SgFunctionDeclaration* funcDecl, 
//               std::unordered_set<std::string>** readSet, std::unordered_set<std::string>** writeSet) {

//We have to do a little work to get the filename as a relative path, 
//and deal with the compiler generated case
//  boost::filesystem::path filePath(SageInterface::getEnclosingSourceFile(funcDecl)->get_file_info()->get_filenameString());
//fs::current_path().native(), filePath.native());
/*    std::string filename = VxUtilFuncs::getNodeRelativePath(funcDecl); 

      auto fileCacheIt = rwSetCache.find(filename);
      if(fileCacheIt == rwSetCache.end()) {
      //First time we've seen this file, so we have to add it.
      std::unordered_set<std::string> local_readSet;
      std::unordered_set<std::string> local_writeSet;
      rwSetCache[filename][funcDecl]["readSet"] = local_readSet;
      rwSetCache[filename][funcDecl]["writeSet"] = local_writeSet;
      fileCacheIt = rwSetCache.find(filename);  //should exist now
    
      }

      auto funcCacheIt = fileCacheIt->second.find(funcDecl);
      if(funcCacheIt == fileCacheIt->second.end()) {
      //First time we've seen this function, so we have to add it.
      std::unordered_set<std::string> local_readSet;
      std::unordered_set<std::string> local_writeSet;
      rwSetCache[filename][funcDecl]["readSet"] = local_readSet;
      rwSetCache[filename][funcDecl]["writeSet"] = local_writeSet;
      }

      //Ok, now just return the RWSets we found (or maybe just inserted)
      *readSet = &(rwSetCache[filename][funcDecl]["readSet"]);
      *writeSet = &(rwSetCache[filename][funcDecl]["writeSet"]);
      return;
  
      }*/


/**
 * This function fills in the read and write set caches for a particular function.
 * It fills in the global caches directly, so they are not taken as function
 * arguments.
 * Steps: 
 * 1. Get global caches
 * 2. Collect the R/W sets for funcDef only (no recursion)
 * 3. Collect the R/W sets for each function funcDef calls and insert them 
 *    into the R/W set as well
 *
 * param[in] funcDef:  The function we are determining the purity of
 **/
void LocalRWSetGenerator::collectFunctionReadWriteSets(SgFunctionDefinition* funcDef) {
  SgFunctionDeclaration* funcDecl = VxUtilFuncs::getUniqueDeclaration(funcDef);
  SgName namestr = VxUtilFuncs::getExpandedFunctionName(funcDecl);// funcDecl->get_qualified_name();


  //If we have a cached record, use it.
  ReadWriteSets::FunctionReadWriteRecord record(funcDecl, commandLine);
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>::iterator funcCacheIt = rwSetCache.find(record);
  if(funcCacheIt != rwSetCache.end()) {
    record = *funcCacheIt;
  } 

  //Quick safety check, if we have the result cached just skip it.
  //Note, we will end up repeating the rare function that has an empty Read set
  if(record.readSet.size() != 0 || record.writeSet.size() != 0) {
    return;  //Nothing to do.        
  }

  // Skip uninstantiated templates (Empty sets for the cache have been inserted)
  if(funcDef->variantT() == V_SgTemplateFunctionDefinition) {
    return;
  }
  
  //Skip stuff that isn't in the project
  if(!VxUtilFuncs::isInProject(funcDecl)) {
    return;
  }
  

  //Had to move this query up here or it complains about the gotos
  Rose_STL_Container<SgNode*> functionCallList = NodeQuery::querySubTree (funcDef, V_SgFunctionCallExp);

  //Call to get the read/write sets for JUST this function
  collectRWSetsNoRecursion(funcDef, record);

  mlog[INFO] << "Processing function " << namestr << std::endl;
  //Put all functions into the calledFunctions list 
  for (Rose_STL_Container<SgNode*>::iterator it = functionCallList.begin(); it != functionCallList.end(); it++) {
    SgFunctionCallExp* callExp = isSgFunctionCallExp(*it);
    if(callExp == NULL) {  //If I didn't get a callExp, I don't know what it is, so it's not mockable.
      mlog[Sawyer::Message::Common::WARN] << "!WARN: Expected call expression, got " << (*it)->class_name() << "  Invalid R/W sets!" << std::endl;
      mlog[Sawyer::Message::Common::WARN] << "Relevent file info: " << (*it)->get_file_info()->get_filename() << ":" <<
        (*it)->get_file_info()->get_line() << "-" << (*it)->get_file_info()->get_col()<< std::endl;
      mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
      mlog[Sawyer::Message::Common::WARN] << (*it)->unparseToString() << std::endl;
      continue;
    }

    SgFunctionDeclaration* calledFuncDecl = VxUtilFuncs::getUniqueDeclaration(callExp);
    if(calledFuncDecl == NULL) {  
      //Check if it's a function pointer.  If it is, put the SgVarRefExp name in the calledFunctionsList as a warning
      //If not a function pointer, abort so Jim can figure out what itis.
      SgThisExp* thisFuncThis = NULL;
      std::set<ReadWriteSets::AccessSetRecord> callSet = recursivelyMakeAccessRecord(funcDef, callExp, callExp, &thisFuncThis);
      if(callSet.size() == 0) {
        mlog[Sawyer::Message::Common::WARN] << "Got call expression, but wasn't able to resolve it to anything. Report to Jim Leek" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << "Relevent file info: " << (*it)->get_file_info()->get_filename() << ":" <<
          (*it)->get_file_info()->get_line() << "-" << (*it)->get_file_info()->get_col()<< std::endl;
        mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << (*it)->unparseToString() << std::endl;
      }
      if(callSet.size() > 1) {
        mlog[Sawyer::Message::Common::WARN] << "Got call expression, but got multiple AccessSetRecords from it.  Report to Jim Leek" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << "Relevent file info: " << (*it)->get_file_info()->get_filename() << ":" <<
          (*it)->get_file_info()->get_line() << "-" << (*it)->get_file_info()->get_col()<< std::endl;
        mlog[Sawyer::Message::Common::WARN] << "------Relevent Code-------------------------------" << std::endl;
        mlog[Sawyer::Message::Common::WARN] << (*it)->unparseToString() << std::endl;
      }


      //There should only be one thing in the set, but do a loop
      //anyway, and put the name in the called functions list
      //
      std::set<std::string> nameSet = ReadWriteSets::recursivelyMakeName(callSet);
      for(auto nameIt = nameSet.begin(); nameIt != nameSet.end(); ++nameIt) {
        record.calledFunctions.insert(*nameIt);
      }     
      continue;
    }
    record.calledFunctions.insert(VxUtilFuncs::compileInternalFunctionName(calledFuncDecl, VxUtilFuncs::getNodeRelativePath(calledFuncDecl)));
    
  }
  rwSetCache.insert(record);
  

  return;
}



/**
 * Gets all functions, runs collectFunctionReadWriteSets on each (which may 
 * do recursive calls).  The sub calls fill in the global cache directly,
 * so there are no real arguments.
 *
 * param[in] root:  SgProject root of this project.
 **/
void LocalRWSetGenerator::collectReadWriteSets(SgProject *root) 
{

  Rose_STL_Container<SgNode*> functionDefList = NodeQuery::querySubTree (root,V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::iterator it = functionDefList.begin(); it != functionDefList.end(); it++) {
    SgFunctionDefinition* funcDef = isSgFunctionDefinition(*it);
    SgFunctionDeclaration* funcDecl = VxUtilFuncs::getUniqueDeclaration(funcDef);
    //SgName name = funcDecl->get_qualified_name();
    //std::string namestr = name.getString();

    //Skip uninstantiated template functions
    if(funcDecl->variantT() == V_SgTemplateFunctionDeclaration || 
       funcDecl->variantT() == V_SgTemplateMemberFunctionDeclaration) {
      continue;
    }

    collectFunctionReadWriteSets(funcDef);
        
  }
  
}

AccessType LocalRWSetGenerator::checkParentAccessTypeForBinaryOps(SgNode* current) 
{
  SgNode* parentNode = VxUtilFuncs::extractParentFromPossibleCast(current->get_parent());
  
  if(isSgDotExp(parentNode)) {
    return FIELD_ACCESS;
  } else if(isSgArrowExp(parentNode)) {
    return POINTER_ARROW;
  }
  return ACCESSTYPE_UNKNOWN;
}



/**
 * Opens the output file, calls the function to convert the cache to json.
 * Writes out.
 *
 * param[in] outFilename:  The file to write the cache out to as json
 **/
void LocalRWSetGenerator::outputCache(std::string& outFilename) 
{
  std::ofstream outFile;
  outFile.open(outFilename.c_str(), std::ofstream::out);
  if(rwSetCache.size() == 0) {
      //nolhmann json hates empty files, so output an empty set in
      //that case
      outFile << "{}" << std::endl;
  } else {
      outFile << std::setw(4) << ReadWriteSets::convertCacheToJson(rwSetCache) << std::endl;
  }
  
  outFile.close();
}

/**
 * Calls the function to convert the cache to json. Writes out.
 *
 * param[in] os: the output stream written to and returned
 * param[in] rwset: the LocalRWSetGenerator to write out
*/
std::ostream& operator<< (std::ostream& os, const LocalRWSetGenerator& rwset) {
  if(rwset.rwSetCache.size() == 0) {
    //nolhmann json hates empty files, so output an empty set in
    //that case
    os << "{}" << std::endl;
  } else {
    os << std::setw(4) << ReadWriteSets::convertCacheToJson(rwset.rwSetCache) << std::endl;
  }
  return os;
}

const std::set<AccessSetRecord>& LocalRWSetGenerator::getReadSet(SgFunctionDeclaration* sgFunctionDeclaration) 
{
  //Make a fake record to find the real record
  ReadWriteSets::FunctionReadWriteRecord record(sgFunctionDeclaration, commandLine);
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>::iterator funcCacheIt = rwSetCache.find(record);
  if(funcCacheIt != rwSetCache.end()) {
    return funcCacheIt->readSet;
  } 
  std::stringstream ss;
  ss << "getReadSet, unknown function requested " << record.internalFunctionName;
  mlog[WARN] << ss.str() << std::endl;
  throw new std::invalid_argument(ss.str());
}

const std::set<AccessSetRecord>& LocalRWSetGenerator::getWriteSet(SgFunctionDeclaration* sgFunctionDeclaration) 
{
  //Make a fake record to find the real record
  ReadWriteSets::FunctionReadWriteRecord record(sgFunctionDeclaration, commandLine);
  std::unordered_set<ReadWriteSets::FunctionReadWriteRecord, ReadWriteSets::FunctionReadWriteRecord_hash>::iterator funcCacheIt = rwSetCache.find(record);
  if(funcCacheIt != rwSetCache.end()) {
    return funcCacheIt->writeSet;
  } 
  std::stringstream ss;
  ss << "getWriteSet, unknown function requested " << record.internalFunctionName;
  mlog[WARN] << ss.str() << std::endl;
  throw new std::invalid_argument(ss.str());  

}

