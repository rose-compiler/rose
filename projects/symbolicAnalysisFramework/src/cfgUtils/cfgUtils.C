#include "cfgUtils.h"
#include <stdlib.h>
#include <time.h>
 
namespace cfgUtils
{
  SgProject* project;

  // =true if initCFGUtils has been called and =false otherwise
  static bool CFGinitialized=false;

  // initializes the cfgUtils module
  void initCFGUtils(SgProject* project_arg)
  {
    // if the cfgUtils has not been initialized
    if(project==NULL)
    {
      project = project_arg;
      srand(time(NULL));

      initCFGRewrite(project);
    }
    else
      ROSE_ASSERT(project==project_arg);

    CFGinitialized=true;
  }

  SgProject* getProject()
  {
    return project;
  }

  // parses a logical condition, determines whether it is in the form (x <= y + c) and
  // if it is, sets x, y, c to be the unique ids of the relevant variables
  // If the condition is in a similar form (ex: x<y, x>c), x, y and c are normalized to the form x<=y+c
  // negX=true if x is supposed to be negated and false otherwise
  // negY=true if y is supposed to be negated and false otherwise
  bool computeTermsOfIfCondition_LTEQ(SgExpression *expr, varID &x, bool& negX, varID &y, bool& negY, long &c)
  {
    // skip if this is not a binary comparison condition
    SgBinaryOp *b_cond = isSgBinaryOp(expr);
    //printf("computeTermsOfIfCondition() A\n");
    if(!b_cond ||
        (b_cond->variantT() != V_SgLessOrEqualOp &&
         b_cond->variantT() != V_SgGreaterOrEqualOp &&
         b_cond->variantT() != V_SgLessThanOp &&
         b_cond->variantT() != V_SgGreaterThanOp))
    {
#ifdef DEBUG_FLAG1
      cout << "Not a binary comparison condition: " << expr->unparseToString()
        << " has type: " << expr->sage_class_name() << "\n";
#endif
      return false;
    }

    // separate the condition into a left-hand-side and a right-hand-side
    SgExpression *lhs = b_cond->get_lhs_operand();
    SgExpression *rhs = b_cond->get_rhs_operand();

    //printf("computeTermsOfIfCondition() IsConstInt(lhs, c)=%d\n", IsConstInt(lhs, c));
    long cx, cy;
    varID x2, y2;
    bool negX2, negY2;
    if(parseAddition(lhs, x, negX, x2, negX2, cx) &&
        parseAddition(rhs, y, negY, y2, negY2, cy))
    {
      // (negX)x + (negX2)x2 + cx <= (negY)y + (negY2)y2 + cy
      if(x2!=zeroVar || y2!=zeroVar) return false;

      // (negX)x + cx <= (negY)y + cy	
      c = cy-cx;
      //printf("computeTermsOfIfCondition(), cx=%d, cy=%d, c=%d\n", cx, cy, c);
    }
    else
      return false;
    /*
    // c <= op
    if(IsConstInt(lhs, c))
    {
    x = zeroVar;
    if(varID::isValidVarExp(rhs)) // if the rhs is a variable reference of dot expression
    {
    y.init(rhs); // get the unique id of the reference or expression
    c = -(c); // move c from lhs form to rhs form
    }
    return false;
    }
    else
    {
    SgBinaryOp* rhs_b;

    printf("computeTermsOfIfCondition() varID::isValidVarExp(lhs)=%d\n", varID::isValidVarExp(lhs));
    // if the lhs is a non-int expression, set *x to be that expression's id
    if(!varID::isValidVarExp(lhs))
    {
    cout << "Error: If condition on " << lhs->sage_class_name()
    << " in expression: " << lhs->unparseToString() << "\n";
    return false;
    }
    x.init(lhs);

    printf("computeTermsOfIfCondition() parseAddition(rhs, y, c)=%d\n", parseAddition(rhs, y, c));
    if(!parseAddition(rhs, y, c))
    {
    cout << "expression failed: " << expr->unparseToString()
    << " because rhs isInt:" << IsConstInt(rhs, c)
    << "\t" << rhs->sage_class_name() << "\n";
    return false;
    }
    }
    */
    // if the comparison is >/>=, rather than </<=, exchange x and y and flip the sign of c
    if(b_cond->variantT() == V_SgGreaterOrEqualOp ||
        b_cond->variantT() == V_SgGreaterThanOp)
    {
      varID q;
      q=x;
      x=y;
      y=q;
      c=-c;
    }

    // if the comparison is </>, rather than <=/>=, adjust the c
    if(b_cond->variantT() == V_SgLessThanOp ||
        b_cond->variantT() == V_SgGreaterThanOp)
      c--;

#ifdef DEBUG_FLAG1
    cout << "TERMS OF CONDITION ARE x = " << x.getString() << "\ty = " << y.getString() << "\tc = " << c;
#endif
    return true;
  }

  // parses a logical condition, determines whether it is in the form (x == y + c) and
  // if it is, sets x, y, c to be the unique ids of the relevant variables
  // negX=true if x is supposed to be negated and false otherwise
  // negY=true if y is supposed to be negated and false otherwise
  bool computeTermsOfIfCondition_EQ(SgExpression *expr, varID &x, bool& negX, varID &y, bool& negY, long &c)
  {
    // skip if this is not a binary comparison condition
    SgEqualityOp *b_cond = isSgEqualityOp(expr);
    //printf("computeTermsOfIfCondition() A\n");
    if(!b_cond)
    {
#ifdef DEBUG_FLAG1
      cout << "Not a binary equality comparison condition: " << expr->unparseToString()
        << " has type: " << expr->sage_class_name() << "\n";
#endif
      return false;
    }

    // separate the condition into a left-hand-side and a right-hand-side
    SgExpression *lhs = b_cond->get_lhs_operand();
    SgExpression *rhs = b_cond->get_rhs_operand();

    //printf("computeTermsOfIfCondition() IsConstInt(lhs, c)=%d\n", IsConstInt(lhs, c));
    long cx, cy;
    varID x2, y2;
    bool negX2, negY2;
    if(parseAddition(lhs, x, negX, x2, negX2, cx) &&
        parseAddition(rhs, y, negY, y2, negY2, cy))
    {
      // (negX)x + (negX2)x2 + cx == (negY)y + (negY2)y2 + cy
      if(x2!=zeroVar || y2!=zeroVar) return false;

      // (negX)x + cx == (negY)y + cy	
      c = cy-cx;
      //printf("computeTermsOfIfCondition(), cx=%d, cy=%d, c=%d\n", cx, cy, c);
    }
    else
      return false;

#ifdef DEBUG_FLAG1
    cout << "TERMS OF CONDITION ARE x = " << x.getString() << "\ty = " << y.getString() << "\tc = " << c;
#endif
    return true;
  }

