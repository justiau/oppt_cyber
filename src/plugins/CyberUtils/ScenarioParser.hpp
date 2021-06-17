#ifndef _SCENARIO_PARSER_HPP_
#define _SCENARIO_PARSER_HPP_
#include "yaml-cpp/yaml.h"
#include "ScenarioUtils.hpp"
#include "Scenario.hpp"
#include "SVar.hpp"
#include "SAction.hpp"
#include <iostream>
#include <string>
#include <utility>

using namespace std;

class ScenarioParser {
public:
    ScenarioParser() = default;

    virtual ~ScenarioParser() = default;

    void parse(const std::string scenarioPath) {
        root = YAML::LoadFile(scenarioPath);
        scenario = new Scenario();
        parseDiscount();
        parseStateSpace();
        parseObservationSpace();
        parseActionSpace();
        scenario->show();
    }

    Scenario* get() {
        if (scenario == NULL) {
            std::cout << "Pomdp is null. Parse pomdpx before calling 'get()'" << std::endl;
            exit(1);
        }
        return scenario;
    }

    void parseDiscount() {
        scenario->discount = root["discount"].as<float>();
    }

    void parseStateSpace() {
        YAML::Node state = root["state_space"];
        for(YAML::const_iterator si = state.begin(); si != state.end(); ++si) {
            string vname = si->first.as<string>();
            vector<string> values = si->second["values"].as<vector<string>>();
            SVar var(vname, values);
            var.decay = si->second["decay"].as<float>();
            var.fullyObs = si->second["fully_obs"].as<bool>();
            var.initValue = si->second["initial_value"].as<string>();
            scenario->state.insert(std::make_pair(vname, var));
        }
        scenario->nStates = state.size();
    }

    void parseActionSpace() {
        YAML::Node actions = root["action_space"];
        for(YAML::const_iterator ai = actions.begin(); ai != actions.end(); ++ai) {
            string aname = ai->first.as<string>();
            float cost = ai->second["cost"].as<float>();
            float probSuccess = ai->second["prob_success"].as<float>();
            YAML::Node preNode = ai->second["preconditions"];
            vector<Assignment> preconditions = extractAssignments(preNode);
            SAction action(aname, cost, probSuccess, preconditions);
            // find fail node and add on fail state and observation changes
            YAML::Node failNode = ai->second["effects"]["failure"];
            vector<Assignment> onFailState =  extractAssignments(failNode["next_state"]);
            vector<Assignment> onFailObs = extractAssignments(failNode["observation"]);
            action.setFailEffects(onFailState, onFailObs);
            // find success node and add on success state and observation changes
            YAML::Node successNode = ai->second["effects"]["success"];
            vector<Assignment> onSuccessState = extractAssignments(successNode["next_state"]);
            vector<Assignment> onSuccessObs = extractAssignments(successNode["observation"]);
            action.setSuccessEffects(onSuccessState, onSuccessObs);
            scenario->actions.insert(std::make_pair(aname, action));
        }
        scenario->nActions = actions.size();
    }

    vector<Assignment> extractAssignments(YAML::Node node)  {
        vector<Assignment> assignments;
        for(YAML::const_iterator ci = node.begin(); ci != node.end(); ++ci) {
            string name = ci->first.as<string>();
            string val = ci->second.as<string>();
            SVar var = scenario->getVar(name);
            assignments.push_back(var.makeAssign(val));
        }
        return assignments;
    }

    void parseObservationSpace() {
        // non state obs node
        YAML::Node nsObs = root["observation_space"]["non_state_obs"];
        for(YAML::const_iterator oi = nsObs.begin(); oi != nsObs.end(); ++oi) {
            string oname = oi->first.as<string>();
            vector<string> values = oi->second.as<vector<string>>();
            SVar var(oname, values);
            scenario->nonStateObs.insert(std::make_pair(oname, var));
        }
        // state obs node
        YAML::Node sObs = root["observation_space"]["state_obs"];
        scenario->stateObs = sObs.as<vector<string>>();
    }

private:
    YAML::Node root;
    Scenario* scenario = NULL;
};

#endif