#ifndef _SCENARIO_UTILS_HPP_
#define _SCENARIO_UTILS_HPP_
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

template<typename T>
static void print_vector(const T& t) {
    std::cout << "{ ";
    std::copy(t.cbegin(), t.cend(), std::ostream_iterator<typename T::value_type>(std::cout, " "));
    std::cout << "}" << std::endl;
}
vs
template<typename K, typename V>
void print_map(std::map<K, V> const &m) {
    std::cout << "{" << std::endl;
    for (auto const &pair: m) {
        std::cout << pair.first << ": " << pair.second << "\n";
    }
    std::cout << "}" << std::endl;
}

template<typename K, typename V>
static std::vector<V> get_map_values(std::map<K, V> const &m) {
    std::vector<V> values;
    for (auto const &pair : m) {
        values.push_back(pair.second);
    }
    return values;
}

#endif