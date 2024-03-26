#include "Name2Node.h"
#include <AstProcessing.h>
#include "VxUtilFuncs.h"
#include <stdexcept>

/**
 * \class NameFinder 
 * 
 * \brief This class implements the AstPrePostProcessing 
 * search and iterates over the AST to find functions,
 * and initializedNames
 * 
*/ 
class NameFinder : public AstPrePostProcessing {

public: 
    // constructor initializes prefixes with empty string
    NameFinder(std::unordered_map<std::string, SgFunctionDefinition*>& functionMap, std::unordered_map<std::string, SgNode*>& variableMap) : 
        functionMap(functionMap), variableMap(variableMap) {}

protected:
    virtual void preOrderVisit(SgNode* node) {

        // If it's a function def, add it
        if (SgFunctionDefinition* funcDef = isSgFunctionDefinition(node)) {
            SgFunctionDeclaration* funcDecl = funcDef->get_declaration();
            std::string str = VxUtilFuncs::compileInternalFunctionName(funcDecl, VxUtilFuncs::getNodeRelativePath(funcDecl));

            //add the function to the map
            functionMap[str] = funcDef;
            variableMap[str] = node;
        }
        // If it's a variable
        else if (SgInitializedName* initializedName = isSgInitializedName(node)) {
            std::string str = VxUtilFuncs::generateAccessNameStrings(initializedName);

            variableMap[str] = node;
        }
    }

    virtual void postOrderVisit(SgNode* node) {
        //not necessary anymore :D
    }

private:
    //map references
    std::unordered_map<std::string, SgFunctionDefinition*>& functionMap;
    std::unordered_map<std::string, SgNode*>& variableMap;
};


Name2Node::Name2Node(SgProject* root) {
    NameFinder finder(functionMap, variableMap);
    finder.traverse(root);
}

/**
 * \return the SgFunctionDefinition* that points to the function that's name
 * matches name. If it isn't found, it will return NULL. 
 * 
 * \param[in] name : This is the name of the function
*/
SgFunctionDefinition* Name2Node::getFunction(std::string name) {
    //uses the at function so the name isn't added to the map if it doesn't exist
    try {
        return functionMap.at(name);
    }
    //catches out_of_range exception emitted when name isn't in the functionMap
    catch (const std::out_of_range& oor) {
        return NULL;
    }
}

/**
 * \return the SgInitializedName, SgThisExp, or SgFunctionDefinition that's name matches name
 * 
 * \param[in] name : The name of the SgInitializedName, SgThisExp, or SgFunctionDefinition
*/
SgNode* Name2Node::getVariable(std::string name) {
    int nameLength = name.length();
    // if it's an anonymous variable (compiler generated name) throw an exception
    if (name.find("__anonymous_0x") != std::string::npos) {
        throw AnonymousNameException(name);
    }
    else if (name[0] == '!' && name[nameLength-1] == '!') {
        throw AddressReferenceException(name);
    }
    else if (nameLength > 5 && name.substr(nameLength - 5, 5) == "@this") {
        //the "this" case is weird. 

        std::string funcName = name.substr(1, nameLength - 6);
        
        SgFunctionDefinition* funcDef = functionMap.at(funcName);

        ROSE_ASSERT(funcDef != 0);

        //instead of using the map we'll just find an SgThisExp
        Rose_STL_Container<SgNode*> thisExpList = NodeQuery::querySubTree(funcDef, V_SgThisExp);

        if (thisExpList.empty()) {
            return NULL;
        }
        else {
            return thisExpList[0];
        }
    }
    try {
        //uses the at function so the name isn't added to the map if it doesn't exist
        return variableMap.at(name);
    }
    //catches out_of_range exception emitted when name isn't in the variable map
    catch (const std::out_of_range& oor) {
        return NULL;
    }

    Sawyer::Message::mlog[Sawyer::Message::Common::ERROR] << "In Name2Node::getVariable(): got to end of function";
    ROSE_ABORT();
    return NULL;
}
