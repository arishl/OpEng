#include "Option.hpp"

Option::Option(Variety variety, 
                Symbol symbol, 
                Type option_type, 
                Quantity quantity, 
                Strike strike, 
                Expire expire,
                Initial initial,
                Sigma sigma,
                RiskFreeRate riskFreeRate)
                : _variety {variety},
                _symbol {symbol},
                _type {option_type},
                _quantity {quantity},
                _strike {strike},
                _expire {expire},
                _initial {initial},
                _sigma {sigma},
                _riskFreeRate { riskFreeRate }

{
    gen = std::mt19937(rd());
    dist = std::normal_distribution<>(0.0, 1.0);
}

double Option::operator()() const
{
    if (_variety == Variety::American) 
    {
        throw std::runtime_error("American options are not supported in this implementation.");
    }
    double Z = dist(gen);
    double S_T = _initial * std::exp((_riskFreeRate - 0.5 * _sigma * _sigma) * _expire + _sigma * std::sqrt(_expire) * Z);
    double payoff = _type == Type::Call ? std::max(S_T - _strike, 0.0) : std::max(_strike - S_T, 0.0);
    return payoff;
}

Variety Option::GetOptionVariety()
{
    return _variety;
}

Symbol Option::GetSymbolName()
{
    return _symbol;
}

Type Option::GetOptionType()
{
    return _type;
}

Quantity Option::GetQuantity()
{
    return _quantity;
}