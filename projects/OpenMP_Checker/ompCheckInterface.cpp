
/**

* ompCheckInterface.cpp
  
* prototype functions are involved in this source file

* lasted edited by Hongyi on July/16/2012


*/



#include "ompCheckInterface.h"


using namespace std;


namespace ompCheckerInterface{

//! visitorTraversal for AST nodes of OpenMP program 

 visitorTraversal::visitorTraversal() { cout << "\n\n"; }

 void visitorTraversal::visit(SgNode* n) {   // visit AST node of OpenMP program, then analysis the omp parallel region
  
    SgOmpParallelStatement * ps = isSgOmpParallelStatement( n );
    if ( ps ) {
          process_omp_parallel( ps );
    }
  
    // TODO:1. Consider some class or function has OpenMP critical or other omp directives
  
   // TODO:2. Simplified some SMT constraints for some monitored variables, since some code without translation is still fine.
  
}

void visitorTraversal::atTraversalEnd() {
    printf ( "\n\nTraversal ends here.\n" );
}

//! The end of visitorTraversal class definition 

//! This is the definition for class SMTModel      
  
   CheckerModel::CheckerModel() {cout<<"SMT works now"<<endl; }
   
   void CheckerModel::varRaceNumberFinding( Rose_STL_Container< SgNode* >& list )
   {
      varRaceNumber =  list.size();
   }
  
  lbool  CheckerModel::SatOrUnsat( yices_expr varExp )
   {
     yices_context ctx = yices_mk_context();
  
     yices_assert( ctx, varExp );
     
     lbool tag = yices_check( ctx );
    
      if( tag == l_true )
         {
             printf(" Sat for data race! \n");
             return l_true;
         }    
      else if( tag == l_false )
        {
             printf(" UnSat for data race ! \n");
             return l_false;
        }
       else
        {
            printf(" Unknown yet, please feature check ! \n");
            return l_undef;
        }
    
     yices_del_context(ctx);   
   }




//! The end of definition of class SMTModel




/* show reference information   */

