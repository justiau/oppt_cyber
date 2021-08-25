#include "ScenarioParser.hpp"
#include "Scenario.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    std::string scenarioPath = "/home/justin/dev/oppt_cyber/scenarios/es2_red_d.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
    Scenario *scenario = scenarioParser.get();
    vector<int> v1;
}