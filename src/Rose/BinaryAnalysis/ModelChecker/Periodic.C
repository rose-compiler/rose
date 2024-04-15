#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/Periodic.h>

#include <Rose/BinaryAnalysis/ModelChecker/Engine.h>

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

Periodic::~Periodic() {}

void
Periodic::start(const Engine::Ptr &mc, std::chrono::duration<double> period) {
    worker_ = std::thread(&Periodic::run, this, mc, period);
}

void
Periodic::stop() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        stopping_ = true;
    }
    cv_.notify_all();
    worker_.join();
}

void
Periodic::run(const Engine::Ptr &mc, std::chrono::duration<double> period) {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!stopping_) {
        cv_.wait_for(lock, period);
        if (!stopping_)
            step(mc);
    }
}

} // namespace
} // namespace
} // namespace

#endif
