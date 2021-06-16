#ifndef _SCENARIO_PARSER_HPP_
#define _SCENARIO_PARSER_HPP_
#include "yaml-cpp/yaml.h"
#include "ScenarioUtils.hpp"
#include "Scenario.hpp"
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
        parseDiscount();
        parseStateSpace();
        parseActionSpace();
        parseObservationSpace();
        scenario.show();
    }

    void parseDiscount() {
        scenario.discount = root["discount"].as<float>();
    }

    void parseStateSpace() {
        YAML::Node state = root["state_space"];
        for(YAML::const_iterator si = state.begin(); si != state.end(); ++si) {
            string vname = si->first.as<string>();
            vector<string> values = si->second["values"].as<vector<string>>();
            Variable var(vname, values);
            var.decay = si->second["decay"].as<float>();
            var.fullyObs = si->second["fully_obs"].as<bool>();
            var.initValue = si->second["initial_value"].as<string>();
            scenario.state.insert(std::make_pair(vname, var));
        }
    }

    void parseActionSpace() {
        YAML::Node actions = root["action_space"];
        for(YAML::const_iterator ai = actions.begin(); ai != actions.end(); ++ai) {
            string aname = ai->first.as<string>();
            float cost = ai->second["cost"].as<float>();
            float probSuccess = ai->second["prob_success"].as<float>();
            YAML::Node fail = ai->second["effects"]["failure"]["next_state"];
            vector<Assignment> onFail;
            for(YAML::const_iterator fi = fail.begin(); fi != fail.end(); ++fi) {
                string fname = fi->first.as<string>();
                string fval = fi->second.as<string>();
                onFail.push_back(scenario.getStateVar(fname).makeAssign(fval));
            }

            YAML::Node success = ai->second["effects"]["success"]["next_state"];
            vector<Assignment> onSuccess;
            for(YAML::const_iterator si = success.begin(); si != success.end(); ++si) {
                string sname = si->first.as<string>();
                string sval = si->second.as<string>();
                onSuccess.push_back(scenario.getStateVar(sname).makeAssign(sval));
            }

            YAML::Node pre = ai->second["preconditions"];
            vector<Assignment> preconditions;
            for(YAML::const_iterator pi = pre.begin(); pi != pre.end(); ++pi) {
                string pname = pi->first.as<string>();
                string pval = pi->second.as<string>();
                preconditions.push_back(scenario.getStateVar(pname).makeAssign(pval));
            }
            Action action(aname, onFail, onSuccess, preconditions, cost, probSuccess);
            scenario.actions.insert(std::make_pair(aname, action));
        }
        scenario.nActions = actions.size();
    }

    void parseObservationSpace() {
        // non state obs node
        YAML::Node nsObs = root["observation_space"]["non_state_obs"];
        for(YAML::const_iterator oi = nsObs.begin(); oi != nsObs.end(); ++oi) {
            string oname = oi->first.as<string>();
            vector<string> values = oi->second.as<vector<string>>();
            Variable var(oname, values);
            scenario.nonStateObs.insert(std::make_pair(oname, var));
        }
        // state obs node
        YAML::Node sObs = root["observation_space"]["state_obs"];
        scenario.stateObs = sObs.as<vector<string>>();
    }

private:
    YAML::Node root;
    Scenario scenario;
};

#endif