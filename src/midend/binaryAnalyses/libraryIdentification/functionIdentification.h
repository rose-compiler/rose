
#ifndef FUNCTION_IDENTIFICATION_H
#define FUNCTION_IDENTIFICATION_H

#include "sqlite3x.h"
#include "rose.h"
#include "libraryIdentification.h"

namespace LibraryIdentification
   {

     class FunctionIdentification
     {
       public:

         FunctionIdentification(std::string dbName);

         //Make sure that all the tables are defined in the function identification
         //database
         void createTables();

         //Add an entry to store the pair <library_handle,string> in the database
         void set_function_match( library_handle, std::string );

         //Return the library_handle matching string from the database. bool false
         //is returned if no such match was found, true otherwise.
         bool get_function_match(library_handle&, std::string);
       private:
         std::string database_name;
         sqlite3x::sqlite3_connection con;



     };

   };

#endif
