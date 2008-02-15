void flattenScopes(SgFunctionDefinition * fDef)
{
	// * 3.3: find all variables in the function,give them a unique name and move them to the bgeinning of the function, this is allowed because this is for C only!!!. This would not work for classes which must be constructed

	// rename variables
	list<SgNode*> varDeclList=NodeQuery::querySubTree(fDef,V_SgInitializedName);
	for (list<SgNode*>::iterator varDecl=varDeclList.begin();varDecl!=varDeclList.end();varDecl++)
	{
		tring varName=isSgInitializedName(*varDecl)->get_name().getString();
		char numberCString[255];
		sprintf(numberCString,"%i",idNr);
		string newVarName=string("PML")+string(numberCString)+string("_")+varName;
		idNr++;

		isSgInitializedName(*varDecl)->set_name(SgName(newVarName.c_str()));		
	}					

	list<SgNode*> newDeclList;
	varDeclList.clear();
	varDeclList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgVariableDeclaration);
	// the move the variable declaration to the function begin and replace the old declaration site with a definition
	for (list<SgNode*>::iterator varDecl=varDeclList.begin();varDecl!=varDeclList.end();varDecl++)
	{
		SgVariableDeclaration * varDeclStmt=isSgVariableDeclaration(*varDecl);
		SgVariableDefinition * varDef=varDeclStmt->get_definition();
		SgInitializedName *iniName=isSgInitializedName(varDef->get_vardefn());
		if (iniName->get_initializer () !=NULL)
		{
			//				cout <<"VarDecl >"<<iniName->get_name().getString()<<"< has initilizer >"<<iniName->get_initializer ()->unparseToString()<<"<"<<endl;
			// determine if it is safe to separate initializer from decl
			// for now true
			if (1)
			{
				Sg_File_Info  * fi=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
				SgVarRefExp * varRef=new	SgVarRefExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),new SgVariableSymbol(iniName));
				SgType * type=isSgAssignInitializer(iniName->get_initializer())->get_type ();
				SgAssignInitializer * sai=isSgAssignInitializer(iniName->get_initializer());
				if (sai==NULL)
				{
					cerr<<"isSgAssignInitializer(iniName->get_initializer())=NULL"<<endl;
					exit(-1);
				}
				SgExpression *lhs,*rhs;
				lhs=varRef;
				rhs=sai->get_operand ();
				if (lhs==NULL)
				{
					cerr<<"lhs=NULL"<<endl;
					exit(-1);
				}
				if (rhs==NULL)
				{
					cerr<<"rhs=NULL"<<endl;
					exit(-1);
				}
				SgAssignOp * assignment=new SgAssignOp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs,rhs);
				SgExprStatement * expr=new SgExprStatement(fi,assignment);
				if (expr==NULL)
				{
					cerr<<"construction of expr failed"<<endl;
				}
				isSgAssignInitializer(iniName->get_initializer ())->set_operand(NULL);
				free(iniName->get_initializer ());
				iniName->set_initializer (NULL);
				// put the iniName in the list
				newDeclList.push_back(varDeclStmt);
				if (isSgStatement(varDeclStmt)==NULL)
				{
					cerr<<"isSgStatement(varDeclStmt)==NULL"<<endl;
				}
				LowLevelRewrite::replace(isSgStatement(varDeclStmt),isSgStatement(expr));					
			}				
		}
		else
		{
			cout <<"VarDecl >"<<iniName->get_name().getString()<<"> is uninitialized"<<endl;
			newDeclList.push_back(varDeclStmt);
			LowLevelRewrite::remove(isSgStatement(varDeclStmt));
		}
	}

	for (list<SgNode*>::iterator varDecl=newDeclList.begin();varDecl!=newDeclList.end();varDecl++)
	{
		fDef->prepend_statement(isSgStatement(*varDecl));
	}
	return ;
}
