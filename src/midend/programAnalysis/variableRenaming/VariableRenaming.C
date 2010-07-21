/**
 * Category: SSA Analysis
 * VariableRenaming Declaration
 * Created by frye5 in Jun2010
 */

#include "sage3basic.h"
#include "VariableRenaming.h"
#include "sageInterface.h"
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <fstream>
#include <stack>
#include <boost/foreach.hpp>
#define foreach BOOST_FOREACH

using namespace std;

//Initializations of the static attribute tags
std::string VariableRenaming::varKeyTag = "rename_KeyTag";
SgInitializedName* VariableRenaming::thisDecl = NULL;
VariableRenaming::VarName VariableRenaming::emptyName;
VariableRenaming::NumNodeRenameTable VariableRenaming::emptyRenameTable;
VariableRenaming::NumNodeRenameEntry VariableRenaming::emptyRenameEntry;

//Printing functions
std::string VariableRenaming::keyToString(VarName vec)
{
    std::string name = "";
    foreach(VarName::value_type& iter, vec)
    {
        if(iter != vec.front())
        {
            name += ":";
        }
        name += iter->get_name().getString();
    }

    return name;
}

void VariableRenaming::printDefs(SgNode* node)
{
    if(!DEBUG_MODE)
        return;

    std::cout << "Def Table for [" << node->class_name() << ":" << node <<"]:" << std::endl;

    foreach(TableEntry::value_type& entry, defTable[node])
    {
        std::cout << "  Defs for [" << keyToString(entry.first) << "]:" << std::endl;
        foreach(NodeVec::value_type& iter, entry.second)
        {
            std::cout << "    -[" << iter->class_name() << ":" << iter << "]" << std::endl;
        }
    }
}

void VariableRenaming::printDefs(std::map< std::vector<SgInitializedName*>, std::vector<SgNode*> >& table)
{
    if(!DEBUG_MODE)
        return;

    std::cout << "Def Table:" << std::endl;

    foreach(TableEntry::value_type& entry,table)
    {
        std::cout << "  Defs for [" << keyToString(entry.first) << "]:" << std::endl;
        foreach(NodeVec::value_type& iter, entry.second)
        {
            std::cout << "    -[" << iter->class_name() << ":" << iter << "]" << std::endl;
        }
    }
}

void VariableRenaming::printOriginalDefs(SgNode* node)
{
    if(!DEBUG_MODE)
        return;

    std::cout << "Original Def Table for [" << node->class_name() << ":" << node <<"]:" << std::endl;

    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        std::cout << "  Defs for [" << keyToString(entry.first) << "]:" << std::endl;
        foreach(NodeVec::value_type& iter, entry.second)
        {
            std::cout << "    -[" << iter->class_name() << ":" << iter << "]" << std::endl;
        }
    }
}

void VariableRenaming::printOriginalDefTable()
{
    if(!DEBUG_MODE)
        return;
    std::cout << "Original Def Table:" << endl;

    foreach(DefUseTable::value_type& node, originalDefTable)
    {
        std::cout << "  Original Def Table for [" << node.first->class_name() << ":" << node.first <<"]:" << std::endl;

        foreach(TableEntry::value_type& entry, originalDefTable[node.first])
        {
            std::cout << "    Defs for [" << keyToString(entry.first) << "]:" << std::endl;
            foreach(NodeVec::value_type& iter, entry.second)
            {
                std::cout << "      -[" << iter->class_name() << ":" << iter << "]" << std::endl;
            }
        }
    }
}

void VariableRenaming::printUses(SgNode* node)
{
    if(!DEBUG_MODE)
        return;

    std::cout << "Use Table for [" << node->class_name() << ":" << node <<"]:" << std::endl;

    foreach(TableEntry::value_type& entry,useTable[node])
    {
        std::cout << "  Uses for [" << keyToString(entry.first) << "]:" << std::endl;
        foreach(NodeVec::value_type& iter, entry.second)
        {
            std::cout << "    -[" << iter->class_name() << ":" << iter << "]" << std::endl;
        }
    }
}

void VariableRenaming::printUses(std::map< std::vector<SgInitializedName*>, std::vector<SgNode*> >& table)
{
    if(!DEBUG_MODE)
        return;

    std::cout << "Use Table:" << std::endl;

    foreach(TableEntry::value_type& entry,table)
    {
        std::cout << "  Uses for [" << keyToString(entry.first) << "]:" << std::endl;
        foreach(NodeVec::value_type& iter, entry.second)
        {
            std::cout << "    -[" << iter->class_name() << ":" << iter << "]" << std::endl;
        }
    }
}

void VariableRenaming::printRenameTable()
{
    printRenameTable(numRenameTable);
}

void VariableRenaming::printRenameTable(const VarName& var)
{
    if(DEBUG_MODE)
        return;

    cout << "Names for [" << keyToString(var) << "]:" << endl;

    printRenameEntry(numRenameTable[var]);
}

void VariableRenaming::printRenameTable(const NodeNumRenameTable& table)
{
    if(!DEBUG_MODE)
        return;

    cout << "Rename Table:" << endl;

    //Iterate the table
    foreach(const NodeNumRenameTable::value_type& entry, table)
    {
        cout << "  Names for [" << keyToString(entry.first) << "]:" << endl;

        //Print out the renamings in order
        printRenameEntry(entry.second);
    }
}

void VariableRenaming::printRenameTable(const NumNodeRenameTable& table)
{
    if(!DEBUG_MODE)
        return;

    cout << "Rename Table:" << endl;

    //Iterate the table
    foreach(const NumNodeRenameTable::value_type& entry, table)
    {
        std::cout << "  Names for [" << keyToString(entry.first) << "]:" << std::endl;

        //Print out the renamings in order.
        printRenameEntry(entry.second);
    }
}

void VariableRenaming::printRenameEntry(const NodeNumRenameEntry& entry)
{
    if(!DEBUG_MODE)
        return;

    int start = 0;
    int end = 0;
    
    //Iterate the entry
    foreach(const NodeNumRenameEntry::value_type& iter, entry)
    {
        if(start == 0 && end == 0)
        {
            start = iter.second;
            end = iter.second;
            continue;
        }

        if(start > iter.second)
        {
            start = iter.second;
        }

        if(end < iter.second)
        {
            end = iter.second;
        }
    }

    while(start <= end)
    {
        SgNode* current = NULL;
        //Find the entry for start if it exists
        foreach(const NodeNumRenameEntry::value_type& iter, entry)
        {
            if(iter.second == start)
            {
                current = iter.first;
                break;
            }
        }

        if(current != NULL)
        {
            cout << "      " << start << ": " << current << endl;
        }

        start++;
    }
}

void VariableRenaming::printRenameEntry(const NumNodeRenameEntry& entry)
{
    if(!DEBUG_MODE)
        return;

    //Iterate the entry
    foreach(const NumNodeRenameEntry::value_type& iter, entry)
    {
        cout << "      " << iter.first << ": " << iter.second << endl;
    }
}

VarUniqueName* VariableRenaming::getUniqueName(SgNode* node)
{
    if(!node->attributeExists(VariableRenaming::varKeyTag))
    {
        return NULL;
    }
    VarUniqueName* uName = dynamic_cast<VarUniqueName*>(node->getAttribute(VariableRenaming::varKeyTag));
    return uName;
}

VariableRenaming::VarName VariableRenaming::getVarName(SgNode* node)
{
    if(getUniqueName(node) == NULL)
    {
        return VariableRenaming::VarName();
    }
    return getUniqueName(node)->getKey();
}


bool VariableRenaming::isFromLibrary(SgInitializedName* initName)
{
  Sg_File_Info* fi = initName->get_file_info();
  if (fi->isCompilerGenerated())
    return true;
  string filename = fi->get_filenameString();
  if ((filename.find("/include/") != std::string::npos))
      return true;

  return false;
}

bool VariableRenaming::isPrefixOfName(VarName name, VarName prefix)
{
    VarName::iterator iter;
    // Search for the first occurance of prefix in name
    iter = std::search(name.begin(), name.end(), prefix.begin(), prefix.end());

    //If the prefix is at the beginning of the name
    if(iter == name.begin())
    {
        return true;
    }
    //If the prefix is not in the name
    else if(iter == name.end())
    {
        return false;
    }
    //Found the prefix, but inside the name instead of at beginning
    else
    {
        //We don't want to assert, just return false.
        return false;
    }
}

