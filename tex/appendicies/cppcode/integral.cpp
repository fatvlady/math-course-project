#include <boost/math/special_functions/gamma.hpp>
#include <boost/math/constants/constants.hpp>

#include <iostream>
#include <cmath>

using namespace boost::math;

int main() {
    double step = 1e-4;
    long double integral = 0;
    for (int i = 1; i <= 1000000; i++)
        integral += std::exp(- 2 * (tgamma(1e-10, i * step)) - 2 * std::log(i * step)) * step;
    step *= 10;
    for (int i = 100001; i <= 10000000; i++)
        integral += std::exp(- 2 * (tgamma(1e-10, i * step)) - 2 * std::log(i * step)) * step;
    step *= 10;
    for (int i = 1000001; i <= 10000000; i++)
        integral += std::exp(- 2 * (tgamma(1e-10, i * step)) - 2 * std::log(i * step)) * step;
    std::cout << integral / std::exp(2 * constants::euler<double>()) << std::endl;
    return 0;
}
