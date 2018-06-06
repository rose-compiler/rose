#ifndef FUNCTION_INFO_H
#define FUNCTION_INFO_H

#include <BinaryAstHash.h>
#include "Combinatorics.h"

/** LibraryIdentification.
 *
 *  This namespace encapsulates function for FLIRT ( Fast Library
 *  Identification and Recognition Technology) like functionality for
 *  ROSE binary analysis.
 *  Currently only uses the Fowler-Noll-Vo HasherFnv class in
 *  Combinatorics.h.  We should add options for others.
 **/
namespace LibraryIdentification
{
    class FunctionInfo
    {
    public:
        /**
         *  FunctionInfo
         *
         *  Constructor.  Combines all the information required to
         *  identify a function. 
         *  This constructor allows the user to define everything.
         *
         * @param[in] funcName  Name of the function to add
         * @param[in] funcHash  Unique Hash of the function to add
         * (Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h)
         * @param[in] libHash  Unique Hash of the library this
         * function belongs to. The Library should already be in the database.
         **/
    FunctionInfo(const std::string& ifuncName, const std::string& ifuncHash, const std::string& ilibHash) :
        funcName(ifuncName), funcHash(ifuncHash), libHash(ilibHash), sgAsmFunc(0) {};
        
        /**
         *  FunctionInfo
         *
         *  Constructor.  Combines all the information required to
         *  identify a function. 
         *  This constructor constructs the hash from the
         *  SgAsmFunction node.
         *  Note that currently on FNV hasher is used.  This should be
         *  an option.
         *
         * @param[in] function Binary AST Function Node
         * @param[in] libHash  Unique Hash of the library this
         * function belongs to. The Library should already be in the database.
         **/
    FunctionInfo(SgAsmFunction* function, const std::string& ilibHash) :
        funcName(function->get_name()),  libHash(ilibHash), sgAsmFunc(function)
        {
             initializeHash(function);
        }
        
        /**
         *  FunctionInfo
         *
         *  Constructor.  Combines all the information required to
         *  identify a function. 
         *  This constructor constructs the hash from the
         *  SgAsmFunction node.
         *  Note that currently on FNV hasher is used.  This should be
         *  an option.
         *
         * @param[in] function Binary AST Function Node
         * @param[in] libInfo  LibraryInfo that should be in the database
         **/
    FunctionInfo(SgAsmFunction* function, LibraryInfo& libInfo) :
        funcName(function->get_name()),  libHash(libInfo.libHash), sgAsmFunc(function)
        {
             initializeHash(function);
        }

        /**
         *  FunctionInfo
         *
         *  Constructor.  Only takes the hash, Rest to be filled in from
         *  matching in the database. 
         *
         * @param[in] funcHash  Unique Hash of the function to add
         * (Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h)
         **/
    FunctionInfo(const std::string& ifuncHash) :
        funcName(""), funcHash(ifuncHash), libHash(""), sgAsmFunc(0) {};

        /**
         *  FunctionInfo
         *
         *  Constructor just takes a function, but only initializes
         *  the hash. Rest to be filled in from
         *  matching in the database
         *
         * @param[in] function Binary AST Function Node
         **/
    FunctionInfo(SgAsmFunction* function) :
        funcName(function->get_name()),  libHash(""), sgAsmFunc(function)
        {
            initializeHash(function);
        }

        friend bool operator<(const FunctionInfo& lhs, const FunctionInfo& rhs) 
        {
            return lhs.funcHash < rhs.funcHash;
        }


        //@brief The name of the function
        std::string funcName;

        //@brief A hash that should uniquely identify the function
        std::string funcHash;

        //@brief A hash that should uniquely identify the library the
        //function belongs to.  It should already be in the database.
        std::string libHash;

        //@brief A pointer to the binary version of this function IF
        //IT'S AVAILIBLE.  THIS IS LIKELY TO BE NULL
        SgAsmFunction* sgAsmFunc;
        

    private:
        void initializeHash(SgNode* function) 
        {
            //Should make the hash optional via factory method in Combinatorics.h
            Rose::Combinatorics::HasherFnv fnv;
            Rose::Combinatorics::Hasher& hasher = dynamic_cast<Rose::Combinatorics::Hasher&>(fnv);            

            Rose::BinaryAnalysis::AstHash astHash(hasher);            
            astHash.traverse((SgNode*)function,preorder);
            funcHash = fnv.toString();
        }
    };
    
}



#endif //FUNCTION_INFO