//Function to perform the VariableRenaming and annotate the AST
void VariableRenaming::run()
{
    //if(DEBUG_MODE)
        //cout << "Performing Variable location traversal." << endl;

    //VariableRenaming::VarLocatorTraversal varTraverse(this);
    //varTraverse.traverse(this->project,VariableRenaming::VarLocatorInheritedAttr(VariableRenaming::VarLocatorInheritedAttr::Global,this->project));

    if(DEBUG_MODE)
        cout << "Locating global variables." << endl;

    findGlobalVars();

    if(DEBUG_MODE)
        cout << "Performing UniqueNameTraversal..." << endl;

    UniqueNameTraversal uniqueTrav(this);
    std::vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
    std::vector<SgFunctionDefinition*>::iterator iter = funcs.begin();
    for(;iter != funcs.end(); ++iter)
    {
        SgFunctionDeclaration* func = (*iter)->get_declaration();
        ROSE_ASSERT(func);
        uniqueTrav.traverse(func);
    }

    if(DEBUG_MODE)
        cout << "Finished UniqueNameTrav..." << endl;

    VariableRenaming::VarDefUseTraversal defUseTrav(this);
    for(iter = funcs.begin();iter != funcs.end(); ++iter)
    {
        SgFunctionDeclaration* func = (*iter)->get_declaration();
        ROSE_ASSERT(func);
        if(DEBUG_MODE)
            cout << "Running defUseTrav on function: " << func->get_name().getString() << endl;
        defUseTrav.traverse(func);
    }

    if(DEBUG_MODE)
        cout << "Finished DefUseTraversal." << endl;

    if(DEBUG_MODE)
        cout << "Inserting global variable definitions." << endl;

    //Insert the global variables as being defined at every function entry and
    //at every function call
    insertGlobalVarDefinitions();

    printOriginalDefTable();
    
    if(DEBUG_MODE)
        cout << "Performing DefUse." << endl;

    for(iter = funcs.begin();iter != funcs.end(); ++iter)
    {
        SgFunctionDefinition* func = (*iter);
        ROSE_ASSERT(func);
        if(DEBUG_MODE)
            cout << "Running DefUse on function: " << func->get_declaration()->get_name().getString() << endl;
        runDefUse(func);
    }

    return;
}

void VariableRenaming::findGlobalVars()
{
    InitNameVec vars = SageInterface::querySubTree<SgInitializedName>(project, V_SgInitializedName);
    foreach(InitNameVec::value_type& iter, vars)
    {
        //Ignore library/compiler generated variables.
        if(isFromLibrary(iter))
            continue;

        //Check if we are in global scope.
        SgNode* scope = iter->get_scope();
        if(isSgGlobal(scope))
        {
            //Since forward declaration parameters are inserted in global scope,
            //Check if we are in a forward declaration
            if(SageInterface::getEnclosingFunctionDeclaration(iter))
            {
                //We are in a declaration, so not a global var.
                continue;
            }
            //Add the variable to the global scope and name it.
            VarUniqueName *uName = new VarUniqueName(iter);
            iter->setAttribute(VariableRenaming::varKeyTag, uName);
            //Add to the global var list
            globalVarList.push_back(uName->getKey());
            if(DEBUG_MODE)
                cout << "Added global variable [" << iter->get_name().getString() << "] - " << iter << endl;
        }
    }
}

void VariableRenaming::insertGlobalVarDefinitions()
{
    if(DEBUG_MODE)
        cout << "Global Var List size: " << globalVarList.size() << endl;

    //Iterate the function definitions and insert definitions for all global variables
    std::vector<SgFunctionDefinition*> funcs = SageInterface::querySubTree<SgFunctionDefinition>(project, V_SgFunctionDefinition);
    foreach(std::vector<SgFunctionDefinition*>::value_type& iter, funcs)
    {
        SgFunctionDefinition* func = iter;
        ROSE_ASSERT(func);

        //Iterate the global table insert a def for each name at the function definition
        foreach(GlobalTable::value_type& entry, globalVarList)
        {
            //Add this function definition as a definition point of this variable
            originalDefTable[func][entry].push_back(func);
        }
    }


    //Iterate the function calls and insert definitions for all global variables
    std::vector<SgFunctionCallExp*> calls = SageInterface::querySubTree<SgFunctionCallExp>(project, V_SgFunctionCallExp);
    foreach(std::vector<SgFunctionCallExp*>::value_type& iter, calls)
    {
        SgFunctionCallExp* call = iter;
        ROSE_ASSERT(call);

        //Iterate the global table insert a def for each name at the function call
        foreach(GlobalTable::value_type& entry, globalVarList)
        {
            //Add this function call as a definition point of this variable
            originalDefTable[call][entry].push_back(call);
        }
    }
}

void VariableRenaming::toDOT(const std::string fileName)
{
    ROSE_ASSERT(fileName != "");

    typedef std::vector<SgSourceFile*> fileVec;
    fileVec files = SageInterface::querySubTree<SgSourceFile>(project, V_SgSourceFile);

    //print all the functions in each file
    foreach(fileVec::value_type& file, files)
    {
        ofstream outFile((StringUtility::stripPathFromFileName(file->getFileName())
                            + "_" + fileName).c_str());

        if(!outFile.good())
        {
            if(DEBUG_MODE)
                cout << "Error printing DOT." << endl;

            return;
        }

        //Print the CFG of the function
        printToDOT(file, outFile);
    }
}

void VariableRenaming::toFilteredDOT(const std::string fileName)
{
    ROSE_ASSERT(fileName != "");

    typedef std::vector<SgSourceFile*> fileVec;
    fileVec files = SageInterface::querySubTree<SgSourceFile>(project, V_SgSourceFile);

    //print all the functions in each file
    foreach(fileVec::value_type& file, files)
    {
        ofstream outFile((StringUtility::stripPathFromFileName(file->getFileName())
                            + "_filtered_" + fileName).c_str());

        if(!outFile.good())
        {
            if(DEBUG_MODE)
                cout << "Error printing DOT." << endl;

            return;
        }

        //Print the CFG of the function
        printToFilteredDOT(file, outFile);
    }
}

void VariableRenaming::printToDOT(SgSourceFile* source, std::ofstream &outFile)
{
    if(!outFile.good())
    {
        if(DEBUG_MODE)
            cout << "Error: Bad ofstream in printToDOT()" << endl;
        return;
    }

    typedef CFGNode cfgNode;
    typedef CFGEdge cfgEdge;

    typedef std::vector<SgFunctionDefinition*> funcDefVec;
    funcDefVec funcs = SageInterface::querySubTree<SgFunctionDefinition>(source, V_SgFunctionDefinition);

    //Start the graph
    outFile << "digraph SSAGraph {\n";

    //Iterate all the functions and print them in the same file.
    foreach(funcDefVec::value_type& func, funcs)
    {
        vector<cfgNode> visited;
        stack<cfgNode> traverse;
        cfgNode current = cfgNode(func->cfgForBeginning());

        traverse.push(current);

        while(!traverse.empty())
        {
            current = traverse.top();

            //If not visited
            if(std::count(visited.begin(),visited.end(),current) == 0)
            {
                //Visit the node and print it
                std::string id = current.id();
                std::string nodeColor = "black";

                bool uniqueName = current.getNode()->attributeExists(VariableRenaming::varKeyTag);

                if (isSgStatement(current.getNode()))
                    nodeColor = "blue";
                else if (isSgExpression(current.getNode()))
                    nodeColor = "green";
                else if (isSgInitializedName(current.getNode()))
                    nodeColor = "red";

                std::string name = "";
                if(uniqueName)
                {
                    if(DEBUG_MODE)
                        cout << "Getting Unique Name attribute." << endl;
                    VarUniqueName *attr = getUniqueName(current.getNode());
                    ROSE_ASSERT(attr);

                    name = attr->getNameString();
                }

                //Print the defs to a string
                std::stringstream defUse;
                foreach(TableEntry::value_type& entry, defTable[current.getNode()])
                {
                    defUse << "Def [" << keyToString(entry.first) << "]: ";
                    foreach(NodeVec::value_type& val, entry.second)
                    {
                        defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
                    }
                    defUse << "\\n";
                }
                //Print the uses to a string
                foreach(TableEntry::value_type& entry, useTable[current.getNode()])
                {
                    defUse << "Use [" << keyToString(entry.first) << "]: ";
                    foreach(NodeVec::value_type& val, entry.second)
                    {
                        defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
                    }
                    defUse << "\\n";
                }

                //Copy out the string and trim off the last '\n'
                std::string defUseStr = defUse.str().substr(0,defUse.str().size()-2);


                //Print this node
                outFile << id << " [label=\"<"  << escapeString(current.getNode()->class_name()) << ">:" << current.getNode()
                        //Now we add the unique name information
                        << ((name != "")? "\\n" : "") << name
                        << ((defUseStr != "")? "\\n" : "") << defUseStr
                        << "\", color=\"" << nodeColor << "\", style=\""
                        << (current.isInteresting()? "solid" : "dotted") << "\"];\n";

                //Now print the out edges
                std::vector<cfgEdge> outEdges = current.outEdges();

                foreach(std::vector<cfgEdge>::value_type& edge, outEdges)
                {
                    outFile << edge.source().id() << " -> " << edge.target().id()
                            << " [label=\"" << escapeString(edge.toString())
                            << "\"];\n";
                }
            }

            visited.push_back(current);

            std::vector<cfgEdge> outEdges = current.outEdges();
            foreach(std::vector<cfgEdge>::value_type& edge, outEdges)
            {
                //If we haven't seen the target of this node yet, process the node
                if(std::count(visited.begin(),visited.end(),edge.target()) == 0)
                {
                    traverse.push(edge.target());
                    break;
                }
            }

            //If there are no new out edges to explore
            if(traverse.top() == current)
            {
                std::vector<cfgEdge> inEdges = current.inEdges();
                foreach(std::vector<cfgEdge>::value_type& edge, inEdges)
                {
                    //If we haven't seen the target of this node yet, process the node
                    if(std::count(visited.begin(),visited.end(),edge.target()) == 0)
                    {
                        traverse.push(edge.source());
                        break;
                    }
                }
            }

            //No out or in edges left to print, pop this node
            if(traverse.top() == current)
            {
                traverse.pop();
            }
        }
    }

    //End the graph
    outFile << "}\n";
}

