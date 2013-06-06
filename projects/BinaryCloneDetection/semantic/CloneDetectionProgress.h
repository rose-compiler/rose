#ifndef RSIM_CloneDetectionProgress_H
#define RSIM_CloneDetectionProgress_H

#include <stdlib.h>

namespace CloneDetection {

/** Show progress bar indicator on standard error. The progress bar's constructor should indicate the total number of times
 *  that the show() method is expected to be called.  If the standard error stream is a terminal (or force_output is set) then
 *  a progress bar is emitted at most once per RTP_INTERVAL seconds (default 1).  The cursor is positioned at the beginning
 *  of the progress bar's line each time the progress bar is printed. The destructor erases the progress bar. */
class Progress {
protected:
    size_t cur, total;
    time_t last_report;
    bool is_terminal, force, had_output;
    enum { WIDTH=100, RPT_INTERVAL=1 };
    void init();
public:
    Progress(size_t total): cur(0), total(total), last_report(0), is_terminal(false), force(false), had_output(false) { init(); }
    ~Progress() { clear(); }

    /** Force the progress bar to be emitted even if standard error is not a terminal. */
    void force_output(bool b) { force = b; }

    /** Update the progress. The bar is printed only if it's been at least RPT_INTERVAL seconds since the previous update. */
    void show();

    /** Erase the progress bar from the screen by emitting white space. */
    void clear();
};

} // namespace
#endif
