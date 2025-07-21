#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP
 
#include "Option.hpp"
#include <vector>
using Matrix = std::vector<std::vector<double>>;
class MonteCarlo 
{
public:

    static void SimulatePathChunk(Matrix &paths, int start, int end, const Option &option, int steps, unsigned int seed);

    static double ParallelAverage(const std::vector<double> &data, int num_threads);

    static double LSMPricer(const Option &option, int steps, int iterations, int num_threads);

private:
};

#endif // MONTECARLO_HPP