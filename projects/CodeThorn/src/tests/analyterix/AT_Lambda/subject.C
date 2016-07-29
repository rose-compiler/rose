int main() {
  [&](){ }; // does not work: produces [=&]{ };
  [=]{ }; // works
}

