#ifndef MONTECARLO_HPP
#define MONTECARLO_HPP
 
#include "Option.hpp"
#include <vector>
using PathMatrix = std::vector<std::vector<double>>;
using Steps = int;
using Iterations = int;
class MonteCarlo 
{
public:

    static void SimulatePathChunk(PathMatrix &paths, std::size_t start, std::size_t end, const Option &option, std::size_t steps, unsigned int seed);

    static double ParallelAverage(const std::vector<double> &data, int num_threads);

    static double LSMPricer(const Option &option, std::size_t steps, std::size_t iterations, int num_threads);

private:
};

#endif // MONTECARLO_HPP