 void showNodeInfo( SgNode *n )
 {
  std::cout<< " At line: "<<n->get_file_info()->get_line() << endl;
  std::cout<< " Ref: " << n->unparseToString() <<endl;
  std::cout<< " Node class type: "<< n->class_name() << endl;
  
 } /* the end of showNodeInfo function */



/* show the private variables references in clauses in omp parallel directives */

void getClausePrivateVarRefs( const SgOmpClausePtrList & clauses, SgVarRefExpPtrList & vars )
{

  SgOmpPrivateClause* pClause;
  SgOmpFirstprivateClause* fpClause;
  SgOmpLastprivateClause* lpClause;
  SgOmpReductionClause* rClause;


  SgVarRefExpPtrList pList, fpList, lpList, rList;

  const int size = clauses.size();
  /* test the clause in #pragma omp paralell  */


  std::cout<<"the size of clauses is: "<< size <<endl;
 for( int i = 0; i < size; i++ )
  {
     pClause = isSgOmpPrivateClause( clauses.at (i) );
     
     if( pClause )
       {
         pList = pClause->get_variables();
        /*      
         std::cout<<" the first variable is: "<< pList.at(0)->unparseToString()<<endl;
        */
     }

    fpClause = isSgOmpFirstprivateClause( clauses.at(i) );
    if( fpClause )
     {
       fpList = fpClause->get_variables();
       
     } 
    
    lpClause = isSgOmpLastprivateClause (clauses.at(i) );
    if( lpClause )
     {
       lpList = lpClause->get_variables();
     } 

   rClause = isSgOmpReductionClause( clauses.at( i) );
 
    if(rClause )
    {
       rList = rClause->get_variables();
    }


  } 

     /* merge the list */
    
    int pSize = pList.size();
    int fpSize = fpList.size();
    int lpSize = lpList.size();
    
    if( pSize >= fpSize && pSize >= lpSize && pSize >= 0 )
      {
	vars = pList;
        vars.insert ( vars.end(), fpList.begin(), fpList.end() );
        vars.insert ( vars.end(), lpList.begin(), lpList.end() );
                 
      }

     else if( fpSize >= pSize && fpSize >= lpSize && fpSize >= 0 ) 
      {
         vars = fpList;
         vars.insert( vars.end(), pList.begin(), pList.end() );
         vars.insert( vars.end(), lpList.begin(), lpList.end() );
 
    
      }
     else if( lpSize > 0 )
      {
         vars = lpList;
         vars.insert ( vars.end(), pList.begin(), pList.end() );
         vars.insert ( vars.end(), fpList.begin(), fpList.end() );
      }


     vars.insert ( vars.end(), rList.begin(), rList.end() );


}/* the end of getClausePrivateVarRefs functions */




/* show members in wuery list */

void showMembers( const Rose_STL_Container< SgNode* > & list )
{
    Rose_STL_Container< SgNode* >::const_iterator iter = list.begin();
    while ( iter != list.end() )
    {
        std::cout<< " the ref is: " <<(*iter)->unparseToString()<<endl;
        iter++;
    }
}

/* the end of show members */




/* remove the expression from follwoing area atmoic, master, single, critical */

void removeExclusive( Rose_STL_Container< SgNode* > & list )
 {
  Rose_STL_Container< SgNode* >::iterator iter = list.begin();
  unsigned int size = list.size();
  unsigned int i = 0;
  
  while ( i < size )
  {
    /* #pragma omp critical */
    SgOmpCriticalStatement* cp = SageInterface::getEnclosingNode< SgOmpCriticalStatement >(list.at (i) );
    if( cp )
     {
       list.erase(iter + i );
       size--;
       continue;
     }


    // TODO: some index in for statement should not cause race condition but not all variables in expressions
     /* for statement */  
     /*
      SgForStatement*  forP =   SageInterface::getEnclosingNode< SgForStatement >( list.at( i ) );
      if ( forP )
        {
           list.erase( iter + i );
           size--;
           continue;
         }
    */



    /* atomic */
       SgOmpAtomicStatement* ap =   SageInterface::getEnclosingNode< SgOmpAtomicStatement >( list.at( i ) );
        if ( ap ) 
        { 
            list.erase( iter + i ); 
            size--;
            continue; 
        }

     /* single */
  
        SgOmpSingleStatement* sp =  SageInterface::getEnclosingNode< SgOmpSingleStatement >( list.at( i ) );
        if ( sp )
        {
            list.erase( iter + i );
            size--;
            continue;
        }

        /*  MASTER */ 

        SgOmpMasterStatement* mp =  SageInterface::getEnclosingNode< SgOmpMasterStatement >( list.at( i ) );
        if ( mp )
        {
            list.erase( iter + i );
            size--;
            continue;
        }
   
    i++;    
  
  } 
 
 
 } 


/* the end of removeExclusive */


/*    Get name of varrefexp  */
string getName( SgNode * n )
{
    string name;
    SgVarRefExp * var = isSgVarRefExp( n );
    if ( var )
        name = var->get_symbol()->get_name().getString();
    
    return name;
}

/* return the name of varriable in node */

/*    Remove duplicate list entries  */
void getUnique( Rose_STL_Container< SgNode* > & list )
{
    Rose_STL_Container< SgNode* >::iterator start = list.begin();
    unsigned int size = list.size();
    unsigned int i, j;

    if ( size > 1 )
    {
        for ( i = 0; i < size - 1; i++ )
        {
            j = i + 1;
            while ( j < size )
            {
                string is = getName( list.at( i ) );
                string js = getName( list.at( j ) );
                
                if ( is == js )
                {
                    list.erase( start + j );
                    size--;
                    continue;
                }

                j++;
            }
        }
    }
}
/* the end of getUnique name */

 /* gather varaible references from remaining expressions */


void gatherReferences( const Rose_STL_Container< SgNode* >& expr, Rose_STL_Container< SgNode* >& vars)
{
 Rose_STL_Container< SgNode* >::const_iterator iter = expr.begin();
 
  while (iter != expr.end() )
  {
  
  /* c =a + b  */   

  /* the end of judge right and write on both sides */     


     Rose_STL_Container< SgNode* > tempList = NodeQuery::querySubTree(*iter, V_SgVarRefExp );
     
   // SgAssignOp* tmpExpr = isSgAssignOp( *iter );
    //ROSE_ASSERT( tmpExpr);
 
  // SgExprStatement leftExpr = tmpExpr->get_lhs_operand_i();
  //   SgExprStatement* rightExpr = tmpExpr->get_rhs_operand_i();


 //  std::cout<<"the left child is: "<<leftExpr->unparseToString()<<endl;
 //  std::cout<<" the right child is: "<<tmpExpr->unparseToString()<<endl;


      
    Rose_STL_Container< SgNode* >::iterator ti = tempList.begin();
     while ( ti != tempList.end() )
       {
         vars.push_back( *ti );
         ti++;
       }
     iter++;
         
 
  }
   /* then remove the duplicate variables */
   
  getUnique( vars );



} 




