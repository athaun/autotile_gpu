#include "common.h"

int main() {
    Simulator::init("fractal/sierpinksi_triangle");
    Simulator::run_serial();

    return 0;
}
