#include "common.h"
#include <thread>

int main() {
    // Simulator::init("cross/cross");
    // Simulator::init("fractal/sierpinski_triangle");
    // Simulator::init("bin/bincount");
    Simulator::init("infinite_cross/cross");
    // Simulator::init("longline/test");

    // Create threads for the frontend and simulator
    std::thread frontend_thread(Frontend::run);
    std::thread simulator_thread(Simulator::run_serial);

    // Wait for both threads to finish
    frontend_thread.join();
    simulator_thread.join();

    return 0;
}