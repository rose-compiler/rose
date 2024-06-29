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
//      cout <<"eIA"<<endl;
        if (_toSave.count(node))
        {
                
//              cout <<"eIA: this node is in the slice ";
//              printSgNode(node);
//              cout <<endl;
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
        if (isSgFile(node)) {
                currentFile = nullptr;
        }
                
    BooleanSafeKeeper partOfSlice = inherited;
                // calcualte the synth-val of this node
                // if any of the children of this node is in the slice, this node must also be in the slice
                std::vector< std::pair<
                SgNode *, std::string > > mapping=node->returnDataMemberPointers ();
                for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++)
                {
                        partOfSlice.boolean|=(*i).boolean;
                }

                // if this is a bb or gb check for delayed decls
                if (isSgBasicBlock(node) || isSgGlobal(node))
                {
                        bool deleteDelayedNode;
                        // this is a basic block
                        // check if any of the nodes form  delayed remove list must be keept
                        for (std::list<SgNode *>::iterator cand=delayedRemoveListStack.top().begin();cand!=delayedRemoveListStack.top().end();cand++)
                        {
                                deleteDelayedNode=true;
                                // the list is not empty
                                if (isSgVariableDeclaration(*cand))
                                {
                                        SgVariableDeclaration * decl=isSgVariableDeclaration(*cand);
                                        Rose_STL_Container<SgNode*> varUseList=NodeQuery::querySubTree(node,V_SgVarRefExp);
                                        for(Rose_STL_Container<SgNode*>::iterator varUse=varUseList.begin();varUseList.end()!=varUse;varUse++)
                                        {
                                                if (isSgVarRefExp(*varUse)->get_symbol ()->get_declaration ()==decl->get_definition()->get_vardefn())
                                                {
                                                        // there is a use, break from the inner for loop
                                                        deleteDelayedNode=false;
                                                        break;
                                                }
                                        }
                                        if (deleteDelayedNode)
                                        {
                                                // check if the node is from the sourcefile or goes to the source file, if so kill it
                                                // make sure to delete only stuff, the belongs to an unparsed file
                                                if (currentFile &&(*cand)->get_file_info ()->isSameFile(currentFile)) {
#ifndef _MSC_VER                                                        
                                                LowLevelRewrite::remove(isSgStatement(*cand));
#endif
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
                                if (isSgVariableDeclaration(node))
                                {
                                        // remove initialization
                                        isSgVariableDeclaration(node)->get_definition()->get_vardefn()->set_initializer(nullptr);
                                        //check if the variable is used lateron
                                        // but remove the initialisation
                                        delayedRemoveListStack.top().push_back(node);
                                }                       
                                else if (isSgTypedefDeclaration(node) || isSgClassDeclaration(node))
                                {
                                        // POSTPONE DElete
                                        delayedRemoveListStack.top().push_back(node);
                                }
                                else
                                {
                                        // make sure to delete only stuff, the belongs to an unparsed file
                                        if (currentFile && node->get_file_info ()->isSameFile(currentFile)) {
#ifndef _MSC_VER
                                                        LowLevelRewrite::remove(isSgStatement(node));
#else
 #pragma message ("WARNING: No implementation for Windows yet.")
 ROSE_ABORT();

#endif
                                        }
                                }
                        }
                }

                return partOfSlice;

                if (_toSave.count(node)) {
                    partOfSlice = true;
                }
    else {
        for (SubTreeSynthesizedAttributes::iterator i = atts.begin(); i != atts.end(); i++) {
            if ((*i).boolean == true) {
                partOfSlice = true;
                break;
            }
        }
    }

    return partOfSlice;
}
