#include "rose.h"

#ifdef ROSEQT_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>


#include <cassert>

#include "PTracer.h"


using namespace std;


namespace InstructionCountAnnotator  {


const int long_size = sizeof(long);


PTracer::PTracer()
    : curPid(0)
{
}


void PTracer::loadProcess(const std::vector<std::string>  & args)
{
    int wait_val;

    cout << "loadProcess: ";
    for(size_t i=0; i<args.size(); i++)
        cout <<args[i] << " ";
    cout << endl;

    switch (curPid = fork())
    {
        case -1:
                perror("fork");
                return;
        case 0: // child process

                 // must be called in order to allow the
                 // control over the child process
                ptrace(PTRACE_TRACEME, 0, 0, 0);

                // does the actual exec
                execProcess(args);
                //exec never returns
                assert(false);

        default: //parent

                // wait for child calling PTRACE_TRACEME
                wait(&wait_val);
     }
}


void PTracer::run()
{
    if( breakPoints.size() == 0)
        cerr << "Warning: Run called without breakpoints set";


    // Process is already loaded and breakpoints are set
    // calling PTRACE_CONT for running
    int res = ptrace( PTRACE_CONT, curPid, NULL, NULL );
    if(res== -1)
    {
        perror("run PTRACE_CONT");
        return;
    }


    int wait_val;
    while( 1 )
    {
        wait( &wait_val );

        if (WIFSTOPPED( wait_val ))
        {
            int sig = WSTOPSIG(wait_val);
            if(sig != SIGTRAP) //not a sigtrap
            {
                cerr << "Unexpected signal " << dec << sig <<
                        " hit  at "<< hex << getInstructionPointer() << endl;

                printMem(getInstructionPointer()-8, 16 );

                if (ptrace(PTRACE_CONT, curPid, 0, 0) != 0 )
                {
                    perror("atBreakpointHit PTRACE_CONT");
                    errno = 0;
                    exit(1);
                }

                continue;
            }

            atBreakpointHit();
        }
        else if (WIFEXITED( wait_val ))
        {
            cerr << "Traced process terminated normally" << endl;
            return;
        }
        else
            cerr << "Received unexpected Signal " << wait_val << endl;
    }
}


bool PTracer::addStatementForTracing(SgAsmStatement * node)
{
    if(node==NULL)
        return false;

    long addr = node->get_address();

    if(breakPoints.find(addr) != breakPoints.end() )
    {
        cerr << "Breakpoint at " << hex << addr << " already inserted" << endl;
        return false;
    }


    BreakpointInfo info;
    info.node = node;

    // Backup the data which are currently at the breakpoint address
    assert(curPid > 0);
    errno=0;
    info.backupData =  ptrace( PTRACE_PEEKTEXT, curPid, (void *)addr, NULL );
    if(errno != 0)
    {
        perror("addStatementForTracing ptrace PEEKTEXT");
        cerr << "Insertion of breakpoint " << hex << addr << " failed"<<endl;
        return false;
    }

    enableBreakpoint(info);

    breakPoints.insert(make_pair(addr,info));

    return true;
}


void PTracer::printMem(long startAddr, int byteCount)
{
    union u
    {
            long val;
            unsigned char chars[sizeof(long)];
    }data;


    long endAddr=startAddr+byteCount;
    for(long i=startAddr;  i<endAddr; i+=4 )
    {
        data.val = ptrace( PTRACE_PEEKTEXT, curPid, (void *)i, NULL );

        //cerr << hex << i  << "\t " << data.val << endl;


        for(unsigned int j=0; j< sizeof(long); j++)
            cerr << hex << i+j << setw(2) << setfill('0') << "  " << (short) data.chars[j] << endl;
    }

}




bool PTracer::atBreakpointHit()
{
    long ip = getInstructionPointer();
    ip --; // go one step back

    BpIter iter = breakPoints.find(ip);
    if(iter == breakPoints.end())
    {
        cerr << "Breakpoint at " << hex << ip << "not found" << endl;
        return false;
    }

    BreakpointInfo & info = iter->second;
    long addr = iter->first;



    assert((unsigned long) addr == info.node->get_address() );


    // disable breakpoint to continue
    disableBreakpoint(info);

    // Reset instruction pointer before breakpoint
    setInstructionPointer(ip);

    // execute a single instruction
    if (ptrace(PTRACE_SINGLESTEP, curPid, 0, 0) != 0 )
    {
        perror("atBreakpointHit PTRACE_SINGLESTEP");
        errno = 0;
        return false;
    }

    int wait_val;
    while(1)
    {
        wait( &wait_val );
        if( WIFEXITED( wait_val ))
        {
            cerr << "Process exited after PTRACE_SINGLESTEP";
            return false;
        }

        int sig = WSTOPSIG(wait_val);
        if(sig != SIGTRAP) //not a sigtrap
        {
            cerr << "Unexpected signal " << dec << sig << " after PTRACE_SINGLESTEP";
            if (ptrace(PTRACE_CONT, curPid, 0, 0) != 0 )
            {
                perror("atBreakpointHit PTRACE_CONT");
                errno = 0;
                return false;
            }
        }
        else
            break;
    }


    // write back the breakpoint
    enableBreakpoint(info);

    info.hitCount++;

    //continue executing
    if (ptrace(PTRACE_CONT, curPid, 0, 0) != 0 )
    {
        perror("atBreakpointHit PTRACE_CONT");
        errno = 0;
        return false;
    }


    return true;
}

bool PTracer::enableBreakpoint(BreakpointInfo & info)
{
    const unsigned char TRAP_CODE = 0xcc;


    // Write the Trap instruction at the breakpoint address
    assert(curPid > 0);
    long addr = info.node->get_address();


    long data = info.backupData;

    if(data==0)
    {
        cerr << "BackupData is 0. Null-Instruction?!: " << hex << addr << endl;
    }

    data = (data & ~0xff) | TRAP_CODE;
    int res = ptrace( PTRACE_POKETEXT, curPid, (void *)addr, data );
    if(res== -1 || errno != 0)
    {
        perror("ptrace POKETEXT when setting TrapCode");
        return false;
    }

    info.active = true;
    return true;
}

bool PTracer::disableBreakpoint(BreakpointInfo & info)
{
    assert(curPid > 0);

    long addr = info.node->get_address();

    int res = ptrace( PTRACE_POKETEXT, curPid, (void *)addr, info.backupData );
    if(res == -1)
    {
        perror("Disable Breakpoint, PTRACE_POKETEXT");
        errno = 0;
        return false;
    }

    info.active = false;
    return true;
}



void PTracer::execProcess(const vector<string> & args)
{
    int numArgs = args.size();

    char ** argv = new char* [numArgs+1];

    for(int i=0; i < numArgs; i++)
    {
        int size = args[i].size()+1;
        argv[i] = new char [ size ];
        strncpy(argv[i],args[i].c_str(),size);

        assert(strlen(argv[i]) == strlen(args[i].c_str()));
    }

    argv[numArgs]=NULL;
    execv(argv[0], argv);
    perror("execv");
    //execv never returns
    assert(false);
}



long PTracer::getInstructionPointer()
{
#ifdef __x86_64
    return getRegisters().rip;
#elif defined __i386
    return getRegisters().eip;
#else
    #error "Architecture not supported";
#endif
}

void PTracer::setInstructionPointer(long addr)
{
    Registers r = getRegisters();

#ifdef __x86_64
    r.rip=addr;
#elif defined __i386
    r.eip=addr;
#else
    #error "Architecture not supported";
#endif

    setRegisters(r);
}


Registers PTracer::getRegisters()
{
    Registers result;
    int res = ptrace(PTRACE_GETREGS, curPid, NULL, &result);
    if(res == -1)
    {
        perror("getregister");
        errno=0;
    }
    return result;
}

void PTracer::setRegisters(const Registers & data)
{
    int res=ptrace(PTRACE_SETREGS, curPid,NULL, &data);
    if(res== -1)
    {
        perror("setregister");
        errno=0;
    }
}



} //namespace


#ifdef DBG_WITHOUT_ROSE

int main()
{
    InstructionCountAnnotator::PTracer t;

    SgAsmStatement bpInLoop(0x0804870e);

    SgAsmStatement beginning(4196680);
    SgAsmStatement notWorking(0x8048656);

    vector<string> args;
    args.push_back("/export/0/tmp.bauer25/opt/newRoseTree/ROSE/projects/roseQt/demo/inputTest");

    t.loadProcess(args);

    SgAsmStatement * bp = &beginning;
    cerr << "Before Breakpoint set: " << endl;
    t.printMem(bp->get_address() -5,10);
    t.addStatementForTracing(bp);
    cerr << endl << "After Breakpoint set: " << endl;
    t.printMem(bp->get_address()-5,10);


    t.run();

    return 0;
}

#endif


#endif

