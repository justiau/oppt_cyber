#include "ScenarioParser.hpp"
#include <stdlib.h>
#include <iostream>

using namespace std;

string scenarioPath = "/home/justin/dev/oppt_cyber/scenarios/exp_1/es1_red_d.yaml";

int main() {
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
}