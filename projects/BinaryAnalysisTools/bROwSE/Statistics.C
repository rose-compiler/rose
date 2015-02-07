#include <bROwSE/Statistics.h>

namespace bROwSE {

void
FpStatistics::computeRanks(std::vector<double> &dv) {
    std::sort(dv.begin(), dv.end());
    if (dv.empty()) {
        p5_ = p50_ = p95_ = NAN;
    } else {
        size_t p5idx = floor(dv.size()*0.05);
        p5_ = dv[p5idx];
        size_t p95idx = ceil(dv.size()*0.95);
        p95_ = dv[p95idx-1];
        if (dv.size() % 2) {
            p50_ = dv[dv.size()/2];
        } else {
            p50_ = (dv[dv.size()/2-1] + dv[dv.size()/2]) / 2.0;
        }
    }
}

} // namespace
