// grammarBaseClass.h -- header file for the base class of all grammars

#ifndef BASE_GRAMMAR_H
#define BASE_GRAMMAR_H

// class SgFile;

class ROSE_BaseGrammar
   {
  // This class for a base class for all grammars

     public:

          ROSE_BaseGrammar ();

       // Experimental interface
       // ROSE_BaseGrammar ( SgFile *file );
       // SgFile* getProgramFile();

          void setParentGrammar ( ROSE_BaseGrammar* Xptr );
          ROSE_BaseGrammar* getParentGrammar () const;

       // Only one grammar is the root of all others
          bool isRootGrammar() const;

     private:
          ROSE_BaseGrammar* parentGrammar;

       // ROSE_BaseGrammar ();
          ROSE_BaseGrammar ( const ROSE_BaseGrammar & X );
          ROSE_BaseGrammar & operator= ( const ROSE_BaseGrammar & X );
   };

#endif // ifndef BASE_GRAMMAR_H

