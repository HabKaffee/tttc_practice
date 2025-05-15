int foo(int a, int b, int c) {
    double value = 0.0;
    return a + b;
}

// Result
//
// int foo(int a, int b, [[maybe_unused]] int c) {
//     [[maybe_unused]] double value = 0.0;
//     return a + b;
// }