// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,4MB
#include "sage3basic.h"

#include "CreateSlice.h"
#include "iostream"
using namespace std;

void printSgNode(SgNode *node)
{
	cout <<"\"";
	if (isSgInitializedName(node))
		cout <<isSgInitializedName(node)->get_qualified_name().getString();
	else
	if (!isSgPragma(node))
          cout<<node->unparseToString();
        cout <<"\" of class "<<node->class_name();	
}

BooleanSafeKeeper CreateSlice::evaluateInheritedAttribute(SgNode * node, BooleanSafeKeeper partOfSlice)
{

	if (isSgFile(node))
	{
		currentFile=isSgFile(node);
	}
	if (isSgBasicBlock(node) || isSgGlobal(node))
	{
		delayedRemoveListStack.push(std::list<SgNode*>());
	}
//	cout <<"eIA"<<endl;
	if (_toSave.count(node))
	{
		
//		cout <<"eIA: this node is in the slice ";
//		printSgNode(node);
//		cout <<endl;
		return BooleanSafeKeeper(true);
	}
	else
		return BooleanSafeKeeper(false);
	
    // if set::count(node)>0
    if (_toSave.count(node))
    {
/*        if (isSgStatement(node))
        {
            SgPragmaDeclaration *pr =
                new SgPragmaDeclaration(Sg_File_Info::
                                        generateDefaultFileInfoForTransformationNode
                                        (), new SgPragma("slice"));

            ROSE_ASSERT(pr != NULL);
            LowLevelRewrite::insert(isSgStatement(node), isSgStatement(pr));
            _toSave.insert(pr);
        }*/
        // node is in set -> preserve this node
        partOfSlice = true;
    }

    if (partOfSlice.boolean)
        _toSave.insert(node);
				
    return partOfSlice;
}

