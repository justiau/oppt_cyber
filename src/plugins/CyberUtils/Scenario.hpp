#ifndef _SCENARIO_HPP_
#define _SCENARIO_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <stdexcept>
#include <iostream>
#include "SAction.hpp"
#include "SVar.hpp"
#include "ScenarioUtils.hpp"

class Scenario {
public:
    Scenario() = default;

    virtual ~Scenario() = default;

    int nStates;

    int nActions;

    int nObservations;

    float discount;

    // std::unordered_map<std::string, SVar> state;
    std::map<std::string, SVar> state;
    // std::vector<SVar> state;

    // std::unordered_map<std::string, SAction> actions;
    std::map<std::string, SAction> actions;
    // std::vector<SAction> actions;

    // std::unordered_map<std::string, SVar> nonStateObs;
    std::map<std::string, SVar> nonStateObs;
    // std::vector<SVar> nonStateObs;

    std::vector<std::string> stateObs;

    SVar getStateVar(std::string vname) {
        if (state.find(vname) == state.end()) {
            std::cout << vname << std::endl;
            print_map(state);
            throw std::invalid_argument("Variable name not found in state variables");
        }
        return state[vname];
        
    }

    SVar getObsVar(std::string vname) {
        if (nonStateObs.find(vname) == nonStateObs.end()) {
            std::cout << vname << std::endl;
            print_map(nonStateObs);
            throw std::invalid_argument("Variable name not found in observation variables");
        }
        return nonStateObs[vname];
    }

    SVar getVar(std::string vname) {
        if (state.find(vname) != state.end()) {
            return state[vname];
        } else if (nonStateObs.find(vname) != nonStateObs.end()) {
            return nonStateObs[vname];
        }
        throw std::invalid_argument("Variable name not found in state or observation variables");
    }

    void show() {
        std::cout << "Number of Actions: " << nActions << std::endl;
        std::cout << "Number of States: " << nStates << std::endl;
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
        print_map(state);
    }

    void showActions() {
        std::cout << "Action Space: " << std::endl;
        print_map(actions);
    }

    void showNonStateObs() {
        std::cout << "Non State Observations: " << std::endl;
        print_map(nonStateObs);
    }

    void showStateObs() {
        std::cout << "State Observations: " << std::endl;
        print_vector(stateObs);
    }

    std::vector<SVar> getStateVars() {
        std::cout << "Getting states.." << std::endl;
        std::vector<SVar> vars = get_map_values(state);
        return vars;
    }

    std::vector<SAction> getActions() {
        std::cout << "Getting actions.." << std::endl;
        std::vector<SAction> acts = get_map_values(acts);
        return acts;
    }

    std::vector<float> getActionProbs() {
        std::cout << "Gettin action probs.." << std::endl;
        std::vector<SAction> acts = getActions();
        std::vector<float> probs;
        for (auto const &a : acts) {
            probs.push_back(a.probSuccess_);
        }
        return probs;
    }
    
};

#endif