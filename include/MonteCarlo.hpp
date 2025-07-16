#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP
 
#include "Option.hpp"

class MonteCarlo 
{
public:
    MonteCarlo(Option& option, int numIterations);
    void RunSimulation(int numIterations);
    void PrintResult() const;
    void clearResult();
private:
    Option& _option;
    const int _numIterations;
    double _result;
};

#endif // MONTECARLO_HPP