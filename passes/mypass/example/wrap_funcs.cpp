#include <iostream>

extern "C" void loop_start() {
    std::cout << "loop_start\n";
}

extern "C" void loop_end() {
    std::cout << "loop_end\n";
}