void VariableRenaming::printToFilteredDOT(SgSourceFile* source, std::ofstream& outFile)
{
    if(!outFile.good())
    {
        if(DEBUG_MODE)
            cout << "Error: Bad ofstream in printToDOT()" << endl;
        return;
    }

    typedef FilteredCFGNode<IsDefUseFilter> cfgNode;
    typedef FilteredCFGEdge<IsDefUseFilter> cfgEdge;

    typedef std::vector<SgFunctionDefinition*> funcDefVec;
    funcDefVec funcs = SageInterface::querySubTree<SgFunctionDefinition>(source, V_SgFunctionDefinition);

    //Start the graph
    outFile << "digraph SSAGraph {\n";

    //Iterate all the functions and print them in the same file.
    foreach(funcDefVec::value_type& func, funcs)
    {
        vector<cfgNode> visited;
        stack<cfgNode> traverse;
        cfgNode current = cfgNode(func->cfgForBeginning());

        traverse.push(current);

        while(!traverse.empty())
        {
            current = traverse.top();

            //If not visited
            if(std::count(visited.begin(),visited.end(),current) == 0)
            {
                //Visit the node and print it
                std::string id = current.id();
                std::string nodeColor = "black";

                bool uniqueName = current.getNode()->attributeExists(VariableRenaming::varKeyTag);

                if (isSgStatement(current.getNode()))
                    nodeColor = "blue";
                else if (isSgExpression(current.getNode()))
                    nodeColor = "green";
                else if (isSgInitializedName(current.getNode()))
                    nodeColor = "red";

                std::string name = "";
                if(uniqueName)
                {
                    if(DEBUG_MODE)
                        cout << "Getting Unique Name attribute." << endl;
                    VarUniqueName *attr = getUniqueName(current.getNode());
                    ROSE_ASSERT(attr);

                    name = attr->getNameString();
                }

                //Print the defs to a string
                std::stringstream defUse;
                foreach(TableEntry::value_type& entry, defTable[current.getNode()])
                {
                    defUse << "Def [" << keyToString(entry.first) << "]: ";
                    foreach(NodeVec::value_type& val, entry.second)
                    {
                        defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
                    }
                    defUse << "\\n";
                }
                //Print the uses to a string
                foreach(TableEntry::value_type& entry, useTable[current.getNode()])
                {
                    defUse << "Use [" << keyToString(entry.first) << "]: ";
                    foreach(NodeVec::value_type& val, entry.second)
                    {
                        defUse << getRenameNumberForNode(entry.first, val) << ": " << val << ", ";
                    }
                    defUse << "\\n";
                }

                //Copy out the string and trim off the last '\n'
                std::string defUseStr = defUse.str().substr(0,defUse.str().size()-2);


                //Print this node
                outFile << id << " [label=\"<"  << escapeString(current.getNode()->class_name()) << ">:" << current.getNode()
                        //Now we add the unique name information
                        << ((name != "")? "\\n" : "") << name
                        << ((defUseStr != "")? "\\n" : "") << defUseStr
                        << "\", color=\"" << nodeColor << "\", style=\""
                        << (current.isInteresting()? "solid" : "dotted") << "\"];\n";

                //Now print the out edges
                std::vector<cfgEdge> outEdges = current.outEdges();

                foreach(std::vector<cfgEdge>::value_type& edge, outEdges)
                {
                    outFile << edge.source().id() << " -> " << edge.target().id()
                            << " [label=\"" << escapeString(edge.toString())
                            << "\"];\n";
                }
            }

            visited.push_back(current);

            std::vector<cfgEdge> outEdges = current.outEdges();
            foreach(std::vector<cfgEdge>::value_type& edge, outEdges)
            {
                //If we haven't seen the target of this node yet, process the node
                if(std::count(visited.begin(),visited.end(),edge.target()) == 0)
                {
                    traverse.push(edge.target());
                    break;
                }
            }

            //If there are no new out edges to explore
            if(traverse.top() == current)
            {
                std::vector<cfgEdge> inEdges = current.inEdges();
                foreach(std::vector<cfgEdge>::value_type& edge, inEdges)
                {
                    //If we haven't seen the target of this node yet, process the node
                    if(std::count(visited.begin(),visited.end(),edge.target()) == 0)
                    {
                        traverse.push(edge.source());
                        break;
                    }
                }
            }

            //No out or in edges left to print, pop this node
            if(traverse.top() == current)
            {
                traverse.pop();
            }
        }
    }
   
    //End the graph
    outFile << "}\n";
}

VariableRenaming::VarRefSynthAttr VariableRenaming::UniqueNameTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
    //First we check if this is an initName
    if(isSgInitializedName(node))
    {
        SgInitializedName* name = isSgInitializedName(node);

        //We want to assign this node its unique name, as well as adding it to the defs.
        VarUniqueName* uName = new VarUniqueName(name);
        name->setAttribute(VariableRenaming::varKeyTag, uName);

        return VariableRenaming::VarRefSynthAttr(name);
    }
    //Next, see if it is a varRef
    else if(isSgVarRefExp(node))
    {
        SgVarRefExp* var = isSgVarRefExp(node);

        if(attrs.size() != 0)
        {
            cout << "Error: Found VarRef with children." << endl;
            ROSE_ASSERT(false);
        }

        //We want to assign this node its unique name, as well as adding it to the defs.
        VarUniqueName* uName = new VarUniqueName(var->get_symbol()->get_declaration());
        var->setAttribute(VariableRenaming::varKeyTag, uName);

        return VariableRenaming::VarRefSynthAttr(var);
    }
    //We check if it is a 'this' expression, since we want to be able to version 'this' as well.
    //We don't have an SgInitializedName for 'this', so we use a flag in the unique names
    else if(isSgThisExp(node))
    {
        SgThisExp* thisExp = isSgThisExp(node);

        if(attrs.size() != 0)
        {
            cout << "Error: Found ThisExp with children." << endl;
            ROSE_ASSERT(false);
        }

        return VariableRenaming::VarRefSynthAttr(thisExp);
    }
    //Now we check if we have reached a Dot Expression, where we have to merge names.
    else if(isSgDotExp(node))
    {
        if(attrs.size() != 2)
        {
            cout << "Error: Found dot operator without exactly 2 children." << endl;
            ROSE_ASSERT(false);
        }

        //We want to update the naming for the RHS child
        //Check if the LHS has at least one varRef
        if(attrs[0].getRefs().size() > 0)
        {
            //Get the unique name from the highest varRef in the LHS, since this will have the most
            //fully qualified UniqueName.
            VarUniqueName* lhsName = dynamic_cast<VarUniqueName*>(attrs[0].getRefs()[attrs[0].getRefs().size() - 1]->getAttribute(VariableRenaming::varKeyTag));
            ROSE_ASSERT(lhsName);

            //Check if the RHS has a single varRef
            if(attrs[1].getRefs().size() == 1)
            {
                SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

                if(varRef)
                {
                    //Create the uniqueName from the uniqueName of the lhs prepended to the rhs uniqueName
                    VarUniqueName* uName = new VarUniqueName(lhsName->getKey(), varRef->get_symbol()->get_declaration());
                    uName->setUsesThis(lhsName->getUsesThis());
                    varRef->setAttribute(VariableRenaming::varKeyTag,uName);

                    VarUniqueName* uName2 = new VarUniqueName(*uName);
                    node->setAttribute(VariableRenaming::varKeyTag,uName2);

                    //Return the combination of the LHS and RHS varRefs
                    return VariableRenaming::VarRefSynthAttr(attrs[0].getRefs(), varRef);
                }
                else
                {
                    //RHS could be a function call.
                    return VariableRenaming::VarRefSynthAttr(varRef);
                }
            }
            else
            {
                //RHS could be a function call.
                return attrs[0];
            }   
        }
        else
        {
            cout << "Error: LHS of dot has no varRefs." << endl;
            ROSE_ASSERT(false);
        }
    }
    //Now we check if we have reached an ArrowExpression, we have to merge names.
    else if(isSgArrowExp(node))
    {
        if(attrs.size() != 2)
        {
            cout << "Error: Found arrow operator without exactly 2 children." << endl;
            ROSE_ASSERT(false);
        }

        //We want to update the naming for the RHS child
        //Check if the LHS has at least one varRef
        if(attrs[0].getRefs().size() > 0)
        {
            bool thisExp = false;
            //Check if our LHS varRef is the 'this' expression
            if(isSgThisExp(attrs[0].getRefs()[attrs[0].getRefs().size() - 1]))
            {
                thisExp = true;
            }
            
            //Get the unique name from the highest varRef in the LHS, since this will have the most
            //fully qualified UniqueName.
            VarUniqueName* lhsName;
            if(!thisExp)
            {
                lhsName = dynamic_cast<VarUniqueName*>(attrs[0].getRefs()[attrs[0].getRefs().size() - 1]->getAttribute(VariableRenaming::varKeyTag));
                ROSE_ASSERT(lhsName);
            }
            
            //Check if the RHS has a single varRef
            if(attrs[1].getRefs().size() == 1)
            {
                SgVarRefExp* varRef = isSgVarRefExp(attrs[1].getRefs()[0]);

                if(varRef)
                {
                    if(!thisExp)
                    {
                        //Create the uniqueName from the uniqueName of the lhs prepended to the rhs uniqueName
                        VarUniqueName* uName = new VarUniqueName(lhsName->getKey(), varRef->get_symbol()->get_declaration());
                        uName->setUsesThis(lhsName->getUsesThis());
                        varRef->setAttribute(VariableRenaming::varKeyTag,uName);

                        VarUniqueName* uName2 = new VarUniqueName(*uName);
                        node->setAttribute(VariableRenaming::varKeyTag,uName2);
                    }
                    else
                    {
                        //Create the UniqueName from the current varRef, and stores that it uses 'this'
                        VarUniqueName* uName = new VarUniqueName(varRef->get_symbol()->get_declaration());
                        uName->setUsesThis(true);
                        varRef->setAttribute(VariableRenaming::varKeyTag,uName);

                        VarUniqueName* uName2 = new VarUniqueName(*uName);
                        node->setAttribute(VariableRenaming::varKeyTag,uName2);
                    }

                    //Return the combination of the LHS and RHS varRefs
                    return VariableRenaming::VarRefSynthAttr(attrs[0].getRefs(), varRef);
                }
                else
                {
                    //RHS could be a function call
                    return VariableRenaming::VarRefSynthAttr(varRef);
                }
            }
            else
            {
                //RHS could be a function call, and wouldn't have any varRefs.
                return VariableRenaming::VarRefSynthAttr();
            }
        }
        else
        {
            cout << "Error: LHS of arrow has no varRefs." << endl;
            ROSE_ASSERT(false);
        }
    }
    //Now we hit the default case. We should return a merged list.
    else
    {
        std::vector<SgNode*> names;
        for(unsigned int i = 0; i < attrs.size(); i++)
        {
            names.insert(names.end(), attrs[i].getRefs().begin(), attrs[i].getRefs().end());
        }

        return VariableRenaming::VarRefSynthAttr(names);
    }
}

