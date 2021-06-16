#ifndef _SCENARIO_HPP_
#define _SCENARIO_HPP_
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include "Action.hpp"
#include "Variable.hpp"
#include "ScenarioUtils.hpp"

class Scenario {
public:
    Scenario() = default;

    virtual ~Scenario() = default;

    int nStates;

    int nActions;

    int nObservations;

    float discount;    

    std::unordered_map<std::string, Variable> state;

    std::unordered_map<std::string, Action> actions;

    std::unordered_map<std::string, Variable> nonStateObs;

    std::vector<std::string> stateObs;

    Variable getStateVar(std::string vname) {
        if (state.find(vname) == state.end())
            throw std::invalid_argument("Variable name not found in state variables");
        return state[vname];
        
    }

    void show() {
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
    
};

#endif