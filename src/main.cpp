#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include "Option.hpp"
#include "MonteCarlo.hpp"

int main(const int argc, char* argv[]) {
    std::string type_str;
    double spot, strike, rate, vol, maturity;
    int steps { 50 };
    int iterations { 100000 };
    const int threads { static_cast<int>(std::thread::hardware_concurrency()) };

    if (argc == 9) {
        type_str   = argv[1];
        spot       = std::stod(argv[2]);
        strike     = std::stod(argv[3]);
        rate       = std::stod(argv[4]);
        vol        = std::stod(argv[5]);
        maturity   = std::stod(argv[6]);
        steps      = std::stoi(argv[7]);
        iterations = std::stoi(argv[8]);
    } else {
        std::cout << "Option type (call or put): ";
        std::cin >> type_str;
        std::cout << "Spot price: ";
        std::cin >> spot;
        std::cout << "Strike price: ";
        std::cin >> strike;
        std::cout << "Risk-free rate: ";
        std::cin >> rate;
        std::cout << "Volatility: ";
        std::cin >> vol;
        std::cout << "Time to maturity (years): ";
        std::cin >> maturity;
        std::cout << "Number of time steps [default 50]: ";
        std::cin >> steps;
        std::cout << "Number of simulations [default 100000]: ";
        std::cin >> iterations;
    }

    OptionType type;
    if (type_str == "call")
        type = OptionType::Call;
    else if (type_str == "put")
        type = OptionType::Put;
    else {
        std::cerr << "Invalid option type. Use 'call' or 'put'." << std::endl;
        return 1;
    }

    Option option{
        type,
        InitialPrice{spot},
        StrikePrice{strike},
        Expire{maturity},
        Sigma{vol},
        RiskFreeRate{rate}
    };

    double price { MonteCarlo::LSMPricer(option, Steps{steps}, Iterations{iterations}, threads) };

    std::cout << "\n=== American Option Pricing ===\n";
    std::cout << "Option Type:     " << type_str << "\n";
    std::cout << "Spot Price:      " << spot << "\n";
    std::cout << "Strike Price:    " << strike << "\n";
    std::cout << "Risk-Free Rate:  " << rate << "\n";
    std::cout << "Volatility:      " << vol << "\n";
    std::cout << "Maturity:        " << maturity << " years\n";
    std::cout << "Steps:           " << steps << "\n";
    std::cout << "Simulations:     " << iterations << "\n";
    std::cout << "Threads Used:    " << threads << "\n";
    std::cout << "Option Price:    " << price << "\n";

    return 0;
}