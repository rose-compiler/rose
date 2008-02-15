// cc.in47
// undefined function name in a template definition

unsigned int getRandom(int low, int high);

template <class T>
void Shuffle(T *array, unsigned int size)
{
  // the function 'swap' isn't declared
  while (--size) {
    swap(array[size], array[getRandom(0, size)]);
  }
}
