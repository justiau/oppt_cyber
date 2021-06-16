#ifndef _SCENARIO_ACTION_HPP_
#define _SCENARIO_ACTION_HPP_
#include <unordered_map>
#include <vector>
#include <string>
#include "Variable.hpp"

class Action {
public:
    Action(std::string name, std::vector<Assignment> onFail, std::vector<Assignment> onSuccess, std::vector<Assignment> preconditions, float cost, float probSuccess)
        : name_(name)
        , onFail_(onFail)
        , onSuccess_(onSuccess)
        , preconditions_(preconditions)
        , cost_(cost)
        , probSuccess_(probSuccess){
    };

    virtual ~Action() = default;

    std::string name_;

    float cost_;

    float probSuccess_;

    std::vector<Assignment> onFail_;

    std::vector<Assignment> onSuccess_;

    std::vector<Assignment> preconditions_;

    friend std::ostream &operator<<(std::ostream &os, Action const &a) {
        os << a.name_ << std::endl;
        os << "onFail: " << std::endl;
        print_vector(a.onFail_);
        os << "onSuccess: " << std::endl;
        print_vector(a.onSuccess_);
        os << "preconditions: " << std::endl;
        print_vector(a.preconditions_);
        return os;
    }

};

#endif