  // returns true if SgNode contains an array access to any variable in arrays
  // wrIndex - map of i+c pairs that contains for each variable in arrays all the indexes 
  //    used to access the array variable for a Write operation
  // rdIndex - map of i+c pairs that containsfor each variable in arrays all the indexes 
  //    used to access the array variable for a Read operation
  // rdFromExp - map of flags indicating for each variable in arrays whether ast_node 
  //    contains an array Read expression of this array with its index in the wrong format: 
  //    something other than i or i+c
  // wrFromExp - map of flags indicating for each variable in arrays whether ast_node 
  //    contains an array Write expression with its index in the wrong format: something 
  //    other than i or i+c
  bool findArrayAccesses(SgNode* ast_node, 
      m_varID2varID2quad &wrIndex, m_varID2varID2quad &rdIndex,
      m_varID2bool &rdFromExp, m_varID2bool &wrFromExp, 
      varIDSet arrays, m_varID2str vars2Name)
  {
    ROSE_ASSERT(0);
    return false;
    /*	SgNode* lhs, rhs;
    // indicates whether this is an expression that reads the left-hand-side (i.e. i++)
    bool readlhs;
    // we only consider assignments
    SgAssignOp* asgn = isSgBinaryOp(ast_node);

    if ( !(AstInterface::IsAssignment( ast_node, &lhs, &rhs, &readlhs ) ) )
    {
#ifdef DEBUG_FLAG1X
cout << "    findArrayAccesses() Not Assignment\n";
#endif
return false;
}

    // list of all array access expressions on the left-hand-side of ast_node
    Rose_STL_Container<SgNode *>arraysLhs;
    if ( lhs )
    arraysLhs = NodeQuery::querySubTree( lhs, V_SgPntrArrRefExp );

    // list of all array access expressions on the right-hand-side of ast_node	
    list<SgNode *>arraysRhs;
    if (rhs)
    arraysRhs = NodeQuery::querySubTree( rhs, V_SgPntrArrRefExp );

    bool found = false;

    // initially assume that no read or write access index is in the wrong format
    for ( set<variable>::iterator curArray = arrays.begin(); 
    curArray != arrays.end(); curArray++ )
    {
    rdFromExp[*curArray] = false;
    wrFromExp[*curArray] = false;
    }

    // processing lhs of assignment
#ifdef DEBUG_FLAG1X
cout << "        LHS accesses: " << arraysLhs.size() << "\tRHS accesses: " << arraysRhs.size() << "\n";
#endif
    // iterate over all the array accesses on the left-hand-side
    for ( Rose_STL_Container<SgNode *>::iterator it = arraysLhs.begin(); it != arraysLhs.end(); it++ )
    {
    ROSE_ASSERT ( *it );
    SgPntrArrRefExp *arr = isSgPntrArrRefExp( *it );
    ROSE_ASSERT ( arr );
    // convert the lhs of the array access expression into either a variable reference or
    // a dot expression
    SgVarRefExp *var = isSgVarRefExp( arr->get_lhs_operand() );
    SgDotExp *dot = isSgDotExp( arr->get_lhs_operand() );
    ROSE_ASSERT ( var || dot );
    varID j, v;
    quad c;
    // gets the id of the variable on the lhs of the array access expression
    v = getRefOfTypeConsidered( arr->get_lhs_operand() );

    // iterate through all the arrays of interest to see if the 
    // array being accessed is one that we care about
    for ( set<variable>::iterator curArray = arrays.begin(); 
    curArray != arrays.end(); curArray++ )
    {
#ifdef DEBUG_FLAG1X
cout << "        B LHS vars2Name["<<v<<"]="<<vars2Name[v]<<"  curArray="<<
vars2Name[*curArray]<<"\n";
#endif
    // if we care about the array being accessed
    if ( vars2Name[v] == vars2Name[*curArray] )
    {
    found = true; // we've found an access to an interesting array

    // index is of form j + c
    if ( parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c ) )
    {
    pair<varID, quad> p( j, c );
    // add <j,c> to the set of indexes written to
    wrIndex[*curArray].insert ( p );
    assert ( wrIndex[*curArray].find( j ) != wrIndex[*curArray].end() );
    // if this type of expression reads the lhs (i.e. i++), add 
    // <j,c> to the set of indexes being read from
    if ( readlhs )
      rdIndex[*curArray].insert( p );
  }
  // index is in the wrong form
    else
    {
      wrFromExp[*curArray] = true;
      if ( readlhs )
        rdFromExp[*curArray] = true;
    }

    // we've found the array, so we're done with this access
    break;
  }
  }
  }

// parsing rhs
for ( Rose_STL_Container<SgNode *>::iterator it = arraysRhs.begin(); it != arraysRhs.end(); it++ )
{
  ROSE_ASSERT ( isSgPntrArrRefExp( *it ) );
  // convert the rhs of the array access expression into either a variable reference or
  // a dot expression
  SgVarRefExp *var = isSgVarRefExp( isSgPntrArrRefExp( *it )->get_lhs_operand() );
  SgDotExp *dot = isSgDotExp( isSgPntrArrRefExp( *it )->get_lhs_operand() );
  ROSE_ASSERT ( var || dot );
  varID j, v;
  quad c;
  // gets the id of the variable on the rhs of the array access expression
  v = getRefOfTypeConsidered( isSgPntrArrRefExp( *it )->get_lhs_operand() );

  // iterate through all the arrays of interest to see if the 
  // array being accessed is one that we care about
  for ( set<variable>::iterator curArray = arrays.begin(); 
      curArray != arrays.end(); curArray++ )
  {
#ifdef DEBUG_FLAG1X
    cout << "        C RHS vars2Name["<<v<<"]="<<vars2Name[v]<<"  curArray="<<vars2Name[*curArray]<<"\n";
#endif

    // if we care about the array being accessed
    if ( vars2Name[v] == vars2Name[*curArray] )
    {
#ifdef DEBUG_FLAG1X
      cout << "        C.1 access found!  parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c )="<<parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c )<<"\n";
#endif

      varID j;
      quad c;
      found = true; // we've found an access to variableConsidered
      if ( parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c ) )
      {
        // add <j,c> to the set of indexes read from
        rdIndex[*curArray].insert ( pair<varID, quad>( j, c ) );
      }
      // index is in the wrong form
      else
        rdFromExp[*curArray] = true;
    }
  }
}
#ifdef DEBUG_FLAG1X
for ( set<variable>::iterator curArray = arrays.begin(); 
    curArray != arrays.end(); curArray++ )
cout << "findArrayAccesses END rdIndex["<<vars2Name[*curArray]<<"].size()="<<rdIndex[*curArray].size()<<"  wrIndex["<<vars2Name[*curArray]<<"].size()="<<wrIndex[*curArray].size()<<"  found["<<vars2Name[*curArray]<<"]="<<found<<"  rdFromExp["<<vars2Name[*curArray]<<"]="<<rdFromExp[*curArray]<<"  wrFromExp["<<vars2Name[*curArray]<<"]="<<wrFromExp[*curArray]<<"\n";
#endif
return found;*/
}

// examines the given expression. 
// If it is an array access, fills wrIndex with the ids of variables on the left-hand-side 
//    of the assignment, fills rdIndex with the ids of variables on the right-hand-side 
//    of the assignment and returns true.
// Otherwise, returns false.
// wrIndexSimp, rdIndexSimp - maps of array variables to variable/constant pairs. Each pair 
//    contains i,c that represent the fact that the given array variable was accessed (for 
//    writing or reading, respectively using the index expression [i+c], where i is a variable 
//    and c is a constant
// wrIndexCpx, rdIndexCpx -  lists of array variables that were accessed (for writing or 
//    reading, respectively) using a complex index expression
bool parseArrayAccess ( SgNode* ast_node,
    m_varID2varID2quad& wrIndexSimp, varIDlist& wrIndexCpx, 
    m_varID2varID2quad& rdIndexSimp, varIDlist& rdIndexCpx,
    m_varID2str vars2Name)
{
  ROSE_ASSERT ( 0 );
  return false;
  /*
     SgNode* lhs, rhs;
  // indicates whether this is an expression that reads the left-hand-side (i.e. i++)
  bool readlhs;
  // we only consider assignments
  if ( !(AstInterface::IsAssignment( ast_node, &lhs, &rhs, &readlhs ) ) )
  {
#ifdef DEBUG_FLAG1X
cout << "parseArrayAccess\n";
#endif
return false;
}

  // list of all array access expressions on the left-hand-side of ast_node
  Rose_STL_Container<SgNode *>arraysLhs;
  if ( lhs )
  arraysLhs = NodeQuery::querySubTree( lhs, V_SgPntrArrRefExp );

  // list of all array access expressions on the right-hand-side of ast_node	
  list<SgNode *>arraysRhs;
  if (rhs)
  arraysRhs = NodeQuery::querySubTree( rhs, V_SgPntrArrRefExp );

  // processing lhs of assignment
#ifdef DEBUG_FLAG1X
cout << "LHS accesses: " << arraysLhs.size() << "\tRHS accesses: " << arraysRhs.size() << "\n";
#endif
  // iterate over all the array accesses on the left-hand-side
  for ( Rose_STL_Container<SgNode *>::iterator it = arraysLhs.begin(); it != arraysLhs.end(); it++ )
  {
  ROSE_ASSERT ( *it );
  SgPntrArrRefExp *arr = isSgPntrArrRefExp( *it );
  ROSE_ASSERT ( arr );
  // convert the lhs of the array access expression into either a variable reference or
  // a dot expression
  SgVarRefExp *var = isSgVarRefExp( arr->get_lhs_operand() );
  SgDotExp *dot = isSgDotExp( arr->get_lhs_operand() );
  ROSE_ASSERT ( var || dot );
  varID j, v;
  quad c;
  // gets the id of the variable on the lhs of the array access expression
  v = getRefOfTypeConsidered( arr->get_lhs_operand() );

#ifdef DEBUG_FLAG1X
cout << "parseArrayAccess B LHS vars2Name["<<v<<"]="<<vars2Name[v]<<"\n";
#endif

  // if index is of form j + c
  if ( parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c ) )
  {
  m_varID2quad p( j, c );
  // add <j,c> to the set of indexes written to
  wrIndexSimp[v] = p;

  // if this type of expression reads the lhs (i.e. i++), add 
  // <j,c> to the set of indexes being read from
  if ( readlhs )
  rdIndexSimp[v] = p;
  }
  // else, if index is in the wrong form
  else
  {
  // add v as an array variable that is being accessed using a complex index expression
  wrIndexCpx.push_front(v);
  // if this type of expression reads the lhs (i.e. i++)
  if ( readlhs )
  rdIndexCpx.push_front(v);	
  }
  }

  // parsing rhs
  for ( Rose_STL_Container<SgNode *>::iterator it = arraysRhs.begin(); it != arraysRhs.end(); it++ )
{
  ROSE_ASSERT ( isSgPntrArrRefExp( *it ) );
  // convert the rhs of the array access expression into either a variable reference or
  // a dot expression
  SgVarRefExp *var = isSgVarRefExp( isSgPntrArrRefExp( *it )->get_lhs_operand() );
  SgDotExp *dot = isSgDotExp( isSgPntrArrRefExp( *it )->get_lhs_operand() );
  ROSE_ASSERT ( var || dot );
  varID j, v;
  quad c;
  // gets the id of the variable on the rhs of the array access expression
  v = getRefOfTypeConsidered( isSgPntrArrRefExp( *it )->get_lhs_operand() );

#ifdef DEBUG_FLAG1X
  cout << "parseArrayAccess C RHS vars2Name["<<v<<"]="<<vars2Name[v]<<"\n";
#endif

  if ( parseAddition( isSgBinaryOp( *it )->get_rhs_operand(), &j, &c ) )
  {
    // add <j,c> to the set of indexes read from
    rdIndexSimp[v] = m_varID2quad( j, c );
  }
  // index is in the wrong form
  else
    rdIndexCpx.push_front(v);
}

// return true if this expression involved an array access (use or def) and false otherwise
return (arraysLhs.size() + arraysRhs.size() > 0);*/
}

// if the given SgNode is an assignment operation, returns true, otherwise false
//// SgExpression that is that 
//// assignment (effectively a type cast) or NULL otherwise
/*SgExpression**/ bool isAssignment(SgNode* n)
{
  if(isSgAssignOp(n) || isSgPlusAssignOp(n) || isSgMinusAssignOp(n) || isSgMultAssignOp(n) || isSgDivAssignOp(n) ||
      isSgMinusMinusOp(n) || isSgPlusPlusOp(n)/* || isSgAssignInitializer(n)*/)
    //return isSgExpression(n);
    return true;
  // variable declaration with an initalization
  else if(isSgVariableDeclaration(n) && isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer() &&
      (isSgAssignInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer())/* ||
                                                                                                               SgAggregateInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer()*/))
      //return isSgExpression(isSgAssignInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer())->get_operand());
      return true;
      else
      //return NULL;
      return false;
      }

      // if the given SgNode is an assignment operation, returns an SgExpression that is the left-hand
      // side of this assignment and NULL otherwise
      SgNode* getAssignmentLHS(SgNode* n)
      {
      //SgExpression* asgn;

      if(isAssignment(n))
      {
      if(isSgBinaryOp(n))
      {
      return isSgBinaryOp(n)->get_lhs_operand();
      }
      else if(isSgUnaryOp(n))
      {
        return isSgUnaryOp(n)->get_operand();
      }
      /*else if(isSgAssignInitializer(asgn))
        {
        return isSgAssignInitializer(asgn)->get_operand();
        }*/
      else if(isSgVariableDeclaration(n))
      {
        if(isSgAssignInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer()))
          return isSgVariableDeclaration(n)->get_definition()->get_vardefn();
        /*else if(SgAggregateInitializer (isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer()))
          return isSgVariableDeclaration(n)->get_definition()->get_vardefn();*/
        else
          return NULL;
      }
      printf("getAssignmentLHS: asgn=<%s | %s>\n", n->class_name().c_str(), n->unparseToString().c_str());
      ROSE_ASSERT(0);
      }
      else
        return NULL;
      }

