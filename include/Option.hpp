#ifndef OPTION_HPP
#define OPTION_HPP

#include <string>
#include <random>

using Symbol = std::string;
using Quantity = int;
using Initial = double;
using Strike = double;
using Sigma = double;
using Expire = double;
using RiskFreeRate = double;

enum class Type {
    Call,
    Put
};

enum class Variety {
    American,
    European
};

class Option {
public:
    Option(Variety variety, 
            Symbol symbol, 
            Type option_type, 
            Quantity quantity, 
            Strike strike, 
            Expire expire,
            Initial initial,
            Sigma sigma,
            RiskFreeRate riskFreeRate);
    double operator()() const;
    Symbol GetSymbolName();
    Variety GetOptionVariety();
    Type GetOptionType();
    Quantity GetQuantity();
    Strike GetStrike();
    Expire GetExpire();
private:
    Variety _variety;
    Symbol _symbol;
    Type _type;
    Quantity _quantity;
    Strike _strike;
    Expire _expire;
    Initial _initial;
    Sigma _sigma;
    RiskFreeRate _riskFreeRate;
    std::random_device rd;
    mutable std::mt19937 gen;
    mutable std::normal_distribution<double> dist;
};

#endif // OPTION_HPP