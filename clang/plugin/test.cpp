/*
Вариант 2.
Найти все переменные или параметры функций, 
которые не используются в коде, и пометить их атрибутом [[maybe_unused]]
*/

int foo(int a, int b, [[maybe_unused]] int c) {
    [[maybe_unused]] double value = 0.0;
    return a + b;
}

int main() {
    int unused_var;
    foo(1, 2, 3);
    return 0;
}