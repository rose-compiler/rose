class DFNode
{
 public:
 	// the information associated with the StartSaveAnalysis
 	StartSaveAnalysis* ssa;
 	
 	// the information associated with the ChkptRangeAnalysis
 	ChkptRangeAnalysis* cra;

	// indicates whether the condition asserted by this node (there can be only one assertCond per CFG node)
	// is the exact opposite of what it should be.
	// (we don't bother bother creating an AST for the opposite condition and instead just set this flag)
	bool negateCondition;
	
	// indicates whether this CFG node represents a loop with a pre-condition
	bool preCondLoop;
	
	// the AST node that is the condition of this node (if it is an If or Loop)
	SgNode* condition;
	
	// the CFG node's id. Node ids are assigned via a pre-order counting scheme, where the CFG is traversed in 
	// a depth-first tree fashion, with sibling nodes traversed from left to right. 
	int nodeId;
	
/*	// the name of the array currently being considered in this phase of the analysis
	string variableConsidered;*/
	
	// set of arrays being used by the application
	varIDSet appArrays;

	// var2Name - the mapping from variable ID to variable name for all application
	//    variables that are relevant in this analysis
	// node - the CFG node that this DFNode's dataflow information corresponds to
	DFNode(m_varID2str *vars2Name, CFGNodeImpl *node, int nodeId, 
	       varIDSet appArrays, bool preCondLoop) {
		negateCondition = false;
		condition = 0;
 		
 		this->nodeId = nodeId;
		this->appArrays = appArrays;
 		this->preCondLoop = preCondLoop;
 		
 		ssa = NULL;
 		cra = NULL;
	}
	
	void initDF(m_varID2str *vars2Name, CFGNodeImpl *node, varIDSet appArrays)
	{
		ssa = new StartSaveAnalysis(vars2Name, node);
		cra = new ChkptRangeAnalysis(vars2Name, node, appArrays);
	}
	
	// delete the current cra information and create a fresh cra object
	void resetCRA(m_varID2str *vars2Name, CFGNodeImpl *node, varIDset appArrays)
	{
		if(cra) delete cra;
		cra = new ChkptRangeAnalysis(vars2Name, node, appArrays); 
	}

	// parses an If or loop statement and sets the condition field to be
	// equal to that if the statement
	void addCondition( SgNode* ast_node)
	{
		varID x, y;
		quad c, value;
	
		SgNode* truebody = 0, falsebody = 0, cond = 0;
		SgNode* init = 0, incr = 0, body = 0;
	
		bool readlhs = false;
	
	  // Make sure that ast_node is in fact an If statement or a Loop.
		if ( !AstInterface::IsIf( ast_node, &cond, &truebody, &falsebody ) )
			if ( !AstInterface::IsLoop( ast_node, &init, &cond, &incr, &body ) )
				assert ( 0 );
		if ( AstInterface::IsPostTestLoop( ast_node ) )
			{
	#ifdef DEBUG_FLAG2
				cout << "Found post loop\n";
	#endif
	
				assert ( 0 );
				return;
			}
	
		// TODO: insert code for conditions that are constants, TRUE, FALSE
		if ( cond == 0 && cond->variantT() == V_SgValueExp )
			{
	#ifdef DEBUG_FLAG1
				cout << "Found CONSTANT condition\n";
	#endif
	
				assert ( 0 );
				return;
			}
	
		// set node condition
		SgTreeCopy treeCopy;
		SgExpression *expr;
	
		// condition is SgStatement for while-do, do-while and if
		if ( isSgExprStatement( &(*cond) ) )
			{
				SgExprStatement *exprStm = isSgExprStatement( &(*cond) );
				assert ( exprStm );
				expr = isSgExpression( exprStm->get_the_expr()->copy( treeCopy ) );
			}
		else
			if ( AstInterface::IsLoop( ast_node ) )
				// for-loop has condition as SgExpression, not SgStatement
				// this is likely to be changed in future versions
				expr = isSgExpression( cond->copy( treeCopy ) );
	
		ROSE_ASSERT ( isSgExpression( expr ) );
		expr->set_parent( 0 ); 
		condition = expr;
	}

	~DFNode ()
	{
		if(ssa!=NULL)
			delete ssa;
		if(cra!=NULL)
			delete cra;
	}
};