#ifndef _SCENARIO_VARIABLE_HPP_
#define _SCENARIO_VARIABLE_HPP_
#include "ScenarioUtils.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <stdexcept>

struct Assignment {
    std::string vname;
    std::string value;

    Assignment(std::string name, std::string val) {
        vname = name;
        value = val;
    }
};

std::ostream &operator<<(std::ostream &os, Assignment const &a) {
    os << "{ " << a.vname << ": " << a.value << " }";
    return os;
}

class Variable {
public:
    Variable() = default;

    Variable(std::string name, std::vector<std::string> values)
        : name_(name)
        , values_(values) {
        for (std::size_t i=0; i != values_.size(); ++i) {
            index_[values_[i]] = i;
        }
    };
    
    virtual ~Variable() = default;

    std::string name_;

    std::vector<std::string> values_;

    std::unordered_map<std::string, int> index_;

    float decay;

    bool fullyObs;

    // current index
    int cindex;

    std::string initValue;

    int getIndex(std::string val) {
        if (index_.find(val) == index_.end())
            return -1;
        return index_[val];
    }

    bool hasValue(std::string val) {
        return getIndex(val) == cindex;
    }

    Assignment makeAssign(std::string val) {
        if (index_.find(val) != index_.end() || (val == "none" || val == "actual")) {
            return Assignment(name_, val);
        }
        throw std::invalid_argument("Invalid value provided to makeAssign");
    }

    void setAssign(Assignment a) {
        if (a.vname == name_ && index_.find(a.value) != index_.end())
            cindex = getIndex(a.value);
    }

    bool isAssign(Assignment a) {
        if (a.vname == name_ && index_.find(a.value) != index_.end())
            return cindex == getIndex(a.value);
        return false;
    }

    friend std::ostream &operator<<(std::ostream &os, Variable const &v) {
        os << v.name_ << std::endl;
        print_vector(v.values_);
        return os;
    }

};

#endif