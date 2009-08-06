
#ifndef PTRACER_H
#define PTRACER_H

#include <vector>
#include <string>
#include <map>


typedef long address;
typedef char byte;

typedef struct user_regs_struct Registers;



#ifdef DBG_WITHOUT_ROSE
class SgAsmStatement
{
    public:
        SgAsmStatement(long a) : address(a) {}
        long get_address() { return address; }
    protected:
        long address;
};
#else
    class SgAsmStatement;
#endif


namespace InstructionCountAnnotator  {

/**
 * \brief Execution Count annotation based on \c ptrace()
 *
 *  This module is only built when ROSEQT_EXPERIMENTAL is set, because it's UNIX specific (problems when building on MAC)
 *  If you want to use this functionality in a architecture independent way use the IntelPIN mechanism: InstructionCountAnnotator::annotate()
 *
 *  This Module operates on a Binary AST - it traverses all SgAsmInstructions, and inserts a breakpoint at the corresponding addresses
 *  Therefore the ptrace() system-call is used. Then the binary is executed and the execution count for each instruction is logged and annotated
 */
class PTracer
{

    public:


        class BreakpointInfo
        {
            friend class PTracer;
            public:
                size_t getHitCount()       const { return hitCount; }
                SgAsmStatement * getNode() const { return node; }

            private:
                BreakpointInfo()  : node(NULL), hitCount(0),backupData(0),active(false)  {}

                //long address;         // no address field -> is stored as key of map
                SgAsmStatement * node;  // the associated instruction node
                size_t hitCount;        // how often this breakpoint was hit
                long backupData;        // the data which has been overwritten by int3
                bool active;            // when true, int3 has been placed in childprocess
        };



    public:
        PTracer();


        // Starts the process which gets monitored
        // @param args  first entry is path to executable
        //              subsequent entries are arguments for this process
        void loadProcess(const std::vector<std::string> & args);



        bool addStatementForTracing(SgAsmStatement * node);


        void run();

        const std::map<address,BreakpointInfo> & getBreakpointInfoMap() const { return breakPoints; }

        typedef std::map<address,BreakpointInfo>::iterator BpIter;
        typedef std::map<address,BreakpointInfo>::const_iterator const_BpIter;

        void printMem(long startAddr, int byteCount);


    protected:

        void insertBreakpoint(address position);
        void removeBreakpoint(address position);



        /// calls exec (does not return),
        /// in args[0] the path of the executable file is expected
        void execProcess(const std::vector<std::string> & args);

        /// Handler function for breakpoint hits
        bool atBreakpointHit();


        /// Enables breakpoint (writes int3 at the right position)
        bool enableBreakpoint (BreakpointInfo & info);

        /// Disables breakpoint (removes int3) but does not delete info structure
        bool disableBreakpoint(BreakpointInfo & info);


        long getInstructionPointer();
        void setInstructionPointer(long addr);

        Registers getRegisters();
        void setRegisters(const Registers & data);



        // for every breakpoint the address and additional info is stored
        std::map<address,BreakpointInfo> breakPoints;


        /// pid of the child process
        pid_t curPid;
};


}//namespace

#endif
