#ifndef bROwSE_Statistics_H
#define bROwSE_Statistics_H

#include <bROwSE/bROwSE.h>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/variance.hpp>

namespace bROwSE {

class FpStatistics {
    typedef boost::accumulators::stats<boost::accumulators::tag::mean,
                                       boost::accumulators::tag::min,
                                       boost::accumulators::tag::max,
                                       boost::accumulators::tag::variance> Features;
    typedef boost::accumulators::accumulator_set<double, Features> AccumulatorSet;
    AccumulatorSet acc_;
    double p5_, p50_, p95_;                             // 5th, 50th, and 95th percentile
    size_t count_;
public:
    FpStatistics(): count_(0) {}
    void insert(double value) { acc_(value); ++count_; }
    void computeRanks(std::vector<double>&);
    size_t count() const { return count_; }
    double minimum() const { return (boost::accumulators::min)(acc_); }  // extra parens to avoid min macro on Microsoft
    double maximum() const { return (boost::accumulators::max)(acc_); }  // extra parens to avoid max macro on Microsoft
    double mean() const { return boost::accumulators::mean(acc_); }
    double variance() const { return boost::accumulators::variance(acc_); }
    double p5() const { return p5_; }
    double p50() const { return p50_; }
    double p95() const { return p95_; }
};

} // namespace
#endif
