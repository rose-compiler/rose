#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H

#include "LibraryInfo.h"
#include "FunctionInfo.h"
#include "FunctionIdDatabaseInterface.h"

/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 **/
namespace LibraryIdentification
   {

/** generate Library Identification Database
 *  This function takes a binary project (presumeably a library) and
 *  hashes every function, in it.  It then inserts the library and
 *  functions into a new sqlite3 database.  If the project was
 *  built with debug information, we should have a database that can
 *  later identify functions in stripped libraries.
 *
 * @param[in] databaseName Filename of the database to create/access
 * @param[in] libraryName  Library names cannot be discovered from all
 *                         library types, so pass in name.
 * @param[in] libraryVersion  Library version, same problem
 * @param[in] project      Rose SgProject that has the functions to
 * write or find
 **/     
       void generateLibraryIdentificationDataBase    ( const std::string& databaseName, 
                                                       const std::string& libraryName, 
                                                       const std::string& libraryVersion, 
                                                       SgProject* project );


     /**
      * @class FlattenAST 
      *
      * A simple traversal that builds up the opcode string for a
      * function.  When constructed, a reference to an
      * SgUnsignedCharList must be passed in to recieve the output
      * data.  It is initialized when traverse is run. The node passed into
      * traverse is presumeably an SgAsmFunction.
      * This class is used by generateOpCodeVector to generate the
      * opCodeVector. 
      * NOTE: FlattenAST has been superceeded by FlattenAST_AndResetImmediateValues
      *
      * Use like this:
      *  SgUnsignedCharList functionBytes;  //Recieves the output data
      *  FlattenAST flatAST(functionBytes);
      *  flatAST.traverse(node,preorder);
      *  size_t startAddress = flatAST.startAddress;
      *  size_t endAddress   = flatAST.endAddress;
      *  
      **/
     class FlattenAST: public AstSimpleProcessing
        {
          public:
            // @brief The reference used to the output data buffer,
            // get the opcodes
               SgUnsignedCharList & data;

            // @brief start address of the function in the memorymap
               size_t startAddress;
            // @brief end address of the function in the memorymap
               size_t endAddress;

            // @brief Constructor just initializes output buffer  
               FlattenAST(SgUnsignedCharList & s) : data(s),startAddress(0),endAddress(0) {}
               
            // @brief Generates the opcodes for each AsmInstruction.
            // (Called by traverse) 
               void visit(SgNode* n);
        };

     
     /**
      * @class FlattenAST_RangeListAttribute
      *
      * FlattenAST_RangeListAttribute is a synthesized attribute
      * (bottom-up attribute) that gathers a list of intermediate bit
      * ranges and passes it up the AST.
      * This list is used to reset all the intermediate (constant)
      * values to 0, so that any pointers are ignored when comparing
      * functions.  
      * The purpose is so that static linking, which changes where
      * pointers jump to, does not change the signature of a library
      * function. 
      * A possible downside is that this will also reset integer
      * constants, so functions containing Value+2 and Value+3 will be
      * seen as the same function.
      * This also currently only works for x86.
      *
      * NOTE: I don't know why we need a rangeList to reset
      * intermediate values. -Jim
      **/
     class FlattenAST_RangeListAttribute
        {
          public:
            // Save the list of ranges of where offsets are stored in each instruction's opcode (used to represent immediates).
               std::vector<std::pair<unsigned char,unsigned char> > rangeList;
        };

     /**
      * @class FlattenAST_AndResetImmediateValues
      *
      * FlattenAST_AndResetImmediateValues is a bottom-up traversal
      * that generates a list of a list of the opcodes in a function.
      * This is used to generate a signature of a function for FLIRT
      * identification. 
      * This is the same as FlattenAST, but also resets all the
      * intermediate (constant) values to zero so that any pointers
      * are ignored when making the signature.  
      * This is so that static linking, which changes where pointers
      * jump to, does not change the signature of a library function. 
      * A possible downside is that this will also reset integer
      * constants, so functions containing Value+2 and Value+3 will be
      * seen as the same function.
      * This also currently only works for x86.
      *
      * NOTE: I don't know why we need a rangeList to reset
      * intermediate values. -Jim
      * DQ (7/11/2009): We need to use a synthesized attribute to gather the list of bit ranges from any nested 
      * SgAsmExpression IR nodes where the opcode stores values (offsets) used to store immediate values (coded 
      * values in the instruction's op-codes.
      **/
     class FlattenAST_AndResetImmediateValues: public AstBottomUpProcessing<FlattenAST_RangeListAttribute>
        {
          public:
            // Save flattended AST in reference initialized at construction.
               SgUnsignedCharList & data;

               size_t startAddress;
               size_t endAddress;

               FlattenAST_AndResetImmediateValues(SgUnsignedCharList & s) : data(s),startAddress(0),endAddress(0) {}

               FlattenAST_RangeListAttribute evaluateSynthesizedAttribute( SgNode* n, SynthesizedAttributesList childAttributes );
        };



   }
#endif
