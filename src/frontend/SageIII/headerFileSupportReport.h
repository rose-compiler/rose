
// This header file defines a data structure to support the reporting of 
// details specific to the header file unparsing support in ROSE.

#error "THIS FILE CAN BE REMOVED!"

class HeaderFileReport
   {
     private:
          std::list<SgSourceFile*> headerFileList;

       // Define these as private so that we can control and/or eliminate their use.
          HeaderFileSupportReport( const HeaderFileSupportReport & X );
          HeaderFileSupportReport & operator= ( const HeaderFileSupportReport & X );

     public:
          void display( std::string label);

          std::list<SgSourceFile*> & get_headerFileList();

          HeaderFileSupportReport( SgSourceFile* sourceFile );
   };