 /* the end of gatherReferences function*/


/*    Remove duplicate variable references from pointer list */ 

void getUnique( SgVarRefExpPtrList & list )
{
    unsigned int size = list.size();
    unsigned int i, j;
    
    if ( size > 1 ) 
    {
        for ( i = 0; i < size - 1; i++ )
        {
            j = i + 1;
            while ( j < size )
            {
                string is = list.at( i )->get_symbol()->get_name().getString();
                string js = list.at( j )->get_symbol()->get_name().getString();

                if ( is == js )
                {
                    list.erase( list.begin() + j );
                    size--;
                    continue;
                }

                j++;
            }
        }
    }
}

/* the end of getUnique in PtrList */






/*    Identify variables declared within parallel region private by default */


void flagLocalPrivate( const Rose_STL_Container< SgNode* > & locals, const Rose_STL_Container< SgNode* > & allRefs, 
                       SgVarRefExpPtrList & priv )
{
    Rose_STL_Container< SgNode* >::const_iterator localsIter, allRefsIter;

    // iterate through all references
    allRefsIter = allRefs.begin();
    while ( allRefsIter != allRefs.end() )
    {
        // cast to a reference
        SgVarRefExp * item = isSgVarRefExp( *allRefsIter );

        // get reference name
        string varName = item->get_symbol()->get_name().getString();

        // iterate through local declarations, check if name matches,
        // place into private
        localsIter = locals.begin();
        while ( localsIter != locals.end() )
        {
            // get local variable from declaration
            SgInitializedNamePtrList vars =  ((SgVariableDeclaration*)(*localsIter))->get_variables();

            // we are only interested in the first reference
            string localName = vars.at( 0 )->get_name().getString();

            // if names match, this is a local declaration to add to 
            // private references
            if ( varName == localName )
                priv.push_back( item );

            localsIter++;
        }

        allRefsIter++;
    }
    

     // remove duplicates
     getUnique( priv );
}

/* the end of flagLocalPrivate funciton */





/* idnetify the non-private varuables in writing shared memory  */
void identifyRaces( const Rose_STL_Container< SgNode* >& vars, const SgVarRefExpPtrList & priv, Rose_STL_Container< SgNode* > & races )
{

  //  yices_context ctx = yices_mk_context();
    

    Rose_STL_Container< SgNode* >::const_iterator varIter = vars.begin();
    unsigned int privIndex;
    unsigned int privSize = priv.size();
    bool match;

    while ( varIter != vars.end() )
     {
      match = false;
   
      privIndex = 0;
   
        while(privIndex < privSize )
         {
           string a = getName( *varIter );
           string b = getName( priv.at( privIndex ));
            if ( a == b)
            {
             match = true;
            }
          privIndex++;
          }
 
        if( !match )
          {
             /* here is the SMT solver to verify the races  */
          yices_context ctx = yices_mk_context();
         
          string txt = "yices_log.txt";
          yices_enable_log_file( (char*) txt.c_str() );
         
          yices_type ty = yices_mk_type( ctx, "int" ); // we use symbolic execution, the variable should be simulated as "int"
   
          yices_var_decl xdecl = yices_mk_var_decl( ctx, (char*) (*varIter)->unparseToString().c_str(), ty);
         //  const string tmp;
          // strcpy( (*varIter)->unparseToString().c_str(), tmp );
          string tmp = "tmp";
 
          yices_var_decl x_1decl = yices_mk_var_decl( ctx, (char*)tmp.c_str() ,  ty );
          
          yices_expr x = yices_mk_var_from_decl( ctx, xdecl );
    
          yices_expr x1 = yices_mk_var_from_decl( ctx, x_1decl );

            
          yices_expr n1 = yices_mk_num ( ctx, 1 );

          yices_expr n2 = yices_mk_num( ctx, 2 );

          yices_expr args[2]; 
    
          args[0] = x;
          args[1] = n1;
         

          yices_expr e1 = yices_mk_sum( ctx, args, 2 ); 
          yices_assert( ctx, e1 );
          // args[0] = y;
         
          yices_expr args_1[2];
 
          args_1[0] = x1;
         
          args_1[1] = n2;
          
          yices_expr e2 = yices_mk_sum ( ctx, args_1, 2);
          yices_assert( ctx, e2 );

          yices_expr e3 = yices_mk_eq( ctx, e1, e2 );
          
          yices_assert( ctx, e3 );
  
          if( yices_inconsistent (ctx ) )
            {
             std::cout<<"unsat:"<<endl;
            }
           else
            {

              std::cout<<"sat:for race conditions"<<endl;

            }         
          yices_check( ctx );

          races.push_back( *varIter );
          
          yices_del_context(ctx);

           /* here is the end of SMT solver to verify the races  */
          }

          varIter++;

        }
    
   getUnique( races );
 


}


/* the end of idnetify non-private variables*/


/*  print out race report  */
void printRaces( const Rose_STL_Container< SgNode* >& races )
{

  if( races.size() > 0 )
   {
      std::cout<<" the following are potential race conditions: \n"<<endl;
 
   }
 
 Rose_STL_Container< SgNode* >::const_iterator iter = races.begin();
  while (iter != races.end() )
  {
     std::cout<< " in parallel region:\n";
     showNodeInfo( *iter );
     SgStatement* def_stmt = SageInterface::getEnclosingStatement( *iter );
     ROSE_ASSERT( def_stmt );
     std::cout<<def_stmt->unparseToString()<<" @ line "<<def_stmt->get_file_info()->get_line()<<":"<<endl;
 
     iter++;
} 

}


/*  the end of printRaces */

/* Process parallel region in code  */ 
void  process_omp_parallel( SgOmpParallelStatement *n )
{
 printf("hello world in omp parallel! \n");
 vector< string > warnings;
 SgOmpClausePtrList clauseList;
 SgVarRefExpPtrList privateVariables;
 Rose_STL_Container< SgNode* > allVariableReferences;
 Rose_STL_Container< SgNode* > localVariables;
 Rose_STL_Container< SgNode* > binaryOps;
 Rose_STL_Container< SgNode* > expressions;
 Rose_STL_Container< SgNode* > forStatements;
 Rose_STL_Container< SgNode* > ompForStatements;
 Rose_STL_Container< SgNode* > races;
 Rose_STL_Container< SgNode* > forRaces;
 /*
 Rose_STL_Container< SgNode* > parallels = NodeQuery::querySubTree(n, V_SgOmpParallelStatement );
std::cout<<" the size of parallel region is: "<<parallels.size()<<endl;
 */
 
 clauseList  = n->get_clauses();
 std::cout<<" the size of clause in this region is: "<<clauseList.size()<<endl;

 if( clauseList.size() > 0 )
  {
    getClausePrivateVarRefs( clauseList , privateVariables );
    
  }
 
    /* gather all expressions  read or write */

   expressions = NodeQuery::querySubTree( n, V_SgExprStatement );
   //! showMembers( expressions);

   /* remove the expression from following area region */
    removeExclusive ( expressions);


   //!  showMembers( expressions);

    /* be careful to process the loop */


 /* gather references from the  remaining expressions */

    gatherReferences ( expressions,  allVariableReferences );
 
/*   cout<<"here is the retrieval variabl name: "<<endl;*/
  
 /*  showMembers( allVariableReferences );*/

 /* mark the local variable in parallel region as private */
  
   localVariables = NodeQuery::querySubTree( n, V_SgVariableDeclaration );
   flagLocalPrivate ( localVariables, allVariableReferences, privateVariables );


    /*omp for function edited by Hongyi Ma*/
 
    ompForStatements = NodeQuery::querySubTree(n, V_SgOmpForStatement );
 
    Rose_STL_Container< SgNode* >::iterator ompfIter = ompForStatements.begin();

    while(ompfIter != ompForStatements.end() )
      {
         std::cout<<"the omp for statement: "<<(*ompfIter)->unparseToString()<<endl;
      
         Rose_STL_Container< SgNode* > reduxClause = NodeQuery::querySubTree( *ompfIter, V_SgOmpReductionClause );
         unsigned int reduxSize = reduxClause.size();

        if(reduxClause.size() == 1)
          {
            SgOmpReductionClause* orc = isSgOmpReductionClause( reduxClause.at(0)  );
            SgVarRefExpPtrList reductionVariables = orc->get_variables();
    
            privateVariables.insert( privateVariables.end(), reductionVariables.begin(), reductionVariables.end() );
         
       
          }
     
           Rose_STL_Container< SgNode* > forRef = NodeQuery::querySubTree( *ompfIter, V_SgVarRefExp);
         //  Rose_STL_Container< SgNode* > arrayPntrList = NodeQuery::querySubTree( *ompfIter, V_SgPntrArrRefExp );
  
    
           std::cout<<" the indices is "<<forRef.at(0)->unparseToString()<<endl;
      
     
           string indexName = forRef.at(0)->unparseToString().c_str();
           std::cout<<" the indices is "<< indexName<<endl;

      
        
           SgVarRefExp * item = isSgVarRefExp( forRef.at(0) );
           ROSE_ASSERT( item );
           /* the indcie in For loop will be inserted into private clause */



           privateVariables.push_back( item );
      
          /* the end of rthe indcie in For loop   */  
     
          /* be careful to process the array */
      
          /* a[i] = a[i + 1] + a[i -1]*/
          /* c[i] = a[i] + b[i]       */
          /* sum = sum + a[i]         */  
          /* a[i + 1] = b[ j + 1] + a[ i ]; */
   
           /***********************************************/


           Rose_STL_Container< SgNode* > arrayPntrList = NodeQuery::querySubTree( *ompfIter, V_SgPntrArrRefExp );
           
           Rose_STL_Container< SgNode* > varLeft; // to  store  writing  variables
           Rose_STL_Container< SgNode* > varRight; // to store reading variables

           /* there is no array for being processed */
           
            
            //  std::cout<<"there is no array for prcocessing! "<<endl;
              /* to judge the  reading and writing variables on both sides of expression statement */            
             
              Rose_STL_Container< SgNode* >::iterator iRef = forRef.begin();
              
              while( iRef != forRef.end() )
                 {
                     if( (*iRef)->unparseToString().c_str() != indexName )
                       { 
                        // std::cout<<"this ref is :"<<(*iRef)->unparseToString().c_str()<<endl;
                                    SgNode* parent = (*iRef)->get_parent();
                                    ROSE_ASSERT( parent );
                                    SgAssignOp* aOp = isSgAssignOp( parent );
                                    
                                    if( aOp )
                                      {
                                        std::cout<<" the varibale is "<<(*iRef)->unparseToString()<<" and his parent is "<<parent->unparseToString()<<endl;
                                        varLeft.push_back( *iRef );
         
                                       }
                                     // SgPntrArrRefExp* atmp = isSgPntrArrRefExp( parent);
                                                        
                                      else
                                        {
                                         SgPntrArrRefExp* atmp = isSgPntrArrRefExp( parent);
                                         if( atmp )
                                            {
                                               SgNode* parent2 = atmp->get_parent();
                                              
                                               ROSE_ASSERT(parent2);
                                              
                                               SgAssignOp* arOp = isSgAssignOp( parent2 );
                                               
                                               if( arOp )
                                                  {
                                                       std::cout<<" the varibale is "<<(*iRef)->unparseToString()<<" and his parent is "<<parent->unparseToString()<<endl;
                                                       varLeft.push_back( *iRef );
 
                                                   }
                                                else
                                                   {
                                                       varRight.push_back( *iRef );
                                                   }
 
                                            }
                                         else{

                                    
                                           varRight.push_back( *iRef );
                                              }
                                        }                                               
     

         
                       } // only process the variable which is not index name in for loop 
                    
                   iRef++;
 
                  }    
                     
             //          SgVarRefExp* tmp = isSgVarRefExp( varRight.at(0) );
              //         cout<<" the first reading variable is: "<< tmp->unparseToString()<<endl;
         

         

             Rose_STL_Container< SgNode* >::iterator iRead = varRight.begin();
             Rose_STL_Container< SgNode* >::iterator iWrite = varLeft.begin();
            // bool onlyRead = true;
      
            

             while( iRead != varRight.end() )
                  {
                    bool onlyRead = true;
                   for( iWrite = varLeft.begin(); iWrite != varLeft.end(); iWrite++)
                      {
                         
                         SgVarRefExp* tmp = isSgVarRefExp( *iRead );
                         SgVarRefExp* tmp1 = isSgVarRefExp( *iWrite );
                

                         if(tmp->unparseToString() == tmp1->unparseToString() )
                           {
                                     onlyRead = false;
                            //  std::cout<<" the read and write variables are: "<<tmp->unparseToString()<<endl;

                           }


                      }
                      
                      if( onlyRead == true )
                         {
                           SgVarRefExp* tmp2 = isSgVarRefExp( *iRead );
                           std::cout<<"the only read array is :"<<tmp2->unparseToString()<<endl;
                            privateVariables.push_back( tmp2 );
                         }

                   iRead++;
                  }     
  

          /********* the end of processing array   ************/   
    
          ompfIter++;

   }  

   
  


   /* gather references from the  remaining expressions */

// 1   gatherReferences ( expressions,  allVariableReferences );
//2   localVariables = NodeQuery::querySubTree( n, V_SgVariableDeclaration );
//3   flagLocalPrivate ( localVariables, allVariableReferences, privateVariables );
   /* identify the non-private variables */



   identifyRaces ( allVariableReferences, privateVariables, races ) ;

   std::cout<<" here is the potential races:" <<endl;

   showMembers(races );

     
   printRaces( races );




}
 /*the end of process_omp_parallel fucntion */
}