VariableRenaming::VarDefUseSynthAttr VariableRenaming::VarDefUseTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList attrs)
{
    cout << "---------<" << node->class_name() << ">-------" << node << endl;
    //We want to propogate the def/use information up from the varRefs to the higher expressions.
    if(isSgInitializedName(node))
    {
        SgInitializedName* name = isSgInitializedName(node);
        ROSE_ASSERT(name);

        VarUniqueName * uName = varRename->getUniqueName(name);
        ROSE_ASSERT(uName);

        //Add this as a def. [node][uniqueName]
        varRename->getDefTable()[name][uName->getKey()].push_back(name);

        if(varRename->getDebug())
        {
            cout << "Defined " << uName->getNameString() << endl;
        }

        //An SgInitializedName should count as a def, since it is the initial definition.
        return VariableRenaming::VarDefUseSynthAttr(name, NULL);
    }
    //Catch all variable references
    else if(isSgVarRefExp(node))
    {
        SgVarRefExp* varRef = isSgVarRefExp(node);
        ROSE_ASSERT(varRef);

        //Get the unique name of the def.
        VarUniqueName * uName = varRename->getUniqueName(varRef);
        ROSE_ASSERT(uName);

        //Add this as a use. We will correct the reference later.
        varRename->getUseTable()[varRef][uName->getKey()].push_back(varRef);

        if(varRename->getDebug())
        {
            cout << "Found use for " << uName->getNameString() << " at " << varRef->cfgForBeginning().toStringForDebugging() << endl;
        }

        //A VarRef is always a use, it only becomes defined by the parent assignment.
        return VariableRenaming::VarDefUseSynthAttr(NULL, varRef);
    }
    //Catch all types of Binary Operations
    else if(isSgBinaryOp(node))
    {
        SgBinaryOp* op = isSgBinaryOp(node);

        if(attrs.size() == 2)
        {
            //If we have an assigning operation, we want to list everything on the LHS as being defined
            //Otherwise, everything is being used.
            VariantT type = op->variantT();
            switch(type)
            {
                //Catch all the types of Ops that define the LHS
                case V_SgAndAssignOp:
                case V_SgAssignOp:
                case V_SgDivAssignOp:
                case V_SgIorAssignOp:
                case V_SgLshiftAssignOp:
                case V_SgMinusAssignOp:
                case V_SgModAssignOp:
                case V_SgMultAssignOp:
                case V_SgPlusAssignOp:
                case V_SgPointerAssignOp:
                case V_SgRshiftAssignOp:
                case V_SgXorAssignOp:
                {
                    //We want to set all the right-most varRef from LHS as being defined
                    std::vector<SgNode*> defs;
                    defs.insert(defs.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
                    defs.insert(defs.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());

                    //We want to set all the varRefs from the RHS as being used here
                    std::vector<SgNode*> uses;
                    uses.insert(uses.end(), attrs[1].getDefs().begin(), attrs[1].getDefs().end());
                    uses.insert(uses.end(), attrs[1].getUses().begin(), attrs[1].getUses().end());

                    //Set only the last def as being defined here.
                    SgNode* def = defs.back();
                    //Get the unique name of the def.
                    VarUniqueName * uName = varRename->getUniqueName(def);
                    ROSE_ASSERT(uName);

                    //Add the varRef as a definition at the current node of the ref's uniqueName
                    varRename->getDefTable()[op][uName->getKey()].push_back(node);

                    if(varRename->getDebug())
                    {
                        cout << "Found def for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
                    }

                    //Set all the uses as being used here.
                    foreach(NodeVec::value_type& iter, uses)
                    {
                        //Get the unique name of the def.
                        VarUniqueName * uName = varRename->getUniqueName(iter);
                        ROSE_ASSERT(uName);

                        //Add the varRef as a use at the current node of the ref's uniqueName
                        //We will correct the reference later.
                        varRename->getUseTable()[op][uName->getKey()].push_back(iter);

                        if(varRename->getDebug())
                        {
                            cout << "Found use for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
                        }
                    }

                    //Cut off the uses here. We will only pass up the defs.
                    return VariableRenaming::VarDefUseSynthAttr(def, NULL);
                }
                //Otherwise cover all the non-defining Ops
                default:
                {
                    //We want to set all the varRefs as being used here
                    std::vector<SgNode*> uses;
                    uses.insert(uses.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
                    uses.insert(uses.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());
                    uses.insert(uses.end(), attrs[1].getDefs().begin(), attrs[1].getDefs().end());
                    uses.insert(uses.end(), attrs[1].getUses().begin(), attrs[1].getUses().end());

                    //Set all the uses as being used here.
                    foreach(NodeVec::value_type& iter, uses)
                    {
                        //Get the unique name of the def.
                        VarUniqueName * uName = varRename->getUniqueName(iter);
                        ROSE_ASSERT(uName);

                        //Add the varRef as a use at the current node of the ref's uniqueName
                        //We will correct the reference later.
                        varRename->getUseTable()[op][uName->getKey()].push_back(iter);

                        if(varRename->getDebug())
                        {
                            cout << "Found use for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
                        }
                    }

                    //Return all the uses.
                    return VariableRenaming::VarDefUseSynthAttr(NULL, uses);
                }
            }
        }
        else
        {
            cout << "Error: BinaryOp without exactly 2 children." << endl;
            ROSE_ASSERT(false);
        }

    }
    //Catch all unary oprtations here.
    else if(isSgUnaryOp(node))
    {
        SgUnaryOp* op = isSgUnaryOp(node);

        //If we have an assigning operation, we want to list everything as being defined
        //Otherwise, everything is being used.
        VariantT type = op->variantT();
        switch(type)
        {
            //Defines the variable
            case V_SgMinusMinusOp:
            case V_SgPlusPlusOp:
            {
                std::vector<SgNode*> defs;
                defs.insert(defs.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
                defs.insert(defs.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());

                //Set only the last def as being defined here.
                SgNode* def = defs.back();
                //Get the unique name of the def.
                VarUniqueName * uName = varRename->getUniqueName(def);
                ROSE_ASSERT(uName);

                //Add the varRef as a definition at the current node of the ref's uniqueName
                varRename->getDefTable()[op][uName->getKey()].push_back(node);

                if(varRename->getDebug())
                {
                    cout << "Found def for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
                }

                //Return the defs from this node.
                return VariableRenaming::VarDefUseSynthAttr(def, NULL);
            }
            default:
            {
                //For all non-defining Unary Ops, add all of them as uses
                //We want to set all the varRefs as being used here
                std::vector<SgNode*> uses;

                //Guard agains unary ops that have no children (exception rethrow statement)
                if(attrs.size() > 0)
                {
                    uses.insert(uses.end(), attrs[0].getDefs().begin(), attrs[0].getDefs().end());
                    uses.insert(uses.end(), attrs[0].getUses().begin(), attrs[0].getUses().end());
                }

                //Set all the uses as being used here.
                foreach(NodeVec::value_type& iter, uses)
                {
                    //Get the unique name of the def.
                    VarUniqueName * uName = varRename->getUniqueName(iter);
                    ROSE_ASSERT(uName);

                    //Add the varRef as a use at the current node of the ref's uniqueName
                    //We will correct the reference later.
                    varRename->getUseTable()[op][uName->getKey()].push_back(iter);

                    if(varRename->getDebug())
                    {
                        cout << "Found use for " << uName->getNameString() << " at " << op->cfgForBeginning().toStringForDebugging() << endl;
                    }
                }

                //Return the combined defs and uses.
                return VariableRenaming::VarDefUseSynthAttr(NULL, uses);
            }
        }
    }
    else if(isSgStatement(node))
    {
        //Don't propogate uses and defs up to the statement level
        return VariableRenaming::VarDefUseSynthAttr();
    }

    //For the default case, we merge the defs and uses of every attribute and pass them upwards
    std::vector<SgNode*> defs;
    std::vector<SgNode*> uses;
    for(unsigned int i = 0; i < attrs.size(); i++)
    {
        cout << "Merging attr[" << i << "]" << endl;
        defs.insert(defs.end(), attrs[i].getDefs().begin(), attrs[i].getDefs().end());
        uses.insert(uses.end(), attrs[i].getUses().begin(), attrs[i].getUses().end());
    }

    //Set all the defs as being defined here.
    foreach(NodeVec::value_type& iter, defs)
    {
        //Get the unique name of the def.
        VarUniqueName * uName = varRename->getUniqueName(iter);
        ROSE_ASSERT(uName);

        //Add the varRef as a definition at the current node of the ref's uniqueName
        varRename->getDefTable()[node][uName->getKey()].push_back(node);

        if(varRename->getDebug())
        {
            cout << "Found def for " << uName->getNameString() << " at " << node->cfgForBeginning().toStringForDebugging() << endl;
        }
    }

    //Set all the uses as being used here.
    foreach(NodeVec::value_type& iter, uses)
    {
        //Get the unique name of the def.
        VarUniqueName * uName = varRename->getUniqueName(iter);
        ROSE_ASSERT(uName);

        //Add the varRef as a use at the current node of the ref's uniqueName
        //We will correct the reference later.
        varRename->getUseTable()[node][uName->getKey()].push_back(iter);

        if(varRename->getDebug())
        {
            cout << "Found use for " << uName->getNameString() << " at " << node->cfgForBeginning().toStringForDebugging() << endl;
        }
    }

    

    return VariableRenaming::VarDefUseSynthAttr(defs, uses);
}

void VariableRenaming::runDefUse(SgFunctionDefinition* func)
{
    //Keep track of visited nodes
    vector<SgNode*> visited;

    //Reset the first def list to prevent errors with global vars.
    firstDefList.clear();
    
    cfgNodeVec worklist;

    cfgNode current = cfgNode(func->cfgForBeginning());
    worklist.push_back(current);

    while(!worklist.empty())
    {
        if(DEBUG_MODE)
            cout << "-------------------------------------------------------------------------" << endl;
        //Get the node to work on
        current = worklist.front();
        worklist.erase(worklist.begin());

        //We don't want to do def_use on the ending CFGNode of the function definition
        //so if we see it, continue.
        //If we do this, then incorrect information will be propogated to the beginning of the function
        if(current == cfgNode(func->cfgForEnd()))
        {
            if(DEBUG_MODE)
                cout << "Skipped defUse on End of function definition." << endl;
            continue;
        }

        bool memberRefInserted = false;
        NodeVec changedNodes;
        bool changed = defUse(current, &memberRefInserted, changedNodes);
        
        //If memberRefs were inserted, then there are nodes previous to this one that are different.
        //Thus, we need to add those nodes to the working list
        if(memberRefInserted)
        {
            //Clear the worklist and visited list
            worklist.clear();
            visited.clear();

            //Insert each changed node into the list
            foreach(SgNode* chNode, changedNodes)
            {
                //Get the cfg node for this node
                cfgNode nextNode = cfgNode(chNode->cfgForBeginning());
                //Only insert the node in the worklist if it isn't there already.
                if(std::find(worklist.begin(), worklist.end(), nextNode) == worklist.end())
                {
                    worklist.push_back(nextNode);
                    if(DEBUG_MODE)
                        cout << "Member Ref Inserted: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
                }
            }

            //Restart work from where the new def was inserted.
            continue;
        }

        //Get the outgoing edges
        cfgEdgeVec outEdges = current.outEdges();

        //For every edge, add it to the worklist if it is not seen or something has changed
        foreach(cfgEdgeVec::value_type& edge, outEdges)
        {
            cfgNode nextNode = edge.target();

            //Only insert the node in the worklist if it isn't there already.
            if(std::find(worklist.begin(), worklist.end(), nextNode) == worklist.end())
            {
                if(changed)
                {
                    //Add the node to the worklist
                    worklist.push_back(nextNode);
                    if(DEBUG_MODE)
                        cout << "Defs Changed: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
                }
                //If the next node has not yet been visited
                else if(std::find(visited.begin(), visited.end(), nextNode.getNode()) == visited.end())
                {
                    //Add it to the worklist
                    worklist.push_back(nextNode);
                    if(DEBUG_MODE)
                        cout << "Next unvisited: Added " << nextNode.getNode()->class_name() << nextNode.getNode() << " to the worklist." << endl;
                }
            }
            
        }

        //Mark the current node as seen
        visited.push_back(current.getNode());
    }
}

bool VariableRenaming::defUse(FilteredCFGNode<IsDefUseFilter> node, bool *memberRefInserted, NodeVec &changedNodes)
{
    SgNode* current = node.getNode();

    //Handle each type of node
    if(DEBUG_MODE)
        cout << "Performing DefUse on " << current->class_name() << ":" << current << endl;

    bool defChanged = false;
    bool defRefInserted = false;
    bool useChanged = false;
    bool useRefInserted = false;
    defChanged = mergeDefs(node, &defRefInserted);
    useChanged = resolveUses(node, &useRefInserted, changedNodes);

    *memberRefInserted = useRefInserted;
    

    if(DEBUG_MODE)
        cout << "Defs were " << ((defChanged)?"changed.":"same.") << endl;
    
    return defChanged;
}

bool VariableRenaming::mergeDefs(cfgNode curNode, bool *memberRefInserted)
{
    SgNode* node = curNode.getNode();

    bool changed = false;

    if(DEBUG_MODE)
    {
        cout << "merging defs..." << endl;
        printDefs(node);
    }

    //We have the definitions stored as follows:
    //defTable contans the definitions as propogated by the CFG analysis
    //originalDefTable contains the original definitions that are part of the
    //given statement.

    //When we want to propogate the defs from the previous node(s) to this one,
    //We perform a few steps. This is dependent on the number of incoming edges.

    /*1 Edge: When we have linear control flow, we do the following:
    *        1. Copy the definitions from the previous node wholesale to a staging table.
    *        2. Copy in the original definitions from the current node, overwriting those
    *           from the previous node.
    *        3. Compare the staging and current tables, and only overwrite if needed.
    */

    /*2+ Edges: When we have branched control flow, we do the following:
     *       1. Copy the definitions from the previous node(s) wholesale to a staging table.
     *          Be careful to not insert duplicates.
     *       2. Copy in the original definitions from the current node, overwriting those
     *          from the previous node(s).
     *       3. Compare the staging and current tables, and only overwrite if needed.
     */

    //Expand any member variable references at the current node.
    *memberRefInserted = expandMemberDefinitions(curNode);
    
    TableEntry propDefs;
    //Retrieve the defs coming from previous cfgNodes
    aggregatePreviousDefs(curNode, propDefs);

    //Replace every entry in staging table that has definition in original defs
    //Also assign renaming numbers to any new definitions
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        //Replace the entry for this variable with the definitions at this node.
        propDefs[entry.first] = entry.second;

        //Now, iterate the definition vector for this node
        foreach(NodeVec::value_type& defNode, entry.second)
        {
            //Assign a number to each new definition. The function will prevent duplicates
            addRenameNumberForNode(entry.first, defNode);
        }
    }

    //For every originalDef, insert expanded defs for any propogated defs
    //that have an originalDef as a prefix
    VarName expVar;
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        foreach(TableEntry::value_type& propEntry, propDefs)
        {
            //Don't insert a def if it is already originally defined.
            if(originalDefTable[node].count(propEntry.first) != 0)
            {
                continue;
            }
            //If the original def is a prefix of the propogated def, add a def at this node
            //Compare sizes to guard against inserting original def in expanded table
            if(isPrefixOfName(propEntry.first, entry.first) && (propEntry.first.size() > entry.first.size()))
            {
                //Set this node as a definition point of the variable.
                expandedDefTable[node][propEntry.first].assign(1,node);
                *memberRefInserted = true;
                if(DEBUG_MODE_EXTRA)
                {
                    cout << "Inserted expandedDef for [" << keyToString(propEntry.first) << "] with originalDef prefix [" << keyToString(entry.first) << "]" << endl;
                }
            }
        }
    }

    //Replace every entry in staging table that has definition in expandedDefs
    //Also assign renaming numbers to any new definitions
    foreach(TableEntry::value_type& entry, expandedDefTable[node])
    {
        propDefs[entry.first] = entry.second;

        //Now, iterate the definition vector for this node
        foreach(NodeVec::value_type& defNode, entry.second)
        {
            //Assign a number to each new definition. The function will prevent duplicates
            addRenameNumberForNode(entry.first, defNode);
        }
    }

    //If there is an initial definition of a name at this node, we should insert it in the table
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        //If the given variable name is not present in the first def table
        if(firstDefList.count(entry.first) == 0)
        {
            //Set this node as the first definition point of this variable.
            firstDefList[entry.first] = node;
        }
    }

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Local Defs replaced in propDefs ";
        printDefs(propDefs);
    }

    //Now do a comparison to see if we should copy
    if(propDefs != defTable[node])
    {
        defTable[node] = propDefs;
        changed = true;
    }
    else
    {
        changed = false;
    }

    if(DEBUG_MODE)
    {
        cout << "Defs after Merge..." << endl;
        printDefs(node);
    }

    if(DEBUG_MODE)
    {
        printRenameTable();
    }

    return changed;
}

