#ifndef _SCENARIO_ACTION_HPP_
#define _SCENARIO_ACTION_HPP_
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <vector>
#include <string>
#include <utility>
#include "SVar.hpp"

class SAction {
public:
    SAction(std::string name, float cost, float probSuccess, std::vector<Assignment> preconditions)
        : name_(name)
        , cost_(cost)
        , probSuccess_(probSuccess)
        , preconditions_(preconditions){
    };

    virtual ~SAction() = default;

    std::string name_;

    float cost_;

    float probSuccess_;

    // onFail first contains state transitions
    // onFail second contains observations
    std::pair<std::vector<Assignment>, std::vector<Assignment>> onFail_;

    std::pair<std::vector<Assignment>, std::vector<Assignment>> onSuccess_;

    std::vector<Assignment> preconditions_;

    void setFailEffects(std::vector<Assignment> onFailState, std::vector<Assignment> onFailObs) {
        onFail_ = std::make_pair(onFailState, onFailObs);
    }

    void setSuccessEffects(std::vector<Assignment> onSuccessState, std::vector<Assignment> onSuccessObs) {
        onSuccess_ = std::make_pair(onSuccessState, onSuccessObs);
    }

    std::unordered_set<std::string> getAffectedSet() {
        std::unordered_set<std::string> affectedSet;
        auto addStateFromAssignment = [&affectedSet](Assignment a){
            affectedSet.insert(a.vname_);
        };
        std::for_each(onFail_.first.begin(), onFail_.first.end(), addStateFromAssignment);
        std::for_each(onFail_.second.begin(), onFail_.second.end(), addStateFromAssignment);
        std::for_each(onSuccess_.first.begin(), onSuccess_.first.end(), addStateFromAssignment);
        std::for_each(onSuccess_.second.begin(), onSuccess_.second.end(), addStateFromAssignment);
        return affectedSet;
    }

    friend std::ostream &operator<<(std::ostream &os, SAction const &a) {
        os << a.name_ << std::endl;
        os << "onFailState: " << std::endl;
        print_vector(a.onFail_.first);
        os << "onFailObs: " << std::endl;
        print_vector(a.onFail_.second);
        os << "onSuccessState: " << std::endl;
        print_vector(a.onSuccess_.first);
        os << "onSuccessObs: " << std::endl;
        print_vector(a.onSuccess_.second);
        os << "preconditions: " << std::endl;
        print_vector(a.preconditions_);
        return os;
    }

};

#endif