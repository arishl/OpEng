#include <iostream>
#include <vector>
#include <random>
#include <thread>
#include <cmath>
#include "Option.hpp"
#include <Eigen/Dense>
#include "MonteCarlo.hpp"

using Matrix = std::vector<std::vector<double>>;

void MonteCarlo::SimulatePathChunk(Matrix& paths, const int start, const int end, const Option& option, const int steps, const unsigned int seed) {
    std::mt19937_64 generator(seed);
    std::normal_distribution<double> distribution(0.0, 1.0);
    const double dt = option.GetExpire() / steps;
    for (int i = start; i < end; ++i) {
        paths[0][i] = option.GetInitialPrice();
        for (int t = 1; t <= steps; ++t) {
            const double z = distribution(generator);
            paths[t][i] = paths[t - 1][i] * std::exp((option.GetRiskFreeRate() - 0.5 * option.GetSigma() * option.GetSigma()) *  + option.GetSigma() * std::sqrt(dt) * z);
        }
    }
}

double MonteCarlo::ParallelAverage(const std::vector<double>& data, const int num_threads) {
    std::vector<std::thread> threads;
    std::vector partial_sums(num_threads, 0.0);
    const int chunk_size { static_cast<int> ( data.size() / num_threads ) };

    auto sum_chunk = [&data, &partial_sums, chunk_size, num_threads](const int tid)
    {
        const int start { tid * chunk_size };
        const int end { (tid == num_threads - 1) ? static_cast<int>( data.size() ): (tid + 1) * chunk_size };
        for (int i = start; i < end; ++i)
            partial_sums[tid] += data[i];
    };

    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back(sum_chunk, t);
    }

    for (auto& t : threads)
    {
        t.join();
    }
    double total { 0.0 };
    for (const double s : partial_sums)
        total += s;

    return total / static_cast<double>( data.size() );
}

// LSM pricer supporting calls and puts
double MonteCarlo::LSMPricer(const Option& option, const int steps, const int iterations, const int num_threads) {
    const double dt = option.GetExpire() / steps;
    const double discount = std::exp(-option.GetRiskFreeRate() * dt);
    Matrix paths(steps + 1, std::vector<double>(iterations));

    // Threaded path simulation
    std::vector<std::thread> threads;
    std::random_device rd;
    const int paths_per_thread = iterations / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int start = t * paths_per_thread;
        int end = (t == num_threads - 1) ? iterations : (t + 1) * paths_per_thread;
        unsigned int seed = rd() + t; // Unique seed per thread
        threads.emplace_back(SimulatePathChunk, std::ref(paths), start, end, option, steps, seed);
    }
    for (auto& t : threads)
    {
        t.join();
    }

    std::vector<double> cashflows(iterations);
    for (int i = 0; i < iterations; ++i)
        cashflows[i] = option.GetOptionType() == OptionType::Put ? std::max(option.GetStrikePrice() - paths[steps][i], 0.0) : std::max(paths[steps][i] - option.GetStrikePrice(), 0.0);

    // Backward induction
    for (int t = steps - 1; t >= 1; --t) {
        std::vector<int> itm_indices;
        std::vector<double> St, Ct;

        for (int i = 0; i < iterations; ++i) {
            if (const double intrinsic = option.GetOptionType() == OptionType::Put ? option.GetStrikePrice() - paths[t][i] : paths[t][i] - option.GetStrikePrice(); intrinsic > 0.0) {
                itm_indices.push_back(i);
                St.push_back(paths[t][i]);
                Ct.push_back(cashflows[i] * discount);
            }
        }

        if (St.empty()) continue;

        Eigen::MatrixXd X(St.size(), 3);
        Eigen::VectorXd Y(St.size());
        for (size_t j = 0; j < St.size(); ++j) {
            X(j, 0) = 1.0;
            X(j, 1) = St[j];
            X(j, 2) = St[j] * St[j];
            Y(j) = Ct[j];
        }

        Eigen::VectorXd coeffs = X.colPivHouseholderQr().solve(Y);

        for (size_t j = 0; j < itm_indices.size(); ++j) {
            const int i = itm_indices[j];
            const double continuation = coeffs[0] + coeffs[1] * St[j] + coeffs[2] * St[j] * St[j];

            if (const double exercise = option.GetOptionType() == OptionType::Put ? option.GetStrikePrice() - St[j] : St[j] - option.GetStrikePrice(); exercise > continuation)
                cashflows[i] = exercise;
            else
                cashflows[i] *= discount;
        }
    }

    return ParallelAverage(cashflows, num_threads) * discount;
}