void VariableRenaming::aggregatePreviousDefs(cfgNode curNode, TableEntry& results)
{
    //SgNode* node = curNode.getNode();
    
    //Get the previous edges in the CFG for this node
    cfgEdgeVec inEdges = curNode.inEdges();

    if(inEdges.size() == 1)
    {
        SgNode* prev = inEdges[0].source().getNode();

        /*
        if(DEBUG_MODE)
        {
            cout << "Merging defs from " << prev->class_name() << prev << " to " << node->class_name() << node << endl;
            printDefs(prev);
        }*/

        //Copy the previous node defs to the staging table
        results = defTable[prev];
    }
    else if(inEdges.size() > 1)
    {
        //Iterate all of the incoming edges
        for(unsigned int i = 0; i < inEdges.size(); i++)
        {
            SgNode* prev = inEdges[i].source().getNode();

            /*
            if(DEBUG_MODE)
            {
                cout << "Merging defs from " << prev->class_name() << prev << endl;
                printDefs(prev);
            }*/

            //Perform the union of all the infoming definitions.
            foreach(TableEntry::value_type& entry, defTable[prev])
            {
                //Insert the definitions for this node at the end of the list
                results[entry.first].insert(results[entry.first].end(), entry.second.begin(), entry.second.end());
            }
        }
    }
    
    //Sort every vector in propDefs and remove duplicates
    foreach(TableEntry::value_type& entry, results)
    {
        std::sort(entry.second.begin(), entry.second.end());
        //Create new sequence of unique elements and remove duplicate ones
        entry.second.resize(std::unique(entry.second.begin(), entry.second.end()) - entry.second.begin());
    }

    /*
    if(DEBUG_MODE_EXTRA)
    {
        cout << "Merged propDefs ";
        printDefs(results);
    }*/

    return;
}


