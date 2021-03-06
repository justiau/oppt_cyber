#ifndef _SCENARIO_VARIABLE_HPP_
#define _SCENARIO_VARIABLE_HPP_
#include "ScenarioUtils.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>
#include <random>
#include <chrono>

std::unordered_set<std::string> STATEOBS_TYPES({"none", "actual", "noisy"});

enum VAR_TYPE {
    STATE_TRANSITION,
    STATE_OBSERVATION,
    NONSTATE_OBSERVATION
};

struct Assignment {
    std::string vname_;
    std::string value_;
    VAR_TYPE type_;
    bool optional_;

    Assignment(std::string vname, std::string value, VAR_TYPE type, bool optional) {
        vname_ = vname;
        value_ = value;
        type_ = type;
        optional_ = optional;
    }
};

std::ostream &operator<<(std::ostream &os, Assignment const &a) {
    os << "{ " << a.vname_ << ": " << a.value_ << " }";
    return os;
}

struct Terminal {
    std::string tname_;
    std::vector<Assignment> conditions_;
    float reward_;

    Terminal(std::string tname, std::vector<Assignment> conditions, float reward) :
        tname_(tname),
        conditions_(conditions),
        reward_(reward) {
    }
};

std::ostream &operator<<(std::ostream &os, Terminal const &t) {
    os << "{ " << t.tname_ << ": " << t.reward_ << " } ";
    os << "{ " << std::endl;
    for (auto c : t.conditions_) {
        os << c << " ";
    }
    os << "}" << std::endl;
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

    float decay = 0;

    bool learnDecay = false;

    int learnDecayMulti = -1;

    bool fullyObs = false;

    bool oneWayDecay = false;

    std::string initValue;

    int getIndex(std::string val) {
        if (index_.find(val) != index_.end())
            return index_[val];
        throw std::invalid_argument("Value name not found in state variable");
    }

    std::string getValue(int index) {
        if (index > -1 && index < values_.size()) {
            return values_[index];
        }
        throw std::invalid_argument("Index out of range for SVar get value");
    }

    bool checkValue(std::string val) {
        return index_.find(val) != index_.end();
    }

    int getRandIndex() {
        unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed1);
        std::uniform_int_distribution<> distribution(0, values_.size() - 1);
        int d = distribution(generator);
        return d;
    }

    int getValueCount() {
        return values_.size();
    }

    std::vector<std::string> getValues() {
        return values_;
    }

    void setType(VAR_TYPE type) {
        type_ = type;
    }

    Assignment createAssignment(std::string val, bool optional=false) {
        if (index_.find(val) != index_.end() || STATEOBS_TYPES.find(val) != STATEOBS_TYPES.end()) {
            return Assignment(name_, val, type_, optional);
        }
        throw std::invalid_argument("Invalid value provided to createAssignment");
    }

    friend std::ostream &operator<<(std::ostream &os, SVar const &v) {
        os << v.name_ << std::endl;
        if (v.decay != 0) {
            os << "decay: " << v.decay << std::endl;
        }
        if (v.learnDecay) {
            os << "learnDecay: " << v.learnDecay << std::endl;
        }
        if (v.learnDecayMulti > -1) {
            os << "learnDecayMulti: " << v.learnDecayMulti << std::endl;
        }
        print_vector(v.values_);
        return os;
    }
};

#endif