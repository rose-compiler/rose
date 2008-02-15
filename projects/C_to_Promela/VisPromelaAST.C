#include <rose.h>
#include "VisPromelaAST.h"
#include "tools.h"
#include <list>
#include <set>
using namespace std;


VisPromelaAST::VisPromelaAST(SgProject *proj):proj(proj)
{
}

// bool traverse(SgNode * node) {return traverse(node, false);}
void VisPromelaAST::generate(string filename)
{
	ofstream of(filename.c_str());
	SgFilePtrList fileList=*proj->get_fileList ();
	vector<SgFile*> fileVec;
	for (SgFilePtrList::iterator it=fileList.begin();fileList.end()!=it;it++)
	{
		fileVec.push_back(*it);
	}
	
	list<SgNode*> statements=NodeQuery::querySubTree(proj,V_SgStatement);
	of <<"digraph \""<<filename<<"\" {"<<endl;
	
	for (list<SgNode*>::iterator it=statements.begin();statements.end()!=it;it++)
	{
		
		SgStatement * stmt=isSgStatement(*it);
//		if (isSgParameterList(stmt)) continue;
		if (!isSgBasicBlock(stmt) &&stmt->get_file_info()->isCompilerGenerated ()) continue;
		bool inSourceFile=false;
		for (int i=0;i<fileVec.size();i++)
			inSourceFile|=stmt->get_file_info()->isSameFile(fileVec[i]);
		if (!stmt->get_file_info()->isOutputInCodeGeneration () )
		if (!inSourceFile && !isSgBasicBlock(stmt) /*&& (isSgFunctionDeclaration(stmt)||isSgFunctionDefinition(stmt))*/) continue;
		SgStatement * parent;
		char stmtID[1024],parentID[1024];
		sprintf(stmtID,"%p",stmt);
		// generate the statement node
		of <<"\"" <<stmtID<< "\" [label=\""<<stmt->class_name();
		if (!isSgScopeStatement(stmt) && !isSgFunctionDeclaration(stmt) &&!isSgFunctionDefinition(stmt))
			of<<"\\n"<<stmt->unparseToString();
		
		of<<"\"";
		
		if (toConvert(stmt))
		{
			of<<",fillcolor=deepskyblue style=\"filled\"";
		}
		of <<"];"<<endl;
		
		if (!isSgProject(stmt))
		{
			SgNode * tmp;
//			cout <<stmt->class_name()<<endl;
			tmp=stmt->get_parent();
			if (tmp==NULL) continue;
			
			
			while(!(isSgStatement(tmp)||isSgFile(tmp)) )
		//	while(!isSgStatement(tmp)&& !isSgProject(tmp) && !isSgFile(tmp) && tmp->get_parent()!=NULL)
				tmp=tmp->get_parent();
			
			sprintf(parentID,"%p",tmp);
			// build edge between parent and child
			of <<"\""<<parentID<<"\"->\""<<stmtID<<"\";\n"<<endl;
		}		
	}
	// get file..
	list<SgNode*> files=NodeQuery::querySubTree(proj,V_SgFile);
	for (list<SgNode*>::iterator it=files.begin();files.end()!=it;it++)
	{
		SgFile * file=isSgFile(*it);
		if (file==NULL) 
		{
			continue;
		}
		char fileID[1024];
		sprintf(fileID,"%p",file);
		of <<"\"" <<fileID<< "\" [label=\""<<string(file->get_sourceFileNameWithoutPath())<<"\"];"<<endl;
	}
	
	of<<"}\n"<<endl;
	of.close();
}
