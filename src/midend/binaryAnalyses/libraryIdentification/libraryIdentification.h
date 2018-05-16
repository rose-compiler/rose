#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H

#include "sqlite3x.h"

/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 **/
namespace LibraryIdentification
   {

/** generate Library Identification Database
 *  This function takes a binary project and analyzes every function,
 *  in the library.  It does this by combining all the instructions in
 *  the function into a unique hash.
 *  The hash is then inserted into a new sqlite3 database.  If the project was
 *  built with debug information, we should have a database that can
 *  later identifiy functions in stripped libraries.
 *
 *  @param std::string databaseName The filename of the database to create
 *  @param SgProject* project       A Rose binary project
 **/     
     void generateLibraryIdentificationDataBase    ( std::string databaseName, SgProject* project );

/** match Library Identification Database
 *  This function takes a binary project and attempts to match the
 *  functions in it to functions that already exist in databaseName
 *  This only results in a message on stdout: found_match true/false
 *
 *  @param std::string databaseName The filename of the database to
 *  read from
 *  @param SgProject* project       A Rose binary project
 **/     
     void matchAgainstLibraryIdentificationDataBase( std::string databaseName, SgProject* project );

/**  libraryIdentificationDataBaseSupport
 *
 *  Code that actually loops through the project, and writes or reads
 *  to/from the database. Implementation of both
 *  generateLibraryIdentificationDataBase() and matchAgainstLibraryIdentificationDataBase()
 **/
     void libraryIdentificationDataBaseSupport( std::string databaseName, SgProject* project, bool generate_database );


/**  testForDuplicateEntries
 *  Debugging support
 **/
     void testForDuplicateEntries( const std::vector<SgUnsignedCharList> & functionOpcodeList );

     /**
      * @class library_handle 
      *
      * A simple data holder for the data that's in the database on a
      * given function.  Name of the database, name of the function,
      * and its size.  
      * TODO: Shouldn't think have the OpCodeString as well?
      * (Counterpoint: that's the key we're using to look things up)
      *  
      **/
     class library_handle
        {
          public:
               std::string filename;
               std::string function_name;
               size_t begin;
               size_t end;

               library_handle() {}              
        };

     /**
      * @class FunctionIdentification
      *
      * Database interface class for reading and writing
      * function/library identification information from the database.
      *  
      **/
     class FunctionIdentification
     {
       public:

         /** @brief Constructor, creates or opens the database */
         FunctionIdentification(std::string dbName);

         /** @brief Make sure that all the tables are defined in the
          * function identification database **/
         void createTables();

         // @brief Add an entry for a function to the database
         void set_function_match( const library_handle & handle, const std::string s );
         void set_function_match( const library_handle & handle, const SgUnsignedCharList & opcode_vector);
         void set_function_match( const library_handle & handle, const unsigned char* str, size_t str_length );

         /** @brief Lookup a function in the database.  True returned if found
          * This can't be const (some sqlite problem).
          **/
         bool get_function_match(library_handle & handle, const std::string s ) const;
         bool get_function_match(library_handle & handle, const SgUnsignedCharList & opcode_vector) const;
         bool get_function_match(library_handle & handle, const unsigned char* str, size_t str_length );

       private:
         // @brief The name of the database
         std::string database_name;

         // @brief SQLite database handle
         sqlite3x::sqlite3_connection con;
     };

  // Add an entry to store the pair <library_handle,string> in the database
     void set_function_match( const library_handle & handle, const std::string & data );

  // Return the library_handle matching string from the database. bool false
  // is returned if no such match was found, true otherwise.
     bool get_function_match( library_handle & handle, const std::string & data );

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

     /**
      * generateOpCodeVector
      *
      * @brief Generates the OpCode vector rooted by root.  Internally
      * calls the traversal defined by the FlattenAST class.
      **/
     SgUnsignedCharList generateOpCodeVector(SgAsmInterpretation* asmInterpretation, SgNode* root, size_t & startOffset, size_t & endOffset);

     /**
      * @brief Writes a function to the FLIRT database
      **/
     void write_database ( FunctionIdentification & ident, const std::string & fileName, const std::string & functionName, size_t startOffset, size_t endOffset, const SgUnsignedCharList & s );
     /**
      * @brief Looks for a function in the database.  Returns true and fills in the arguments if found.
      **/
     bool match_database ( const FunctionIdentification & ident, std::string & fileName, std::string & functionName, size_t & startOffset, size_t & endOffset, const SgUnsignedCharList & s );

#if 0
  //! Add an entry to store the pair <library_handle,string> in the database
      void set_function_match( library_handle, std::string );

  //! Return the library_handle matching string from the database. bool false
  //! is returned if no such match was found, true otherwise.
      bool get_function_match(library_handle&, std::string);
#endif

   }
#endif
