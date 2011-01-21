#ifndef RTEDCPPSUPPORT_H
#define RTEDCPPSUPPORT_H

#ifdef ROSE_WITH_ROSEQT
#include "DebuggerQt/RtedDebug.h"
#endif
#include "CppRuntimeSystem.h"

void
assertme(bool value, const std::string& text, const std::string&, const std::string&) {
  // print error
  if (value==false) {
    SourcePosition curpos;
    RuntimeSystem* rs = RuntimeSystem::instance();
    curpos= rs->getCodePosition();
#ifdef ROSE_WITH_ROSEQT
    if( rs -> isQtDebuggerEnabled() ) {
        RtedDebug* rd = RtedDebug::instance();
        std::string textme = "Error::: "+text+"  Left:" +left + "  Right:" +right;
        rd->addMessage(textme,RtedDebug::ERROR);
        rd->startGui();
    }
#endif
    std::cerr << "Error::: " << text << std::endl;
    rs->setCodePosition(curpos);
  }
}



#endif
