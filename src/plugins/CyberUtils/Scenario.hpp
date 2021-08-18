#ifndef _SCENARIO_HPP_
#define _SCENARIO_HPP_
#include "SAction.hpp"
#include "SVar.hpp"
#include "ScenarioUtils.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <bits/stdc++.h>

class Scenario {
public:
    Scenario() = default;

    virtual ~Scenario() = default;

private:
    // static discount
    float discount = 0;

    // zero-indexed max observation size including null observation
    int maxObsSize = 0;

    // static list of variables
    std::vector<SVar> state;
    std::unordered_map<std::string, int> stateIndex;

    // static list of actions
    std::vector<SAction> actions;
    std::unordered_map<std::string, int> actionIndex;

    // static list of non state observations
    std::vector<SVar> nonStateObs;
    std::unordered_map<std::string, int> nonStateObsIndex;

    // static list of state observations
    std::vector<SVar> stateObs;
    std::unordered_map<std::string, int> stateObsIndex;

    // terminal states
    std::vector<Terminal> terminals;

    // dynamic state vector representing oppt state
    std::vector<int> opptState_;

    // dynamic observation vector representing oppt observations
    std::vector<int> opptObs_;

public:
    // read methods
    float getDiscount() {
        return discount;
    }

    int getStateSize() {
        return state.size();
    }

    SVar getStateVar(int stateIndex) {
        return state[stateIndex];
    }

    SVar getStateVar(std::string vname) {
        int si = getStateIndex(vname);
        return state[si];
    }

    bool isStateObs(std::string oname) {
        return getObsIndex(oname) < stateObs.size();
    }

    bool isStateObs(Assignment a) {
        return getObsIndex(a.vname_) < stateObs.size();
    }

    SVar getObsVar(int obsIndex) {
        return (obsIndex < stateObs.size()) ? stateObs[obsIndex] : nonStateObs[obsIndex - stateObs.size()];
    }

    SVar getObsVar(std::string vname) {
        int oi = getObsIndex(vname);
        return getObsVar(oi);
    }

    SVar getVar(std::string vname) {
        auto si = stateIndex.find(vname);
        auto oi = nonStateObsIndex.find(vname);
        if (si != stateIndex.end()) {
            return state[si->second];
        } else if (oi != nonStateObsIndex.end()) {
            return nonStateObs[oi->second];
        }
        throw std::invalid_argument("Variable name not found in state or observation variables");
    }

    std::vector<SVar> getStateVars() {
        return state;
    }

    SAction getAction(int aIndex) {
        if (aIndex >= 0 && aIndex < actions.size()) {
            return actions[aIndex];
        }
        throw std::invalid_argument("Index out of bounds for getAction");
    }

    SAction getAction(std::string aname) {
        int i = getActionIndex(aname);
        return getAction(i);
    }

    std::vector<SAction> getActions() {
        return actions;
    }

    int getActionsSize() {
        return actions.size();
    }

    std::vector<SVar> getAllObs() {
        std::vector<SVar> obs = stateObs;
        obs.insert(obs.end(), nonStateObs.begin(), nonStateObs.end());
        return obs;
    }

    int getObsSize() {
        // state observations come before non state observations in obs vector
        return stateObs.size() + nonStateObs.size();
    }

    std::vector<Terminal> getTerminals() {
        return terminals;
    }

    // write methods for parsing
    void setDiscount(float d) {
        discount = d;
    }
    
