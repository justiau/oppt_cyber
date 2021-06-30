#ifndef _SCENARIO_HPP_
#define _SCENARIO_HPP_
#include "SAction.hpp"
#include "SVar.hpp"
#include "ScenarioUtils.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <iostream>
#include <utility>

class Scenario {
public:
    Scenario() = default;

    virtual ~Scenario() = default;

private:
    // static discount
    float discount;

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

    // dynamic state vector representing oppt state
    std::vector<int> opptState_;

    // dynamic observation vector representing oppt observations
    std::vector<int> opptObs_;

public:
    bool actionSuccess;

    // read methods
    float getDiscount() {
        return discount;
    }

    int getStateSize() {
        return state.size();
    }

    int getStateIndex(std::string vname) {
        return stateIndex.find(vname)->second;
    }

    SVar getStateVar(std::string vname) {
        int si = getStateIndex(vname);
        return state[si];
    }

    int getObsIndex(std::string oname) {
        auto sit = stateObsIndex.find(oname);
        auto oit = nonStateObsIndex.find(oname);
        if (sit != stateObsIndex.end()) {
            return sit->second;
        } else if (oit != nonStateObsIndex.end()) {
            return stateIndex.size() + oit->second;
        } else {
            throw std::invalid_argument("Obs variable name not found in state or observation variables");
        }
    }

    bool isStateObs(std::string oname) {
        return getObsIndex(oname) < stateObs.size()
    }

    bool isStateObs(Assignment a) {
        return getObsIndex(a.vname_) < stateObs.size();
    }

    SVar getObsVar(int obsIndex) {
        return (obsIndex < stateObs.size()) ? stateObs[obsIndex] : nonStateObs[obsIndex];
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

    int getActionIndex(std::string aname) {
        return actionIndex.find(aname)->second;
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

    // write methods for parsing
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
        var.setType(VAR_TYPE::NONSTATE_OBSERVATION);
        nonStateObsIndex.insert(std::make_pair(var.name_, nonStateObs.size()));
        nonStateObs.push_back(var);
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

    // oppt methods
    void setOpptState(std::vector<float> opptState) {
        // convert vector float to int type for discrete state
        std::vector<int> intVec(opptState.begin(), opptState.end());
        opptState_ = intVec;
    }

    std::vector<float> getOpptState() {
        std::vector<float> floatVec(opptState_.begin(), opptState_.end());
        return floatVec;
    }

    void setOpptObs(std::vector<float> opptObs) {
        // convert vector float to int type for discrete state
        std::vector<int> intVec(opptObs.begin(), opptObs.end());
        opptObs_ = intVec;
    }

    std::vector<float> getOpptObs() {
        std::vector<float> floatVec(opptObs_.begin(), opptObs_.end());
        return floatVec;
    }

    bool isAssignTrue(Assignment a) {
        int si = getStateIndex(a.vname_);
        int vi = state[si].getIndex(a.value_);
        return opptState_[si] == vi;
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
                // get probability of observation error
                // noisy observation results in uniform observation across values
                opptObs_[obsIndex] = obs.getRandIndex();
            ] else {
                throw std::invalid_argument("State observation value was not recognized");
            }
        } else if (a.type_ == VAR_TYPE::NONSTATE_OBSERVATION) {
            // non state observation
            opptObs_[obsIndex] = obs.getIndex(a.value_);
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