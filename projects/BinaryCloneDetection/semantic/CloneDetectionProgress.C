#include "CloneDetectionProgress.h"
#include <cmath>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <string>
#include <unistd.h>

namespace CloneDetection {

void
Progress::init()
{
    is_terminal = isatty(2);
    show();
    cur = 0;
}

void
Progress::show()
{
    ++cur;
    if ((force || is_terminal) && total>0) {
        if ((size_t)(-1)==total) {
            if (had_output)
                fputc('\n', stderr);
        } else {
            size_t n = std::min(cur, total);
            time_t now = time(NULL);
            if (now > last_report || cur==total) {
                int nchars = round((double)n/total * WIDTH);
                fprintf(stderr, " %3d%% |%-*s|%c",
                        (int)round(100.0*n/total), WIDTH, std::string(nchars, '=').c_str(),
                        is_terminal?'\r':'\n');
                fflush(stderr);
                last_report = now;
                had_output = true;
            }
        }
    }
}

void
Progress::clear()
{
    if (had_output) {
        if (is_terminal) {
            fprintf(stderr, "%*s\r", 8+WIDTH, "");
            fflush(stderr);
        }
        had_output = false;
    }
}

} // namespace
