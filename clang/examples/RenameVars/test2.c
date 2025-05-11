int count = 5;

int sum(int a, int b) {
  static int total = 0;
  int temp = a + b;
  total += temp;

  for (int i = 0; i < count; ++i) {
    int temp = i * 2;
    total += temp;
  }

  return total;
}
