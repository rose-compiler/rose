
template <class CudaFn>
__global__
void launcher(CudaFn fn) {
  fn();
}

int main() {
//  launcher<<< 1, 1>>>( [] () -> void {} );
  launcher<<< 1, 1>>>( [] __device__ () -> void {} );
  return 0;
}
