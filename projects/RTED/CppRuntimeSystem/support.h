#ifndef RTEDCPPSUPPORT_H
#define RTEDCPPSUPPORT_H

#ifdef ROSE_WITH_ROSEQT
#include "DebuggerQt/RtedDebug.h"
#endif

/**********************************************************
 *  Convert to string
 *********************************************************/
  template<typename T>
  std::string ToString(T t){
  std::ostringstream myStream; //creates an ostringstream object
  myStream << t << std::flush;
  return myStream.str(); //returns the string form of the stringstream object
 }


void
assertme(bool value, std::string text, std::string left, std::string right) {
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
    cerr << "Error::: " << text << endl;
    rs->setCodePosition(curpos);
  }
}



#endif
