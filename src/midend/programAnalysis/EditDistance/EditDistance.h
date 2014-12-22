#ifndef ROSE_EditDistance_H
#define ROSE_EditDistance_H

namespace rose {

/** Edit distance algorithms and analyses. */
namespace EditDistance {

/** Diagnostic facility for edit distances. */
extern Sawyer::Message::Facility mlog;

/** Initialize diagnostics.  Called from rose::Diagnostics::initialize. */
void initDiagnostics();

} // namespace
} // namespace

#endif
