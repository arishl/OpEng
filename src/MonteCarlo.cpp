#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <cmath>
#include "Option.hpp"
#include <Eigen/Dense>
#include "MonteCarlo.hpp"

using Size = std::size_t;
using PathMatrix = std::vector<std::vector<double>>;
using StepCount = Size;
using IterationCount = Size;
using ThreadList = std::vector<std::thread>;

using EigenMatrix = Eigen::MatrixXd;
using EigenVector = Eigen::VectorXd;

void MonteCarlo::SimulatePathChunk(PathMatrix& paths, const Size start, const Size end,
                                  const Option& option, const StepCount steps, const unsigned int seed)
{
    std::mt19937_64 generator(seed);
    std::normal_distribution<double> distribution(0.0, 1.0);
    const double dt { option.GetExpire() / static_cast<double>(steps) };
    for (Size i = start; i < end; ++i)
    {
        paths[0][i] = option.GetInitialPrice();
        for (StepCount t = 1; t <= steps; ++t)
        {
            const double z = distribution(generator);
            paths[t][i] = paths[t - 1][i] * std::exp((option.GetRiskFreeRate() - 0.5 * option.GetSigma() * option.GetSigma()) * dt
                            + option.GetSigma() * std::sqrt(dt) * z);
        }
    }
}

double MonteCarlo::ParallelAverage(const std::vector<double>& data, const int num_threads)
{
    ThreadList threads;
    std::vector<double> partial_sums(num_threads, 0.0);
    const Size chunk_size = data.size() / num_threads;

    auto sum_chunk = [&data, &partial_sums, chunk_size, num_threads](const int tid)
    {
        const Size start = tid * chunk_size;
        const Size end = (tid == num_threads - 1) ? data.size() : (tid + 1) * chunk_size;
        for (Size i = start; i < end; ++i)
        {
            partial_sums[tid] += data[i];
        }
    };

    for (int t = 0; t < num_threads; ++t)
        threads.emplace_back(sum_chunk, t);

    for (auto& thread : threads)
        thread.join();

    double total = 0.0;
    for (const double partial_sum : partial_sums)
        total += partial_sum;

    return total / static_cast<double>(data.size());
}

double MonteCarlo::LSMPricer(const Option& option, const StepCount steps,
                             const IterationCount iterations, const int num_threads)
{
    const bool is_call = (option.GetOptionType() == OptionType::Call);
    const double dt = option.GetExpire() / static_cast<double>(steps);
    const double discount = std::exp(-option.GetRiskFreeRate() * dt);
    PathMatrix paths(steps + 1, std::vector<double>(iterations));

    ThreadList threads;
    std::random_device rd;

    const Size paths_per_thread = iterations / num_threads;

    for (int t = 0; t < num_threads; ++t)
    {
        Size start = t * paths_per_thread;
        Size end = (t == num_threads - 1) ? iterations : (t + 1) * paths_per_thread;
        unsigned int seed = rd() + t;
        threads.emplace_back(SimulatePathChunk, std::ref(paths), start, end, option, steps, seed);
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    std::vector<double> cashflows(iterations);
    for (Size i = 0; i < iterations; ++i)
    {
        cashflows[i] = !is_call
            ? std::max(option.GetStrikePrice() - paths[steps][i], 0.0)
            : std::max(paths[steps][i] - option.GetStrikePrice(), 0.0);
    }

    for (StepCount t = steps - 1; t >= 1; --t)
    {
        std::vector<Size> itm_indices;
        std::vector<double> St, Ct;

        itm_indices.reserve(iterations);
        St.reserve(iterations);
        Ct.reserve(iterations);

        for (Size i = 0; i < iterations; ++i) {
            const double intrinsic = !is_call ? option.GetStrikePrice() - paths[t][i] : paths[t][i] - option.GetStrikePrice();
            if (intrinsic > 0.0) {
                itm_indices.push_back(i);
                St.push_back(paths[t][i]);
                Ct.push_back(cashflows[i] * discount);
            }
        }

        EigenMatrix X(St.size(), 3);
        EigenVector Y(St.size());
        for (Size j = 0; j < St.size(); ++j) {
            X(j, 0) = 1.0;
            X(j, 1) = St[j];
            X(j, 2) = St[j] * St[j];
            Y(j) = Ct[j];
        }

        EigenVector coeffs = X.colPivHouseholderQr().solve(Y);

        for (Size j = 0; j < itm_indices.size(); ++j)
        {
            const Size i = itm_indices[j];
            const double continuation = coeffs[0] + coeffs[1] * St[j] + coeffs[2] * St[j] * St[j];
            const double exercise = !is_call ? option.GetStrikePrice() - St[j] : St[j] - option.GetStrikePrice();

            if (exercise > continuation)
                cashflows[i] = exercise;
            else
                cashflows[i] *= discount;
        }
    }

    for (double& cashflow : cashflows)
        cashflow *= discount;

    return ParallelAverage(cashflows, num_threads) * discount;
}
