#include "common.h"
#include <thread>

int main() {
    // Simulator::init("cross/cross");
    // Simulator::init("fractal/sierpinski_triangle");
    // Simulator::init("bin/bincount");
    // Simulator::init("test/test");
    // Simulator::init("longline/test");
    Simulator::init("fractal2/fractal");

    std::thread frontend_thread(Frontend::run);
    std::thread simulator_thread(Simulator::run_serial);

    frontend_thread.join();
    simulator_thread.join();

    return 0;
}