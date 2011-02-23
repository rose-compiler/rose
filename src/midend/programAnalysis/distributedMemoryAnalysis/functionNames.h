
#ifndef PARALLEL_COMPASS_FUNCTIONNAMES_H
#define PARALLEL_COMPASS_FUNCTIONNAMES_H

#include "DistributedMemoryAnalysis.h"


#define DEBUG_OUTPUT_FUNCNAMES false

// ----------------------------------------------------
// preTraversal
// ----------------------------------------------------

// The pre-traversal runs before the distributed part of the analysis and is used to propagate context information down
// to the individual function definitions in the AST. Here, it just computes the depth of nodes in the AST.
class FunctionNamesPreTraversal: public AstTopDownProcessing<int>
{
protected:
    int evaluateInheritedAttribute(SgNode *node, int depth)
    {
#if DEBUG_OUTPUT_FUNCNAMES
      std::cout << " eval inherited: node: " << node->class_name() << "  depth:" << depth << std::endl;
#endif
        return depth + 1;
    }
};


// ----------------------------------------------------
// postTraversal
// ----------------------------------------------------

// The post-traversal runs after the distributed part of the analysis and is used to collect the information it
// computed. Here, the synthesized attributes computed by the distributed analysis are strings representing information
// about functions. These strings are concatenated by the post-traversal (and interleaved with newlines where necessary).
class FunctionNamesPostTraversal: public AstBottomUpProcessing<std::string>
{
protected:
    std::string evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synAttributes)
    {
        std::string result = "";
        SynthesizedAttributesList::iterator s;
        for (s = synAttributes.begin(); s != synAttributes.end(); ++s)
        {
            std::string &str = *s;
            result += str;
            if (str.size() > 0 && str[str.size()-1] != '\n')
                result += "\n";
        }
#if DEBUG_OUTPUT_FUNCNAMES
        std::cout << " eval synthesized: node: " << node->class_name() << "  result:" << result << " ..............\n" <<std::endl;
#endif
        return result;
    }

    std::string defaultSynthesizedAttribute()
    {
        return "";
    }
};


// ----------------------------------------------------
// FunctionNames
// ----------------------------------------------------

// This is the distributed part of the analysis. The DistributedMemoryTraversal base class is a template taking an
// inherited and a synthesized attribute type as template parameters; these are the same types used by the pre- and
// post-traversals.
class FunctionNames: public DistributedMemoryTraversal<int, std::string>
{
protected:
 // The analyzeSubtree() method is called for every defining function declaration in the AST. Its second argument is the
 // inherited attribute computed for this node by the pre-traversal, the value it returns becomes the synthesized
 // attribute used by the post-traversal.
    std::string analyzeSubtree(SgFunctionDeclaration *funcDecl, int depth)
    {
        std::string funcName = funcDecl->get_name().str();
        std::stringstream s;
        s << "process " << myID() << ": at depth " << depth << ": function " << funcName;
#if DEBUG_OUTPUT_FUNCNAMES
        std::cout << " analyzeSubtree of funcDecl: " << funcName << "  id:" << myID()
                  << "       result:  " << s.str() << std::endl;
#endif
        return s.str();
    }

 // The user must implement this method to pack a synthesized attribute (a string in this case) into an array of bytes
 // for communication. The first component of the pair is the number of bytes in the buffer.
    std::pair<int, void *> serializeAttribute(std::string attribute) const
    {
        int len = attribute.size() + 1;
        char *str = strdup(attribute.c_str());
        return std::make_pair(len, str);
    }

 // This method must be implemented to convert the serialized data to the application's synthesized attribute type.
    std::string deserializeAttribute(std::pair<int, void *> serializedAttribute) const
    {
        return std::string((const char *) serializedAttribute.second);
    }

 // This method is optional (the default implementation is empty). Its job is to free memory that may have been
 // allocated by the serializeAttribute() method.
    void deleteSerializedAttribute(std::pair<int, void *> serializedAttribute) const
    {
        std::free(serializedAttribute.second);
    }
};

#endif
