#ifndef _SCENARIO_UTILS_HPP_
#define _SCENARIO_UTILS_HPP_
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_map>

template<typename T>
static void print_vector(const T& t) {
    std::cout << "{ ";
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, " "));
    std::cout << "}" << std::endl;
}

template<typename K, typename V>
void print_map(std::unordered_map<K, V> const &m) {
    std::cout << "{" << std::endl;
    for (auto const &pair: m) {
        std::cout << pair.first << ": " << pair.second << "\n";
    }
    std::cout << "}" << std::endl;
}

inline bool floatCompare(float x, float y) {
    const float epsilon = 1e-5;
    return fabs(x - y) < epsilon;
}

#endif