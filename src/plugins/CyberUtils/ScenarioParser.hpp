#ifndef _SCENARIO_PARSER_HPP_
#define _SCENARIO_PARSER_HPP_
#include "yaml-cpp/yaml.h"

using namespace std;

class ScenarioParser {
public:
    ScenarioParser() = default;

    virtual ~ScenarioParser() = default;

    void parse(const std::string scenarioPath) {
        YAML::Node scenario = YAML::LoadFile(scenarioPath);
        if(scenario["action_space"]){
            cout << "cost: " << scenario["action_space"]["IIS_ftp_exploit"]["cost"] << endl;
        }
    }
};

#endif