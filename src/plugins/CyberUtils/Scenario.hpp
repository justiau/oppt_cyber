#ifndef _SCENARIO_HPP_
#define _SCENARIO_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <iostream>
#include <utility>
#include "SAction.hpp"
#include "SVar.hpp"
#include "ScenarioUtils.hpp"

class Scenario {
public:
    Scenario() = default;

    virtual ~Scenario() = default;

private:
    float discount;

    std::vector<SVar> state;
    std::unordered_map<std::string, int> stateIndex;

    std::vector<SAction> actions;
    std::unordered_map<std::string, int> actionIndex;

    std::vector<SVar> nonStateObs;
    std::unordered_map<std::string, int> nonStateObsIndex;

    std::vector<std::string> stateObs;

public:
    // read methods
    float getDiscount() {
        return discount;
    }

    int getStateSize() {
        return state.size();
    }

    SVar getStateVar(std::string vname) {
        int i = stateIndex.find(vname);
        if (i == stateIndex.end()) {
            std::cout << vname << std::endl;
            print_map(state);
            throw std::invalid_argument("Variable name not found in state variables");
        }
        return state[i];
        
    }

    SVar getObsVar(std::string vname) {
        int i = nonStateObsIndex.find(vname);
        if (i == nonStateObsIndex.end()) {
            std::cout << vname << std::endl;
            print_map(nonStateObs);
            throw std::invalid_argument("Variable name not found in observation variables");
        }
        return nonStateObs[i];
    }

    SVar getVar(std::string vname) {
        int si = stateIndex.find(vname);
        int oi = nonStateObsIndex.find(vname);
        if (si != stateIndex.end()) {
            return state[si];
        } else if (oi != nonStateObsIndex.end()) {
            return nonStateObs[oi];
        }
        throw std::invalid_argument("Variable name not found in state or observation variables");
    }

    std::vector<SVar> getStateVars() {
        return state;
    }

    SAction getAction(int aIndex) {
        if (aIndex >= 0 && aIndex < actions.size()) {
            return actions[aIndex]
        }
        throw std::invalid_argument("Index out of bounds for getAction");
    }

    SAction getAction(std::string aname) {
        int i = actionIndex.find(aname);
        return getAction(i);
    }

    std::vector<SAction> getActions() {
        return actions;
    }

    int getActionsSize() {
        return actions.size();
    }

    int getObsSize() {
        return nonStateObs.size() + stateObs.size();
    }

    bool isAssignTrue(Assignment a) {
        SVar v = getStateVar(a.vname);
        return v.isAssign(a);
    }

    bool isPreconditionsTrue(std::vector<Assignment> preconditions) {
        for (auto pre : preconditions) {
            if (!isAssignTrue(pre)) {
                // return false on first fail
                return false;
            }
        }
        return true;
    }

    // write methods
    void setDiscount(float d) {
        discount = d;
    }
    
    void addStateVar(SVar var) {
        if (stateIndex.find(var.name_) != stateIndex.end()) {
            throw std::invalid_argument("Variable name to add already exists in state variables");
        }
        stateIndex.insert(std::make_pair(var.name_, state.size()));
        state.push_back(var);
    }

    void addAction(SAction a) {
        if (actionIndex.find(a.name_) != actionIndex.end()) {
            throw std::invalid_argument("Action name to add already exists in action list");
        }
        actionIndex.insert(std::make_pair(a.name_, actions.size()));
        actions.push_back(a);
    }

    void addNonStateObs(SVar var) {
        if (nonStateObsIndex.find(var.name_) != nonStateObsIndex.end()) {
            throw std::invalid_argument("Observation name to add already exists in non state observation variables");
        }
        nonStateObsIndex.insert(std::make_pair(var.name_, nonStateObs.size()));
        nonStateObs.push_back(var);
    }

    void addStateObs(std::string obsName) {
        if (stateIndex.find(obsName) == stateIndex.end()) {
            throw std::invalid_argument("State observation name to add doesn't exist in state variables");
        }
        stateObs.push_back(obsName);
    }

    void setStateObs(std::vector<std::string> obs) {
        stateObs = obs;
    }

    void makeAssignment(Assignment a) {
        int i = stateIndex.find(a.vname);
        if (i == stateIndex.end()) {
            throw std::invalid_argument("Assignment variable name not found in state index");
        }
        state[i].setAssign(a);
    }

    // debug methods
    void show() {
        std::cout << "Number of Actions: " << getActionsSize() << std::endl;
        std::cout << "Number of States: " << getStateSize() << std::endl;
        showDiscount();
        showStates();
        showActions();
        showNonStateObs();
        showStateObs();
    }

    void showDiscount() {
        std::cout << "Discount: " << discount << std::endl;
    }

    void showStates() {
        std::cout << "State Space: " << std::endl;
        print_vector(state);
    }

    void showActions() {
        std::cout << "Action Space: " << std::endl;
        print_vector(actions);
    }

    void showNonStateObs() {
        std::cout << "Non State Observations: " << std::endl;
        print_vector(nonStateObs);
    }

    void showStateObs() {
        std::cout << "State Observations: " << std::endl;
        print_vector(stateObs);
    }
    
};

#endif