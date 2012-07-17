#ifndef _OMP_CHECKER_INTERFACE_H_
#define _OMP_CHECKER_INTERFACE_H_

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "rose.h"
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include "yices_c.h"
#include "yicesl_c.h"


  #define List Rose_STL_Container< SgNode* >

  #define Iter List::iterator

  #define CIter List::const_iterator

  #define Query NodeQuery::querySubTree

  #define EnclNode SageInterface::getEnclosingNode

namespace ompCheckerInterface
{


 //! visit AST nodes of OpenMP program 

  class visitorTraversal : public AstSimpleProcessing {
      public:
         visitorTraversal();
         virtual void visit( SgNode* n );
         virtual void atTraversalEnd();
   };

 //! setup a constraints for SMT solver
  class SMTModel {
  
   public: 
	    SMTModel();
        
	    int varRaceNumber;
        
	    void varRaceNumberFinding(List& list);
	   
	    yices_expr varConstraints( SgVarRefExp* var );
        
	    bool SatOrUnsat(yices_expr varExp );

  };


 /* Fucntion Prototypes of OpenMP Checker */

 //!  /* show reference node information  */ 
 void showNodeInfo( SgNode* ); 

 //!  /* process #pragma omp parallel region   */
 void process_omp_parallel( SgOmpParallelStatement* );

 //!  /* gather private variable references from omp clause */
 void getClausetPrivateVarRefs( const SgOmpClausePtrList &,  SgVarRefExpPtrList & );

 //! /* gather local variables as private in omp parallel region  */
 void flagLocalPrivate( const List &, const List &, SgVarRefExpPtrList & );

 //! /* delete expressions from critical, single, master.etc */
 void removeExclusive( List &);

 //! /* gather references from remaining expressions in parallel region  */
 void gatherReferences( const List &, List &);

 //! /* delete the duplcate varaibles in all references list*/
 void getUnique( List &);

 //! /* delete the duplicate variables from clauses */
 void getUnique( SgVarRefExpPtrList &);

 //! /* get variables name */
 std::string getName( SgNode* );

 //! /* identify the race condiftions using SMT solver */
 void identifyRaces( const List &, const SgVarRefExpPtrList &, List &);

 //! /* show container information using by debug */
 void showMembers( const List &);

 //! /* show container inoformation using by debug */
 void showMembers( const SgVarRefExpPtrList &);

 //! /* remove duplicate element */
 void removeTrivial( List &);

 //! /* display reference information on warnings */
 void printWarnings( const std::vector<std::string> &);

 //! /* display reference information on RACEs */
 void printRaces( const List &, const List &);


}

#endif // _OMP_CHECKER_INTERFACE_H_
