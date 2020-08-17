#include <coroutine>
#include <iostream>
#include <stdexcept>
#include <thread>

auto switch_to_new_thread(std::jthread& out) {
  struct awaitable {
    std::jthread* p_out;
    bool await_ready() { return false; }
    void await_suspend(std::coroutine_handle<> h) {
      std::jthread& out = *p_out;
      if (out.joinable())
        throw std::runtime_error("Output jthread parameter not empty");
      out = std::jthread([h] { h.resume(); });
      // Potential undefined behavior: accessing potentially destroyed *this
      // std::cout << "New thread ID: " << p_out->get_id() << "\n";
      std::cout << "New thread ID: " << out.get_id() << "\n"; // this is OK
    }
    void await_resume() {}
  };
  return awaitable{&out};
}

#if 1
// DQ (7/26/2020): "task<>" is not yet supported.
void resuming_on_new_thread(std::jthread& out) {};
#else
task<> resuming_on_new_thread(std::jthread& out) {
  std::cout << "Coroutine started on thread: " << std::this_thread::get_id() << "\n";
  co_await switch_to_new_thread(out);
  // awaiter destroyed here
  std::cout << "Coroutine resumed on thread: " << std::this_thread::get_id() << "\n";
}
#endif

int main() {
  std::jthread out;
  resuming_on_new_thread(out);
}

