

#include <chrono>

struct Timer {
  using time_t = std::chrono::time_point<std::chrono::system_clock>;

  time_t start_;
  time_t stop_;
  bool run = false;

  void start() {
    start_ = std::chrono::system_clock::now();
    run = true;
  }
  
  void stop() {
    stop_ = std::chrono::system_clock::now();
    run = false;
  }
    
  double time() {
    auto t = run ? std::chrono::system_clock::now() : stop_;
    auto r = std::chrono::duration_cast<std::chrono::microseconds>(t - start_).count();
    return r;
  }
};

#ifndef USE_META_TRAVERSAL
#  define USE_META_TRAVERSAL 1
#endif

#include "Rose/Traits/Traversal.h"

#if !USE_META_TRAVERSAL
struct Traversal : public AstTopDownProcessing< size_t > {
  size_t & max_depth;
  size_t & count;

  Traversal(size_t & max_depth_, size_t & count_) : max_depth(max_depth_), count(count_) {};

  size_t evaluateInheritedAttribute(SgNode * n, size_t depth) {
    if (depth > max_depth) max_depth = depth;
    count += 1;
    return depth+1;
  }
};
#endif

int main(int argc, char * argv[]) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);
  
  Timer timer;

  // Build a project
  SgProject * project = args.size() == 1 ? new SgProject() : frontend(args);

  timer.start();

  // A simple depth counting traversal
  size_t max_depth = 0;
  size_t count     = 0;

#if USE_META_TRAVERSAL
  size_t depth = 0;

  //  pre-order functor
  auto pre = [&]<typename NodeT, typename FieldT>(NodeT * node, FieldT pfld) {
    if (depth > max_depth) max_depth = depth;
    depth += 1;
    count += 1;
  };

  // post-order functor
  auto post = [&]<typename NodeT, typename FieldT>(NodeT * node, FieldT pfld) {
    depth -= 1;
  };
  Rose::Traits::traverse(project, pre, post);
#else
  Traversal traversal(max_depth, count);
  traversal.traverse(project, 0);
#endif

  timer.stop();
  
#if USE_META_TRAVERSAL
  std::cout << "Compile\t" << timer.time() << "\t" << max_depth << "\t" << count << std::endl;
#else
  std::cout << "Runtime\t" << timer.time() << "\t" << max_depth << "\t" << count << std::endl;
#endif

  return 0;
}

