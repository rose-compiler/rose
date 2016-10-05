
#if 0
Here is the lambda unparsing bug that Simon found:

Minimal example is:
int main() {
  [&](){ }; // does not work: produces [=&](){ };
  [=](){ }; // works
}

thanks,
Markus

#endif


int main() {
  [&](){ }; // does not work: produces [=&](){ };
  [=](){ }; // works
}
