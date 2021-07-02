#include "ScenarioParser.hpp"
#include <iostream>
#include <string>

int main() {
    std::string scenarioPath = "/home/justin/dev/oppt_cyber/scenarios/exp_1/es1_red_d.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
}