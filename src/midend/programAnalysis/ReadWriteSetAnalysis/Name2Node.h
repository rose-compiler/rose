#ifndef NAME2NODE_H
#define NAME2NODE_H

#include <string>
#include <unordered_map>

/**
 * \class Name2Node
 *
 * \brief   Name2Node iterates over the AST and finds nodes that correspond to names
 *          It looks for any SgInitializedName, SgThisExp, or SgFunctionDefinitions
 *          and adds them to a string to node* map that can be read with getFunction()
 *          or getVariable()
 *
 **/

class Name2Node {

public:
    Name2Node(SgProject* root);

    /**
     * \return the SgFunctionDefinition* that points to the function that's name
     * matches name. If it isn't found, it will return NULL. 
     * 
     * \param[in] name : This is the name of the function
    */
    SgFunctionDefinition* getFunction(std::string name);

    /**
     * \return the SgInitializedName, SgThisExp, or SgFunctionDefinition that's name matches name
     * 
     * \param[in] name : The name of the SgInitializedName, SgThisExp, or SgFunctionDefinition
    */
    SgNode* getVariable(std::string name);

    private:
    // maps from names to nodes.
    std::unordered_map<std::string, SgFunctionDefinition*> functionMap;
    std::unordered_map<std::string, SgNode*> variableMap;
};

/**
 * \class AnonymousNameException 
 * 
 * This class implements std::exception for whenever an anonymous name is passed into getVariable.
 * Anonymous names are not consistent across different compilations
 * */ 

class AnonymousNameException : public std::exception {
private:
    std::string message;

public:
    AnonymousNameException(std::string(name)) {
        message = "Anonymous name given: " + name + " Its name is generated at compile time.";
    }

    /**
     * returns a message that shows what name was given.
    */
    virtual const char* what() const noexcept {
        return message.c_str();
    }
};

/**
 * \class AddressReferenceException 
 * 
 * This class implements std::exception for whenever an address is passed into getVariable.
 * */ 

class AddressReferenceException : public std::exception {
private:
    std::string message;

public:
    AddressReferenceException(std::string(name)) {
        message = "Address reference given: " + name;
    }

    /**
     * returns a message that shows what address was given.
    */
    virtual const char* what() const noexcept {
        return message.c_str();
    }
};

#endif //NAME2NODE_H
