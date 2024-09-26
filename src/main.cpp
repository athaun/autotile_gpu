#include "common.h"

int main() {
    Simulator::init("input/rules_test.txt", "input/seed.txt");
    Simulator::run_serial();

    return 0;
}
