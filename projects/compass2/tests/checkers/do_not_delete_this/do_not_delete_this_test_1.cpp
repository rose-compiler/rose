// 1 Matches

class dont {
public:
  void del();
};

void dont::del() {
  delete this;
}

int main() {
  dont *t = new dont;
  t->del();
  return 0;
}
