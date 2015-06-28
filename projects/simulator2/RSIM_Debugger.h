#ifndef ROSE_RSIM_Debugger_H
#define ROSE_RSIM_Debugger_H

namespace RSIM_Debugger {

/** Attach an interactive debugger to the simulator.  This must be done before the process is loaded. */
void attach(RSIM_Simulator &simulator, std::istream &in=std::cin, std::ostream &out=std::cout);

} // namespace

#endif