BooleanSafeKeeper CreateSlice::evaluateSynthesizedAttribute(SgNode * node, BooleanSafeKeeper inherited,
                                               SubTreeSynthesizedAttributes atts)
{
	if (isSgFile(node))
	{
		currentFile=NULL;
	}
        /*
                if (isSgStatement(node))                
                        cout << "processing statement ";
                else
                        cout <<"processing node ";
                        
          printSgNode(node);
                cout <<endl;
        */      
                
    BooleanSafeKeeper partOfSlice = inherited;
        //      partOfSlice=BooleanSafeKeeper(false);
                // calcualte the synth-val of this node
                // if any of the children of this node is in the slice, this node must also be in the slice
                int count=0;
                std::vector< std::pair<
                SgNode *, std::string > > mapping=node->returnDataMemberPointers ();
//              cout <<"\t synth-attrib count "<<atts.size()<<"\t mapping count "<<mapping.size()<<endl;
                for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++)
                {
        /*              if ((*i).boolean)
                        {
                //              cout <<"\t"<<count<<" is true"<<endl;
//                              SgNode * first=((mapping[count]).first);
        //                      if (first!=NULL)
        //                      cout <<"\t"<<((mapping[count]).first)->class_name()<<endl;
                        }
                        */
                        count ++;
                        partOfSlice.boolean|=(*i).boolean;
                }

                // if this is a bb or gb check for delayed decls
                if (isSgBasicBlock(node) || isSgGlobal(node))
                {
        //              cout <<"current scope: >"<<node->unparseToString()<<"< of class "<<node->class_name()<<endl;
                        bool deleteDelayedNode;
                        // this is a basic block
                        // check if any of the nodes form  delayed remove list must be keept
                        for (std::list<SgNode *>::iterator cand=delayedRemoveListStack.top().begin();cand!=delayedRemoveListStack.top().end();cand++)
                        {
                                deleteDelayedNode=true;
                                // the list is not empty
                                if (isSgVariableDeclaration(*cand))
                                {
        //                              cout <<"delayed varDecl >"<<(*cand)->unparseToString()<<"<"<<endl;
                                        SgVariableDeclaration * decl=isSgVariableDeclaration(*cand);
                                        Rose_STL_Container<SgNode*> varUseList=NodeQuery::querySubTree(node,V_SgVarRefExp);
                                        for(Rose_STL_Container<SgNode*>::iterator varUse=varUseList.begin();varUseList.end()!=varUse;varUse++)
                                        {
                //                              cout <<"\t\tchecking: "<<(*varUse)->unparseToString()<<endl;
                                                if (isSgVarRefExp(*varUse)->get_symbol ()->get_declaration ()==decl->get_definition()->get_vardefn())
                                                {
                                                        // there is a use, break from the innter for loop
                                                        //                                                              cout <<"found use in "<<(*varUse)->get_parent()->unparseToString()<<endl;
                                                        deleteDelayedNode=false;
                                                        break;
                                                }
                                        }
                                        if (deleteDelayedNode)
                                        {
                        //                      cout <<"\t*found no use, deleting"<<endl;
                                                // check if the node is from the sourcefile or goes to the source file, if so kill it
                                                // make shure to delete only stuff, the belongs to an unparsed file
//                                              if ((*cand)->get_file_info ()->isOutputInCodeGeneration ())
                                                if (currentFile &&(*cand)->get_file_info ()->isSameFile(currentFile)) {
#ifndef _MSC_VER                                                        
                                                LowLevelRewrite::remove(isSgStatement(*cand));
#endif
        //                                      delete (*cand);
                                                }
                                        }
                                }
                                else if (isSgTypedefDeclaration(*cand) ||isSgClassDeclaration(*cand))
                                {
                //                      cout <<"delayed struct/type: "<<(*cand)->unparseToString()<<endl;
                                        //      cerr <<"CreateSlice.C:evaluateSynthesizedAttribute does not maintain delayed deleting for structs and typedefs, every struct and type will remain within the slice"<<endl;
                                }
                        }
                        delayedRemoveListStack.top().clear();
                        delayedRemoveListStack.pop();
                }


                // this is a statement, if any of its child-nodes is in the slice, keep it, else discard id
                if (isSgStatement(node))
                {


                        
                        // if any of its sythesized attributes is true, keep is, else dicard
                        if (!partOfSlice.boolean)
                        {
                                if (isSgVariableDefinition(node))
                                {
//                                      cout <<" is varDef"<<endl;
                                }
                                else if (isSgVariableDeclaration(node))
                                {
//                                      cout <<" is varDec"<<endl;
                                        // remove initialization
                                        // SgNode * toDelNode=isSgVariableDeclaration(node)->get_definition()->get_vardefn()->get_initializer();
                                        //cleanly delete this opject!!
//                                      delete(toDelNode);
                                        isSgVariableDeclaration(node)->get_definition()->get_vardefn()->set_initializer(NULL);
                                        //check if the variable is used lateron
                                        // but remove the initialisation
                                        delayedRemoveListStack.top().push_back(node);
                                }                       
                                else if (isSgTypedefDeclaration(node) || isSgClassDeclaration(node))
                                {
                                        // POSTPONE DElete
//                                      cout <<" is typedef or struct"<<endl;
                                        delayedRemoveListStack.top().push_back(node);
                                }
                                else
                                {
                                                // make shure to delete only stuff, the belongs to an unparsed file
//                                              if (node->get_file_info ()->isOutputInCodeGeneration ())
                                        if (currentFile && node->get_file_info ()->isSameFile(currentFile)) {
#ifndef _MSC_VER
							LowLevelRewrite::remove(isSgStatement(node));
#else
 #pragma message ("WARNING: No implementation for Windows yet.")
 ROSE_ASSERT(false);

#endif
                                }
                                //      delete(node);
                                }
                        }
                }


                //              cout <<"\tinherited:"<<inherited.boolean<<endl;
                //              cout <<"\tsynthesized:"<<partOfSlice.boolean<<endl;
                //              return BooleanSafeKeeper(false);
                return partOfSlice;

                // if set::count(node)>0
                if (_toSave.count(node))
                {/*
                                if (isSgStatement(node))
                                {
                                SgPragmaDeclaration *pr =
                                new SgPragmaDeclaration(Sg_File_Info::
                                generateDefaultFileInfoForTransformationNode
                                (), new SgPragma("slice"));

            ROSE_ASSERT(pr != NULL);
            LowLevelRewrite::insert(isSgStatement(node), isSgStatement(pr));
            _toSave.insert(pr);

        }*/
        partOfSlice = true;
    }
    else
    {
        for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++)
        {
            if ((*i).boolean == true)
            {
                partOfSlice = true;
                break;
            }
        }
    }

    if (!partOfSlice.boolean && isSgStatement(node))
    {
    //    LowLevelRewrite::remove(isSgStatement(node));
    }
    /* 
       if (isSgLocatedNode(node) && !partOfSlice) { Sg_File_Info * file_info = 
       node->get_file_info();

       // file_info->unsetCompilerGeneratedNodeToBeUnparsed(); //
       file_info->setCompilerGenerated(); } */

    return partOfSlice;
}