// if the given SgNode is an assignment operation, adds to rhs the set of SgNodes that comprise the right-hand
// side of this assignment
void getAssignmentRHS(SgNode* n, set<SgNode*>& rhs)
{
  //SgExpression* asgn;

  if(isAssignment(n))
  {
    if(isSgBinaryOp(n))
    {
      rhs.insert(isSgBinaryOp(n)->get_rhs_operand());
      if(isSgPlusAssignOp(n) || isSgMinusAssignOp(n) || isSgMultAssignOp(n) || isSgDivAssignOp(n))
        rhs.insert(isSgBinaryOp(n)->get_lhs_operand());
      return;
    }
    else if(isSgUnaryOp(n))
    {
      rhs.insert(isSgUnaryOp(n)->get_operand());
      return;
    }
    /*else if(isSgAssignInitializer(asgn))
      {
      return isSgAssignInitializer(asgn)->get_operand();
      }*/
    else if(isSgVariableDeclaration(n))
    {
      if(isSgAssignInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer()))
      {
        rhs.insert(isSgAssignInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer())->get_operand());
        return;
      }
      /*else if(SgAggregateInitializer (isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer()))
        return SgAggregateInitializer(isSgVariableDeclaration(n)->get_definition()->get_vardefn()->get_initializer())->get_initializers();*/
    }
  }

  printf("getAssignmentRHS: n=<%s | %s>\n", n->class_name().c_str(), n->unparseToString().c_str());
  ROSE_ASSERT(0);
}


