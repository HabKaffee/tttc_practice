#include <iostream>

void test_loop(int n) {
    for (int i = 0; i < n; ++i) {
        std::cout << i << "\n";
    }
}

int main() {
    test_loop(5);
    return 0;
}