bool VariableRenaming::expandMemberDefinitions(cfgNode curNode)
{
    SgNode* node = curNode.getNode();

    bool changed = false;

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Expanding member defs at " << node->class_name() << node << endl;
        cout << "Original Node ";
        printDefs(originalDefTable[node]);
    }

    //We want to iterate the vars defined on this node, and expand them
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        if(DEBUG_MODE_EXTRA)
        {
            cout << "Checking [" << keyToString(entry.first) << "]" << endl;
        }

        //Check if the variableName has multiple parts
        if(entry.first.size() == 1)
        {
            continue;
        }

        //We are dealing with a multi-part variable, loop the entry and expand it
        //Start at one so we don't get the same defs in the original and expanded defs
        for(unsigned int i = 1; i < entry.first.size(); i++)
        {
            //Create a new varName vector that goes from beginning to end - i
            VarName newName;
            newName.assign(entry.first.begin(), entry.first.end() - i);

            if(DEBUG_MODE_EXTRA)
            {
                cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
            }

            //Only insert the new definition if it does not already exist
            if(originalDefTable[node].count(newName) == 0 && expandedDefTable[node].count(newName) == 0)
            {
                //Insert the new name as being defined here.
                expandedDefTable[node][newName] = NodeVec(1, node);
                changed = true;

                if(DEBUG_MODE_EXTRA)
                {
                    cout << "Inserted new name [" << keyToString(newName) << "] into defs." << endl;
                }
            }
        }
    }

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Expanded Node";
        printDefs(expandedDefTable[node]);
    }

    return changed;
}

bool VariableRenaming::resolveUses(FilteredCFGNode<IsDefUseFilter> curNode, bool *memberRefInserted, NodeVec &changedNodes)
{
    SgNode* node = curNode.getNode();
    
    bool changed = false;

    //We want to resolve the uses at the current node
    //We need to look to the defs at the current node, and match them
    //with the uses


    if(DEBUG_MODE)
        cout << "Resolving uses at " << node->class_name() << node << endl;

    changed = expandMemberUses(curNode);

    //Iterate every use at the current node
    foreach(TableEntry::value_type& entry, useTable[node])
    {
        //Check the defs that are active at the current node to find the reaching definition
        //We want to check if there is a definition entry for this use at the current node
        if(defTable[node].find(entry.first) != defTable[node].end())
        {
            //There is a definition entry. Now we want to see if the use is already up to date
            if(useTable[node][entry.first] != defTable[node][entry.first])
            {
                //The use was not up to date, so we update it
                changed = true;
                //Overwrite the use with this definition location(s).
                useTable[node][entry.first] = defTable[node][entry.first];
            }            
        }
        else
        {
            //If there are no defs for this use at this node, then we have a multi-part name
            //that has not been expanded. Thus, we want to expand it.
            *memberRefInserted = insertExpandedDefsForUse(curNode, entry.first, changedNodes);
        }
    }

    TableEntry results;
    //Get the previous defs
    aggregatePreviousDefs(curNode, results);

    //However, if there is a def at the current node, we want to use the previous
    //def as the use for this node.

    //Iterate every use at the current node
    foreach(TableEntry::value_type& entry, useTable[node])
    {
        //If any of these uses are for a variable defined at this node, we will
        //set the flag and correct it later.
        if(originalDefTable[node].count(entry.first) != 0)
        {
            useTable[node][entry.first] = results[entry.first];

            if(DEBUG_MODE)
                cout << "Fixed use of local def." << endl;
        }
    }

    return changed;
}

bool VariableRenaming::expandMemberUses(cfgNode curNode)
{
    SgNode* node = curNode.getNode();

    bool changed = false;

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Expanding member uses at " << node->class_name() << node << endl;
        cout << "Original Node ";
        printUses(useTable[node]);
    }

    //We want to iterate the vars used on this node, and expand them
    foreach(TableEntry::value_type& entry, useTable[node])
    {
        if(DEBUG_MODE_EXTRA)
        {
            cout << "Checking [" << keyToString(entry.first) << "]" << endl;
        }

        //Check if the variableName has multiple parts
        if(entry.first.size() == 1)
        {
            continue;
        }

        //We are dealing with a multi-part variable, loop the entry and expand it
        //Start at one so we don't reinsert same use
        for(unsigned int i = 1; i < entry.first.size(); i++)
        {
            //Create a new varName vector that goes from beginning to end - i
            VarName newName;
            newName.assign(entry.first.begin(), entry.first.end() - i);

            if(DEBUG_MODE_EXTRA)
            {
                cout << "Testing for presence of [" << keyToString(newName) << "]" << endl;
            }

            //Only insert the new definition if it does not already exist
            if(useTable[node].count(newName) == 0)
            {
                //Insert the new name as being used here.
                useTable[node][newName] = NodeVec(1, node);
                changed = true;

                if(DEBUG_MODE_EXTRA)
                {
                    cout << "Inserted new name [" << keyToString(newName) << "] into uses." << endl;
                }
            }
        }
    }

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Expanded Node ";
        printUses(useTable[node]);
    }

    return changed;
}