// Returns true if the expression is of the permitted type
//    and sets op, i, j, k and c appropriately to represent an expression of the form
//    i = j op k op c 
//    where op may be either + (add), * (mult) or / (divide)
//    op may be = none if the rhs has only one term
// This function parses expressions such as i = j op k, i = j op c, i op= j, i++)
bool parseAssignment(/*SgExpression*/SgNode* expr, short& op, varID &i, varID &j, bool& negJ, varID &k, bool& negK, long &c)
{
  SgBinaryOp* binOp;
  SgUnaryOp* upOp;

  negJ = false;
  negK = false;

  //printf("parseAssignment(<%s | %s>)\n", expr->class_name().c_str(), expr->unparseToString().c_str());

  if((binOp = isSgBinaryOp(expr)))
  {
    //printf("parseAssignment binOp\n");
    SgExpression* lhs = binOp->get_lhs_operand();
    SgExpression* rhs = binOp->get_rhs_operand();

    // if this is an assignment operation
    if(isSgAssignOp(binOp))
    {
      //printf("parseAssignment isSgAssignOp  valid lhs=%d <%s | %s>\n", varID::isValidVarExp(lhs), lhs->class_name().c_str(), lhs->unparseToString().c_str());
      // if the lhs is not a variable that we can understand, break
      if(!varID::isValidVarExp(lhs)) return false;

      /*			printf("parseAssignment isSgAssignOp  valid rhs=%d (%s)\n", parseAddition(rhs, j, c), lhs->class_name().c_str());				
                                printf("parseAssignment isSgAssignOp  i=%s j=%s c=%d  &i=0x%x &j=0x%x\n", i.str().c_str(), j.str().c_str(), c, &i, &j);*/

      // if the rhs of the assignment is not parseable operation, abort
      if(!parseExpr(rhs, op, j, negJ, k, negK, c))
        return false;

      // set i to be the expression on the lhs
      i.init(lhs); 

      /* // if the rhs of the assignment is a parseable addition operation
         if(parseAddition(rhs, j, negJ, k, negK, c))
         {
      // if the rhs only has one term
      if((c==0 && (j==zeroVar || k==zeroVar)) || 
      (c!=0 && j==zeroVar && k==zeroVar))
      op = none;
      else
      op = add;
      i.init(lhs); // initialize i to be the expression on the lhs
      }
      // if the rhs of the assignment is a parseable multiplication operation
      else if(parseMultiplication(rhs, j, k, c))
      {
      // if the rhs only has one term
      if((c==0 && (j==zeroVar || k==zeroVar)) || 
      (c!=0 && j==zeroVar && k==zeroVar))
      op = none;
      else
      op = mult;
      i.init(lhs); // initialize i to be the expression on the lhs
      }
      // if the rhs of the assignment is a parseable division operation
      else if(parseDivision(rhs, j, k, c))
      {
      // if the rhs only has one term
      if((c==0 && (j==zeroVar || k==zeroVar)) || 
      (c!=0 && j==zeroVar && k==zeroVar))
      op = none;
      else
      op = divide;
      i.init(lhs); // initialize i to be the expression on the lhs
      }
      else return false;*/

      /*printf("1-------\n");
        printf("parseAssignment isSgAssignOp  i=%s &i=0x%x\n", i.str().c_str(), &i);
        printf("parseAssignment isSgAssignOp  j=%s &j=0x%x\n", j.str().c_str(), &j);
        printf("1-------\n");*/
    }
    // if this is +=, *= or /= update operation
    else if(isSgPlusAssignOp(binOp) ||
        isSgMinusAssignOp(binOp) ||
        isSgMultAssignOp(binOp) ||
        isSgDivAssignOp(binOp))
    {
      //printf("parseAssignment OpAssign, varID::isValidVarExp(lhs)=%d\n", varID::isValidVarExp(lhs));
      // if the lhs is not a variable that we can understand, break
      if(!varID::isValidVarExp(lhs)) return false;

      if(isSgPlusAssignOp(binOp) ||
          isSgMinusAssignOp(binOp))
        op = add;
      else if(isSgMultAssignOp(binOp))
        op = mult;
      else if(isSgDivAssignOp(binOp))
        op = divide;

      //printf("      op=%d\n");

      // if the rhs of the op= is a constant integer
      if(IsConstInt(rhs, c))
      {
        //printf("      const rhs, c=%d\n", c);
        // both i and j are equal to the variable on the lhs
        i.init(lhs);
        j.init(lhs);
        if(isSgPlusAssignOp(binOp) ||
            isSgMinusAssignOp(binOp))
          k = zeroVar;
        else
          k = oneVar;

        // if this is a -=, c gets its sign flipped
        if(isSgMinusAssignOp(binOp))
          c = 0-c;
      }
      // if the rhs of the op= is a variable
      else if(varID::isValidVarExp(rhs))
      {
        //printf("      variable rhs\n");
        // both i and j are equal to the variable on the lhs
        i.init(lhs);
        j.init(lhs);
        k.init(rhs);

        if(isSgPlusAssignOp(binOp) ||
            isSgMinusAssignOp(binOp))
          c = 0;
        else
          c = 1;

        if(isSgMinusAssignOp(binOp))
          negK = true;
      }
      else return false;
    }
    else return false;
  }
  else if ((upOp = isSgUnaryOp(expr)))
  {
    SgExpression* lhs = upOp->get_operand();

    // if the lhs is not a variable that we can understand, break
    if(!varID::isValidVarExp(lhs)) return false;

    i.init(lhs);
    j.init(lhs);
    k.init(zeroVar);
    op = add;

    // i++, i--
    if(expr->variantT() == V_SgMinusMinusOp)
      c = -1;
    else if(expr->variantT() == V_SgPlusPlusOp)
      c = 1;
    else
      return false;
  }
  // !!!! This should eventually be removed as it has been superceded by the case below 
  else if(isSgAssignInitializer(expr))
  {
    SgAssignInitializer* asgnInit = isSgAssignInitializer(expr);
    //printf("parseAssignment() isSgAssignInitializer: asgnInit->get_operand_i() = <%s | %s>\n", asgnInit->get_operand_i()->class_name().c_str(), asgnInit->get_operand_i()->unparseToString().c_str());

    if(IsConstInt(asgnInit->get_operand_i(), c))
    {
      SgInitializedName* initName = isSgInitializedName(asgnInit->get_parent());
      ROSE_ASSERT(initName);
      i.init(initName);
      j.init(zeroVar);
      k.init(zeroVar);
      op = none;
    }
  }
  else if(isSgVariableDeclaration(expr))
  {
    SgAssignInitializer* asgnInit;
    if((asgnInit = isSgAssignInitializer(isSgVariableDeclaration(expr)->get_definition()->get_vardefn()->get_initializer())))
    {
      if(IsConstInt(asgnInit->get_operand(), c))
      {
        SgInitializedName* initName = isSgVariableDeclaration(expr)->get_definition()->get_vardefn();
        ROSE_ASSERT(initName);
        i.init(initName);
        j.init(zeroVar);
        k.init(zeroVar);
        op = none;
      }
      else
        return false;
    }
  }
  else 
    return false;

  /*printf("2-------\n");
    printf("parseAssignment isSgAssignOp  i=%s &i=0x%x\n", i.str().c_str(), &i);
    printf("parseAssignment isSgAssignOp  j=%s &j=0x%x\n", j.str().c_str(), &j);
    printf("2-------\n");

    printf("parseAssignment  i=%s j=%s c=%d\n", i.str().c_str(), j.str().c_str(), c);*/
  return true;
}

