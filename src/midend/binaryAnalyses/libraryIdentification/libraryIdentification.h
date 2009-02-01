
namespace LibraryIdentification
   {
  // This is an implementation of Fast Library Identification and Recognition Technology
     void generateLibraryIdentificationDataBase    ( std::string databaseName, SgProject* project );
     void matchAgainstLibraryIdentificationDataBase( std::string databaseName, SgProject* project );

     struct library_handle
        {
          std::string filename;
          std::string function_name;
          size_t begin;
          size_t end;
        };

  // Add an entry to store the pair <library_handle,string> in the database
     void set_function_match( const library_handle & handle, const std::string & data );

  // Return the library_handle matching string from the database. bool false
  // is returned if no such match was found, true otherwise.
     bool get_function_match( library_handle & handle, const std::string & data );

     class FindFunctions: public AstSimpleProcessing
        {
          public:
            // std::map<SgAsmGenericSymbol::addr_t,SgAsmGenericSymbol*> globalVariableMap;
            // std::map<SgAsmGenericSymbol::addr_t,SgAsmGenericSymbol*> globalFunctionMap;

            // Store the current statement as we traverse so that we can use it to call nextAsmStatement().
            // We need the address of the next statement to evaluate references to global variables.
               SgAsmStatement*   currentAsmFunction;
               SgAsmInstruction* firstAsmInstruction;
               SgAsmInstruction* lastAsmInstruction;

               void visit(SgNode* n);
        };

     class FlattenAST: public AstSimpleProcessing
        {
          public:
            // Save flattended AST in reference initialized at construction.
               SgUnsignedCharList & data;

               FlattenAST(SgUnsignedCharList & s) : data(s) {}

               void visit(SgNode* n);
        };

   }

