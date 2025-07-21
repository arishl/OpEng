#include <iostream>
#include <thread>
#include "Option.hpp"
#include "MonteCarlo.hpp"

int main() {
    Option option {OptionType::Put, InitialPrice {100.00}, StrikePrice {103.0}, Expire{ 1.0 }
                    ,Sigma { 0.2 }, RiskFreeRate { 0.05 } };
    double call_price = MonteCarlo::LSMPricer(option, 50, 100, std::thread::hardware_concurrency());
    std::cout << "American Call Option Price (LSM): " << call_price << std::endl;
    return 0;
}