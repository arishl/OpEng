

#include "Option.hpp"
#include "MonteCarlo.hpp"

int main() {
    Option option(Variety::European, "AAPL", Type::Call, 100, 150.0, 1.0, 145.0, 0.2, 0.05);
    MonteCarlo mc(option, 10000);
    mc.RunSimulation(10000);
    mc.PrintResult();

    return 0;
}