// Returns true if the expression is of the permitted type
//    and sets op, i, j and c appropriately to represent an expression of the form
//    i op j op c 
//    where op may be either + (add), * (mult) or / (divide)
//    op may be = none if the rhs has only one term
// This function parses non-assignment expressions such as i op j or c but not i = j op c, i op= j or i++
bool parseExpr(SgExpression* expr, short& op, varID &i, bool &negI, varID& j, bool &negJ, long &c)
{
  // if the expression is a parseable addition operation
  if(parseAddition(expr, i, negI, j, negJ, c))
  {
    // if the rhs only has one term
    if((c==0 && (i==zeroVar || j==zeroVar)) || 
        (c!=0 && i==zeroVar && j==zeroVar))
      op = none;
    else
      op = add;
  }
  // if the rhs of the assignment is a parseable multiplication operation
  else if(parseMultiplication(expr, i, j, c))
  {
    // if the rhs only has one term
    if((c==0 && (i==zeroVar || j==zeroVar)) || 
        (c!=0 && i==zeroVar && j==zeroVar))
      op = none;
    else
      op = mult;
  }
  // if the rhs of the assignment is a parseable division operation
  else if(parseDivision(expr, i, j, c))
  {
    // if the rhs only has one term
    if((c==0 && (i==zeroVar || j==zeroVar)) || 
        (c!=0 && i==zeroVar && j==zeroVar))
      op = none;
    else
      op = divide;
  }
  else return false;

  return true;
}