bool VariableRenaming::insertExpandedDefsForUse(cfgNode curNode, VarName name, NodeVec &changedNodes)
{
    SgNode* node = curNode.getNode();

    bool changed = false;

    if(DEBUG_MODE_EXTRA)
    {
        cout << "Checking for needed extra defs for uses at " << node->class_name() << node << endl;
        cout << "Checking for [" << keyToString(name) << "]" << endl;
    }

    //Check if the given name has a def at this node
    if(defTable[node].count(name) != 0)
    {
        if(DEBUG_MODE_EXTRA)
            cout << "Already have def." << endl;
        
        //If there is already a def, then nothing changes
        return false;
    }

    //No def for this name at this node, so we need to insert a def at the location
    //where the first part of this name was defined.
    //eg. s.a.b = x; (insert definition of s.a & s.a.b where s is first defined.)

    //Get the root of this name
    VarName rootName;
    rootName.assign(1,name[0]);

    if(firstDefList.count(rootName) == 0)
    {
        cout << "Error: No entry in first def list for root name [" << keyToString(rootName) << "]" << endl;
        ROSE_ASSERT(false);
    }

    //Start from the end of the name and insert definitions of every part
    //at the first definition point
    for(int i = 0; i < (signed int)name.size(); i++)
    {
        //Create a new varName vector that goes from beginning to end - i
        VarName newName;
        newName.assign(name.begin(), name.end() - i);

        if(DEBUG_MODE_EXTRA)
        {
            cout << "Testing for def of [" << keyToString(newName) << "] at var initial def." << endl;
        }

        if(originalDefTable[firstDefList[rootName]].count(newName) == 0)
        {
            originalDefTable[firstDefList[rootName]][newName].push_back(firstDefList[rootName]);
            changed = true;
            changedNodes.push_back(firstDefList[rootName]);
            if(DEBUG_MODE_EXTRA)
            {
                cout << "Inserted def for [" << keyToString(newName) << "] (root) [" << keyToString(rootName) << "] at node " << firstDefList[rootName] << endl;
            }
        }
    }

    if(DEBUG_MODE_EXTRA)
        cout << "Finished inserting references. Changed: " << ((changed)?"true":"false") << endl;

    return changed;
}

int VariableRenaming::getRenameNumberForNode(const VarName& var, SgNode* node) const
{
    ROSE_ASSERT(node);

    NodeNumRenameTable::const_iterator iter;
    iter = nodeRenameTable.find(var);

    if(iter == nodeRenameTable.end())
    {
        return -1;
    }
    else
    {
        //Try and get the number for the node
        NodeNumRenameEntry::const_iterator iter2;
        iter2 = (*iter).second.find(node);
        if(iter2 != (*iter).second.end())
        {
            return (*iter2).second;
        }
        //Node not in table
        else
        {
            return -1;
        }
    }
}

SgNode* VariableRenaming::getNodeForRenameNumber(const VarName& var, int num) const
{
    ROSE_ASSERT(num > 0);

    NumNodeRenameTable::const_iterator iter;
    iter = numRenameTable.find(var);
    if(iter == numRenameTable.end())
    {
        return NULL;
    }
    else
    {
        //Try and get the node for the number
        NumNodeRenameEntry::const_iterator iter2;
        iter2 = (*iter).second.find(num);
        if(iter2 != (*iter).second.end())
        {
            return (*iter2).second;
        }
        //Number not in table
        else
        {
            return NULL;
        }
    }
}

int VariableRenaming::getMaxRenameNumberForName(const VarName& var) const
{
    int res = -1;

    NumNodeRenameTable::const_iterator iter;
    iter = numRenameTable.find(var);
    if(iter == numRenameTable.end())
    {
        return res;
    }

    NumNodeRenameEntry::const_iterator iter2;
    for(iter2 = (*iter).second.begin(); iter2 != (*iter).second.end(); ++iter2)
    {
        if((*iter2).first > res)
        {
            res = (*iter2).first;
        }
    }

    return res;
}

int VariableRenaming::addRenameNumberForNode(const VarName& var, SgNode* node)
{
    ROSE_ASSERT(node);

    //Check if the var/node combination is already in the table.
    if(nodeRenameTable[var].count(node) == 1)
    {
        return nodeRenameTable[var][node];
    }

    //Not in the table, so we now need to add it.
    int nextNum = nodeRenameTable[var].size() + 1;

    nodeRenameTable[var][node] = nextNum;
    numRenameTable[var][nextNum] = node;

    if(DEBUG_MODE)
        cout << "Renaming Added:[" << keyToString(var) << "]:" << nextNum << " - " << node << endl;

    return nextNum;
}

