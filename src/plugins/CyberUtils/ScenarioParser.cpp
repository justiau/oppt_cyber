#include "ScenarioParser.hpp"
#include "Scenario.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    std::string scenarioPath = "/home/justin/dev/id-oppt/data/es1_LD/ld_020/es1_red_ld_020.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
    Scenario *scenario = scenarioParser.get();
    scenario->show();
}