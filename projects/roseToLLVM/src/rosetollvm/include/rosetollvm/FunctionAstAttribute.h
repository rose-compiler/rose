#ifndef FUNCTION_AST_ATTRIBUTE
#define FUNCTION_AST_ATTRIBUTE

#include <rosetollvm/DeclarationsAstAttribute.h>

class LLVMAstAttributes;
class FunctionAstAttribute : public DeclarationsAstAttribute { 
public: 

    FunctionAstAttribute(SgFunctionDeclaration *function_declaration_, SgClassType *n = NULL) : DeclarationsAstAttribute(n),
                                                                                                function_declaration(function_declaration_),
                                                                                                entry_label(".entry"),
                                                                                                current_label(entry_label),
                                                                                                return_label(".return")
    {}

    int numCoerces() { return coerce_names.size(); }
    std::string getCoerceName(int i) { return coerce_names[i]; }
    SgType *getCoerceType(int i) { return coerce_types[i]; }
    void addCoerce(std::string name, SgType *type) {
        coerce_names.push_back(name);
        coerce_types.push_back(type);
    }

    SgFunctionDeclaration *getFunctionDeclaration() { return function_declaration; }
    SgFunctionType *getFunctionType() { return function_declaration -> get_type(); }

    std::string getEntryLabel()  { return entry_label; }
    void resetCurrentLabel(std::string target) { current_label = target; }
    std::string getCurrentLabel()  { return current_label; }
    std::string getReturnLabel() { return return_label; }

    /**
     *
     * There is a goto from the current block to this target
     */
    void addLabelPredecessor(std::string target) {
        label_map[target].push_back(current_label);
    }

    void addLabelPredecessor(std::string target, std::string predecessor) {
        label_map[target].push_back(predecessor);
    }

    void removeLastLabelPredecessor(std::string target) {
        label_map[target].pop_back();
    }

    int numLabelPredecessors(std::string target) {
        return label_map[target].size();
    }

    std::string getLabelPredecessor(std::string target, int i) {
        return label_map[target][i];
    }

    std::vector<std::string> &getLabelPredecessors(std::string target) {
        return label_map[target];
    }

    int numRemoteVariableDeclarations() { return remote_variable_declarations.size(); }
    SgInitializedName *getRemoteVariableDeclaration(int i) { return remote_variable_declarations[i]; }
    void insertRemoteVariableDeclaration(SgInitializedName *decl) {
        remote_variable_declarations.push_back(decl);
    }

private:

    SgFunctionDeclaration *function_declaration;

    /**
     * This is a map from each label l into the set of labels associated with basic blocks that can
     * branch into l.
     */
    std::map<std::string, std::vector<std::string> > label_map;
    std::string entry_label,
                current_label,
                return_label;

    std::vector<std::string> coerce_names;
    std::vector<SgType *> coerce_types;

    std::vector<SgInitializedName *> remote_variable_declarations;
};

#endif