// returns true if parsing was successful
//    (expressions accepted: c, -c, j, c +/- j, j +/- c, j +/- k
// and sets *j, *k and *c appropriately
// negJ=true if j is supposed to be negated and false otherwise
// negK=true if k is supposed to be negated and false otherwise
bool parseAddition(SgExpression* expr, varID &j, bool &negJ, varID& k, bool &negK, long &c)
{
  //printf("   parseAddition(<%s | %s>)\n", expr->class_name().c_str(), expr->unparseToString().c_str());
  expr = unwrapCasts(expr);
  //printf("   unwrapped expr: <%s | %s>\n", expr->class_name().c_str(), expr->unparseToString().c_str());

  negJ = false;
  negK = false;

  if(varID::isValidVarExp(expr))
  {
    j.init(expr);
    k = zeroVar;
    c = 0;
  }
  else if(isSgUnaryOp(expr))
  {
    // unary expressions accepted -c
    if (IsConstInt(isSgUnaryOp(expr)->get_operand(), c) &&
        isSgMinusOp( expr ))
    {
      c = -c;
      j = zeroVar;
      k = zeroVar;
    }
    else
      return false;
  }
  else if(isSgSubtractOp(expr) || isSgAddOp(expr))
  {
    // j +/- ?
    //printf("   parseAddition add/subtract valid lhs=%d\n", varID::isValidVarExp(isSgBinaryOp(expr)->get_lhs_operand()));
    if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand())))
    {
      // j +/- c?
      //printf("   parseAddition add/subtract const rhs=%d\n", IsConstInt( isSgBinaryOp(expr)->get_rhs_operand(), c));
      if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()), c))
      {
        // flip c if its a subtraction
        if (isSgSubtractOp(expr))
          c = -c;
        k = zeroVar;
      }
      // j +/- k
      else if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        //printf("parseAddition() j+/-k, isSgSubtractOp(<%s | %s>)=%d\n", expr->class_name().c_str(), expr->unparseToString().c_str(), isSgSubtractOp(expr));
        k.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        negK = isSgSubtractOp(expr);

        c=0;
      }
      else
        return false;
      j.init(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()));

      //printf("   parseAddition add/subtract lhs=%s rhs=%d  &j=0x%x\n", j.str().c_str(), c, &j);
    }
    // c +/- ???
    else if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()), c))
    {
      // c +/- j
      if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        j.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        negJ = isSgSubtractOp(expr);
        k = zeroVar;
      }
    }
    else
      return false;
  }
  // expr: c
  else if (IsConstInt(expr, c))
  {
    j = zeroVar;
    k = zeroVar;
  }
  else
  {
    //		printf("      invalid\n");
    return false;
  }		

  return true;
}

