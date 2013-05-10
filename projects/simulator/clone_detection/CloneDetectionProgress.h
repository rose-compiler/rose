#ifndef RSIM_CloneDetectionProgress_H
#define RSIM_CloneDetectionProgress_H

#include <stdlib.h>

namespace CloneDetection {

class Progress {
protected:
    size_t cur, total;
    time_t last_report;
    bool is_terminal, had_output;
    enum { WIDTH=100, RPT_INTERVAL=1 };
    void init();
public:
    Progress(size_t total): cur(0), total(total), last_report(0), is_terminal(false), had_output(false) { init(); }
    ~Progress() { clear(); }
    void show();
    void clear();
};

} // namespace
#endif