    void addStateVar(SVar var) {
        if (stateIndex.find(var.name_) != stateIndex.end()) {
            throw std::invalid_argument("Variable name to add already exists in state variables");
        }
        // add var to index and map of state vars
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

    void addTerminal(Terminal t) {
        terminals.push_back(t);
    }

    void setStateObs(std::vector<std::string> onames) {
        std::vector<SVar> obsVec;
        std::unordered_map<std::string, int> obsIndexMap;
        for (unsigned int i = 0; i < onames.size(); ++i) {
            std::string oname = onames[i];
            SVar var = getStateVar(oname);
            var.setType(VAR_TYPE::STATE_OBSERVATION);
            obsIndexMap.insert(std::make_pair(var.name_, i));
            obsVec.push_back(var);
        }
        stateObs = obsVec;
        stateObsIndex = obsIndexMap;
    }

    void addNonStateObs(SVar var) {
        if (nonStateObsIndex.find(var.name_) != nonStateObsIndex.end()) {
            throw std::invalid_argument("Observation name to add already exists in non state observation variables");
        }
        var.setType(VAR_TYPE::NONSTATE_OBSERVATION);
        nonStateObsIndex.insert(std::make_pair(var.name_, nonStateObs.size()));
        nonStateObs.push_back(var);
    }

    // oppt methods
    void setOpptState(std::vector<double> opptState) {
        // convert vector float to int type for discrete state
        std::vector<int> intVec(opptState.begin(), opptState.end());
        opptState_ = intVec;
    }

    std::vector<double> getOpptState() {
        std::vector<double> floatVec(opptState_.begin(), opptState_.end());
        return floatVec;
    }

    void setOpptObs(std::vector<double> opptObs) {
        // convert vector float to int type for discrete state
        std::vector<int> intVec(opptObs.begin(), opptObs.end());
        opptObs_ = intVec;
    }

    std::vector<double> getOpptObs() {
        std::vector<double> floatVec(opptObs_.begin(), opptObs_.end());
        return floatVec;
    }

    bool isAssignTrue(Assignment a) {
        int si = getStateIndex(a.vname_);
        int vi = state[si].getIndex(a.value_);
        return opptState_[si] == vi;
    }

    bool isAllAssignTrue(std::vector<Assignment> assigns) {
        for (auto a : assigns) {
            if (!isAssignTrue(a)) {
                // return false on first fail
                return false;
            }
        }
        return true;
    }

    void updateMaxObsSize() {
        int maxSize = 0;
        for (auto var : getAllObs()) {
            // include null observation to value count
            int size = var.getValueCount() + 1;
            if (size > maxSize) maxSize = size;
        }
        maxObsSize = maxSize;
    }

    long getBinNumber() {
        long binNumber = 0;
        for (int i = 0; i < opptObs_.size(); ++i) {
            // 0-index observation value
            int obsVal = opptObs_[i] + 1;
            binNumber += obsVal * (std::pow(maxObsSize,i));
        }
        return binNumber;
    }

    int getOpptVal(std::string vname) {
        auto si = stateIndex.find(vname);
        return opptState_[si->second];
    }

    int getOpptVal(int index) {
        return opptState_[index];
    }

    void assignState(Assignment a) {
        // apply assignment to loaded oppt state vector
        int si = getStateIndex(a.vname_);
        int vi = state[si].getIndex(a.value_);
        opptState_[si] = vi;
    }

    void assignObs(Assignment a) {
        // apply assignment to loaded oppt observation vector
        int obsIndex = getObsIndex(a.vname_);
        SVar obs = getObsVar(obsIndex);
        if (a.type_ == VAR_TYPE::STATE_OBSERVATION) {
            // state observation
            if (a.value_ == "actual") {
                int stateIndex = getStateIndex(a.vname_);
                opptObs_[obsIndex] = opptState_[stateIndex];
            } else if (a.value_ == "none") {
                // set null observation
                opptObs_[obsIndex] = -1;
            } else if (a.value_ == "noisy") {
                // noisy observation results in uniform observation across values
                int obsValue = obs.getRandIndex();
                opptObs_[obsIndex] = obsValue;
            } else {
                throw std::invalid_argument("State observation value was not recognized");
            }
        } else if (a.type_ == VAR_TYPE::NONSTATE_OBSERVATION) {
            // non state observation
            int obsValue = obs.getIndex(a.value_);
            opptObs_[obsIndex] = obsValue;
        } else {
            throw std::invalid_argument("Assignment type was not an observation but passed to assignObs function");
        }
    }

    // debug methods
    void show() {
        std::cout << "Number of Actions: " << getActionsSize() << std::endl;
        std::cout << "Number of States: " << getStateSize() << std::endl;
        showDiscount();
        showStates();
        showActions();
        showStateObs();
        showNonStateObs();
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

private:
    int getStateIndex(std::string vname) {
        return stateIndex.find(vname)->second;
    }

    int getObsIndex(std::string oname) {
        auto sit = stateObsIndex.find(oname);
        auto oit = nonStateObsIndex.find(oname);
        if (sit != stateObsIndex.end()) {
            return sit->second;
        } else if (oit != nonStateObsIndex.end()) {
            return stateObs.size() + oit->second;
        } else {
            throw std::invalid_argument("Obs variable name not found in observation variables");
        }
    }

    int getActionIndex(std::string aname) {
        return actionIndex.find(aname)->second;
    }
    
};

#endif