#include <Eigen/Dense>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {

int parsePositiveInt(const char* value, const std::string& name) {
    const int parsed = std::stoi(value);
    if (parsed <= 0) {
        throw std::runtime_error(name + " must be positive");
    }
    return parsed;
}

std::string envOrUnset(const char* name) {
    const char* value = std::getenv(name);
    return value ? value : "not_set";
}

}  // namespace

int main(int argc, char** argv) {
    try {
    const int N = argc > 1 ? parsePositiveInt(argv[1], "matrix_size") : 512;
    const int iters = argc > 2 ? parsePositiveInt(argv[2], "iterations") : 10;
    const char* threads_env = std::getenv("EIGEN_BENCH_THREADS");
    const int threads = argc > 3 ? parsePositiveInt(argv[3], "threads") : (threads_env ? parsePositiveInt(threads_env, "EIGEN_BENCH_THREADS") : 8);
    Eigen::setNbThreads(threads);

    Eigen::MatrixXf A = Eigen::MatrixXf::Random(N, N);
    Eigen::MatrixXf B = Eigen::MatrixXf::Random(N, N);
    Eigen::MatrixXf C(N, N);

    C.noalias() = A * B; // warmup

    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iters; ++i)
        C.noalias() = A * B;
    auto t1 = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / iters;
    std::cout << "Eigen version: " << EIGEN_WORLD_VERSION << '.' << EIGEN_MAJOR_VERSION << '.' << EIGEN_MINOR_VERSION << '\n';
    std::cout << "SimdInstructionSetsInUse(): " << Eigen::SimdInstructionSetsInUse() << '\n';
#ifdef EIGEN_HAS_OPENMP
    std::cout << "EIGEN_HAS_OPENMP: defined\n";
#else
    std::cout << "EIGEN_HAS_OPENMP: not_defined\n";
#endif
    std::cout << "matrix_size: " << N << '\n';
    std::cout << "iterations: " << iters << '\n';
    std::cout << "requested_threads: " << threads << '\n';
    std::cout << "Eigen nbThreads: " << Eigen::nbThreads() << '\n';
    std::cout << "OMP_NUM_THREADS: " << envOrUnset("OMP_NUM_THREADS") << '\n';
    std::cout << "EIGEN_BENCH_THREADS: " << envOrUnset("EIGEN_BENCH_THREADS") << '\n';
    std::cout << N << "x" << N << " matmul avg: " << ms << " ms\n";
    return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