VariableRenaming::NodeVec VariableRenaming::getAllUsesForDef(const VarName& var, int num)
{
    NodeVec res;
    SgNode* defNode = getNodeForRenameNumber(var, num);

    if(defNode == NULL)
    {
        res.clear();
        return res;
    }

    //Traverse the use Table looking for locations where the def is used
    foreach(DefUseTable::value_type& entry, useTable)
    {
        //If this entry contains the variable that we want
        if(entry.second.count(var) != 0)
        {
            //See if the use vector contains the defining node we want
            if(std::find(entry.second[var].begin(), entry.second[var].end(), defNode) != entry.second[var].end())
            {
                //Add the current node to list of those using the variable
                res.push_back(entry.first);
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getReachingDefsAtNode(SgNode* node)
{
    //We want to get all the reaching defs at this node and insert them into the result table

    NumNodeRenameTable res;

    //Iterate every variable definition reaching this node
    foreach(TableEntry::value_type& entry, defTable[node])
    {
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res[entry.first].count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[entry.first][renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same def propogated twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                cout << "Error: Found propogated def with no entry in rename table." << endl;
                ROSE_ASSERT(false);
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getReachingDefsAtNodeForName(SgNode* node, const VarName& var)
{
    //We want to get all the reaching defs at this node and insert them into the result table

    NumNodeRenameEntry res;

    //Iterate every variable definition reaching this node
    foreach(TableEntry::value_type& entry, defTable[node])
    {
        //Check that the current var is the one we want.
        if(entry.first != var)
        {
            continue;
        }
        
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res.count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same def propogated twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                cout << "Error: Found propogated def with no entry in rename table." << endl;
                ROSE_ASSERT(false);
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getUsesAtNode(SgNode* node)
{
    //We want to get all the uses at this node and insert them into the result table

    NumNodeRenameTable res;

    //Iterate every variable definition used at this node
    foreach(TableEntry::value_type& entry, useTable[node])
    {
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res[entry.first].count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[entry.first][renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same use propogated twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                /* This situation can happen in certain cases, so we don;t want to assert.
                 *
                 * ex. for(int i = 0; i < 10; i++)
                 *     {
                 *        return i;
                 *     }
                 *
                 * The varRef for i++ will not have its uses corrected, and so will
                 * be using itself. This is not technically wrong, since control will
                 * never reach that varRef.
                 */
                if(DEBUG_MODE)
                {
                    cout << "Warning: Found use with no entry in rename table." << endl;
                }
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getUsesAtNodeForName(SgNode* node, const VarName& var)
{
    //We want to get all the uses at this node and insert them into the result table

    NumNodeRenameEntry res;

    //Iterate every variable use at this node
    foreach(TableEntry::value_type& entry, useTable[node])
    {
        //Check that the current var is the one we want.
        if(entry.first != var)
        {
            continue;
        }
        
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res.count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same use propogated twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                /* This situation can happen in certain cases, so we don't want to assert.
                 *
                 * ex. for(int i = 0; i < 10; i++)
                 *     {
                 *        return i;
                 *     }
                 *
                 * The varRef for i++ will not have its uses corrected, and so will
                 * be using itself. This is not technically wrong, since control will
                 * never reach that varRef.
                 */
                if(DEBUG_MODE)
                {
                    cout << "Warning: Found use with no entry in rename table." << endl;
                }
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getDefsAtNode(SgNode* node)
{
    NumNodeRenameTable original = getOriginalDefsAtNode(node);
    NumNodeRenameTable expanded = getExpandedDefsAtNode(node);

    //Loop the expanded table and insert it into the original table
    foreach(NumNodeRenameTable::value_type& entry, expanded)
    {
        //Insert the entry wholesale
        if(original.count(entry.first) == 0)
        {
            original[entry.first] = entry.second;
        }
        //Or merge it with an existing one
        else
        {
            foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
            {
                //Insert the entry wholesale
                if(original[entry.first].count(tableEntry.first) == 0)
                {
                    original[entry.first][tableEntry.first] = tableEntry.second;
                }
                else
                {
                    cout << "Error: Same renaming present in original and expanded defs." << endl;
                    cout << "At node " << node << endl;
                    cout << "Original ";
                    printRenameTable(original);
                    cout << "Expanded ";
                    printRenameTable(expanded);
                    ROSE_ASSERT(false);
                }
            }
        }
    }

    return original;
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getDefsAtNodeForName(SgNode* node, const VarName& var)
{
    NumNodeRenameEntry original = getOriginalDefsAtNodeForName(node, var);
    NumNodeRenameEntry expanded = getExpandedDefsAtNodeForName(node, var);

    //Loop the expanded table and insert it into the original table
    foreach(NumNodeRenameEntry::value_type& tableEntry, expanded)
    {
        //Insert the entry wholesale
        if(original.count(tableEntry.first) == 0)
        {
            original[tableEntry.first] = tableEntry.second;
        }
        else
        {
            cout << "Error: Same renaming present in original and expanded defs." << endl;
            ROSE_ASSERT(false);
        }
    }

    return original;
}


VariableRenaming::NumNodeRenameTable VariableRenaming::getOriginalDefsAtNode(SgNode* node)
{
    //We want to get all the original defs at this node and insert them into the result table

    NumNodeRenameTable res;

    //Iterate every variable definition reaching this node
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res[entry.first].count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[entry.first][renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same def originally defined twice on same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                /* This situation can happen in certain cases, so we don't want to assert.
                 *
                 * ex. for(int i = 0; i < 10; i++)
                 *     {
                 *        return i;
                 *     }
                 *
                 * The i++ will have an original def for i. However, it will not be in the rename
                 * table for i. This is not technically wrong, since control will never
                 * reach that i++, so it will not have an ordering wrt. the other definitions.
                 */
                if(DEBUG_MODE)
                {
                    cout << "Warning: Found original def with no entry in rename table." << endl;
                }
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getOriginalDefsAtNodeForName(SgNode* node, const VarName& var)
{
    //We want to get all the original defs at this node and insert them into the result table

    NumNodeRenameEntry res;

    //Iterate every variable use at this node
    foreach(TableEntry::value_type& entry, originalDefTable[node])
    {
        //Check that the current var is the one we want.
        if(entry.first != var)
        {
            continue;
        }
        
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res.count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same original Def twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                /* This situation can happen in certain cases, so we don't want to assert.
                 *
                 * ex. for(int i = 0; i < 10; i++)
                 *     {
                 *        return i;
                 *     }
                 *
                 * The i++ will have an original def for i. However, it will not be in the rename
                 * table for i. This is not technically wrong, since control will never
                 * reach that i++, so it will not have an ordering wrt. the other definitions.
                 */
                if(DEBUG_MODE)
                {
                    cout << "Warning: Found original def with no entry in rename table." << endl;
                }
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getExpandedDefsAtNode(SgNode* node)
{
    //We want to get all the expanded defs at this node and insert them into the result table

    NumNodeRenameTable res;

    //Iterate every variable definition expanded on this node
    foreach(TableEntry::value_type& entry, expandedDefTable[node])
    {
        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res[entry.first].count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[entry.first][renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same def expanded twice on same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                cout << "Error: Found expanded def with no entry in rename table." << endl;
                ROSE_ASSERT(false);
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getExpandedDefsAtNodeForName(SgNode* node, const VarName& var)
{
    //We want to get all the uses at this node and insert them into the result table

    NumNodeRenameEntry res;

    //Iterate every variable use at this node
    foreach(TableEntry::value_type& entry, expandedDefTable[node])
    {
        //Check that the current var is the one we want.
        if(entry.first != var)
        {
            continue;
        }

        //Iterate every definition site for this variable
        foreach(NodeVec::value_type& defEntry, entry.second)
        {
            //Get the rename number for the current variable at the current def site
            int renameNum = getRenameNumberForNode(entry.first, defEntry);

            //If the name is defined at this node
            if(renameNum > 0)
            {
                //If the renumbering is not already in the result
                if(res.count(renameNum) == 0)
                {
                    //Add the renumbering to the result
                    res[renameNum] = defEntry;
                }
                else
                {
                    cout << "Error: Same expanded def twice to same node." << endl;
                    ROSE_ASSERT(false);
                }
            }
            else
            {
                cout << "Error: Found expanded def with no entry in rename table." << endl;
                ROSE_ASSERT(false);
            }
        }
    }

    return res;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getDefsForSubtree(SgNode* node)
{
    class DefSearchTraversal : public AstSimpleProcessing
     {
     public:
         VariableRenaming::NumNodeRenameTable result;
         VariableRenaming* varRenamingAnalysis;

         virtual void visit(SgNode* node)
         {
             //Look up defs at this particular node
             VariableRenaming::NumNodeRenameTable defsAtNode = varRenamingAnalysis->getDefsAtNode(node);

             //Traverse the defs
             foreach(VariableRenaming::NumNodeRenameTable::value_type& entry, defsAtNode)
             {
                 //If this is the first time the var has been seen, add it wholesale
                 if(result.count(entry.first) == 0)
                 {
                     result[entry.first] = entry.second;
                     continue;
                 }
                 //Traverse each definition of the variable
                 foreach(VariableRenaming::NumNodeRenameEntry::value_type& tableEntry, entry.second)
                 {
                     if(result[entry.first].count(tableEntry.first) == 0)
                     {
                         result[entry.first][tableEntry.first] = tableEntry.second;
                     }
                     else
                     {
                         cout << "Error: Same rename number defined on multiple nodes." << endl;
                         ROSE_ASSERT(false);
                     }
                 }
             }
         }
     };

     DefSearchTraversal traversal;
     traversal.varRenamingAnalysis = this;
     traversal.traverse(node, preorder);

     return traversal.result;
}

VariableRenaming::NumNodeRenameTable VariableRenaming::getReachingDefsAtFunctionEnd(SgFunctionDefinition* node)
{
    ROSE_ASSERT(node);
    NumNodeRenameTable result;

    cfgNode lastNode = cfgNode(node->cfgForEnd());
    cfgEdgeVec lastEdges = lastNode.inEdges();
    if(lastEdges.size() == 0)
    {
        cout << "Error: No incoming edges to end of function definition." << endl;
        ROSE_ASSERT(false);
    }
    else if(lastEdges.size() == 1)
    {
        return getReachingDefsAtNode(lastEdges[0].source().getNode());
    }
    else
    {
        //Iterate and merge each edge
        foreach(cfgEdgeVec::value_type& edge, lastEdges)
        {
            NumNodeRenameTable temp = getReachingDefsAtNode(edge.source().getNode());

            //Merge the tables
            foreach(NumNodeRenameTable::value_type& entry, temp)
            {
                //Insert the entry wholesale
                if(result.count(entry.first) == 0)
                {
                    result[entry.first] = entry.second;
                }
                //Or merge it with an existing one
                else
                {
                    foreach(NumNodeRenameEntry::value_type& tableEntry, entry.second)
                    {
                        //Insert the entry wholesale
                        if(result[entry.first].count(tableEntry.first) == 0)
                        {
                            result[entry.first][tableEntry.first] = tableEntry.second;
                        }
                        else
                        {
                            //Check for equivalence
                            if(result[entry.first][tableEntry.first] != tableEntry.second)
                            {
                                cout << "Error: Same Renaming number has two different definition points." << endl;
                                ROSE_ASSERT(false);
                            }
                        }
                    }
                }
            }
        }

        return result;
    }
}

VariableRenaming::NumNodeRenameEntry VariableRenaming::getReachingDefsAtFunctionEndForName(SgFunctionDefinition* node, const VarName& var)
{
    ROSE_ASSERT(node);
    NumNodeRenameEntry result;

    cfgNode lastNode = cfgNode(node->cfgForEnd());
    cfgEdgeVec lastEdges = lastNode.inEdges();
    if(lastEdges.size() == 0)
    {
        cout << "Error: No incoming edges to end of function definition." << endl;
        ROSE_ASSERT(false);
    }
    else if(lastEdges.size() == 1)
    {
        return getReachingDefsAtNodeForName(lastEdges[0].source().getNode(), var);
    }
    else
    {
        //Iterate and merge each edge
        foreach(cfgEdgeVec::value_type& edge, lastEdges)
        {
            NumNodeRenameEntry temp = getReachingDefsAtNodeForName(edge.source().getNode(), var);

            foreach(NumNodeRenameEntry::value_type& tableEntry, temp)
            {
                //Insert the entry wholesale
                if(result.count(tableEntry.first) == 0)
                {
                    result[tableEntry.first] = tableEntry.second;
                }
                else
                {
                    //Check for equivalence
                    if(result[tableEntry.first] != tableEntry.second)
                    {
                        cout << "Error: Same Renaming number has two different definition points." << endl;
                        ROSE_ASSERT(false);
                    }
                }
            }
        }

        return result;
    }
}