// returns true if parsing was successful
//    (expressions accepted: c, -c, j, c * j, j * c, j * k
// and sets *j, *k and *c appropriately
bool parseMultiplication(SgExpression* expr, varID &j, varID& k, long &c)
{
  //printf("   parseAddition expr = <%s | %s>\n", expr->class_name().c_str(), expr->unparseToString().c_str());
  expr = unwrapCasts(expr);

  if(varID::isValidVarExp(expr))
  {
    j.init(expr);
    k = oneVar;
    c = 1;
  }
  else if(isSgUnaryOp(expr))
  {
    // unary expressions accepted -c
    if (IsConstInt(isSgUnaryOp(expr)->get_operand(), c) &&
        isSgMinusOp( expr ))
    {
      c = -c;
      j = oneVar;
      k = oneVar;
    }
    else
      return false;
  }
  else if(isSgMultiplyOp(expr))
  {
    // j * ?
    //printf("   parseAddition add/subtract valid lhs=%d\n", varID::isValidVarExp(isSgBinaryOp(expr)->get_lhs_operand()));
    if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand())))
    {
      // j * c?
      //printf("   parseAddition add/subtract const rhs=%d\n", IsConstInt( isSgBinaryOp(expr)->get_rhs_operand(), c));
      if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()), c))
      {
        k = oneVar;
      }
      // j * k
      else if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        //printf("parseAddition() j+/-k, isSgSubtractOp(<%s | %s>)=%d\n", expr->class_name().c_str(), expr->unparseToString().c_str(), isSgSubtractOp(expr));
        k.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        c=1;
      }
      else
        return false;
      j.init(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()));

      //printf("   parseAddition add/subtract lhs=%s rhs=%d  &j=0x%x\n", j.str().c_str(), c, &j);
    }
    // c * ???
    else if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()), c))
    {
      // c * j
      if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        j.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        k = oneVar;
      }
    }
  }
  // expr: c
  else if (IsConstInt(expr, c))
  {
    j = oneVar;
    k = oneVar;
  }
  else
  {
    //		printf("      invalid\n");
    return false;
  }		

  return true;
}

// returns true if parsing was successful
//    (expressions accepted: c, -c, j, j / c, c / k, j / k
// and sets *j, *k and *c appropriately
bool parseDivision(SgExpression* expr, varID &j, varID& k, long &c)
{
  //printf("   parseAddition expr = <%s | %s>\n", expr->class_name().c_str(), expr->unparseToString().c_str());
  expr = unwrapCasts(expr);

  if(varID::isValidVarExp(expr))
  {
    j.init(expr);
    k = oneVar;
    c = 1;
  }
  else if(isSgUnaryOp(expr))
  {
    // unary expressions accepted -c
    if (IsConstInt(isSgUnaryOp(expr)->get_operand(), c) &&
        isSgMinusOp( expr ))
    {
      c = -c;
      j = oneVar;
      k = oneVar;
    }
    else
      return false;
  }
  else if(isSgDivideOp(expr))
  {
    // j / ?
    //printf("   parseAddition add/subtract valid lhs=%d\n", varID::isValidVarExp(isSgBinaryOp(expr)->get_lhs_operand()));
    if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand())))
    {
      // j / c?
      //printf("   parseAddition add/subtract const rhs=%d\n", IsConstInt( isSgBinaryOp(expr)->get_rhs_operand(), c));
      if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()), c))
      {
        k = oneVar;
      }
      // j / k
      else if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        //printf("parseAddition() j+/-k, isSgSubtractOp(<%s | %s>)=%d\n", expr->class_name().c_str(), expr->unparseToString().c_str(), isSgSubtractOp(expr));
        k.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        c=1;
      }
      else
        return false;
      j.init(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()));

      //printf("   parseAddition add/subtract lhs=%s rhs=%d  &j=0x%x\n", j.str().c_str(), c, &j);
    }
    // c / ???
    else if(IsConstInt(unwrapCasts(isSgBinaryOp(expr)->get_lhs_operand()), c))
    {
      // c / k
      if(varID::isValidVarExp(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand())))
      {
        k.init(unwrapCasts(isSgBinaryOp(expr)->get_rhs_operand()));
        j = oneVar;
      }
    }
  }
  // expr: c
  else if (IsConstInt(expr, c))
  {
    j = oneVar;
    k = oneVar;
  }
  else
  {
    //		printf("      invalid\n");
    return false;
  }		

  return true;
}

// returns true if the given SgValueExp is some type of integral value, rather than a string or something more complex
/*bool isIntegralVal(SgValueExp* exp)
  {
  switch(exp->variantT)
  {
  case V_SgBoolVal: case V_SgCharVal: case V_SgDoubleVal: case V_SgEnumVal:
  case V_SgFloatVal: case V_SgIntVal: case V_SgLogDoubleVal: case V_SgLongIntVal: 
  case V_SgLongLongIntVal: case V_SgShortVal: case V_SgStringVal: case V_SgUnsignedCharVal: 
  case V_SgUnsignedIntVal: case V_SgUnsignedLongLongVal: case V_SgUnsignedLongVal: case V_SgUnsignedShortVal: 
  case V_SgWcharVal:
return true;

default:
return false;
}
}

// if the given SgValueExp contains an integral value, returns that integer
// no error return value, since caller supposed to check with isIntegralVal()
int getIntegralVal(SgValueExp* exp)
{
switch(exp->variantT)
{
case V_SgBoolVal: case V_SgCharVal: case V_SgDoubleVal: case V_SgEnumVal: 
case V_SgFloatVal: case V_SgIntVal: case V_SgLogDoubleVal: case V_SgLongIntVal: 
case V_SgLongLongIntVal: case V_SgShortVal: case V_SgStringVal: case V_SgUnsignedCharVal: 
case V_SgUnsignedIntVal: case V_SgUnsignedLongLongVal: case V_SgUnsignedLongVal: case V_SgUnsignedShortVal: 
case V_SgWcharVal:
return (int)exp->get_value();

default:
return 0;
}
}*/

// returns whether a given AST node that represents a constant is an integer and
// sets val to be the numeric value of that integer (all integer types are included
// but not floating point, characters, etc.)
bool IsConstInt(SgExpression* rhs, long &val)
{
  //	printf("rhs = %s: %s\n", rhs->unparseToString().c_str(), rhs->class_name().c_str());

  /*SgCastExp* cast;
    if(cast = isSgCastExp(rhs))
    {
    printf("cast = %s: %s\n", cast->get_type()->unparseToString().c_str(), cast->get_type()->class_name().c_str());
    }*/

  switch(rhs->variantT())
  {
    case V_SgIntVal:
      {
        val = isSgIntVal(rhs)->get_value();
        return true;
      }
    case V_SgLongIntVal:
      {
        val = isSgLongIntVal(rhs)->get_value();
        return true;
      }
    case V_SgLongLongIntVal:
      {
        val = isSgLongLongIntVal(rhs)->get_value();
        return true;
      }
    case V_SgShortVal:
      {
        val = isSgShortVal(rhs)->get_value();
        return true;
      }
    case V_SgUnsignedIntVal:
      {
        val = isSgUnsignedIntVal(rhs)->get_value();
        return true;
      }
    case V_SgUnsignedLongVal:
      {
        val = isSgUnsignedLongVal(rhs)->get_value();
        return true;
      }
    case V_SgUnsignedLongLongIntVal:
      {
        val = isSgUnsignedLongLongIntVal(rhs)->get_value();
        return true;
      }
    case V_SgUnsignedShortVal:
      {
        val = isSgUnsignedShortVal(rhs)->get_value();
        return true;
      }
    default:
      {
        return false;
      }
  }
}

} /* namespace cfgUtils */

