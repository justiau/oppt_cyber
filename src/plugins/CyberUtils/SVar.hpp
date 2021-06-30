#ifndef _SCENARIO_VARIABLE_HPP_
#define _SCENARIO_VARIABLE_HPP_
#include "ScenarioUtils.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>
#include <random>
#include <chrono>

enum VAR_TYPE {
    STATE_TRANSITION,
    STATE_OBSERVATION,
    NONSTATE_OBSERVATION
};

struct Assignment {
    std::string vname_;
    std::string value_;
    VAR_TYPE type_;

    Assignment(std::string vname, std::string value, VAR_TYPE type) {
        vname_ = vname;
        value_ = value;
        type_ = type;
    }
};

std::ostream &operator<<(std::ostream &os, Assignment const &a) {
    os << "{ " << a.vname_ << ": " << a.value_ << " }";
    return os;
}

class SVar {
public:
    SVar() = default;

    SVar(std::string name, std::vector<std::string> values)
        : name_(name)
        , values_(values) {
        for (std::size_t i=0; i != values_.size(); ++i) {
            index_[values_[i]] = i;
        }
    };
    
    virtual ~SVar() = default;

    std::string name_;

    std::vector<std::string> values_;

    std::unordered_map<std::string, int> index_;

    VAR_TYPE type_ = VAR_TYPE::STATE_TRANSITION;

    float decay;

    bool fullyObs;

    std::string initValue;

    int getIndex(std::string val) {
        if (index_.find(val) == index_.end())
            return -1;
        return index_[val];
    }

    int getRandIndex() {
        unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed1);
        std::uniform_int_distribution<> distribution(0, values_.size() - 1);
        return distribution(generator);
    }

    int getValueCount() {
        return values_.size();
    }

    void setType(VAR_TYPE type) {
        type_ = type;
    }

    Assignment createAssignment(std::string val) {
        if (index_.find(val) != index_.end() || (val == "none" || val == "actual")) {
            return Assignment(name_, val, type_);
        }
        throw std::invalid_argument("Invalid value provided to createAssignment");
    }

    friend std::ostream &operator<<(std::ostream &os, SVar const &v) {
        os << v.name_ << std::endl;
        print_vector(v.values_);
        return os;
    }

private:
    // STD

};

#endif