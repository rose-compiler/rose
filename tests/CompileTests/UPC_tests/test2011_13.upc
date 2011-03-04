// Note from Indranil Roy:
// For example
// 1. shared [THREADS] int arr1[THREADS*THREADS] would require you to specify the threads at compile time.
// 2. shared [1] int arr2[THREADS*THREADS] doesn't require you to specify the threads at compile time.

shared [1] int arr2[THREADS*THREADS]; // doesn't require you to specify the threads at compile time.