// Liao 10/7/2010, made a few functions' namespace explicit
// pulls off all the SgCastExps that may be wrapping the given expression, returning the expression that is being wrapped
SgExpression* cfgUtils::unwrapCasts(SgExpression* e)
{
  if(isSgCastExp(e))
  {
    return unwrapCasts(isSgCastExp(e)->get_operand());
  }
  else return e;
}

// returns the DataflowNode that represents that start of the CFG of the given function's body
DataflowNode cfgUtils::getFuncStartCFG(SgFunctionDefinition* func)
{
  DataflowNode funcCFGStart = (DataflowNode)func->cfgForBeginning();
  for(VirtualCFG::iterator it(funcCFGStart); it!=VirtualCFG::iterator::end(); it++)
  {
    //		printf("getFuncStartCFG(): isSgFunctionParameterList((*it).getNode())=%d (*it)=<%s | %s>\n", isSgFunctionParameterList((*it).getNode()), (*it).getNode()->unparseToString().c_str(), (*it).getNode()->class_name().c_str());
    if(isSgFunctionParameterList((*it).getNode()))
    {
      //			printf("getFuncStartCFG(): returning a SgFunctionParameterList\n");
      return (*it);
    }
  }
  //	printf("getFuncStartCFG(): returning function's declaration\n");
  return funcCFGStart;
  /*
     SgBasicBlock *funcBody = func->get_body();
     DataflowNode bodyStart = (DataflowNode)funcBody->cfgForBeginning();
     ROSE_ASSERT(bodyStart.outEdges().size());

     return (*(bodyStart.outEdges().begin())).target();*/
}

// returns the DataflowNode that represents that end of the CFG of the given function's body
DataflowNode cfgUtils::getFuncEndCFG(SgFunctionDefinition* func)
{
  return (DataflowNode) func->cfgForEnd();
}

// returns a string containing a unique name that is not otherwise used inside this project
string cfgUtils::genUniqueName()
{
  string name = "temp_";

  Rose_STL_Container<SgNode*> initNames = NodeQuery::querySubTree(project, V_SgInitializedName);
  for(Rose_STL_Container<SgNode*>::iterator it = initNames.begin(); it!= initNames.end(); it++)
  {
    SgInitializedName *curName;
    ROSE_ASSERT(curName = isSgInitializedName(*it));
    // while our chosen "unique" name conflicts with the current SgInitializedName
    // keep adding random numbers to the end of the the "unique" name until it becomes unique
    //		printf("SgInitializedName: name<%s> == curName->get_name().getString()<%s> = %d\n", name.c_str(), curName->get_name().getString().c_str(), name == curName->get_name().getString());
    while(name == curName->get_name().getString())
    {
      char num[2];
      num[0] = '0'+rand()%10;
      num[1] = 0;			
      name = name + num;
    }
  }

  Rose_STL_Container<SgNode*> funcDecls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  for(Rose_STL_Container<SgNode*>::iterator it = funcDecls.begin(); it!= funcDecls.end(); it++)
  {
    SgFunctionDeclaration *curDecl;
    ROSE_ASSERT(curDecl = isSgFunctionDeclaration(*it));
    // while our chosen "unique" name conflicts with the current SgFunctionDeclaration
    // keep adding random numbers to the end of the the "unique" name until it becomes unique
    //		printf("SgFunctionDeclaration: name<%s> == curDecl->get_name().getString()<%s> = %d\n", name.c_str(), curDecl->get_name().getString().c_str(), name == curDecl->get_name().getString());
    while(name == curDecl->get_name().getString())
    {
      char num[2];
      snprintf(num, 2, "%s", (char*)(rand()%10));
      name = name + num;
    }
  }
  return name;
}

// returns the SgFunctionDeclaration for the function with the given name
SgFunctionDeclaration* cfgUtils::getFuncDecl(string name)
{
  Rose_STL_Container<SgNode*> funcDecls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
  for(Rose_STL_Container<SgNode*>::iterator it = funcDecls.begin(); it!= funcDecls.end(); it++)
  {
    SgFunctionDeclaration *curDecl;
    ROSE_ASSERT(curDecl = isSgFunctionDeclaration(*it));
    // if we've found our function
    while(name == curDecl->get_name().getString())
    {
      return curDecl;
    }
  }
  return NULL;
}

// given a function's declaration, returns the function's definition.
// handles the case where decl->get_definition()==NULL
SgFunctionDefinition* cfgUtils::funcDeclToDef(SgFunctionDeclaration* decl)
{
  ROSE_ASSERT(CFGinitialized);
  if(decl->get_definition())
    return decl->get_definition();
  else
  {
    Rose_STL_Container<SgNode*> funcDefs = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
    for(Rose_STL_Container<SgNode*>::iterator it = funcDefs.begin(); it!= funcDefs.end(); it++)
    {
      ROSE_ASSERT(isSgFunctionDefinition(*it));
      // if the current definition has the same mangled name as the function declaration, we've found it
      if(isSgFunctionDefinition(*it)->get_mangled_name() == decl->get_mangled_name())
        return isSgFunctionDefinition(*it);
    }
  }
  return NULL;
}


