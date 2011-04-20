
__global__ void square_array() {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
}

int main(int argc, char ** argv) {

	dim3 threads(1, 1);
	dim3 grid(1, 1, 1);

	square_array <<< threads, grid >>> ();
}
