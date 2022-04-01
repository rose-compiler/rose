enum Options {
  OPTION_1,
  OPTION_2,
  END_OF_OPTIONS
};
int optionArray[END_OF_OPTIONS];

void setOptionTrue(enum Options inOpt) {
   optionArray[inOpt] = 1;
}

int main() {
  setOptionTrue(OPTION_2);
  return 0;
}
