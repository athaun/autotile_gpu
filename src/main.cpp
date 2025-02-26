#include "common.h"

int main() {
    // Simulator::init("cross/cross");
    Simulator::init("fractal/sierpinski_triangle");
    Simulator::run_serial();

    return 0;
}
