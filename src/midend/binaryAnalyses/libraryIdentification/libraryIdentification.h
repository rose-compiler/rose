#ifndef LIBRARY_IDENTIFICATION_H
#define LIBRARY_IDENTIFICATION_H

#include "sqlite3x.h"

// #include "functionIdentification.h"
// #include "rose.h"
// #include "libraryIdentification.h"

namespace LibraryIdentification
   {
  // This is an implementation of Fast Library Identification and Recognition Technology
     void generateLibraryIdentificationDataBase    ( std::string databaseName, SgProject* project );
     void matchAgainstLibraryIdentificationDataBase( std::string databaseName, SgProject* project );

  // Low level factored code to support generateLibraryIdentificationDataBase() and 
  // matchAgainstLibraryIdentificationDataBase() interface functions.
     void libraryIdentificationDataBaseSupport( std::string databaseName, SgProject* project, bool generate_database );

  // Debugging support
     void testForDuplicateEntries( const std::vector<SgUnsignedCharList> & functionOpcodeList );

     class library_handle
        {
          public:
               std::string filename;
               std::string function_name;
               size_t begin;
               size_t end;

               library_handle() {}              
        };

  // Copied from Andreas' code in functionIdentification.h
     class FunctionIdentification
     {
       public:

         FunctionIdentification(std::string dbName);

         //Make sure that all the tables are defined in the function identification
         //database
         void createTables();

         //Add an entry to store the pair <library_handle,string> in the database
         void set_function_match( const library_handle & handle, const std::string s );
         void set_function_match( const library_handle & handle, const SgUnsignedCharList & opcode_vector);
         void set_function_match( const library_handle & handle, const unsigned char* str, size_t str_length );

      // Return the library_handle matching string from the database. bool false
      // is returned if no such match was found, true otherwise.
      // This can't be const (some sqlite problem).
         bool get_function_match(library_handle & handle, const std::string s ) const;

      // Make these const functions, since they are ment to be const.
         bool get_function_match(library_handle & handle, const SgUnsignedCharList & opcode_vector) const;
         bool get_function_match(library_handle & handle, const unsigned char* str, size_t str_length );

       private:
         std::string database_name;

      // SQLite database handle
         sqlite3x::sqlite3_connection con;
     };

  // Add an entry to store the pair <library_handle,string> in the database
     void set_function_match( const library_handle & handle, const std::string & data );

  // Return the library_handle matching string from the database. bool false
  // is returned if no such match was found, true otherwise.
     bool get_function_match( library_handle & handle, const std::string & data );

     class FlattenAST: public AstSimpleProcessing
        {
          public:
            // Save flattended AST in reference initialized at construction.
               SgUnsignedCharList & data;

               size_t startAddress;
               size_t endAddress;

               FlattenAST(SgUnsignedCharList & s) : data(s),startAddress(0),endAddress(0) {}

               void visit(SgNode* n);
        };

 //! This function calls the traversal defined by the FlattenAST class.
     SgUnsignedCharList generateOpCodeVector(SgAsmInterpretation* asmInterpretation, SgNode* node, size_t & startOffset, size_t & endOffset);

     void write_database ( FunctionIdentification & ident, const std::string & fileName, const std::string & functionName, size_t startOffset, size_t endOffset, const SgUnsignedCharList & s );
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
