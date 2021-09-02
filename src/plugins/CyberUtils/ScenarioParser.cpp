#include "ScenarioParser.hpp"
#include "Scenario.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    std::string scenarioPath = "/home/justin/dev/oppt_cyber/scenarios/es1_red_decay_ld_0100.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
    Scenario *scenario = scenarioParser.get();
    scenario->show();
    vector<int> v1;
}