#include "ScenarioParser.hpp"
#include "Scenario.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    std::string scenarioPath = "/home/justin/dev/id-oppt/data/es1_D/d_080/es1_red_d_080.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
    Scenario *scenario = scenarioParser.get();
    vector<int> v1;
}