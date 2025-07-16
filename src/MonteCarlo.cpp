#include <iostream>
#include "MonteCarlo.hpp"

MonteCarlo::MonteCarlo(Option& option, int numIterations)
    : _option(option), _numIterations(numIterations)    
{

}

void MonteCarlo::RunSimulation(int numIterations)
{
    clearResult();
    for (int i = 0; i < numIterations; ++i) {
        _result += _option();
    }
    _result /= numIterations;
}

void MonteCarlo::clearResult()
{
    _result = 0.0;
}

void MonteCarlo::PrintResult() const
{
    std::cout << ", Result: " << _result << std::endl;
}