
__global__ void square_array() {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
}

int main(int argc, char ** argv) {
	square_array <<< 1,1 >>> ();
}
