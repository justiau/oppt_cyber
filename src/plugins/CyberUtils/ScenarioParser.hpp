#ifndef _SCENARIO_PARSER_HPP_
#define _SCENARIO_PARSER_HPP_
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/exceptions.h"
#include "ScenarioUtils.hpp"
#include "Scenario.hpp"
#include "SVar.hpp"
#include "SAction.hpp"
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cmath>

using std::cout;
using std::endl;

class ScenarioParser {
public:
    ScenarioParser() = default;

    virtual ~ScenarioParser() = default;

    void parse(const std::string scenarioPath) {
        root = YAML::LoadFile(scenarioPath);
        scenario = new Scenario();
        parseDiscount();
        parseDecayStep();
        parseDecayFactors();
        parseStateSpace();
        parseObservationSpace();
        parseActionSpace();
        parseTerminalStates();
    }

    Scenario* get() {
        if (scenario == NULL) {
            std::cout << "Scenario is null. Parse the scenario before calling 'get()'" << std::endl;
            exit(1);
        }
        return scenario;
    }

    void parseDiscount() {
        scenario->setDiscount(root["discount"].as<float>());
    }

    void parseDecayStep(){
        // check decay resolution divides into 1
        if (root["decay_step"]) {
            float decayStep = root["decay_step"].as<float>();
            if (decayStep != 0) {
                float r = fmod(1.0, decayStep);
                bool decayValid = r == 0 || floatCompare(r,decayStep);
                if (decayValid) {
                    scenario->setDecayStep(decayStep);
                } else {
                    std::cout << "Decay step does not divide into 1" << std::endl;
                    exit(1);
                }
            }
        }
    }

    void parseDecayFactors() {
        if (root["decay_factors"]) {
            int decayFactors = root["decay_factors"].as<int>();
            scenario->setDecayFactors(decayFactors);
        }
    }

    void parseStateSpace() {
        YAML::Node state = root["state_space"];
        for(YAML::const_iterator si = state.begin(); si != state.end(); ++si) {
            std::string vname = si->first.as<std::string>();
            std::vector<std::string> values = si->second["values"].as<std::vector<std::string>>();
            SVar var(vname, values);
            if (si->second["decay"]) {
                try {
                    var.decay = si->second["decay"].as<float>();
                } catch (const YAML::BadConversion& e1) {
                    var.decay = 0;
                    try {
                        var.learnDecay = si->second["decay"].as<bool>();   
                    } catch (const YAML::BadConversion& e2) {
                        var.learnDecay = false;
                        try {
                            std::string decayFactor = si->second["decay"].as<std::string>();
                            var.learnDecayMulti = std::stoi(decayFactor.substr(1));
                        } catch (const YAML::BadConversion& e3) {
                            var.learnDecayMulti = -1;
                        }
                    }
                }
            }
            var.fullyObs = (si->second["fully_obs"]) ? si->second["fully_obs"].as<bool>() : false;
            var.initValue = si->second["initial_value"].as<std::string>();
            var.oneWayDecay = (si->second["one_way_decay"]) ? si->second["one_way_decay"].as<bool>() : false;
            scenario->addStateVar(var);
        }
    }

    void parseActionSpace() {
        YAML::Node actions = root["action_space"];
        for(YAML::const_iterator ai = actions.begin(); ai != actions.end(); ++ai) {
            std::string aname = ai->first.as<std::string>();
            float cost = ai->second["cost"].as<float>();
            float probSuccess = ai->second["prob_success"].as<float>();
            YAML::Node preNode = ai->second["preconditions"];
            std::vector<Assignment> preconditions = extractAssignments(preNode);
            SAction action(aname, cost, probSuccess, preconditions);
            // find fail node and add on fail state and observation changes
            YAML::Node failNode = ai->second["effects"]["failure"];
            std::vector<Assignment> onFailState = extractAssignments(failNode["next_state"]);
            std::vector<Assignment> onFailObs = extractAssignments(failNode["observation"], true);
            action.setFailEffects(onFailState, onFailObs);
            // find success node and add on success state and observation changes
            YAML::Node successNode = ai->second["effects"]["success"];
            std::vector<Assignment> onSuccessState = extractAssignments(successNode["next_state"]);
            std::vector<Assignment> onSuccessObs = extractAssignments(successNode["observation"], true);
            action.setSuccessEffects(onSuccessState, onSuccessObs);
            scenario->addAction(action);
        }
    }

    std::vector<Assignment> extractAssignments(YAML::Node node, bool isObsNode=false)  {
        std::vector<Assignment> assignments;
        if (node.size() > 0) {
            bool optional = node.begin()->first.as<std::string>() == "optional" && node.begin()->second.IsMap();
            if (optional) node = node["optional"];
            for (YAML::const_iterator ci=node.begin(); ci != node.end(); ++ci) {
                std::string name = ci->first.as<std::string>();
                std::string val = ci->second.as<std::string>();
                SVar var = (isObsNode) ? scenario->getObsVar(name) : scenario->getStateVar(name);
                assignments.push_back(var.createAssignment(val, optional));
            }
        }
        return assignments;
    }

    void parseObservationSpace() {
        // non state obs node
        YAML::Node nsObs = root["observation_space"]["non_state_obs"];
        for(YAML::const_iterator oi = nsObs.begin(); oi != nsObs.end(); ++oi) {
            std::string oname = oi->first.as<std::string>();
            std::vector<std::string> values = oi->second.as<std::vector<std::string>>();
            SVar var(oname, values);
            scenario->addNonStateObs(var);
        }
        // state obs node
        YAML::Node sObs = root["observation_space"]["state_obs"];
        // list of state observations
        std::vector<std::string> stateObs = sObs.as<std::vector<std::string>>();
        // add fully observed state variables to state observations
        // std::vector<SVar> stateVars = scenario->getStateVars();
        // for(size_t i=0; i<stateVars.size(); ++i) {
        //     SVar var = stateVars[i];
        //     if (var.fullyObs) {
        //         stateObs.push_back(var.name_);
        //     }
        // }
        scenario->setStateObs(stateObs);
    }

    void parseTerminalStates() {
        YAML::Node terminal = root["terminal_states"];
        for (YAML::const_iterator ti = terminal.begin(); ti != terminal.end(); ++ti) {
            std::string tname = ti->first.as<std::string>();
            float reward;
            try {
                reward = ti->second["reward"].as<float>();
            } catch (const YAML::BadConversion& e) {
                reward = ti->second["reward"].as<int>();
            }
            std::vector<Assignment> conditions = extractAssignments(ti->second["state"]);
            Terminal terminal(tname, conditions, reward);
            scenario->addTerminal(terminal);
        }
    }

private:
    YAML::Node root;
    Scenario* scenario = NULL;
};

#endif