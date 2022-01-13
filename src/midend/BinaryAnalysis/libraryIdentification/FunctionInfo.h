#ifndef FUNCTION_INFO_H
#define FUNCTION_INFO_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/AstHash.h>
#include "Combinatorics.h"
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>

// documented elsewhere
namespace LibraryIdentification
{
    /** Information about a function.
     *
     *  Combines all the information to uniquely identify a single function in one object.
     **/
    class FunctionInfo
    {
    public:
        /** Constructor.
         *
         *  Combines all the information required to identify a function.  This constructor allows the user to define
         *  everything.
         *
         * @param[in] funcName  Name of the function to add
         * @param[in] funcHash  Unique Hash of the function to add Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h.
         * @param[in] libHash  Unique Hash of the library this function belongs to. The Library should already be in the database.
         **/
        FunctionInfo(const std::string& funcName, const std::string& funcHash, const std::string& libHash) :
        funcName(funcName), funcHash(funcHash), libHash(libHash), binaryFunction() {};
        
        /** Constructor.
         *
         *  Combines all the information required to identify a function.  This constructor constructs the hash from the
         *  SgAsmFunction node.  Note that currently on FNV hasher is used.  This should be an option.
         *
         * @param[in] partitioner Required to get the basic blocks of the function
         * @param[in] function Binary AST Function Node
         * @param[in] libHash  Unique Hash of the library this function belongs to. The Library should already be in the database.
         **/
        FunctionInfo(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                     Rose::BinaryAnalysis::Partitioner2::Function::Ptr function, const std::string& libHash) :
        funcName(function->name()), libHash(libHash), binaryFunction(function)
        {
             initializeHash(partitioner, function);
        }

        /** Constructor.
         *
         *  Combines all the information required to identify a function.  This constructor constructs the hash from the
         *  SgAsmFunction node.  Note that currently on FNV hasher is used.  This should be an option.
         *
         * @param[in] partitioner Required to get the basic blocks of the function
         * @param[in] function Binary AST Function Node
         **/
        FunctionInfo(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                     Rose::BinaryAnalysis::Partitioner2::Function::Ptr function) :
        funcName(function->name()),  libHash(), binaryFunction(function)
        {
             initializeHash(partitioner, function);
        }
        
        /** Constructor.
         *
         *  Combines all the information required to identify a function.  This constructor constructs the hash from the
         *  SgAsmFunction node.  Note that currently on FNV hasher is used.  This should be an option.
         *
         * @param[in] partitioner Required to get the basic blocks of the function
         * @param[in] function Binary AST Function Node
         * @param[in] libInfo  LibraryInfo that should be in the database
         **/
        FunctionInfo(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner, Rose::BinaryAnalysis::Partitioner2::Function::Ptr function, LibraryInfo& libInfo) :
        funcName(function->name()),  libHash(libInfo.libHash), binaryFunction(function)
        {
             initializeHash(partitioner, function);
        }

        /** Constructor.
         *
         *  Only takes the hash. (Never used?  Nomrmally there is a binaryFunction availible as well.)  Rest to be filled in
         *  from matching in the database.
         *
         * @param[in] funcHash  Unique Hash of the function
         **/
        FunctionInfo(const std::string& ifuncHash) :
        funcName(""), funcHash(ifuncHash), libHash(""), binaryFunction() {};

        /** Constructor.
         *
         *  Only takes the hash and the binaryFunction.  Rest to be filled in from matching in the database.
         *
         * @param[in] funcHash  Unique Hash of the function to add. Please use Fowler-Noll-Vo HasherFnv class in Combinatorics.h
         * @param[in] binaryFunction  from the partitioner
         **/
        FunctionInfo(const std::string& ifuncHash, Rose::BinaryAnalysis::Partitioner2::Function::Ptr function) :
        funcHash(ifuncHash), libHash(""), binaryFunction(function) {
            if(function != 0) {
                funcName = function->name();    
            }
        }; 

        /** True if the first hash is less than the second hash. */
        friend bool operator<(const FunctionInfo& lhs, const FunctionInfo& rhs)
        {
            return lhs.funcHash < rhs.funcHash;
        }

        /** True if the name, function hash, and library hash are equal. */
        bool operator==(const FunctionInfo& rhs)
        {
            if(funcName == rhs.funcName &&
               funcHash == rhs.funcHash &&
               libHash == rhs.libHash) 
                {
                    return true;
                }
            return false;
            
        }

        /** The name of the function. */
        std::string funcName;

        /** A hash that should uniquely identify the function. */
        std::string funcHash;

        /** Hash uniquely identifying the library to which the function belongs. */
        std::string libHash;

        /** Optional pointer to the function definition.
         *
         *  Null if the function definition is not available. */
        Rose::BinaryAnalysis::Partitioner2::Function::Ptr binaryFunction;

        /** Compute the hash for the specified function. */
        static std::string getHash(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner,
                                   Rose::BinaryAnalysis::Partitioner2::Function::Ptr function)
        {
            boost::shared_ptr<Rose::Combinatorics::Hasher> hasher =
                Rose::Combinatorics::Hasher::HasherFactory::Instance().createHasher("SHA256");
            Rose::BinaryAnalysis::AstHash astHash(hasher);            

            const std::set<rose_addr_t>& basicBlocks = function->basicBlockAddresses();
            
            for(std::set<rose_addr_t>::const_iterator bbIt = basicBlocks.begin(); bbIt != basicBlocks.end(); ++bbIt) 
                {
                    Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr bb = partitioner.basicBlockExists(*bbIt);
                    ROSE_ASSERT(bb != NULL);
                    astHash.appendBasicBlock(bb);
                    
                }

            return hasher->toString();
        }

    private:
        void initializeHash(const Rose::BinaryAnalysis::Partitioner2::Partitioner& partitioner, Rose::BinaryAnalysis::Partitioner2::Function::Ptr function) 
        {   //Ordered set, so it should always be the same order...
            funcHash = getHash(partitioner, function);
            
        }
    };
}

#endif
#endif
