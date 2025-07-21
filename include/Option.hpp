#ifndef OPTION_HPP
#define OPTION_HPP

#include <string>
#include <random>

using InitialPrice = double;
using StrikePrice = double;
using Sigma = double;
using Expire = double;
using RiskFreeRate = double;

enum class OptionType {
    Call,
    Put
};

class Option {
public:
    Option(OptionType option_type, 
            StrikePrice strike, 
            Expire expire,
            InitialPrice initial,
            Sigma sigma,
            RiskFreeRate riskFreeRate);

    [[nodiscard]] OptionType GetOptionType() const;

    [[nodiscard]] InitialPrice GetInitialPrice() const;

    [[nodiscard]] StrikePrice GetStrikePrice() const;

    [[nodiscard]] Sigma GetSigma() const;

    [[nodiscard]] RiskFreeRate GetRiskFreeRate() const;

    [[nodiscard]] Expire GetExpire() const;

private:
    OptionType _type;
    StrikePrice _strike;
    Expire _expire;
    InitialPrice _initial;
    Sigma _sigma;
    RiskFreeRate _risk_free_rate;
};

#endif // OPTION_HPP