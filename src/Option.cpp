#include "Option.hpp"

Option::Option(const OptionType option_type,
                const StrikePrice strike,
                const Expire expire,
                const InitialPrice initial,
                const Sigma sigma,
                const RiskFreeRate risk_free_rate)
                :_type {option_type},
                _strike {strike},
                _expire {expire},
                _initial {initial},
                _sigma {sigma},
                _risk_free_rate { risk_free_rate }

{
    if (_expire <= 0.0) 
    {
        throw std::invalid_argument("Expiration time must be greater than zero.");
    }
    if (_initial <= 0.0) 
    {
        throw std::invalid_argument("Initial price must be greater than zero.");
    }
    if (_strike <= 0.0) 
    {
        throw std::invalid_argument("Strike price must be greater than zero.");
    }
    if (_sigma < 0.0) 
    {
        throw std::invalid_argument("Volatility (sigma) must be non-negative.");
    }
    if (_risk_free_rate < 0.0)
    {
        throw std::invalid_argument("Risk-free rate must be non-negative.");
    }
}


OptionType Option::GetOptionType() const
{
    return _type;
}

InitialPrice Option::GetInitialPrice() const
{
    return _initial;
}

StrikePrice Option::GetStrikePrice() const
{
    return _strike;
}

Expire Option::GetExpire() const
{
    return _expire;
}

Sigma Option::GetSigma() const
{
    return _sigma;
}
RiskFreeRate Option::GetRiskFreeRate () const
{
    return _risk_free_rate;
}
