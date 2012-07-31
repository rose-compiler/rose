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
  class CheckerModel {
  
   public: 
	    CheckerModel();
        
	    int varRaceNumber;
        
	    void varRaceNumberFinding(Rose_STL_Container< SgNode* >& list);
	   
	    yices_expr varConstraints( SgVarRefExp* var );
        
	    lbool SatOrUnsat(yices_expr varExp );

  };


 //! Fucntion Prototypes of OpenMP Checker 

 //! show reference node information  
 void showNodeInfo( SgNode* ); 

 //!  process #pragma omp parallel region 
 void process_omp_parallel( SgOmpParallelStatement* );

 //! gather private variable references from omp clause
 void getClausetPrivateVarRefs( const SgOmpClausePtrList &,  SgVarRefExpPtrList & );

 //!  gather local variables as private in omp parallel region
 void flagLocalPrivate( const Rose_STL_Container< SgNode* > &, const Rose_STL_Container< SgNode* > &, SgVarRefExpPtrList & );

 //!  delete expressions from critical, single, master.etc
 void removeExclusive( Rose_STL_Container< SgNode* > &);

 //!  gather references from remaining expressions in parallel region 
 void gatherReferences( const Rose_STL_Container< SgNode* > &, Rose_STL_Container< SgNode* > &);

 //!  delete the duplcate varaibles in all references list
 void getUnique( Rose_STL_Container< SgNode* > &);

 //!  delete the duplicate variables from clauses 
 void getUnique( SgVarRefExpPtrList &);

 //!   get variables name 
 std::string getName( SgNode* );

 //!  identify the race condiftions using SMT solver 
 void identifyRaces( const Rose_STL_Container< SgNode* > &, const SgVarRefExpPtrList &, Rose_STL_Container< SgNode* > &);

 //!  show container information using by debug 
 void showMembers( const Rose_STL_Container< SgNode* > &);

 //!  show container inoformation using by debug */
 void showMembers( const SgVarRefExpPtrList &);

 //!   remove duplicate element 
 void removeTrivial( Rose_STL_Container< SgNode* > &);

 //!   display reference information on warnings 
 void printWarnings( const std::vector<std::string> &);

 //!   display reference information on race
 void printRaces( const Rose_STL_Container< SgNode* > &, const Rose_STL_Container< SgNode* > &);


}

#endif // _OMP_CHECKER_INTERFACE_H_
