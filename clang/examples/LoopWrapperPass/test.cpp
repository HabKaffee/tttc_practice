void loop_start();
void loop_end();

int main() {
  int sum = 0;
  for (int i = 0; i < 10; ++i) {
    sum += i;
  }

  int i = 0;
  while (i < 5) {
    sum += i;
    ++i;
  }

  return sum;
}
